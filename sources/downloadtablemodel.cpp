#include "precompiled.h"
#include "downloadtablemodel.h"

DownloadTableModel::DownloadTableModel( QObject *parent )
    : QAbstractTableModel( parent ),
      m_rows( 0 )
{
    m_downloader = new Downloader();
    connect( m_downloader->reply(), &QNetworkReply::downloadProgress, this, &DownloadTableModel::onProgressReceived );
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
    if( index.column() > m_data.at( index.row())->size() - 1 )
        return QVariant();
    else
        return m_data.at( index.row())->at( index.column() );
}

void DownloadTableModel::addDownload( QUrl url )
{
    beginInsertRows( QModelIndex(), m_rows, m_rows );

    ++m_rows;
    QVariantList *temp = new QVariantList();
    temp->append( QFileInfo( url.path() ).fileName() );
    m_data.append( temp );

    endInsertRows();
    qDebug() << m_data.last()->last() << index( 0, 0 ) << index( m_rows - 1, NUMBER_OF_COLUMNS - 1 );
    Q_EMIT dataChanged( index( 0, 0 ), index( m_rows - 1, NUMBER_OF_COLUMNS - 1 ) );
    m_downloader->doDownload( url );
}

void DownloadTableModel::onProgressReceived( qint64 bytesReceived, qint64 bytesTotal )
{
    qDebug() << "progress received";
    QVariantList* temp = m_data.last();
    if( temp->size() < 1 )
        temp->append( QString::number( bytesTotal ) );
    if( temp->size() < 2 )
        temp->append( "speed" );
    if( temp->size() < 3 )
         temp->append( QString::number( bytesReceived ) + '/' + QString::number( bytesTotal ) );
    else
        temp->replace( 3, QString::number( bytesReceived ) + '/' + QString::number( bytesTotal ) );
    qDebug() << *temp;

}
