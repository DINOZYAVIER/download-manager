#include "precompiled.h"
#include "downloadtablemodel.h"

DownloadTableModel::DownloadTableModel( QObject *parent )
    : QAbstractTableModel( parent ),
      m_rows( 0 )
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

int DownloadTableModel::rowCount( const QModelIndex &parent ) const
{
    return m_rows;
}

int DownloadTableModel::columnCount( const QModelIndex &parent ) const
{
    return NUMBER_OF_COLUMNS;
}

QVariant DownloadTableModel::data( const QModelIndex &index, int role ) const
{
    if( role == Qt::DisplayRole && index.isValid() )
    {
        if( index.column() > m_data.at( index.row())->size() - 1 )
            return QVariant();
        else
            return m_data.at( index.row())->at( index.column() );
    }
    return QVariant();
}

void DownloadTableModel::setDataList( QVariantList* dataList, int number )
{
    if( number >= m_rows )
    {
        beginInsertRows( QModelIndex(), m_rows, m_rows );
        m_data.append( dataList );
        ++m_rows;
        endInsertRows();
    }
    Q_EMIT dataChanged( index( 0, 0 ), index( m_rows - 1, NUMBER_OF_COLUMNS - 1 ) );
}



