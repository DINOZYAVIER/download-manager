#include "precompiled.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller.h"
#include "downloadtablemodel.h"

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent )
    , m_ui( new Ui::MainWindow )

{
    m_ui->setupUi( this );
    m_downloadTableModel = new DownloadTableModel( this );
    m_controller = new Controller( *m_downloadTableModel, this );

    m_ui->downloadTableView->setModel( m_downloadTableModel );
    m_ui->downloadTableView->resizeRowsToContents();

    connect( m_ui->downloadButton, &QAbstractButton::clicked, this, &MainWindow::onDownload );
    connect( m_ui->aResume, &QAction::triggered, this, &MainWindow::onResume );
    connect( m_ui->aPause, &QAction::triggered, this, &MainWindow::onPause );
    connect( m_ui->aStop, &QAction::triggered, this, &MainWindow::onStop );

    qDebug() << "Main thread ID:" << QThread::currentThreadId();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::onDownload()
{
    QUrl url = QUrl::fromEncoded( m_ui->PathEdit->text().toLocal8Bit() );
    m_controller->addDownload( url );
}

void MainWindow::onResume()
{
    auto currentIndex = m_ui->downloadTableView->selectionModel()->currentIndex();
    if( currentIndex.isValid() )
    {
        int id = currentIndex.row();
        m_controller->resume( id );
    }
}

void MainWindow::onPause()
{
    auto currentIndex = m_ui->downloadTableView->selectionModel()->currentIndex();
    if( currentIndex.isValid() )
    {
        int id = currentIndex.row();
        m_controller->pause( id );
    }
}

void MainWindow::onStop()
{
    auto currentIndex = m_ui->downloadTableView->selectionModel()->currentIndex();
    if( currentIndex.isValid() )
    {
        int id = currentIndex.row();
        m_controller->stop( id );
    }
}
