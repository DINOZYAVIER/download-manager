#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkReply>

class QElapsedTimer;

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader( const QUrl& url );
    ~Downloader();

    Q_SLOT void doDownload();

    bool saveToDisk( const QString& filename, QIODevice* data );
    static bool isHttpRedirect( QNetworkReply* reply );

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
    QString        m_file;
    QNetworkReply* m_reply;
    QElapsedTimer* m_elapsedTimer;
    QThread*       m_thread;

    static QString saveFileName( const QUrl& url );
};

#endif // DOWNLOADER_H
