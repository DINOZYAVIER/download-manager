#include "precompiled.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent )
    , m_ui( new Ui::MainWindow )
{
    m_ui->setupUi( this );
    m_downloader = new Downloader();
    connect( m_ui->downloadButton, &QAbstractButton::clicked, this, &MainWindow::onDownload );

    m_downloadModel = new DownloadTableModel( m_downloader );
    m_ui->tableView->setModel( m_downloadModel );
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_downloader;
}

void MainWindow::onDownload()
{
    QUrl url = QUrl::fromEncoded(m_ui->PathEdit->text().toLocal8Bit());
    m_downloader->doDownload( url );
}
