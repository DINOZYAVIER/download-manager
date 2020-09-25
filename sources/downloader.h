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
    explicit Downloader( QObject* parent = nullptr, const QUrl& url = QUrl(), QVariant id = 0 );
    ~Downloader();

    void doDownload();
    static QString saveFileName( const QUrl& url );
    bool saveToDisk( const QString& filename, QIODevice* data );
    static bool isHttpRedirect( QNetworkReply* reply );
    QVariantList* dataList() { return &m_dataList; }
    QNetworkReply* reply() { return m_currentReply; }
Q_SIGNALS:
    void sendProgress( QVariantList* list );
private Q_SLOTS:
    void onProcess( qint64 bytesReceived, qint64 bytesTotal );
    void downloadFinished( QNetworkReply* reply );
    void sslErrors( const QList<QSslError>& errors );
private:
    QNetworkAccessManager* m_manager;
    QVariantList m_dataList;
    QNetworkReply* m_currentReply;
    QUrl m_currentUrl;
    QElapsedTimer* m_elapsedTimer;
};

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller( DownloadTableModel* model );
    ~Controller();
    void addDownload( QUrl url );
Q_SIGNALS:
    void sendProgress( int i, QVariantList* );
private Q_SLOTS:
    void onDisplay( QVariantList* list );
private:
    DownloadTableModel* m_model;
    QVector<QThread*> m_downloadThread;
};

#endif // DOWNLOADER_H
