#include "tp_lyricsviewer.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include "tp_menu.h"

#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QMouseEvent>

#include <stack>

TP_LyricsViewer::TP_LyricsViewer( QWidget *parent ) :
    QListWidget         { parent }
  , b_hasLrcFile        {}
  , currentIdx          {}
  , currentPosition     {}
  , currentLyricsURL    {}
{
    setMouseTracking( true );

    initializeUI();
    initializeMenu();
}


void
TP_LyricsViewer::updatePosition( qint64 I_ms )
{
    currentPosition = I_ms;

    if( ! b_hasLrcFile )
        return;

    auto idx { findItemByTimestamp( I_ms ) };

    if( idx != currentIdx )
    {
        clearSelection();
        unsetCurrentItemBold();
        currentIdx = idx;
        setCurrentItemBold();
        scrollToItem( item( currentIdx ), QAbstractItemView::PositionAtCenter );
    }
}


void
TP_LyricsViewer::readLyricsFile( const QUrl &I_url )
{
    clear();
    scrollToTop();
    b_hasLrcFile = false;
    currentIdx = -1;
    currentPosition = 0;
    const auto &qstr_LocalPath { I_url.toLocalFile() };
    QFile file { qstr_LocalPath };

    qDebug() << "[Lyrics Viewer] Reading LRC file from" << qstr_LocalPath;

    if( file.open( QIODeviceBase::ReadOnly ) )
    {
        QTextStream inputStream { &file };

        while( ! inputStream.atEnd() )
        {
            const auto &qstr_Line { inputStream.readLine().trimmed() };

            // No ']', no valid timestamp
            if( qstr_Line.lastIndexOf( ']' ) == -1 )
                continue;

            // Split the line by ']'
            auto qstrList_Timestamp { qstr_Line.split( ']', Qt::SkipEmptyParts ) };

            // If the last segmentation began with '['
            // (such as the line "[00:01.100][00:02.200]"),
            // the lyrics sentence of this line is empty.
            // If not
            // (such as the line "[00:01.100][00:02.200]You're a cat"),
            // the last segmentation is the lyrics sentence of this line.
            QString qstr_Sentence {};
            if( qstrList_Timestamp.last()[0] != '[' )
                qstr_Sentence = qstrList_Timestamp.takeLast().trimmed();

            for( auto &qstr_Timestamp : qstrList_Timestamp )
            {
                // Eliminate all whitespaces
                qstr_Timestamp = qstr_Timestamp.simplified().replace( " ", "" );

                // The smallest length of the segment should be 6, e.g. "[00:10"
                if( qstr_Timestamp[ 0 ] != '[' || qstr_Timestamp.size() < 6 )
                    continue;

                // Remove the initial '['
                qstr_Timestamp = qstr_Timestamp.last( qstr_Timestamp.size() - 1 );

                int m {}, s {}, ms {};
                bool b_conversionOK {};

                auto idx_Point { qstr_Timestamp.lastIndexOf( '.' ) };
                if( idx_Point != -1
                        && qstr_Timestamp.size() - idx_Point - 1 > 0        // Number of digits of ms > 0
                        && qstr_Timestamp.size() - idx_Point - 1 <= 3 )     // Number of digits of ms <= 3
                {
                    // Extract substring after '.'
                    auto qstr_ms { qstr_Timestamp.last( qstr_Timestamp.size() - idx_Point - 1 ) };

                    // Pad zeros.
                    // For example, if there is a timestamp "[00:01.54]",
                    // the number of ms should be 540 instead of 54.
                    if( qstr_ms.size() == 2 )
                        qstr_ms += "0";
                    else if( qstr_ms.size() == 1 )
                        qstr_ms += "00";

                    ms = qstr_ms.toInt( &b_conversionOK );
                    if( ! b_conversionOK || ms < 0 )
                        continue;

                    // Remove all characters from '.'
                    qstr_Timestamp = qstr_Timestamp.first( idx_Point );
                }

                auto idx_Colon { qstr_Timestamp.indexOf( ':' ) };

                auto qstr_s { qstr_Timestamp.last( qstr_Timestamp.size() - idx_Colon - 1 ) };
                s = qstr_s.toInt( &b_conversionOK );
                if( ! b_conversionOK || s > 59 || s < 0 )
                    continue;

                auto qstr_m { qstr_Timestamp.first( idx_Colon ) };
                m = qstr_m.toInt( &b_conversionOK );
                if( ! b_conversionOK || m < 0 )
                    continue;

                m = m * 60000;
                s = s * 1000;
                auto total_ms { m + s + ms };

                addItem( qstr_Sentence );
                item( count() - 1 )->setData( TP::role_TimeStampInMs, total_ms );
                item( count() - 1 )->setTextAlignment( Qt::AlignCenter );
                b_hasLrcFile = true;
            }       // for( auto &qstr : qstrList )
        }       // while( inputStream.atEnd() )
    }       // if( file.open( QIODeviceBase::ReadOnly ) )

    if( b_hasLrcFile )
    {
        sortByTimestamp();
        refreshFont();
        setSelectionMode( QAbstractItemView::SingleSelection );
        setFocusPolicy( Qt::StrongFocus );
        currentLyricsURL = I_url;
        return;
    }

    // -------------------- No valid lyrics file --------------------

    addItem( tr( "No lyrics" ) );
    item( 0 )->setData( TP::role_TimeStampInMs, -1 );
    item( 0 )->setTextAlignment( Qt::AlignCenter );
    refreshFont();
    setSelectionMode( QAbstractItemView::NoSelection );
    setFocusPolicy( Qt::NoFocus );
    currentLyricsURL.clear();
    return;
}


