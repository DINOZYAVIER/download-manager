#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QFile>

class QElapsedTimer;

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader( const QUrl& url, QString path );
    ~Downloader();

    QNetworkReply* reply() { return m_reply; }

    Q_SLOT void doDownload();

    bool saveToDisk( QIODevice* data );
    static bool isHttpRedirect( QNetworkReply* reply );
    void resume();
    void pause();
    void checkFileLocation();

Q_SIGNALS:
    void progressChanged( QVariantList data );
    void finished();

private Q_SLOTS:
    void onProgress( qint64 bytesReceived, qint64 bytesTotal );
    void onFinished();
    void onError( QNetworkReply::NetworkError code );
    void onSSLError( const QList<QSslError>& errors );

private:
    QUrl           m_url;
    QFile          m_file;
    QNetworkReply* m_reply;
    QElapsedTimer* m_elapsedTimer;
    QThread*       m_thread;
    QNetworkAccessManager* m_manager;
    QNetworkRequest* m_request;
    QString        m_downloadDir;

    static QString saveFileName( const QUrl& url );
};

#endif // DOWNLOADER_H
