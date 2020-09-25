#include "precompiled.h"
#include "downloader.h"

Downloader::Downloader( QObject* parent ) :
    QObject( parent )
{
    m_manager = new QNetworkAccessManager();
    connect( m_manager, &QNetworkAccessManager::finished, this, &Downloader::downloadFinished );
}

Downloader::~Downloader()
{
    delete m_manager;
}

void Downloader::doDownload( QUrl url)
{
    Q_EMIT sendName( saveFileName( url ) );
    QNetworkRequest request( url );
     m_currentReply = m_manager->get( request );
    Q_EMIT setProgress();

#if QT_CONFIG(ssl)
    connect( m_currentReply, &QNetworkReply::sslErrors,
            this, &Downloader::sslErrors );
#endif
    Q_EMIT setProgress();
    m_elapsedTimer = new QElapsedTimer();
    m_elapsedTimer->start();
}

QString Downloader::saveFileName( const QUrl& url )
{
    QString path = url.path();
    QString basename = QFileInfo( path ).fileName();

    if ( basename.isEmpty() )
        basename = "download";

    if ( QFile::exists( basename ) )
    {
        // already exists, don't overwrite
        int i = 0;
        basename += '_';
        while ( QFile::exists( basename + QString::number( i ) ) )
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
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void Downloader::sslErrors(const QList<QSslError>& sslErrors)
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
    qDebug() << "All downloads finished";
}

Controller::Controller( int id ) :
    m_id( id )
{
    m_dataList = new QVariantList();
    Downloader* download = new Downloader();
    download->moveToThread( &m_downloadThread );
    connect( &m_downloadThread, &QThread::finished, download, &QObject::deleteLater );
    connect( download->reply(), &QNetworkReply::downloadProgress, this, &Controller::onProgress );
    m_downloadThread.start();
    m_elapsedTimer = new QElapsedTimer();
    m_elapsedTimer->start();
}

Controller::~Controller()
{
    delete m_elapsedTimer;
    m_downloadThread.quit();
    m_downloadThread.wait();
}

void Controller::onProgress( qint64 bytesReceived, qint64 bytesTotal )
{
    if( m_dataList->size() < 2 )
        m_dataList->append( QString::number( bytesTotal / 1048576 ) + "MB" );
    if( m_dataList->size() < 3 )
        m_dataList->append( QString::number( bytesReceived * 1000 / m_elapsedTimer->elapsed() / 1024 ) + "KB/sec" );
    else
        m_dataList->replace( 2, QString::number( bytesReceived * 1000 / m_elapsedTimer->elapsed() / 1024 ) + "KB/sec" );
    if( m_dataList->size() < 4 )
         m_dataList->append( QString::number( bytesReceived / 1048576 ) + "MB / " + QString::number( bytesTotal / 1048576 ) + "MB" );
    else
        m_dataList->replace( 3, QString::number( bytesReceived / 1048576 ) + "MB / " + QString::number( bytesTotal / 1048576 ) + "MB" );
    Q_EMIT sendProgress( m_id, m_dataList );
   // Q_EMIT dataChanged( index( 0, 0 ), index( m_rows - 1, NUMBER_OF_COLUMNS - 1 ) );
}


