#include "precompiled.h"
#include "downloader.h"

Downloader::Downloader( QObject* parent, const QUrl& url, QVariant id ) :
    QObject( parent )
  , m_currentUrl( url )
{
    m_dataList.append( id );
    m_manager = new QNetworkAccessManager();
    connect( m_manager->get( QNetworkRequest( m_currentUrl ) ), &QNetworkReply::downloadProgress, this, &Downloader::onProcess );
    connect( m_manager, &QNetworkAccessManager::finished, this, &Downloader::downloadFinished );
}

Downloader::~Downloader()
{
    delete m_manager;
}

void Downloader::doDownload()
{
    m_dataList.append( saveFileName( m_currentUrl ) );
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
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
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
    qDebug() << "All downloads finished";
}

void Downloader::onProcess( qint64 bytesReceived, qint64 bytesTotal )
{
    if( m_dataList.size() < 2 )
        m_dataList.append( QString::number( bytesTotal / 1048576 ) + "MB" );
    if( m_dataList.size() < 3 )
        m_dataList.append( QString::number( bytesReceived * 1000 / m_elapsedTimer->elapsed() / 1024 ) + "KB/sec" );
    else
        m_dataList.replace( 2, QString::number( bytesReceived * 1000 / m_elapsedTimer->elapsed() / 1024 ) + "KB/sec" );
    if( m_dataList.size() < 4 )
         m_dataList.append( QString::number( bytesReceived / 1048576 ) + "MB / " + QString::number( bytesTotal / 1048576 ) + "MB" );
    else
        m_dataList.replace( 3, QString::number( bytesReceived / 1048576 ) + "MB / " + QString::number( bytesTotal / 1048576 ) + "MB" );
    //Q_EMIT sendProgress( &m_dataList );
}

Controller::Controller( DownloadTableModel* model ) :
    m_model( model )
{
}

void Controller::addDownload( QUrl url )
{
    Downloader* download = new Downloader( nullptr, url );
    m_downloadThread.append( new QThread() );
    download->moveToThread( m_downloadThread.last() );
    connect( m_downloadThread.last(), &QThread::started, download, &Downloader::doDownload );
    connect( download, &Downloader::sendProgress, this, &Controller::onDisplay );
    connect( m_downloadThread.last(), &QThread::finished, download, &QObject::deleteLater );
    m_downloadThread.last()->start();
}

Controller::~Controller()
{
    m_downloadThread.last()->quit();
    m_downloadThread.last()->wait();
}

void Controller::onDisplay( QVariantList* list )
{

}


