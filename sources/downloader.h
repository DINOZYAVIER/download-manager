#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QThread>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader( QObject *parent = nullptr );
    ~Downloader();

    void doDownload( QUrl url );
    static QString saveFileName( const QUrl &url );
    bool saveToDisk( const QString &filename, QIODevice *data );
    static bool isHttpRedirect( QNetworkReply *reply );
    int currentDownloads () { return 1; }
    void doSetup( QThread &cThread, const QUrl &url );
    QNetworkReply* reply() { return m_currentDownloads; }
Q_SIGNALS:
    void sendName( QString name );
private Q_SLOTS:
    //void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
private:
    QNetworkAccessManager *m_manager;
    QNetworkReply * m_currentDownloads;
    QUrl m_currentUrl;
};

#endif // DOWNLOADER_H
