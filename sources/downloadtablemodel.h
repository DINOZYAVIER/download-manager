#ifndef DOWNLOADTABLEMODEL_H
#define DOWNLOADTABLEMODEL_H

#include <QAbstractTableModel>

class DownloadTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DownloadTableModel( QObject* parent = nullptr );
   ~DownloadTableModel();
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex& parent = QModelIndex() ) const override;
    void setDataList( QVariantList dataList, int number );
    void removeDownload( int id );


private:
    QList<QVariantList> m_data;
};

#endif // DOWNLOADTABLEMODEL_H
