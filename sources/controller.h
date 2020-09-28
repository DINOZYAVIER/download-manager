#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include "downloader.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller( DownloadTableModel* model );
    ~Controller();
    void addDownload( QUrl url );
private Q_SLOTS:
    void onDisplay();
private:
    DownloadTableModel* m_model;
    QMap<int, QPair<QThread*, Downloader*>*> m_journal;
    int m_threads;
};

#endif // CONTROLLER_H
