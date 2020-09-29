#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui { class MainWindow; }
class Controller;
class DownloadTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* parent = nullptr );
    ~MainWindow();

private Q_SLOTS:
    void onDownload();
    void onResume();
    void onPause();
    void onStop();

private:
    Ui::MainWindow*     m_ui;
    DownloadTableModel* m_downloadTableModel;
    Controller*         m_controller;
};
#endif // MAINWINDOW_H
