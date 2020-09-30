#include "precompiled.h"
#include "progressbardelegate.h"

static const int PROGRESS_BAR_HEIGHT_PX = 10;
static const int MAX_PROGRESS_VALUE = 100;

ProgressBarDelegate::ProgressBarDelegate( QObject* parent ) :
    QStyledItemDelegate( parent )
{
}

void ProgressBarDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    if( index.column() == 3 )
    {
        if ( QTableView* tableView = qobject_cast<QTableView*>(this->parent()) )
        {
            int x = 0;
            for( int i = 0; i < 3; ++i)
                x += tableView->columnWidth( i );

            int progress = index.data().toInt();
            QStyleOptionProgressBar progressBarOption;
            QRect r = QRect( QPoint( x, 25 * index.row() ), QPoint( x + tableView->columnWidth( 3 ), 25 * ( index.row() + 1 ) ) );
            r.setHeight( PROGRESS_BAR_HEIGHT_PX );
            //r.moveCenter( option.rect.center() );
            progressBarOption.rect = r;
            progressBarOption.textAlignment = Qt::AlignRight;
            progressBarOption.minimum = 0;
            progressBarOption.maximum = MAX_PROGRESS_VALUE;
            progressBarOption.progress = progress;
            progressBarOption.text = QString::number( progress ) + "%";
            progressBarOption.textVisible = true;

            QStyledItemDelegate::paint( painter, option, QModelIndex() );
            QApplication::style()->drawControl( QStyle::CE_ProgressBar, &progressBarOption, painter );
        }
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}



