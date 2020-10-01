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
    m_file.setFileName( m_downloadDir + saveFileName( m_url ) );
    //checkFileLocation();
    m_manager = new QNetworkAccessManager( this );
    m_request = new QNetworkRequest( m_url );

    if( !m_file.open( QIODevice::ReadWrite ) )
        qDebug() << "Could not open" << qPrintable( m_file.fileName() ) << "for writing" << qPrintable( m_file.errorString() );
}

Downloader::~Downloader()
{
    if( m_reply && m_reply->isRunning() )
    {
        if( m_reply->isRunning() )
        {
            m_reply->abort();
            m_reply->close();
            // TODO Delete file
        }
        m_reply->deleteLater();
    }

    if( m_elapsedTimer )
    {
        m_elapsedTimer->invalidate();
        delete m_elapsedTimer;
    }

    delete m_request;
}

void Downloader::doDownload()
{
    qDebug() << "Starting download" << m_url;
    qDebug() << "Thread ID" << QThread::currentThreadId();

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
    m_reply->open( QIODevice::ReadWrite );
}

bool Downloader::saveToDisk()
{
    checkFileLocation();
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
            qDebug() << "Request was redirected.\n";
        else
        {
            if( saveToDisk() )
                qDebug() << "Download of" << url.toEncoded().constData() << "succeeded ( saved to" << qPrintable( m_file.fileName() ) << ')';
        }
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

void Downloader::resume()
{
    if( m_reply == 0 )
    {
        QByteArray rangeHeaderValue = "bytes=" + QByteArray::number( m_downloadProgressAtPause ) + '-';
        qDebug() << m_request->rawHeader( "Range" );
        m_request->setRawHeader( "Range", rangeHeaderValue );
        qDebug() << m_request->rawHeader( "Range" );
        //m_file.open( QIODevice::ReadWrite );

        doDownload();
    }
}

void Downloader::pause()
{
    qDebug() << "pause at" << m_downloadProgress;
    if( m_reply == 0 )
    {
        return;
    }
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

void Downloader::checkFileLocation()
{
    m_file.close();
    //m_file.setFileName( m_downloadDir + saveFileName( m_url ) );
    if( QFile::exists( m_file.fileName() ) )
    {
        // already exists, don't overwrite
        int i = 0;
        m_file.setFileName( m_file.fileName() + '_' );
        while( QFile::exists( m_file.fileName() + QString::number( i ) ) )
            ++i;

        m_file.setFileName( m_file.fileName() + QString::number( i ) );
    }
    m_file.open( QIODevice::ReadWrite );
}


