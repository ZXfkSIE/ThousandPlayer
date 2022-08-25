#include "tp_globalfunction.h"

#include "tp_globalconst.h"
#include "tp_globalenum.h"
#include "tp_globalvariable.h"

#include <QFileInfo>
#include <QUrl>

// Headers of TagLib
#include <apetag.h>
#include <fileref.h>
#include <flacfile.h>
#include <id3v2tag.h>
#include <mp4file.h>
#include <mpegfile.h>
#include <relativevolumeframe.h>
#include <tpropertymap.h>
#include <vorbisfile.h>
#include <wavproperties.h>
#include <xiphcomment.h>


TP::CursorPositionType
TP::getCursorPositionType( QWidget *I_widget, const QPoint &I_point )
{
    if ( I_point.x() <= TP::borderSize )
    {
        return TP::CursorPositionType::Left;
    }
    else if ( I_widget->width() - I_point.x() <= TP::borderSize )
    {
        return TP::CursorPositionType::Right;
    }
    else if ( I_widget->height() - I_point.y() <= TP::borderSize )
    {
        return TP::CursorPositionType::Bottom;
    }

    return TP::CursorPositionType::NonBorder;
}


QUrl
TP::getLyricsURL( QListWidgetItem *I_item )
{
    if( ! TP::currentItem()
            || TP::currentItem()->data( TP::role_SourceType ).value< TP::SourceType >() != TP::SourceType::SingleFile )
        return {};

    auto qstr_fileName { TP::currentItem()->data( TP::role_FileName ).toString() };
    qstr_fileName = qstr_fileName.left( qstr_fileName.lastIndexOf( '.' ) ) + QString { ".lrc" };
    return {
        TP::currentItem()->data( TP::role_URL ).toUrl().adjusted( QUrl::RemoveFilename ).toString()
        + qstr_fileName
    };
}


TP::AudioFormat
TP::getAudioFormat( const QUrl &I_url )
{
    auto qstr_url { I_url.toString() };
    auto extension { qstr_url.right( qstr_url.size() - qstr_url.lastIndexOf( '.' ) - 1 ).toUpper() };
    if( extension.size() <= 2 || extension.size() >= 5 )
        return TP::AudioFormat::NotSupported;

    if( extension == QString { "FLAC" } )
        return TP::AudioFormat::FLAC;

    if( extension == QString { "ALAC" } )
        return TP::AudioFormat::ALAC;

    if( extension == QString { "M4A" } || extension == QString { "AAC" } )
        return TP::AudioFormat::AAC;

    if( extension == QString { "MP3" } )
        return TP::AudioFormat::MP3;

    if( extension == QString { "WAV" } )
        return TP::AudioFormat::WAV;

    if( extension == QString { "OGG" } )
        return TP::AudioFormat::OGG;

    return TP::AudioFormat::NotSupported;
}


TP::AudioFormat
TP::getAudioFormat( const QString &I_path )
{
    auto extension { QFileInfo { I_path }.suffix().toUpper() };
    if( extension.size() <= 2 || extension.size() >= 5 )
        return TP::AudioFormat::NotSupported;

    if( extension == QString { "FLAC" } )
        return TP::AudioFormat::FLAC;

    if( extension == QString { "ALAC" } )
        return TP::AudioFormat::ALAC;

    if( extension == QString { "M4A" } || extension == QString { "AAC" } )
        return TP::AudioFormat::AAC;

    if( extension == QString { "MP3" } )
        return TP::AudioFormat::MP3;

    if( extension == QString { "WAV" } )
        return TP::AudioFormat::WAV;

    if( extension == QString { "OGG" } )
        return TP::AudioFormat::OGG;

    return TP::AudioFormat::NotSupported;
}


