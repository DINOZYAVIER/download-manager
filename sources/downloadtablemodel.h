#ifndef DOWNLOADTABLEMODEL_H
#define DOWNLOADTABLEMODEL_H

#include <QAbstractTableModel>
#include <QUrl>

#define NUMBER_OF_COLUMNS 4

class DownloadTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DownloadTableModel( QObject *parent = nullptr );
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    void addDownload( QVariant data );
    void addDownloadSize( QVariant data );
    void addDownloadSpeed( QVariant data );
    //void addDownloadProgress( QVariant data );
Q_SIGNALS:
    void recieveName( QString name );
private Q_SLOTS:
    void onRecieveName( QString name );
private:
    int m_rows;
    QVector<QVariantList*> m_data;
};

#endif // DOWNLOADTABLEMODEL_H
