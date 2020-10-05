#include "precompiled.h"
#include "controller.h"
#include "downloader.h"
#include "downloadtablemodel.h"

#ifndef INVALID_INDEX
#define INVALID_INDEX -1
#endif

Controller::Controller( DownloadTableModel& model, QObject* parent) :
    QObject( parent ),
    m_model( model )
{
    m_downloadPath = qApp->applicationDirPath() + '/';
}

void Controller::addDownload( QUrl url )
{
    QThread* downloadThread = new QThread();
    Downloader* downloader = new Downloader( url, m_downloadPath );
    downloader->moveToThread( downloadThread );
    m_journal.append( JournalItem( downloader, downloadThread ) );

    connect( downloadThread, &QThread::started, downloader, &Downloader::doDownload );
    connect( this, &Controller::resumeSignal, downloader, &Downloader::resume );
    connect( this, &Controller::pauseSignal, downloader, &Downloader::pause );
    connect( downloader, &Downloader::progressChanged, this, &Controller::displayData );
    connect( downloader, &Downloader::finished, this, &Controller::freeResources );
    connect( downloadThread, &QThread::finished, downloader, &Downloader::deleteLater );
    Q_EMIT downloader->progressChanged( QVariantList() );
    downloadThread->start();
}

Controller::~Controller()
{
    for( auto& item : m_journal )
        releaseItem( item );
}

int Controller::findDownloader( QObject* downloader )
{
    for( int i = 0; i < m_journal.size(); ++i )
    {
        if( m_journal.at( i ).downloader == downloader )
            return i;
    }
    return INVALID_INDEX;
}

void Controller::removeItem( int index )
{
    if( index >= m_journal.size() )
        return;
    releaseItem( m_journal[ index ] );
    m_journal.removeAt( index );
    // TODO: Remove row from table
}

void Controller::releaseItem( JournalItem& item )
{
    if( item.downloader )
        disconnect( item.downloader, nullptr, this, nullptr );
    item.downloader = nullptr;
    if( item.thread && item.thread->isRunning() )
    {
        item.thread->quit();
        item.thread->wait();
    }
    delete item.thread;
    item.thread = nullptr;
}

void Controller::displayData( QVariantList data )
{
    auto* downloader = sender();
    if( downloader )
    {
        int index = findDownloader( downloader );
        if( index != INVALID_INDEX )
            m_model.setDataList( data, index );
    }
}

void Controller::freeResources()
{
    auto* downloader = sender();
    if( downloader )
    {
        int index = findDownloader( downloader );
        if( index != INVALID_INDEX )
            releaseItem( m_journal[ index ] );
    }
}

void Controller::removeDownload( int id )
{
    releaseItem( m_journal[ id ] );
    removeItem( id );
    m_model.removeDownload( id );
}

void Controller::resume( Downloader* downloader )
{
    Q_EMIT resumeSignal( downloader );
}

void Controller::pause( Downloader* downloader )
{
    Q_EMIT pauseSignal( downloader );
}
