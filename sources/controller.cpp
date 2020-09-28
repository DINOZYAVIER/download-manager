#include "controller.h"


Controller::Controller( DownloadTableModel* model ) :
    m_model( model )
  , m_threads( 0 )
{
}

void Controller::addDownload( QUrl url )
{
    QThread* downloadThread = new QThread();
    Downloader* download = new Downloader( nullptr, url );
    m_journal.insert( m_threads++, new QPair<QThread*, Downloader*>( downloadThread, download ) );
    download->moveToThread( downloadThread );
    connect( downloadThread, &QThread::started, download, &Downloader::doDownload );
    connect( download, &Downloader::sendProgress, this, &Controller::onDisplay );
    connect( downloadThread, &QThread::finished, download, &QObject::deleteLater );
    downloadThread->start();
    qDebug() << "Thread ID: " << downloadThread->currentThreadId();
}

Controller::~Controller()
{
  /*  for( int i = 0; i < m_journal.size(); ++i )
    {

        m_journal.value( i )->first->quit();
        m_journal.value( i )->first->wait();
        delete m_journal.value( i )->second;
    }*/
}

void Controller::onDisplay()
{
    for( int i = 0; i < m_journal.size(); ++i )
        m_model->setDataList( m_journal.value( i )->second->dataList(), i );
}
