#include "precompiled.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

QThread myThread;

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
    , m_ui( new Ui::MainWindow )

{
    m_ui->setupUi( this );
    m_downloadTableModel = new DownloadTableModel();
    m_downloader = new Downloader();
    m_ui->tableView->setModel( m_downloadTableModel );

    connect( m_ui->downloadButton, &QAbstractButton::clicked, this, &MainWindow::onDownload );
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_downloadTableModel;
}

void MainWindow::onDownload()
{
    QUrl url = QUrl::fromEncoded(m_ui->PathEdit->text().toLocal8Bit());
    m_downloader->doSetup( myThread, url );
}
