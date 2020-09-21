#include "downloadtablemodel.h"

DownloadTableModel::DownloadTableModel( QObject *parent, Downloader *downloader )
    : QAbstractItemModel(parent), m_downloader( downloader)
{
    m_rows = 1;
}

QVariant DownloadTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool DownloadTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

QModelIndex DownloadTableModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex( row, column );
}

QModelIndex DownloadTableModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
    return index.parent();
}

int DownloadTableModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        qDebug() << "row invalid";
        return 0;
    }

    return m_rows;
}

int DownloadTableModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        qDebug() << "column invalid";
        return 0;
    }

    return NUMBER_OF_COLUMNS;
}

QVariant DownloadTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        qDebug() << "invalid";
        return QVariant();
    }

    qDebug() << "before switch";

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
            return QVariant ( "progress" );
            break;
        case 3:
            return QVariant( "speed" );
            break;
    }


    return QVariant();
}

bool DownloadTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags DownloadTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool DownloadTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool DownloadTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
}

bool DownloadTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

bool DownloadTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
}
