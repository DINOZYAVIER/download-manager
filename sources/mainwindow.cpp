#include "precompiled.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
    , m_ui( new Ui::MainWindow )

{
    m_ui->setupUi( this );
    m_downloadTableModel = new DownloadTableModel();
    m_downloader = new Downloader();
    m_ui->downloadTableView->setModel( m_downloadTableModel );

    connect( m_ui->downloadButton, &QAbstractButton::clicked, this, &MainWindow::onDownload );
    connect( m_downloader, &Downloader::sendName, this, &MainWindow::onSendName );
    //connect( )
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_downloadTableModel;

/*
    while( !m_threads.isEmpty() )
    {
        if( m_threads.last()->isFinished() )
            delete m_threads.takeLast();
    }
*/
}

void MainWindow::onDownload()
{
    m_threads.append( new QThread() );
    QUrl url = QUrl::fromEncoded( m_ui->PathEdit->text().toLocal8Bit() );
    m_downloader->doSetup( *m_threads.last(), url );
}

void MainWindow::onSendName( QString name )
{
    Q_EMIT m_downloadTableModel->recieveName( name );
}
