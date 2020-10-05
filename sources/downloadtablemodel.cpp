#include "precompiled.h"
#include "downloadtablemodel.h"

#define NUMBER_OF_COLUMNS 4

DownloadTableModel::DownloadTableModel( QObject *parent )
    : QAbstractTableModel( parent )
{
}

DownloadTableModel::~DownloadTableModel()
{
}

QVariant DownloadTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if( Qt::Orientation::Horizontal == orientation && Qt::DisplayRole == role )
    {
        switch( section )
        {
            case 0:
                return QVariant( "Name" );
                break;
            case 1:
                return QVariant( "Size" );
                break;
            case 2:
                return QVariant( "Speed" );
                break;
            case 3:
                return QVariant( "Progress" );
                break;
            default:
                return QVariant();
                break;
        }
    }
    else
        return QVariant();
}

int DownloadTableModel::rowCount( const QModelIndex& ) const
{
    return m_data.size();
}

int DownloadTableModel::columnCount(const QModelIndex& ) const
{
    return NUMBER_OF_COLUMNS;
}

QVariant DownloadTableModel::data( const QModelIndex& index, int role ) const
{
    if( role == Qt::DisplayRole && index.isValid() )
    {
        if( index.column() > m_data.at( index.row() ).size() - 1 )
            return QVariant();
        else
            return m_data.at( index.row() ).at( index.column() );
    }
    return QVariant();
}

void DownloadTableModel::setDataList( QVariantList dataList, int number )
{
    if( number >= m_data.size() )
    {
        beginInsertRows( QModelIndex(), number, number );
        m_data.append( dataList );
        endInsertRows();
    }
    else
    {
        m_data.replace( number, dataList );
        Q_EMIT dataChanged( index( number, 0 ), index( number, NUMBER_OF_COLUMNS - 1 ) );
    }
}

void DownloadTableModel::removeDownload( int id )
{
    if( id >= m_data.size() )
        return;
    beginRemoveRows( QModelIndex(), id, id );
    m_data.removeAt( id );
    endRemoveRows();
    Q_EMIT dataChanged( index( id, 0 ), index( id, NUMBER_OF_COLUMNS - 1 ) );
}


