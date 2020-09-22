#ifndef DOWNLOADTABLEMODEL_H
#define DOWNLOADTABLEMODEL_H

#include <QAbstractTableModel>
#include <QUrl>
#include "downloader.h"

#define NUMBER_OF_COLUMNS 4

class DownloadTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DownloadTableModel(QObject *parent = nullptr );
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
Q_SIGNALS:
    void newDownloadSignal( QUrl url );
private Q_SLOTS:
    void onNewDownload( QUrl url );
private:
    Downloader *m_downloader;
    int m_rows;
};

#endif // DOWNLOADTABLEMODEL_H
