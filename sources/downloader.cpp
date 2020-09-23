#include "precompiled.h"
#include "downloader.h"

Downloader::Downloader( QObject *parent ) :
    QObject( parent )
{
    m_manager = new QNetworkAccessManager();
    connect( m_manager, &QNetworkAccessManager::finished, this, &Downloader::downloadFinished );
}

Downloader::~Downloader()
{
    delete m_manager;
}

void Downloader::doDownload()
{
    qDebug() << "Thread started";
    QNetworkRequest request( m_currentUrl );
    QNetworkReply *reply = m_manager->get( request );

#if QT_CONFIG(ssl)
    connect( reply, &QNetworkReply::sslErrors,
            this, &Downloader::sslErrors );
#endif

    m_currentDownloads.append( reply );
}

QString Downloader::saveFileName( const QUrl &url )
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

bool Downloader::saveToDisk( const QString &filename, QIODevice *data )
{
    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        qDebug() << "Could not open" << qPrintable( filename ) << "for writing" << qPrintable( file.errorString() );
        return false;
    }

    file.write( data->readAll() );
    file.close();

    return true;
}

bool Downloader::isHttpRedirect( QNetworkReply *reply )
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void Downloader::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG( ssl )
    for ( const QSslError &error : sslErrors )
        fprintf( stderr, "SSL error: %s\n", qPrintable( error.errorString() ) );
#else
    Q_UNUSED(sslErrors);
#endif
}

void Downloader::downloadFinished( QNetworkReply *reply )
{
    qDebug() << "Thread finished";
    QUrl url = reply->url();
    if ( reply->error() )
        qDebug() << "Download of %s failed:" << url.toEncoded().constData() << qPrintable( reply->errorString() );
    else
    {
        if ( isHttpRedirect(reply) )
            qDebug() << "Request was redirected.\n" << stderr;        
        else
        {
            QString filename = saveFileName(url);
            if ( saveToDisk(filename, reply) )
                qDebug() << "Download of" << url.toEncoded().constData() << "succeeded ( saved to" << qPrintable( filename ) << ')';
        }
    }

    m_currentDownloads.removeAll( reply );
    reply->deleteLater();

    if ( m_currentDownloads.isEmpty() )
    {
        // all downloads finished
        qDebug() << "All downloads finished";
        //QCoreApplication::instance()->quit();
    }
}

void Downloader::doSetup( QThread &cThread, const QUrl &url )
{
    connect( &cThread, &QThread::started, this, &Downloader::doDownload );
    m_currentUrl = url;
    cThread.start();
}

