#ifndef DOWNLOADTABLEMODEL_H
#define DOWNLOADTABLEMODEL_H

#include <QAbstractTableModel>
#include <QUrl>
#include <downloader.h>

#define NUMBER_OF_COLUMNS 4

class DownloadTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DownloadTableModel( QObject *parent = nullptr );
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    void addDownload( QUrl url );
    void addDownloadSize( QVariant data );
    void addDownloadSpeed( QVariant data );
    //void addDownloadProgress( QVariant data );
Q_SIGNALS:

private Q_SLOTS:
    void onProgressReceived( qint64 bytesReceived, qint64 bytesTotal );
private:
    int m_rows;
    QVector<QVariantList*> m_data;
    Downloader* m_downloader;
};

#endif // DOWNLOADTABLEMODEL_H
