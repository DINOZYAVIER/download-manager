#ifndef DOWNLOADTABLEMODEL_H
#define DOWNLOADTABLEMODEL_H

#include <QAbstractTableModel>
#include <QUrl>

#define NUMBER_OF_COLUMNS 4

class DownloadTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DownloadTableModel( QObject* parent = nullptr );
   ~DownloadTableModel();
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex& parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    void setDataList( QVariantList* dataList, int number );
Q_SIGNALS:
private Q_SLOTS:
private:
    int m_rows;
    QVector<QVariantList*> m_data;
};

#endif // DOWNLOADTABLEMODEL_H
