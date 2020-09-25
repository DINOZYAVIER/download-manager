#include "precompiled.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent )
    , m_ui( new Ui::MainWindow )

{
    m_ui->setupUi( this );
    m_downloadTableModel = new DownloadTableModel();
    m_controller = new Controller( m_downloadTableModel );

    m_ui->downloadTableView->setModel( m_downloadTableModel );
    m_ui->downloadTableView->resizeRowsToContents();

    connect( m_ui->downloadButton, &QAbstractButton::clicked, this, &MainWindow::onDownload );
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_controller;
    delete m_downloadTableModel;
}

void MainWindow::onDownload()
{
    QUrl url = QUrl::fromEncoded( m_ui->PathEdit->text().toLocal8Bit() );
    m_controller->addDownload( url );
    //m_downloadTableModel->addDownload( url );
}
