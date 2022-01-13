﻿#include "tp_lyricsviewer.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include <QFile>
#include <QFileInfo>
#include <QMouseEvent>

#include <stack>

TP_LyricsViewer::TP_LyricsViewer( QWidget *parent ) :
    QListWidget     { parent }
  , b_hasLrcFile    {}
  , currentIdx      {}
{
    initializeUI();
}


void
TP_LyricsViewer::updatePosition( qint64 I_ms )
{
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
TP_LyricsViewer::readLrcFile( const QString &I_qstr_Path )
{
    b_hasLrcFile = false;
    setSelectionMode( QAbstractItemView::NoSelection );
    setFocusPolicy( Qt::NoFocus );
    clear();
    scrollToTop();
    currentIdx = -1;

    // No audio file being played
    if( I_qstr_Path.isEmpty() )
    {
        addItem( tr( "No lyrics" ) );
        item( 0 )->setData( TP::role_TimeStampInMs, -1 );
        item( 0 )->setTextAlignment( Qt::AlignCenter );
        refreshFont();
        return;
    }

    qDebug() << "[Lyrics Viewer] Reading LRC file from" << I_qstr_Path;

    if( std::filesystem::exists( I_qstr_Path
#ifdef Q_OS_WIN
    .toStdWString().c_str()
#else
    .toLocal8Bit().constData()
#endif
            ))
    {
        QFile file { I_qstr_Path };
        if( file.open( QIODeviceBase::ReadOnly ) )
        {
            QTextStream inputStream { &file };

            while( ! inputStream.atEnd() )
            {
                const auto &qstr_Line { inputStream.readLine() };

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
                    qstr_Timestamp = qstr_Timestamp.remove( 0, 1 );

                    int m {}, s {}, ms {};
                    bool b_conversionOK {};

                    auto idx_Point { qstr_Timestamp.lastIndexOf( '.' ) };
                    if( idx_Point != -1
                            && qstr_Timestamp.size() - idx_Point - 1 > 0    // Number of digits of ms > 0
                            && qstr_Timestamp.size() - idx_Point - 1 <= 3 ) // Number of digits of ms <= 3
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
            return;
        }
    }       // std::filesystem::exists

    // No valid LRC file
    addItem( tr( "No lyrics" ) );
    item( 0 )->setData( TP::role_TimeStampInMs, -1 );
    item( 0 )->setTextAlignment( Qt::AlignCenter );
    refreshFont();
    return;
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
                position = 0;
            emit signal_lyricsDoubleClicked( position );
        }
    }
    else
        event->ignore();
}

// *****************************************************************
// private
// *****************************************************************

void
TP_LyricsViewer::initializeUI()
{
    setMouseTracking( true );
    setStyleSheet(
"color: #CCC;"
"border-width: 0px;"
                );
    setFrameStyle( QFrame::NoFrame );
    // setResizeMode( QListView::Adjust );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
}

void
TP_LyricsViewer::sortByTimestamp()
{
    if( count() <= 1 )
        return;

    int maxIdx { count() - 1 }, r {}, low {}, high {};
    std::uniform_int_distribution< int > distribution { 0, maxIdx } ;

    // Shuffle whole list to avoid worst situation
    for( unsigned i {}; i <= maxIdx; i++ )
    {
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

    while( true )
    {
        if( stack.empty() )
            break;

        auto [ left, right ] = stack.top();
        low = left, high = right;
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
