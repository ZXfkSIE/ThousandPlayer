#include "tp_globalfunction.h"

#include "tp_globalconst.h"
#include "tp_globalenum.h"

#include <QFileInfo>
#include <QUrl>

#include <filesystem>

// Headers of TagLib
#include <apetag.h>
#include <fileref.h>
#include <flacfile.h>
#include <flacproperties.h>
#include <id3v2tag.h>
#include <mp4file.h>
#include <mpegfile.h>
#include <relativevolumeframe.h>
#include <tpropertymap.h>
#include <vorbisfile.h>
#include <xiphcomment.h>

QString
TP::extension( const QString &path )
{
    auto extension { QFileInfo { path }.suffix().toLower() };

    if( extension == QString { "m4a" } )
        extension = QString { "aac" };

    return extension;
}


void
TP::storeInformation( QListWidgetItem * I_item )
{
    auto url = I_item->data( TP::role_URL ).toUrl();
    auto qstr_localFilePath = url.toLocalFile();
    QFileInfo fileInfo { QFile { qstr_localFilePath } };
    auto qstr_Filename = fileInfo.fileName();

    TagLib::FileRef fileRef { qstr_localFilePath
#ifdef Q_OS_WIN
    .toStdWString().c_str()
#else
    .toLocal8Bit().constData()
#endif
                            };

    auto qstr_title = TStringToQString( fileRef.tag()->title() );
    auto qstr_artist = TStringToQString( fileRef.tag()->artist() );
    auto qstr_album = TStringToQString( fileRef.tag()->album() );

    auto duration = fileRef.audioProperties()->lengthInSeconds();
    auto bitrate = fileRef.audioProperties()->bitrate();
    auto sampleRate = fileRef.audioProperties()->sampleRate() / 1000;
    auto bitDepth = -1 ;

    auto extension { TP::extension ( qstr_localFilePath ) };

    if( extension == QString { "flac" } )
        bitDepth = dynamic_cast< TagLib::FLAC::Properties * >( fileRef.audioProperties() )->bitsPerSample();

    I_item->setData( TP::role_Duration,     duration );             // set duration
    I_item->setData( TP::role_Bitrate,      bitrate );              // set bitrate
    I_item->setData( TP::role_SampleRate,   sampleRate );           // set sample rate
    I_item->setData( TP::role_BitDepth,     bitDepth );             // set bit depth
    I_item->setData( TP::role_FileName,     qstr_Filename );        // set file name

    // set descrption, artist, title, album
    if( qstr_title.length() == 0 )
    {
        // No title in tag, meaning that no valid tag is contained in the file
        I_item->setData( TP::role_Description, qstr_Filename );
    }
    else
    {
        // will be able to customized in the future
        I_item->setData( TP::role_Description,  qstr_artist + QString{ " - " } + qstr_title );
        I_item->setData( TP::role_Artist,       qstr_artist );
        I_item->setData( TP::role_Title,        qstr_title );
        I_item->setData( TP::role_Album,        qstr_album );
    }

    // ------------------------- set ReplayGains -------------------------

    auto replayGain_Track { std::numeric_limits< float >::max() };
    auto replayGain_Album { std::numeric_limits< float >::max() };


    if( extension == QString { "flac" } )
    {
        // FLAC files may contain Xiph comments and ID3v2 frames.
        auto *flacFile { dynamic_cast< TagLib::FLAC::File * >( fileRef.file() ) };

        if( flacFile->hasXiphComment() )
        {
            auto *xiphComment { flacFile->xiphComment() };
            replayGain_Track = getReplayGainTrackFromTag( xiphComment );
            replayGain_Album = getReplayGainAlbumFromTag( xiphComment );
        }
        if( flacFile->hasID3v2Tag()
                && replayGain_Track == std::numeric_limits< float >::max()  // ReplayGains have not been set yet
                && replayGain_Album == std::numeric_limits< float >::max()
                )
        {
            auto *id3v2Tag { flacFile->ID3v2Tag() };
            replayGain_Track = getReplayGainTrackFromTag( id3v2Tag );
            replayGain_Album = getReplayGainAlbumFromTag( id3v2Tag );
        }
    }
    else if( extension == QString { "alac" } || extension == QString { "aac" } )
    {
        // MPEG-4 audio files may contain MP4 tags.
        auto *mp4File { dynamic_cast< TagLib::MP4::File * >( fileRef.file() ) };

        if( mp4File->hasMP4Tag() )
        {
            auto *mp4Tag { mp4File->tag() };
            replayGain_Track = getReplayGainTrackFromTag( mp4Tag );
            replayGain_Album = getReplayGainAlbumFromTag( mp4Tag );
        }
    }
    else if( extension == QString { "ogg" } )
    {
        // Vorbis audio files may contain Xiph Comments.
        auto *vorbisFile { dynamic_cast< TagLib::Ogg::Vorbis::File * >( fileRef.file() ) };
        auto *xiphComment { vorbisFile->tag() };
        replayGain_Track = getReplayGainTrackFromTag( xiphComment );
        replayGain_Album = getReplayGainAlbumFromTag( xiphComment );
    }
    else if( extension == QString { "mp3" } )
    {
        // MP3 audio files may contain APE tags and ID3v2 frames.
        auto *mp3File { dynamic_cast< TagLib::MPEG::File * >( fileRef.file() ) };

        if( mp3File->hasAPETag() )
        {
            auto *apeTag { mp3File->APETag() };
            replayGain_Track = getReplayGainTrackFromTag( apeTag );
            replayGain_Album = getReplayGainAlbumFromTag( apeTag );
        }
        if( mp3File->hasID3v2Tag()
                && replayGain_Track == std::numeric_limits< float >::max()  // ReplayGains have not been set yet
                && replayGain_Album == std::numeric_limits< float >::max()
                )
        {
            auto *id3v2Tag { mp3File->ID3v2Tag() };
            replayGain_Track = getReplayGainTrackFromTag( id3v2Tag );
            replayGain_Album = getReplayGainAlbumFromTag( id3v2Tag );
        }
    }

    I_item->setData( TP::role_ReplayGainTrack, replayGain_Track );
    I_item->setData( TP::role_ReplayGainAlbum, replayGain_Album );
}