void
TP::storeInformation( QListWidgetItem *I_item )
{
    const auto &url { I_item->data( TP::role_URL ).toUrl() };
    const auto &qstr_localFilePath { url.toLocalFile() };
    QFileInfo fileInfo { QFile { qstr_localFilePath } };
    const auto &qstr_fileName { fileInfo.fileName() };

    if( ! std::filesystem::exists( url.toLocalFile().
#ifdef Q_OS_WIN
                                 toStdWString()
#else
                                 toLocal8Bit().constData()
#endif
                                 ) )
    {
        I_item->setData( TP::role_Description, qstr_fileName );
        return;
    }

    const auto &lastModifiedDateTime { fileInfo.lastModified() };

    TagLib::FileRef fileRef { qstr_localFilePath
#ifdef Q_OS_WIN
    .toStdWString().c_str()
#else
    .toLocal8Bit().constData()
#endif
                            };

    const auto &qstr_title { TStringToQString( fileRef.tag()->title() ) };
    const auto &qstr_artist { TStringToQString( fileRef.tag()->artist() ) };
    const auto &qstr_album { TStringToQString( fileRef.tag()->album() ) };

    auto duration { fileRef.audioProperties()->lengthInSeconds() };
    auto bitrate { fileRef.audioProperties()->bitrate() };
    auto sampleRate { fileRef.audioProperties()->sampleRate() / 1000 };
    auto bitDepth { -1 };
    auto replayGain_Track { std::numeric_limits< float >::max() };
    auto replayGain_Album { std::numeric_limits< float >::max() };

    switch( I_item->data( TP::role_AudioFormat ).value< TP::AudioFormat >() )
    {
    case TP::AudioFormat::FLAC :
    {
        bitDepth = dynamic_cast< TagLib::FLAC::Properties * >( fileRef.audioProperties() )->bitsPerSample();

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
        break;
    }

    case TP::AudioFormat::ALAC :
    case TP::AudioFormat::AAC :
    {
        // MPEG-4 audio files may contain MP4 tags.
        bitDepth = dynamic_cast< TagLib::MP4::Properties * >( fileRef.audioProperties() )->bitsPerSample();

        auto *mp4File { dynamic_cast< TagLib::MP4::File * >( fileRef.file() ) };

        if( mp4File->hasMP4Tag() )
        {
            auto *mp4Tag { mp4File->tag() };
            replayGain_Track = getReplayGainTrackFromTag( mp4Tag );
            replayGain_Album = getReplayGainAlbumFromTag( mp4Tag );
        }
        break;
    }

    case TP::AudioFormat::MP3 :
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
        break;
    }

    case TP::AudioFormat::WAV :
    {
        bitDepth = dynamic_cast< TagLib::RIFF::WAV::Properties * >( fileRef.audioProperties() )->bitsPerSample();
        break;
    }

    case TP::AudioFormat::OGG :
    {
        // Vorbis audio files may contain Xiph Comments.
        auto *vorbisFile { dynamic_cast< TagLib::Ogg::Vorbis::File * >( fileRef.file() ) };
        auto *xiphComment { vorbisFile->tag() };
        replayGain_Track = getReplayGainTrackFromTag( xiphComment );
        replayGain_Album = getReplayGainAlbumFromTag( xiphComment );
        break;
    }

    default:
        break;
    }

    I_item->setData( TP::role_Duration,     duration );                 // Set duration
    I_item->setData( TP::role_Bitrate,      bitrate );                  // Set bitrate
    I_item->setData( TP::role_SampleRate,   sampleRate );               // Set sample rate
    I_item->setData( TP::role_BitDepth,     bitDepth );                 // Set bit depth
    I_item->setData( TP::role_FileName,     qstr_fileName );            // Set file name
    I_item->setData( TP::role_LastModified, lastModifiedDateTime );     // Set last modified time & date
    I_item->setData( TP::role_ReplayGainTrack, replayGain_Track );      // Set Track ReplayGain
    I_item->setData( TP::role_ReplayGainAlbum, replayGain_Album );      // Set Audio ReplayGain

    // -------------------- set descrption, artist, title, album --------------------

    if( ! qstr_title.length() )
        // No title in tag, meaning that no valid tag is contained in the file
    {
        I_item->setData( TP::role_Description, qstr_fileName );
    }
    else
    {
        // will be able to customized in the future
        I_item->setData( TP::role_Description,  qstr_artist + QString{ " - " } + qstr_title );
        I_item->setData( TP::role_Artist,       qstr_artist );
        I_item->setData( TP::role_Title,        qstr_title );
        I_item->setData( TP::role_Album,        qstr_album );
    }
}


