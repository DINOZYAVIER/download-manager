#include "precompiled.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
    , m_ui( new Ui::MainWindow )

{
    m_ui->setupUi( this );
    m_downloadTableModel = new DownloadTableModel();
    m_ui->downloadTableView->setModel( m_downloadTableModel );

    connect( m_ui->downloadButton, &QAbstractButton::clicked, this, &MainWindow::onDownload );
    //connect( m_downloader, &Downloader::sendName, this, &MainWindow::onSendName );
    //connect( )
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_downloadTableModel;
}

void MainWindow::onDownload()
{
    m_threads.append( new QThread() );
    QUrl url = QUrl::fromEncoded( m_ui->PathEdit->text().toLocal8Bit() );
    m_downloadTableModel->addDownload( url );
    //m_downloader->doSetup( *m_threads.last(), url );
}
