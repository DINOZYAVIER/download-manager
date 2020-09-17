#include "downloader.h"

Downloader::Downloader( QObject *parent ) :
    QObject( parent )
{
    m_manager = new QNetworkAccessManager();
    connect(m_manager, &QNetworkAccessManager::finished, this, &Downloader::onResult);
}

void Downloader::data( QString data )
{
    QUrl url( "http://www.mtbank.by/currxml.php" );
    QNetworkRequest request;
    request.setUrl( url) ;
    m_manager->get(request);
}

void Downloader::onResult( QNetworkReply *reply )
{
    if( reply->error() )
    {
        qDebug() << "ERROR";
        qDebug() << reply->errorString();
    }
    else
    {
        QFile *file = new QFile( QCoreApplication::applicationDirPath() + "/file");
        if( file->open( QFile::WriteOnly ) )
        {
            file->write(reply->readAll());
            file->close();
            qDebug() << "Downloading is completed";
            Q_EMIT onReady();
        }
    }
}
