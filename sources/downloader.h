#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "downloadtablemodel.h"

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader( QObject *parent = nullptr );
    ~Downloader();

    void doDownload( const QUrl &url );
    static QString saveFileName( const QUrl &url );
    bool saveToDisk( const QString &filename, QIODevice *data );
    static bool isHttpRedirect( QNetworkReply *reply );
    int currentDownloads () { return m_currentDownloads.size(); }
    DownloadTableModel* downloadTableModel() { return m_downloadTableModel; }

private Q_SLOTS:
    //void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
private:
    QNetworkAccessManager *m_manager;
    QVector<QNetworkReply *> m_currentDownloads;
    DownloadTableModel *m_downloadTableModel;
};

#endif // DOWNLOADER_H
