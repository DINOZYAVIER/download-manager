#include "precompiled.h"
#include "downloader.h"

Downloader::Downloader( QObject* parent, const QUrl& url, QVariant id ) :
    QObject( parent )
  , m_currentUrl( url )
{
    m_dataList.append( saveFileName( m_currentUrl ) );
    m_dataList.append( "" );
    m_dataList.append( "" );
    m_dataList.append( "" );
}

Downloader::~Downloader()
{
    delete m_elapsedTimer;
    delete m_manager;
    qDebug() << "Thread with ID " << QThread::currentThreadId() << "closed";
    QThread::currentThread()->quit();
}

void Downloader::doDownload()
{
    qDebug() << "Download started. Current thread ID:" << QThread::currentThreadId();
    m_manager = new QNetworkAccessManager();
    connect( m_manager->get( QNetworkRequest( m_currentUrl ) ), &QNetworkReply::downloadProgress, this, &Downloader::onProcess );
    connect( m_manager, &QNetworkAccessManager::finished, this, &Downloader::downloadFinished );
    QNetworkRequest request( m_currentUrl );

#if QT_CONFIG(ssl)
    connect( m_manager->get( request ), &QNetworkReply::sslErrors,
            this, &Downloader::sslErrors );
#endif
    m_elapsedTimer = new QElapsedTimer();
    m_elapsedTimer->start();
}

QString Downloader::saveFileName( const QUrl& url )
{
    QString path = url.path();
    QString basename = QFileInfo( path ).fileName();

    if( basename.isEmpty() )
        basename = "download";

    if( QFile::exists( basename ) )
    {
        // already exists, don't overwrite
        int i = 0;
        basename += '_';
        while( QFile::exists( basename + QString::number( i ) ) )
            ++i;

        basename += QString::number( i );
    }

    return basename;
}

bool Downloader::saveToDisk( const QString& filename, QIODevice* data )
{
    QFile file( filename );
    if( !file.open( QIODevice::WriteOnly ) )
    {
        qDebug() << "Could not open" << qPrintable( filename ) << "for writing" << qPrintable( file.errorString() );
        return false;
    }

    file.write( data->readAll() );
    file.close();

    return true;
}

bool Downloader::isHttpRedirect( QNetworkReply* reply )
{
    int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void Downloader::sslErrors( const QList<QSslError>& sslErrors )
{
#if QT_CONFIG( ssl )
    for( const QSslError &error : sslErrors )
        fprintf( stderr, "SSL error: %s\n", qPrintable( error.errorString() ) );
#else
    Q_UNUSED(sslErrors);
#endif
}

void Downloader::downloadFinished( QNetworkReply* reply )
{
    QUrl url = reply->url();
    if( reply->error() )
        qDebug() << "Download of %s failed:" << url.toEncoded().constData() << qPrintable( reply->errorString() );
    else
    {
        if( isHttpRedirect( reply ) )
            qDebug() << "Request was redirected.\n" << stderr;        
        else
        {
            QString filename = saveFileName( url );
            if( saveToDisk( filename, reply ) )
                qDebug() << "Download of" << url.toEncoded().constData() << "succeeded ( saved to" << qPrintable( filename ) << ')';
        }
    }

    reply->deleteLater();

    // download finished
    qDebug() << "Download finished. Thread ID: " << QThread::currentThreadId();
}

void Downloader::onProcess( qint64 bytesReceived, qint64 bytesTotal )
{
    m_dataList.replace( 1, QString::number( bytesTotal / 1048576 ) + "MB" );
    m_dataList.replace( 2, QString::number( bytesReceived * 1000 / m_elapsedTimer->elapsed() / 1024 ) + "KB/sec" );
    m_dataList.replace( 3, QString::number( bytesReceived / 1048576 ) + "MB / " + QString::number( bytesTotal / 1048576 ) + "MB" );
    Q_EMIT sendProgress();
}



