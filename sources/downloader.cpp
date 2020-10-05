#include "precompiled.h"
#include "downloader.h"

Downloader::Downloader( const QUrl& url, QString path ) :
    QObject( nullptr ),
    m_url( url ),
    m_reply( nullptr ),
    m_elapsedTimer( nullptr ),
    m_thread( nullptr ),
    m_downloadDir( path ),
    m_downloadProgress( 0 ),
    m_downloadProgressAtPause( 0 )
{
    QString name = m_downloadDir + saveFileName( m_url ) + ".part";
    m_file.setFileName( name );
    if( QFile::exists( name ) )
    {
        // already exists, don't overwrite
        int i = 0;
        name += '_' ;
        while( QFile::exists( name + QString::number( i ) ) )
            ++i;

       name += QString::number( i );
    }
    m_file.setFileName( name );

    m_manager = new QNetworkAccessManager( this );
    m_request = new QNetworkRequest( m_url );

    if( !m_file.open( QIODevice::ReadWrite ) )
        qDebug() << "Could not open" << qPrintable( m_file.fileName() ) << "for writing" << qPrintable( m_file.errorString() );
    m_manager->setRedirectPolicy( QNetworkRequest::NoLessSafeRedirectPolicy );
}

Downloader::~Downloader()
{
    if( m_reply && m_reply->isRunning() )
    {
        if( m_reply->isRunning() )
        {
            m_reply->abort();
            m_reply->close();
        }
        m_reply->deleteLater();
    }

    if( m_elapsedTimer )
    {
        m_elapsedTimer->invalidate();
        delete m_elapsedTimer;
    }

    if( m_file.isOpen() )
    {
        m_file.close();
        m_file.remove();
    }

    delete m_request;
}

void Downloader::doDownload()
{
    qDebug() << "Starting download" << m_url;
    qDebug() << "Thread ID" << QThread::currentThreadId();
    qDebug() << "Thread address" << QThread::currentThread();

    m_reply = m_manager->get( *m_request );

    if( m_elapsedTimer )
        m_elapsedTimer->restart();
    else
    {
        m_elapsedTimer = new QElapsedTimer();
        m_elapsedTimer->start();
    }

    connect( m_reply, &QNetworkReply::downloadProgress, this, &Downloader::onProgress );
    connect( m_reply, &QNetworkReply::finished, this, &Downloader::onFinished );
    connect( m_reply, &QNetworkReply::errorOccurred, this, &Downloader::onError );
    connect( m_reply, &QNetworkReply::sslErrors, this, &Downloader::onSSLError );
    qDebug() << "Headers list:" << m_reply->rawHeaderPairs();
    qDebug() << "File info:" << m_reply->header( QNetworkRequest::ContentDispositionHeader );
}

bool Downloader::saveToDisk()
{
    QString name = m_downloadDir + saveFileName( m_url );
    if( QFile::exists( name ) )
    {
        // already exists, don't overwrite
        QFileInfo info( saveFileName( m_url ) );
        int cnt = 0;
        while( QFile::exists( m_downloadDir + info.baseName() + '_' + QString::number( cnt ) + '.' + info.completeSuffix() ) )
        {
            ++cnt;
        }
        QFile::rename( m_file.fileName(), m_downloadDir + info.baseName() + '_' + QString::number( cnt ) + '.' + info.completeSuffix() );
    }
    else
        QFile::rename( m_file.fileName(), name );

    if( m_file.isOpen() )
    {
        m_file.close();
        return true;
    }
    return false;
}

bool Downloader::isHttpRedirect( QNetworkReply* reply )
{
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void Downloader::onError( QNetworkReply::NetworkError code )
{
    qDebug() << QString( "Downloading of %1 failed" ).arg( m_url.toString() );
    qDebug() << "Error code:" << code << " Error:" << m_reply->errorString();
    switch( code )
    {
    case QNetworkReply::ProtocolUnknownError:
        m_reply->disconnect();
        Q_EMIT errorOccured();
        break;
    default:
        break;
    }
}
void Downloader::onSSLError( const QList<QSslError>& sslErrors )
{
    for( const auto& error : sslErrors )
        qDebug() << "SSL error:" << error.errorString();
}

void Downloader::onFinished()
{
    QUrl url = m_reply->url();
    if( m_reply->error() == QNetworkReply::NoError )
    {
        if( isHttpRedirect( m_reply ) )
            qDebug() << "Request was redirected.";
        if( saveToDisk() )
            qDebug() << "Download of" << url.toEncoded().constData() << "succeeded ( saved to" << qPrintable( m_file.fileName() ) << ')';
    }
    Q_EMIT finished();
}

void Downloader::onProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    m_downloadProgress = m_downloadProgressAtPause + bytesReceived;
    m_file.write( m_reply->readAll() );
    QVariantList data;
    data.append( m_url.fileName() );
    data.append( QString::number( ( bytesReceived + m_downloadProgressAtPause ) / 1048576) + '/' + QString::number( ( bytesTotal + m_downloadProgressAtPause ) / 1048576 ) + "MB" );
    data.append( QString::number( bytesReceived * 1000 / m_elapsedTimer->elapsed() / 1024 ) + "KB/sec" );
    data.append( ( ( bytesReceived + m_downloadProgressAtPause ) * 100  / ( bytesTotal + m_downloadProgressAtPause ) ) );
    Q_EMIT progressChanged( data );
}

QString Downloader::saveFileName( const QUrl& url )
{
    QString path = url.path();
    QString basename = QFileInfo( path ).fileName();

    if( basename.isEmpty() )
        basename = "download";

    return basename;
}

void Downloader::resume( Downloader* downloader )
{
    if( m_reply == 0 && this == downloader )
    {
        QByteArray rangeHeaderValue = "bytes=" + QByteArray::number( m_downloadProgressAtPause ) + '-';
        m_request->setRawHeader( "Range", rangeHeaderValue );

        doDownload();
    }
}

void Downloader::pause( Downloader* downloader )
{
    if( m_reply != 0 && this == downloader )
    {
        qDebug() << "Pause at" << m_downloadProgress << "bytes";

        disconnect( m_reply, &QNetworkReply::downloadProgress, this, &Downloader::onProgress );
        disconnect( m_reply, &QNetworkReply::finished, this, &Downloader::onFinished );
        disconnect( m_reply, &QNetworkReply::errorOccurred, this, &Downloader::onError );
        disconnect( m_reply, &QNetworkReply::sslErrors, this, &Downloader::onSSLError );

        m_reply->abort();
        m_file.flush();

        m_downloadProgressAtPause = m_downloadProgress;
        m_downloadProgress = 0;
        m_reply = 0;
    }
}
