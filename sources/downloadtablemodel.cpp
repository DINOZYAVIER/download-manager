#include "precompiled.h"
#include "downloadtablemodel.h"

DownloadTableModel::DownloadTableModel( QObject *parent )
    : QAbstractTableModel( parent ),
      m_rows( 0 )
{
    m_downloader = new Downloader();
    connect( m_downloader, &Downloader::setProgress, this, &DownloadTableModel::onProgressSet );




}

DownloadTableModel::~DownloadTableModel()
{
    delete m_downloader;
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
                break;
        }
    }
    else
        return QVariant();

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
    if( role == Qt::DisplayRole )
    {
        if( index.column() > m_data.at( index.row())->size() - 1 )
            return QVariant();
        else
            return m_data.at( index.row())->at( index.column() );
    }
    return QVariant();
}

void DownloadTableModel::addDownload( QUrl url )
{
    beginInsertRows( QModelIndex(), m_rows, m_rows );

    ++m_rows;
    QVariantList *temp = new QVariantList();
    temp->append( QFileInfo( url.path() ).fileName() );
    m_data.append( temp );

    endInsertRows();

    Q_EMIT dataChanged( index( 0, 0 ), index( m_rows - 1, NUMBER_OF_COLUMNS - 1 ) );
    m_downloader->doDownload( url );
}

void DownloadTableModel::onProgressReceived( qint64 bytesReceived, qint64 bytesTotal )
{
    QVariantList* temp = m_data.last();
    if( temp->size() < 2 )
        temp->append( QString::number( bytesTotal / 1048576 ) + "MB" );
    if( temp->size() < 3 )
        temp->append( QString::number( bytesReceived * 1000 / m_downloader->elapsedTimer()->elapsed() / 1024 ) + "KB/sec" );
    else
        temp->replace( 2, QString::number( bytesReceived * 1000 / m_downloader->elapsedTimer()->elapsed() / 1024 ) + "KB/sec" );
    if( temp->size() < 4 )
         temp->append( QString::number( bytesReceived / 1048576 ) + "MB / " + QString::number( bytesTotal / 1048576 ) + "MB" );
    else
        temp->replace( 3, QString::number( bytesReceived / 1048576 ) + "MB / " + QString::number( bytesTotal / 1048576 ) + "MB" );
    Q_EMIT dataChanged( index( 0, 0 ), index( m_rows - 1, NUMBER_OF_COLUMNS - 1 ) );
}

void DownloadTableModel::onProgressSet()
{
    qDebug() << connect( m_downloader->reply(), &QNetworkReply::downloadProgress, this, &DownloadTableModel::onProgressReceived );
}
