#include "precompiled.h"
#include "downloader.h"

Downloader::Downloader( const QUrl& url ) :
    QObject( nullptr ),
    m_url( url ),
    m_reply( nullptr ),
    m_elapsedTimer( nullptr ),
    m_thread( nullptr )
{
    m_file.setFileName( saveFileName( url ) );
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
}

void Downloader::doDownload()
{
    qDebug() << "Starting download" << m_url;
    qDebug() << "Thread ID" << QThread::currentThreadId();

    auto* manager = new QNetworkAccessManager( this );
    m_reply = manager->get( QNetworkRequest( m_url ) );

    m_elapsedTimer = new QElapsedTimer();
    m_elapsedTimer->start();

    connect( m_reply, &QNetworkReply::downloadProgress, this, &Downloader::onProgress );
    connect( m_reply, &QNetworkReply::finished, this, &Downloader::onFinished );
    connect( m_reply, &QNetworkReply::errorOccurred, this, &Downloader::onError );
    connect( m_reply, &QNetworkReply::sslErrors, this, &Downloader::onSSLError );
}

bool Downloader::saveToDisk( QIODevice* data )
{
    QMutex fileMutex;
    fileMutex.lock();
    if( !m_file.open( QIODevice::WriteOnly ) )
    {
        qDebug() << "Could not open" << qPrintable( m_file.fileName() ) << "for writing" << qPrintable( m_file.errorString() );
        fileMutex.unlock();
        return false;
    }

    if( QFile::exists( m_file.fileName() ) )
    {
        m_file.close();
        // already exists, don't overwrite
        int i = 0;
        m_file.setFileName( m_file.fileName() + '_' );
        while( QFile::exists( m_file.fileName() + QString::number( i ) ) )
            ++i;

        m_file.setFileName( m_file.fileName() + QString::number( i ) );
        if( !m_file.open( QIODevice::WriteOnly ) )
        {
            qDebug() << "Could not open" << qPrintable( m_file.fileName() ) << "for writing" << qPrintable( m_file.errorString() );
            fileMutex.unlock();
            return false;
        }
    }

    m_file.write( data->readAll() );
    m_file.close();
    fileMutex.unlock();


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
            if( saveToDisk( m_reply ) )
                qDebug() << "Download of" << url.toEncoded().constData() << "succeeded ( saved to" << qPrintable( m_file.fileName() ) << ')';
        }
    }

    Q_EMIT finished();
}

void Downloader::onProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    QVariantList data;
    data.append( m_file.fileName() );
    data.append( QString::number( bytesTotal / 1048576 ) + "MB" );
    data.append( QString::number( bytesReceived * 1000 / m_elapsedTimer->elapsed() / 1024 ) + "KB/sec" );
    data.append( QString::number( bytesReceived / 1048576 ) + "MB / " + QString::number( bytesTotal / 1048576 ) + "MB" );
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

bool Downloader::resume()
{

}

bool Downloader::pause()
{
    if( m_reply == 0 )
    {
        return false;
    }
    disconnect( m_reply, &QNetworkReply::downloadProgress, this, &Downloader::onProgress );
    disconnect( m_reply, &QNetworkReply::finished, this, &Downloader::onFinished );
    disconnect( m_reply, &QNetworkReply::errorOccurred, this, &Downloader::onError );
    disconnect( m_reply, &QNetworkReply::sslErrors, this, &Downloader::onSSLError );

    m_reply->abort();
    if( saveToDisk( m_reply ) )
        qDebug() << "Download of" << m_url.toEncoded().constData() << "has been paused and temporary file is" << qPrintable( m_file.fileName() );
    m_reply = 0;
    return true;
}