void
TP_LyricsViewer::reloadLyricsFile( const QUrl &I_url )
{
    if( I_url == currentLyricsURL )
    {
        readLyricsFile( I_url );
        updatePosition( currentPosition );
    }
}


void
TP_LyricsViewer::refreshFont()
{
    setFont( TP::config().getLyricsFont() );
    auto height { font().pointSize() << 1 };   // I_font.pointSize() * 2
    for( unsigned i {}; i < count(); i++ )
    {
        item( i )->setFont( TP::config().getLyricsFont() );
        item( i )->setSizeHint( { 0, height } );
    }
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_LyricsViewer::slot_switchToLyricsEditor()
{
    emit signal_switchToLyricsEditor( currentLyricsURL );
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_LyricsViewer::mouseMoveEvent( QMouseEvent *event )
{
    event->ignore();
}


void
TP_LyricsViewer::mouseDoubleClickEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
    {
        auto *clickedItem = itemAt ( event->pos() );
        if( clickedItem )
        {
            auto position { clickedItem->data( TP::role_TimeStampInMs ).value< qint64 >() };
            position += TP::config().getJumpingTimeOffset_ms();
            if( position < 0 )
                position = 0;               // position cannot be negative
            emit signal_lyricsDoubleClicked( position );
        }
    }
    else
        event->ignore();
}


void
TP_LyricsViewer::contextMenuEvent( QContextMenuEvent *event )
{
    rightClickMenu->exec( event->globalPos() );
}

// *****************************************************************
// private
// *****************************************************************

void
TP_LyricsViewer::initializeUI()
{
    setStyleSheet(
"background-color: #777;"
"color: #CCC;"
"border-width: 0px;"
                );
    setFrameStyle( QFrame::NoFrame );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
}


void
TP_LyricsViewer::initializeMenu()
{
    rightClickMenu = new TP_Menu { this };

    action_switchToLyricsEditor = new QAction { tr( "Lyrics &editor" ) };
    connect( action_switchToLyricsEditor,   &QAction::triggered,
             this,                          &TP_LyricsViewer::slot_switchToLyricsEditor );

    rightClickMenu->addAction( action_switchToLyricsEditor );
}


void
TP_LyricsViewer::sortByTimestamp()
{
    if( count() <= 1 )
        return;

    int maxIdx { count() - 1 };
    std::uniform_int_distribution< int > distribution { 0, maxIdx } ;

    // Shuffle whole list to avoid worst situation
    for( int i {}; i <= maxIdx; i++ )
    {
        int r { i };
        while( i == r )
            r = distribution( TP::randomEngine() );

        // swap item( i ) & item( r )
        if( i < r )
        {
            insertItem( r, item( i )->clone() );            // item( i )->i,   item( r ) -> r+1
            insertItem( i, item( r + 1 )->clone() );        // item( i )->i+1, item( r ) -> r+2
            delete takeItem( r + 2 );
            delete takeItem( i + 1 );
        }
        else    // i > r
        {
            insertItem( i, item( r )->clone() );
            insertItem( r, item( i + 1 )->clone() );
            delete takeItem( i + 2 );
            delete takeItem( r + 1 );
        }
    }

    // Use own stack to prevent god damn stack overflow
    std::stack< std::pair< int, int > > stack {};
    stack.push( { 0, maxIdx } );

    while( ! stack.empty() )
    {
        auto [ left, right ] = stack.top();
        auto low { left }, high { right };
        stack.pop();
        auto *pivot = item( left )->clone();

        while( low < high )
        {
            while( low < high &&
                   item( high )->data( TP::role_TimeStampInMs ).value< qint64 >()
                   >= pivot->data( TP::role_TimeStampInMs ).value< qint64 >() )
                high--;

            if( low < high )
            {                                       // item[ low ] = item[ high ]
                insertItem( low, item( high )->clone() );
                delete takeItem( low + 1 );
                // Now it is certain that item[ low ] < pivot,
                // thus item[ low ] can be jumped.
                low++;
            }

            while( low < high &&
                   item( low )->data( TP::role_TimeStampInMs ).value< qint64 >()
                   < pivot->data( TP::role_TimeStampInMs ).value< qint64 >() )
                low++;

            if( low < high )
            {                                       // item[ high ] = item[ low ]
                delete takeItem( high );
                insertItem( high, item( low )->clone() );
                // Now it is certain that item[ high ] >= pivot,
                // thus item[ high ] can be jumped.
                high--;
            }
        }       // while( low < high )

        if( left < low )
        {                                           // item[ low ] = pivot
            delete takeItem( low );
            insertItem( low, pivot );
        }
        else
            delete pivot;

        if( low - 1 - left < right - low - 1 )      // smaller range first
        {
            if( low + 1 < right )
                stack.push( { low + 1, right } );
            if( left < low - 1 )
                stack.push( { left, low - 1 } );
        }
        else
        {
            if( left < low - 1 )
                stack.push( { left, low - 1 } );
            if( low + 1 < right )
                stack.push( { low + 1, right } );
        }
    }       // while( true )
}

int
TP_LyricsViewer::findItemByTimestamp( qint64 I_ms )
{
    if( I_ms < item( 0 )->data( TP::role_TimeStampInMs ).value< qint64 >() )
        return -1;

    if( I_ms >= item( count() - 1 )->data( TP::role_TimeStampInMs ).value< qint64 >() )
        return count() - 1;

    int left {}, right { count() - 2 };

    // Target of the binary searching is to find an index i
    // which satisfies time[ i ] <= I_ms < time[ i + 1 ].
    while( left <= right )
    {
        auto middle { ( left + right ) >> 1 };

        // I_ms >= time[ i + 1 ]
        if( I_ms >= item( middle + 1 )->data( TP::role_TimeStampInMs ).value< qint64 >() )
            left = middle + 1;
        // I_ms < time[ i ]
        else if( I_ms < item( middle )->data( TP::role_TimeStampInMs ).value< qint64 >() )
            right = middle - 1;
        else
            return middle;
    }

    return left;
}


void
TP_LyricsViewer::setCurrentItemBold()
{
    if( currentIdx < 0 )
        return;

    auto font { TP::config().getLyricsFont() };
    font.setBold( true );
    item( currentIdx )->setFont( font );
    item( currentIdx )->setForeground( Qt::white );
}


void
TP_LyricsViewer::unsetCurrentItemBold()
{
    if( currentIdx < 0 )
        return;

    item( currentIdx )->setFont( TP::config().getLyricsFont() );
    item( currentIdx )->setForeground( QColor { "#ccc" } );
}
