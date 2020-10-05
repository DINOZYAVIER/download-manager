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
    void removeDownload( int id );
    void setDownloadPath( QString path ) { m_downloadPath = path; }
    Downloader* downloader( int id ) { return m_journal[ id ].downloader; }
    void resume( Downloader* downloader );
    void pause( Downloader* downloader );
private:
    DownloadTableModel& m_model;
    QList<JournalItem>  m_journal;
    QString m_downloadPath;

    int findDownloader( QObject* downloader );
    void removeItem( int index );
    void releaseItem( JournalItem& item );

    Q_SIGNAL void resumeSignal( Downloader* downloader );
    Q_SIGNAL void pauseSignal( Downloader* downloader );

    Q_SLOT void displayData( QVariantList data );
    Q_SLOT void freeResources();
    Q_SLOT void onErrorOccured();
};

#endif // CONTROLLER_H
