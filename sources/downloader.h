#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QFile>

class QElapsedTimer;

const uint BYTES_IN_KILOBYTES = 1000;
const uint BYTES_IN_MEGABYTES = 1000000;
const uint MILISECONS_IN_SECONDS = 1000;

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader( const QUrl& url, QString path );
    ~Downloader();

    QNetworkReply* reply() { return m_reply; }

    Q_SLOT void doDownload();
    Q_SLOT void resume( Downloader* downloader );
    Q_SLOT void pause( Downloader* downloader );

    bool saveToDisk();
    static bool isHttpRedirect( QNetworkReply* reply );
Q_SIGNALS:
    void progressChanged( QVariantList data );
    void finished();
    void protocolErrorOccured();

private Q_SLOTS:
    void onProgress( qint64 bytesReceived, qint64 bytesTotal );
    void onFinished();
    void onError( QNetworkReply::NetworkError code );
    void onSSLError( const QList<QSslError>& errors );


private:
    QUrl                   m_url;
    QFile                  m_file;
    QNetworkReply*         m_reply;
    QNetworkAccessManager* m_manager;
    QNetworkRequest*       m_request;
    QElapsedTimer*         m_elapsedTimer;
    QThread*               m_thread;
    QString                m_downloadDir;
    qint64                 m_downloadProgress;
    qint64                 m_downloadProgressAtPause;

    static QString saveFileName( const QUrl& url );
};

#endif // DOWNLOADER_H