float
TP::getReplayGainTrackFromTag( const TagLib::Ogg::XiphComment *I_xiphComment )
{
    // The value of ReplayGain is stored in the format like "+1.14 dB", "-5.14 dB".

    if( I_xiphComment->contains( "REPLAYGAIN_TRACK_GAIN" ) )
        return TStringToQString( I_xiphComment->properties()[ "REPLAYGAIN_TRACK_GAIN" ][0] )
                .split(' ')[0]
                .toFloat();

    if( I_xiphComment->contains( "replaygain_track_gain" ) )
        return TStringToQString( I_xiphComment->properties()[ "replaygain_track_gain" ][0] )
                .split(' ')[0]
                .toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainAlbumFromTag( const TagLib::Ogg::XiphComment *I_xiphComment )
{
    if( I_xiphComment->contains( "REPLAYGAIN_ALBUM_GAIN" ) )
        return TStringToQString( I_xiphComment->properties()[ "REPLAYGAIN_ALBUM_GAIN" ][0] )
                .split(' ')[0]
                .toFloat();

    if( I_xiphComment->contains( "replaygain_album_gain" ) )
        return TStringToQString( I_xiphComment->properties()[ "replaygain_album_gain" ][0] )
                .split(' ')[0]
                .toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainTrackFromTag( const TagLib::ID3v2::Tag *I_id3v2Tag )
{
    const auto &frameList { I_id3v2Tag->frameList( "RVA2" ) };
    for( const auto &frame : frameList )
    {
        auto *relativeVolumeFrame { dynamic_cast< TagLib::ID3v2::RelativeVolumeFrame* >( frame ) };
        const auto &frameIdentification { TStringToQString( relativeVolumeFrame->identification() ) };

        if( ! frameIdentification.contains( QString{ "album" }, Qt::CaseInsensitive )
                && relativeVolumeFrame->channels().contains( TagLib::ID3v2::RelativeVolumeFrame::MasterVolume ) )
            return relativeVolumeFrame->volumeAdjustment();
    }

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainAlbumFromTag( const TagLib::ID3v2::Tag *I_id3v2Tag )
{
    const auto &frameList { I_id3v2Tag->frameList( "RVA2" ) };
    for( const auto &frame : frameList )
    {
        auto *relativeVolumeFrame { dynamic_cast< TagLib::ID3v2::RelativeVolumeFrame* >( frame ) };
        const auto &frameIdentification { TStringToQString( relativeVolumeFrame->identification() ) };

        if( frameIdentification.contains( QString{ "album" }, Qt::CaseInsensitive )
                && relativeVolumeFrame->channels().contains( TagLib::ID3v2::RelativeVolumeFrame::MasterVolume ) )
            return relativeVolumeFrame->volumeAdjustment();
    }

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainTrackFromTag( const TagLib::MP4::Tag *I_mp4Tag )
{
    // The value of ReplayGain is stored in the format like "+1.14 dB", "-5.14 dB".

    if( I_mp4Tag->contains( "----:com.apple.iTunes:REPLAYGAIN_TRACK_GAIN" ) )
        return TStringToQString(
                I_mp4Tag->itemMap()[ "----:com.apple.iTunes:REPLAYGAIN_TRACK_GAIN" ].toStringList()[0] )
                .split(' ')[0]
                .toFloat();
    else if( I_mp4Tag->contains( "----:com.apple.iTunes:replaygain_track_gain" ) )
        // For some reason, properties() does not work here.
        return TStringToQString(
                I_mp4Tag->itemMap()[ "----:com.apple.iTunes:replaygain_track_gain" ].toStringList()[0] )
                .split(' ')[0]
                .toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainAlbumFromTag( const TagLib::MP4::Tag *I_mp4Tag )
{
    if( I_mp4Tag->contains( "----:com.apple.iTunes:REPLAYGAIN_ALBUM_GAIN" ) )
        return TStringToQString(
                I_mp4Tag->itemMap()[ "----:com.apple.iTunes:REPLAYGAIN_ALBUM_GAIN" ].toStringList()[0] )
                .split(' ')[0]
                .toFloat();
    else if( I_mp4Tag->contains( "----:com.apple.iTunes:replaygain_album_gain" ) )
        return TStringToQString(
                I_mp4Tag->itemMap()[ "----:com.apple.iTunes:replaygain_album_gain" ].toStringList()[0] )
                .split(' ')[0]
                .toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainTrackFromTag( const TagLib::APE::Tag *I_apeTag )
{
    // The value of ReplayGain is stored in the format like "+1.14 dB", "-5.14 dB".

    const auto &itemListMap { I_apeTag->itemListMap() };
    for( const auto &pair : itemListMap )
        if( TStringToQString( pair.first ) == QString{ "REPLAYGAIN_TRACK_GAIN" } ||
            TStringToQString( pair.first ) == QString{ "replaygain_track_gain" } )
            return TStringToQString( pair.second.toString() ).split(' ')[0].toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainAlbumFromTag( const TagLib::APE::Tag *I_apeTag )
{
    const auto &itemListMap { I_apeTag->itemListMap() };
    for( const auto &pair : itemListMap )
        if( TStringToQString( pair.first ) == QString{ "REPLAYGAIN_ALBUM_GAIN" } ||
            TStringToQString( pair.first ) == QString{ "replaygain_album_gain" } )
            return TStringToQString( pair.second.toString() ).split(' ')[0].toFloat();

    return std::numeric_limits< float >::max();
}


float
TP::getReplayGainFromItem( const QListWidgetItem *I_item )
{
    auto mode { TP::config().getReplayGainMode() };

    if( mode == TP::ReplayGainMode::Disabled )
        return 0;

    auto dB_Track { I_item->data( TP::role_ReplayGainTrack ).toFloat() };
    auto dB_Album { I_item->data( TP::role_ReplayGainAlbum ).toFloat() };
    auto dB_Total { TP::config().getPreAmp_dB() };

    switch ( mode )
    {
    case TP::ReplayGainMode::Track :

        if( dB_Track < 114514 )
        {
            dB_Total += dB_Track;
            break;
        }
        else if( dB_Album < 114514 )
        {
            dB_Total += dB_Album;
            break;
        }
        else
        {
            dB_Total += TP::config().getDefaultGain_dB();
            break;
        }

    case TP::ReplayGainMode::Album :

        if( dB_Album < 114514 )
        {
            dB_Total += dB_Album;
            break;
        }
        else if( dB_Track < 114514 )
        {
            dB_Total += dB_Track;
            break;
        }
        else
        {
            dB_Total += TP::config().getDefaultGain_dB();
            break;
        }

    default:
        break;
    }

    return dB_Total;
}
