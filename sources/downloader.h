#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QThread>
#include <QTime>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader( QObject* parent = nullptr );
    ~Downloader();

    void doDownload( QUrl url );
    static QString saveFileName( const QUrl& url );
    bool saveToDisk( const QString& filename, QIODevice* data );
    static bool isHttpRedirect( QNetworkReply* reply );
    int currentDownloads () { return 1; }
    QNetworkReply* reply() { return m_currentReply; }
    QElapsedTimer* elapsedTimer() { return m_elapsedTimer; }
Q_SIGNALS:
    void sendName( QString name );
    void setProgress();
private Q_SLOTS:
    void downloadFinished( QNetworkReply* reply );
    void sslErrors( const QList<QSslError>& errors );
private:
    QNetworkAccessManager* m_manager;
    QNetworkReply* m_currentReply;
    QElapsedTimer* m_elapsedTimer;
};

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller();
    ~Controller();
private Q_SLOTS:
    void onProgress( qint64 bytesReceived, qint64 bytesTotal );
private:
    QThread downloadThread;

};

#endif // DOWNLOADER_H
