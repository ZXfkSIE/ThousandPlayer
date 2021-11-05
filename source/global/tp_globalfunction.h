#ifndef TP_GLOBALFUNCTION_H
#define TP_GLOBALFUNCTION_H

#include <QListWidgetItem>
#include <QString>

namespace TagLib
{
namespace Ogg   { class XiphComment; }
namespace ID3v2 { class Tag; }
}

namespace TP
{
    QString extension( const QString &path );
    void storeInformation( QListWidgetItem * I_item );

    float getReplayGainTrackFromTag( TagLib::Ogg::XiphComment *xiphComment );
    float getReplayGainAlbumFromTag( TagLib::Ogg::XiphComment *xiphComment );

    float getReplayGainTrackFromTag( TagLib::ID3v2::Tag *tag );
    float getReplayGainAlbumFromTag( TagLib::ID3v2::Tag *tag );
}

#endif // TP_GLOBALFUNCTION_H
