#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QCoreApplication>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);

Q_SIGNALS:
    void onReady();

public slots:
    void data( QString data );
    void onResult( QNetworkReply *reply );

private:
    QNetworkAccessManager *m_manager;
};

#endif // DOWNLOADER_H
