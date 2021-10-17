﻿#include "tp_globalfunction.h"

#include "tp_globalconst.h"
#include "tp_globalenum.h"

#include <QFileInfo>
#include <QUrl>

#include <filesystem>

// Headers of TagLib
#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>
#include <flacfile.h>
#include <flacproperties.h>
#include <xiphcomment.h>

QString
TP::extension( const QString &path )
{
    return QFileInfo { path }.suffix().toLower();
}

void
TP::storeInformation( QListWidgetItem * I_item )
{
    QUrl url = I_item->data( TP::role_URL ).toUrl();
    QString qstr_localFilePath = url.toLocalFile();
    QFileInfo fileInfo { QFile { qstr_localFilePath } };
    QString qstr_Filename = fileInfo.fileName();

#ifdef Q_OS_WINDOWS
    TagLib::FileRef fileRef { qstr_localFilePath.toStdWString().c_str() };
#else
    TagLib::FileRef fileRef { qstr_localFilePath.toLocal8Bit().constData() };
#endif
    QString qstr_title = TStringToQString( fileRef.tag()->title() );
    QString qstr_artist = TStringToQString( fileRef.tag()->artist() );
    QString qstr_album = TStringToQString( fileRef.tag()->album() );

    int duration = fileRef.audioProperties()->lengthInSeconds();
    int bitrate = fileRef.audioProperties()->bitrate();
    int sampleRate = fileRef.audioProperties()->sampleRate() / 1000;
    int bitDepth = -1 ;

    QString extension { QFileInfo { url.toLocalFile() }.suffix().toLower() };
    if( extension == QString { "flac" } )
        bitDepth = dynamic_cast< TagLib::FLAC::Properties * >( fileRef.audioProperties() )->bitsPerSample();

    I_item->setData( TP::role_SourceType,   TP::singleFile );       // set source type
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
        I_item->setData( TP::role_Description,  qstr_artist + QString(" - ") + qstr_title );
        I_item->setData( TP::role_Artist,       qstr_artist );
        I_item->setData( TP::role_Title,        qstr_title );
        I_item->setData( TP::role_Album,        qstr_album );
    }

    // set ReplayGains
    if( extension == QString { "flac" } )
    {
        // The gain is stored as Xiph Comment in the format like "+6.49 dB"
        TagLib::Ogg::XiphComment *xiphComment { static_cast< TagLib::FLAC::File * >( fileRef.file() )->xiphComment() };
        if( xiphComment->contains( "REPLAYGAIN_TRACK_GAIN" ) )
        {
            float gainTrack {
                TStringToQString( xiphComment->properties()["REPLAYGAIN_TRACK_GAIN"][0] )
                    .split(' ')[0]
                    .toFloat()
            };
            I_item->setData( TP::role_ReplayGainTrack, gainTrack );
        }
        else
            I_item->setData( TP::role_ReplayGainTrack, std::numeric_limits<float>::max() );

        if( xiphComment->contains( "REPLAYGAIN_ALBUM_GAIN" ) )
        {
            float gainAlbum {
                TStringToQString( xiphComment->properties()["REPLAYGAIN_ALBUM_GAIN"][0] )
                    .split(' ')[0]
                    .toFloat()
            };
            I_item->setData( TP::role_ReplayGainAlbum, gainAlbum );
        }
        else
            I_item->setData( TP::role_ReplayGainAlbum, std::numeric_limits<float>::max() );
    }
}
