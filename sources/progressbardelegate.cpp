#include "precompiled.h"
#include "progressbardelegate.h"

ProgressBarDelegate::ProgressBarDelegate( QObject* parent ) :
    QStyledItemDelegate( parent )
{
}

void ProgressBarDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    if( index.column() == 3 && index.isValid() )
    {
        int progress = index.data( Qt::DisplayRole ).toInt();
        QProgressBar downloadProgressBar;

        downloadProgressBar.resize( option.rect.size() );
        downloadProgressBar.setMinimum( 0 );
        downloadProgressBar.setMaximum( 100 );
        downloadProgressBar.setValue( progress );
        downloadProgressBar.setTextVisible( true );

        painter->save();
        painter->translate( option.rect.topLeft() );
        downloadProgressBar.render( painter );
        painter->restore();
    }
    else
       QStyledItemDelegate::paint( painter, option, index );
}



