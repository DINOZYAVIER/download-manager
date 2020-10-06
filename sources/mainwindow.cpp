#include "precompiled.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller.h"
#include "downloader.h"
#include "downloadtablemodel.h"
#include "progressbardelegate.h"

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent )
    , m_ui( new Ui::MainWindow )
{
    m_ui->setupUi( this );
    m_downloadTableModel = new DownloadTableModel( this );
    m_controller = new Controller( *m_downloadTableModel, this );
    loadSettings();

    m_ui->downloadTableView->setModel( m_downloadTableModel );
    m_ui->downloadTableView->resizeRowsToContents();
    m_ui->downloadTableView->setItemDelegate( new ProgressBarDelegate );

    connect( m_ui->downloadButton, &QAbstractButton::clicked, this, &MainWindow::onDownload );
    connect( m_ui->aResume, &QAction::triggered, this, &MainWindow::onResume );
    connect( m_ui->aPause, &QAction::triggered, this, &MainWindow::onPause );
    connect( m_ui->aRemove, &QAction::triggered, this, &MainWindow::onRemove );
    connect( m_ui->aOpen, &QAction::triggered, this, &MainWindow::onFileOpen );
    connect( m_ui->aSetDownloadDir, &QAction::triggered, this, &MainWindow::onGetDownloadDir );
    connect( m_ui->aAbout, &QAction::triggered, this, &MainWindow::onAboutClicked );
    connect( m_ui->aAboutQt, &QAction::triggered, this, &MainWindow::onAboutQtClicked );
    connect( m_ui->aClose, &QAction::triggered, this, &MainWindow::close );

    connect( m_ui->downloadTableView, &QTableView::clicked, this, &MainWindow::onButtonsEnable );

    m_ui->aResume->setEnabled( false );
    m_ui->aPause->setEnabled( false );
    m_ui->aRemove->setEnabled( false );


    qDebug() << "Main thread ID:" << QThread::currentThreadId();
}

MainWindow::~MainWindow()
{
    delete m_ui;
    saveSettings();
}

void MainWindow::loadSettings()
{
    //here we load settings
    QSettings settings( QStandardPaths::displayName (QStandardPaths::AppDataLocation) + "/download_manager.ini",
                        QSettings::IniFormat );
    settings.beginGroup( "MainWindow" );
    m_downloadDir = settings.value( "dir", qApp->applicationDirPath() + '/' ).toString();
    m_controller->setDownloadPath( m_downloadDir );
    settings.endGroup();
}
void MainWindow::saveSettings()
{
    //here we save settings
    QSettings settings( QStandardPaths::displayName (QStandardPaths::AppDataLocation) + "/download_manager.ini",
                        QSettings::IniFormat);
    settings.beginGroup( "MainWindow" );
    settings.setValue( "dir", m_downloadDir );
    settings.endGroup();
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
        m_controller->resume( m_controller->downloader( id ) );
    }
}

void MainWindow::onPause()
{
    auto currentIndex = m_ui->downloadTableView->selectionModel()->currentIndex();
    if( currentIndex.isValid() )
    {
        int id = currentIndex.row();
        m_controller->pause( m_controller->downloader( id ) );
    }
}

void MainWindow::onRemove()
{
    m_ui->aResume->setEnabled( false );
    m_ui->aPause->setEnabled( false );
    m_ui->aRemove->setEnabled( false );
    auto currentIndex = m_ui->downloadTableView->selectionModel()->currentIndex();
    if( currentIndex.isValid() )
    {
        int id = currentIndex.row();
        m_controller->removeDownload( id );
    }
}

void MainWindow::onFileOpen()
{
    QString fileName = QFileDialog::getOpenFileName( this,
        tr( "Open text file" ), qApp->applicationDirPath(), tr( "Text Files (*.txt *.rtf)" ) );
    QFile file( fileName );
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return;
    QTextStream in( &file);

    while ( !in.atEnd() )
        m_controller->addDownload( in.readLine() );
    file.close();
}

void MainWindow::onGetDownloadDir()
{
    m_downloadDir = QFileDialog::getExistingDirectory( this,
        tr( "Open text file" ), qApp->applicationDirPath() ) + '/';
    setDownloadDir( m_downloadDir );
}

void MainWindow::setDownloadDir( QString path )
{
    m_controller->setDownloadPath( path );
}

void MainWindow::onAboutClicked()
{
    QMessageBox::about( this, "About the program",
                              "Download Manager Application \nTest assignment by Arthur Konovalov for NIX Solutions.");
}

void MainWindow::onAboutQtClicked()
{
    QMessageBox::aboutQt( this, "About Qt" );
}

void MainWindow::onButtonsEnable()
{
    m_ui->aResume->setEnabled( true );
    m_ui->aPause->setEnabled( true );
    m_ui->aRemove->setEnabled( true );
}
