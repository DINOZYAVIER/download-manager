#ifndef PROGRESSBARDELEGATE_H
#define PROGRESSBARDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class ProgressBarDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ProgressBarDelegate( QObject* parent = nullptr );

    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};
#endif // PROGRESSBARDELEGATE_H
