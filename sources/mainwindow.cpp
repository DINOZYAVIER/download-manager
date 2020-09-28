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