float
TP::getReplayGainTrackFromTag( TagLib::Ogg::XiphComment *xiphComment )
{
    // The value of ReplayGain is stored in the format like "+1.14 dB", "-5.14 dB".

    if( xiphComment->contains( "REPLAYGAIN_TRACK_GAIN" ) )
        return TStringToQString( xiphComment->properties()[ "REPLAYGAIN_TRACK_GAIN" ][0] )
                .split(' ')[0]
                .toFloat();

    if( xiphComment->contains( "replaygain_track_gain" ) )
        return TStringToQString( xiphComment->properties()[ "replaygain_track_gain" ][0] )
                .split(' ')[0]
                .toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainAlbumFromTag( TagLib::Ogg::XiphComment *xiphComment )
{
    if( xiphComment->contains( "REPLAYGAIN_ALBUM_GAIN" ) )
        return TStringToQString( xiphComment->properties()[ "REPLAYGAIN_ALBUM_GAIN" ][0] )
                .split(' ')[0]
                .toFloat();

    if( xiphComment->contains( "replaygain_album_gain" ) )
        return TStringToQString( xiphComment->properties()[ "replaygain_album_gain" ][0] )
                .split(' ')[0]
                .toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainTrackFromTag( TagLib::ID3v2::Tag *tag )
{
    auto frameList { tag->frameList( "RVA2" ) };
    for( const auto &frame : frameList )
    {
        auto *relativeVolumeFrame { dynamic_cast< TagLib::ID3v2::RelativeVolumeFrame* >( frame ) };
        auto frameIdentification { TStringToQString( relativeVolumeFrame->identification() ) };

        if( ! frameIdentification.contains( QString{ "album" }, Qt::CaseInsensitive )
                && relativeVolumeFrame->channels().contains( TagLib::ID3v2::RelativeVolumeFrame::MasterVolume ) )
            return relativeVolumeFrame->volumeAdjustment();
    }

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainAlbumFromTag( TagLib::ID3v2::Tag *tag )
{
    auto frameList { tag->frameList( "RVA2" ) };
    for( const auto &frame : frameList )
    {
        auto *relativeVolumeFrame { dynamic_cast< TagLib::ID3v2::RelativeVolumeFrame* >( frame ) };
        auto frameIdentification { TStringToQString( relativeVolumeFrame->identification() ) };

        if( frameIdentification.contains( QString{ "album" }, Qt::CaseInsensitive )
                && relativeVolumeFrame->channels().contains( TagLib::ID3v2::RelativeVolumeFrame::MasterVolume ) )
            return relativeVolumeFrame->volumeAdjustment();
    }

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainTrackFromTag( TagLib::MP4::Tag *tag )
{
    // The value of ReplayGain is stored in the format like "+1.14 dB", "-5.14 dB".

    if( tag->contains( "----:com.apple.iTunes:REPLAYGAIN_TRACK_GAIN" ) )
            return TStringToQString( tag->itemMap()[ "----:com.apple.iTunes:REPLAYGAIN_TRACK_GAIN" ].toStringList()[0] )
                    .split(' ')[0]
                    .toFloat();
    else if( tag->contains( "----:com.apple.iTunes:replaygain_track_gain" ) )
        // For some reason, properties() does not work here.
        return TStringToQString( tag->itemMap()[ "----:com.apple.iTunes:replaygain_track_gain" ].toStringList()[0] )
                .split(' ')[0]
                .toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainAlbumFromTag( TagLib::MP4::Tag *tag )
{
    if( tag->contains( "----:com.apple.iTunes:REPLAYGAIN_ALBUM_GAIN" ) )
            return TStringToQString( tag->itemMap()[ "----:com.apple.iTunes:REPLAYGAIN_ALBUM_GAIN" ].toStringList()[0] )
                    .split(' ')[0]
                    .toFloat();
    else if( tag->contains( "----:com.apple.iTunes:replaygain_album_gain" ) )
        return TStringToQString( tag->itemMap()[ "----:com.apple.iTunes:replaygain_album_gain" ].toStringList()[0] )
                .split(' ')[0]
                .toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainTrackFromTag( TagLib::APE::Tag *tag )
{
    // The value of ReplayGain is stored in the format like "+1.14 dB", "-5.14 dB".

    const auto itemListMap { tag->itemListMap() };
    for( const auto &pair : itemListMap )
        if( TStringToQString( pair.first ) == QString{ "REPLAYGAIN_TRACK_GAIN" } ||
            TStringToQString( pair.first ) == QString{ "replaygain_track_gain" } )
            return TStringToQString( pair.second.toString() ).split(' ')[0].toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainAlbumFromTag( TagLib::APE::Tag *tag )
{
    const auto itemListMap { tag->itemListMap() };
    for( const auto &pair : itemListMap )
        if( TStringToQString( pair.first ) == QString{ "REPLAYGAIN_ALBUM_GAIN" } ||
            TStringToQString( pair.first ) == QString{ "replaygain_album_gain" } )
            return TStringToQString( pair.second.toString() ).split(' ')[0].toFloat();

    return std::numeric_limits< float >::max();
}
