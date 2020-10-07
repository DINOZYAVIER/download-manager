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
    m_manager = new QNetworkAccessManager( this );
    m_request = new QNetworkRequest( m_url );

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
}

bool Downloader::saveToDisk()
{
    if( !m_file.isOpen() )
        return false;

    m_file.close();
    QString name;
    if( m_reply->header( QNetworkRequest::ContentDispositionHeader ).isValid() )
        name = m_downloadDir + m_reply->header( QNetworkRequest::ContentDispositionHeader ).toString().section( '"', 1, 1 );
    else
        name = m_downloadDir + saveFileName( m_url );

    if( QFile::exists( name ) )
    {
        // already exists, don't overwrite
        QFileInfo info( name );
        int cnt = 0;
        while( QFile::exists( m_downloadDir + info.baseName() + '_' + QString::number( cnt ) + '.' + info.completeSuffix() ) )
        {
            ++cnt;
        }
        QFile::rename( m_file.fileName(), m_downloadDir + info.baseName() + '_' + QString::number( cnt ) + '.' + info.completeSuffix() );
        m_file.setFileName( m_downloadDir + info.baseName() + '_' + QString::number( cnt ) + '.' + info.completeSuffix() );
    }
    else
    {
        QFile::rename( m_file.fileName(), name );
        m_file.setFileName( name );
    }
    return true;
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
        Q_EMIT protocolErrorOccured();
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
    QString name;
    QVariantList data;
    if( m_reply->header( QNetworkRequest::ContentDispositionHeader ).isValid() )
        name = m_reply->header( QNetworkRequest::ContentDispositionHeader ).toString().section( '"', 1, 1 );
    else
        name = saveFileName( m_url );
    data.append( name );

    if( !m_file.isOpen() )
    {
        name = m_downloadDir + name + ".download";
        if( QFile::exists( name ) )
        {
            // already exists, don't overwrite
            int i = 0;
            name += '_' ;
            while( QFile::exists( name + QString::number( i ) ) )
                ++i;

           name += QString::number( i );
        }
        qDebug() << name;
        m_file.setFileName( name );
        m_file.open( QIODevice::ReadWrite );
    }

    auto totalDataReceived = bytesReceived + m_downloadProgressAtPause;
    auto totalData = bytesTotal + m_downloadProgressAtPause;
    m_downloadProgress = m_downloadProgressAtPause + bytesReceived;
    m_file.write( m_reply->readAll() );

    data.append( QString::number( totalDataReceived / BYTES_IN_MEGABYTES ) + '/' + QString::number( totalData / BYTES_IN_MEGABYTES ) + "MB" );
    data.append( QString::number( bytesReceived * MILISECONS_IN_SECONDS / m_elapsedTimer->elapsed() / BYTES_IN_KILOBYTES ) + "KB/sec" );
    data.append( ( ( totalDataReceived ) * 100 / ( totalData ) ) );
    Q_EMIT progressChanged( data );
}

QString Downloader::saveFileName( const QUrl& url )
{
    QString path = url.path();
    QString basename = QFileInfo( path ).fileName();

    return basename;
}

void Downloader::resume( Downloader* downloader )
{
    if( m_reply != 0 || this != downloader )
        return;

    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number( m_downloadProgressAtPause ) + '-';
    m_request->setRawHeader( "Range", rangeHeaderValue );

    doDownload();

}

void Downloader::pause( Downloader* downloader )
{

    if( m_reply == 0 || this != downloader )
        return;

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
