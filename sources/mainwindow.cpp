#include "mainwindow.h"
#include "downloader.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    Downloader *downloader = new Downloader();
    downloader->data( m_ui->PathEdit->text() );
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

