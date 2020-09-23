#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QThread>
#include "downloadtablemodel.h"

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader( QObject *parent = nullptr );
    ~Downloader();

    void doDownload();
    static QString saveFileName( const QUrl &url );
    bool saveToDisk( const QString &filename, QIODevice *data );
    static bool isHttpRedirect( QNetworkReply *reply );
    int currentDownloads () { return m_currentDownloads.size(); }
    void doSetup( QThread &cThread, const QUrl &url );
private Q_SLOTS:
    //void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
private:
    QNetworkAccessManager *m_manager;
    QVector<QNetworkReply *> m_currentDownloads;
    QVector<QThread> m_threads;
    QUrl m_currentUrl;
};

#endif // DOWNLOADER_H
