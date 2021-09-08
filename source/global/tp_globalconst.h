#ifndef TP_GLOBALCONST_H
#define TP_GLOBALCOSNT_H

#include <QString>

namespace TP
{

static const unsigned mainWindow        = 0;
static const unsigned playlistWindow    = 1;
static const unsigned equalizerWindow   = 2;
static const unsigned lyricsWindow      = 3;

static const int borderSize = 5;
static const int snapRange  = 8;

static const QString configDirectoryPath    { "./userData" };
static const QString configFilePath         { configDirectoryPath + QString { "/config.ini" } };
static const QString playlistFilePath       { configDirectoryPath + QString { "/playlists.xml" } };

static const int role_Duration      = 0x0100;
static const int role_Bitrate       = 0x0101;
static const int role_SampleRate    = 0x0102;
static const int role_BitDepth      = 0x0103;
static const int role_Artist        = 0x0104;
static const int role_Title         = 0x0105;
static const int role_Album         = 0x0106;
static const int role_FileName      = 0x0107;
static const int role_URL           = 0x0108;
static const int role_SourceType    = 0x0109;
static const int role_Description   = 0x010A;


static const int iconSize_Play          = 20;
static const int iconSize_Pause         = 18;

static const int iconSize_SingleTime    = 26;
static const int iconSize_Repeat        = 28;
static const int iconSize_Sequential    = 22;
static const int iconSize_Shuffle       = 30;

}       //namespace TP

#endif // TP_GLOBALCONST_H
