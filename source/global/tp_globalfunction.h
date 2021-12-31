#ifndef TP_GLOBALFUNCTION_H
#define TP_GLOBALFUNCTION_H

#include "tp_globalenum.h"

#include <QListWidgetItem>
#include <QString>

namespace TagLib
{

namespace APE   { class Tag; }
namespace ID3v2 { class Tag; }
namespace MP4   { class Tag; }
namespace Ogg   { class XiphComment; }

}

namespace TP
{
    TP::CursorPositionType getCursorPositionType( QWidget *I_widget, const QPoint &I_point );

    QString extension( const QString &I_path );
    void storeInformation( QListWidgetItem *I_item );

    float getReplayGainTrackFromTag( const TagLib::Ogg::XiphComment *I_xiphComment );
    float getReplayGainAlbumFromTag( const TagLib::Ogg::XiphComment *I_xiphComment );

    float getReplayGainTrackFromTag( const TagLib::ID3v2::Tag *I_id3v2Tag );
    float getReplayGainAlbumFromTag( const TagLib::ID3v2::Tag *I_id3v2Tag );

    float getReplayGainTrackFromTag( const TagLib::MP4::Tag *I_mp4Tag );
    float getReplayGainAlbumFromTag( const TagLib::MP4::Tag *I_mp4Tag );

    float getReplayGainTrackFromTag( const TagLib::APE::Tag *I_apeTag );
    float getReplayGainAlbumFromTag( const TagLib::APE::Tag *I_apeTag );

    float getReplayGainFromItem( const QListWidgetItem *I_item );
}

#endif // TP_GLOBALFUNCTION_H
