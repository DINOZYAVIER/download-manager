#ifndef PROGRESSBARDELEGATE_H
#define PROGRESSBARDELEGATE_H

#include <QItemDelegate>
#include <QObject>
#include <QModelIndex>
#include <QProgressBar>


class ProgressBarDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ProgressBarDelegate();
};

#endif // PROGRESSBARDELEGATE_H
