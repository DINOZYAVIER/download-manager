#include "downloader.h"

Downloader::Downloader( QObject *parent ) :
    QObject( parent )
{
    m_manager = new QNetworkAccessManager();
    connect(m_manager, &QNetworkAccessManager::finished, this, &Downloader::onResult);
}

void Downloader::data( QString data )
{
    QUrl url( data );
    QNetworkRequest request;
    request.setUrl( url );
    request.setAttribute( QNetworkRequest::FollowRedirectsAttribute, true );
    request.setMaximumRedirectsAllowed(3);
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
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        QFile *file = new QFile( QCoreApplication::applicationDirPath() + "/file");
        if( file->open( QFile::WriteOnly ) )
        {
            file->write( reply->readAll() );
            file->flush();
            file->close();
            qDebug() << "Downloading is completed";
            Q_EMIT onReady();
        }
        delete file;
    }
}
