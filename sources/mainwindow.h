#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "downloader.h"
#include <downloadtablemodel.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = nullptr );
    ~MainWindow();

private Q_SLOTS:
    void onDownload();

private:
    Ui::MainWindow *m_ui;
    Downloader *m_downloader;
    DownloadTableModel* m_downloadModel;
};
#endif // MAINWINDOW_H
