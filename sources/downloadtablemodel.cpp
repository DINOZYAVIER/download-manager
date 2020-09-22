#include "precompiled.h"
#include "downloadtablemodel.h"
#include <QTableView>
DownloadTableModel::DownloadTableModel( QObject *parent )
    : QAbstractTableModel( parent ), m_rows( 0 )
{
    qDebug() << setHeaderData( 0, Qt::Horizontal, "Name" );
    qDebug() << setHeaderData( 1, Qt::Horizontal, "Size" );
    qDebug() << setHeaderData( 2, Qt::Horizontal, "Speed" );
    qDebug() << setHeaderData( 3, Qt::Horizontal, "Progress" );

    connect( this, &DownloadTableModel::newDownloadSignal, this, &DownloadTableModel::onNewDownload );
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
    qDebug() << index.row() << index.column();
    switch ( index.column() )
    {
        qDebug() << "switch";
        case 0:
            return QVariant ( "name" );
            break;
        case 1:
            return QVariant ( "size" );
            break;
        case 2:
            return QVariant ( "speed" );
            break;
        case 3:
            return QVariant( "progress" );
            break;
    }

    return QVariant();
}

void DownloadTableModel::onNewDownload( QUrl url )
{

}

