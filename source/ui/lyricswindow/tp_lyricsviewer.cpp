#include "tp_lyricsviewer.h"

#include "tp_globalconst.h"

#include <QFile>
#include <QFileInfo>

TP_LyricsViewer::TP_LyricsViewer( QWidget *parent ) :
    QListWidget     { parent }
  , b_hasLrcFile    { false }
{

}


void
TP_LyricsViewer::updatePosition( qint64 ms )
{
    if( b_hasLrcFile )
    {

    }
}


void
TP_LyricsViewer::readLrcFile( const QString &I_qstr_Path )
{
    b_hasLrcFile = false;

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

            while( inputStream.atEnd() )
            {
                const auto &qstr_Line { inputStream.readLine() };

                // No ']', no valid timestamp
                if( qstr_Line.lastIndexOf( ']' ) == -1 )
                    continue;

                // Split the line by ']'
                auto qstrList { qstr_Line.split( ']', Qt::SkipEmptyParts ) };

                // If the last segmentation began with '['
                // (such as the line "[00:01.100][00:02.200]"),
                // the lyrics sentence of this line is empty.
                // If not
                // (such as the line "[00:01.100][00:02.200]You're a cat"),
                // the last segmentation is the lyrics sentence of this line.
                QString qstr_Sentence {};
                if( qstrList.last()[0] != '[' )
                    qstr_Sentence = qstrList.takeLast().trimmed();

                for( auto &qstr : qstrList )
                {
                    // Eliminate all whitespaces
                    qstr = qstr.simplified().replace( " ", "" );

                    // The smallest length of the segment should be 6, e.g. "[00:10"
                    if( qstr[ 0 ] != '[' || qstr.size() < 6 )
                        continue;

                    // Remove the first [
                    qstr = qstr.remove( 0, 1 );

                    int m {}, s {}, ms {};
                    bool b_conversionOK {};

                    auto idx_positionOfPoint { qstr.lastIndexOf( '.' ) };
                    if( idx_positionOfPoint != -1 )
                    {
                        // Extract substring after '.'
                        auto qstr_ms { qstr.last( qstr.size() - idx_positionOfPoint - 1 ) };

                        ms = qstr_ms.toInt( &b_conversionOK );
                        if( ! b_conversionOK || ms > 999 || ms < 0)
                            continue;

                        // Remove characters from '.'
                        qstr = qstr.first( idx_positionOfPoint );
                    }

                    auto idx_positionOfColon { qstr.indexOf( ':' ) };

                    auto qstr_s { qstr.last( qstr.size() - idx_positionOfColon - 1 ) };
                    s = qstr_s.toInt( &b_conversionOK );
                    if( ! b_conversionOK || s > 59 || s < 0 )
                        continue;

                    auto qstr_m { qstr.first( idx_positionOfColon ) };
                    m = qstr_m.toInt( &b_conversionOK );
                    if( ! b_conversionOK || m < 0 )
                        continue;

                    m = m * 60 * 1000;
                    s = s * 1000;
                    auto total_ms { m + s + ms };

                    QListWidgetItem *newItem {
                        new QListWidgetItem { qstr_Sentence, this }
                    };
                    newItem->setData( TP::role_TimeStampInMs, total_ms );
                    b_hasLrcFile = true;
                }       // for( auto &qstr : qstrList )
            }       // while( inputStream.atEnd() )

            if( b_hasLrcFile )
                return;
        }
    }

    // No valid LRC file
    clear();
    addItem( QFileInfo { I_qstr_Path }.baseName() );
    item( 0 )->setData( TP::role_TimeStampInMs, -1 );
    return;
}
