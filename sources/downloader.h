#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>
#include <QCoreApplication>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);
    ~Downloader();

    void doDownload( const QUrl &url );
    static QString saveFileName( const QUrl &url );
    bool saveToDisk( const QString &filename, QIODevice *data );
    static bool isHttpRedirect( QNetworkReply *reply );

public Q_SLOTS:
    //void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
private:
    QNetworkAccessManager *m_manager;
    QVector<QNetworkReply *> currentDownloads;
};

#endif // DOWNLOADER_H
