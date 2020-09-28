#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QVariant>

class QThread;
class Downloader;
class DownloadTableModel;

struct JournalItem
{
    JournalItem() : downloader( nullptr ), thread( nullptr ) {}
    JournalItem( Downloader* d, QThread* t ) : downloader( d ), thread( t ) {}
    Downloader* downloader;
    QThread* thread;
};

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller( DownloadTableModel& model, QObject* parent = nullptr );
    ~Controller();
    void addDownload( QUrl url );

private:
    DownloadTableModel& m_model;
    QList<JournalItem>  m_journal;

    int findDownloader( QObject* downloader );
    void removeItem( int index );
    void releaseItem( JournalItem& item );

    Q_SLOT void displayData( QVariantList data );
    Q_SLOT void freeResources();
};

#endif // CONTROLLER_H
