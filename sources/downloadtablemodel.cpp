#include "precompiled.h"
#include "downloadtablemodel.h"

DownloadTableModel::DownloadTableModel( QObject *parent )
    : QAbstractTableModel( parent ),
      m_rows( 0 )
{
    connect( this, &DownloadTableModel::recieveName, this, &DownloadTableModel::onRecieveName );
    qDebug() << setHeaderData( 0, Qt::Horizontal, "Name" );
    qDebug() << setHeaderData( 1, Qt::Horizontal, "Size" );
    qDebug() << setHeaderData( 2, Qt::Horizontal, "Speed" );
    qDebug() << setHeaderData( 3, Qt::Horizontal, "Progress" );


}

int DownloadTableModel::rowCount(const QModelIndex &parent) const
{
    return m_rows;
}

int DownloadTableModel::columnCount(const QModelIndex &parent) const
{
    return NUMBER_OF_COLUMNS;
}

QVariant DownloadTableModel::data(const QModelIndex &index, int role) const
{
    if( index.column() > 0 )
        return QVariant();
    else
        return m_data.at( index.row())->at( index.column() );
}

void DownloadTableModel::addDownload( QVariant data )
{
    beginInsertRows( QModelIndex(), m_rows, m_rows );

    ++m_rows;
    QVariantList *temp = new QVariantList();
    temp->append( data );
    m_data.append( temp );

    endInsertRows();
    qDebug() << m_data.last()->last() << index( 0, 0 ) << index( m_rows - 1, NUMBER_OF_COLUMNS - 1 );
    Q_EMIT dataChanged( index( 0, 0 ), index( m_rows - 1, NUMBER_OF_COLUMNS - 1 ) );

}

void DownloadTableModel::onRecieveName( QString name )
{
    addDownload( QVariant( name ) );
}
