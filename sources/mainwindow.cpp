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
    qDebug() << "onResume";
    m_controller->resume( 0 );
}

void MainWindow::onPause()
{
    qDebug() << "onPause";
    m_controller->pause( 0 );
}

void MainWindow::onStop()
{
    qDebug() << "onStop";
    m_controller->stop( 0 );
}
