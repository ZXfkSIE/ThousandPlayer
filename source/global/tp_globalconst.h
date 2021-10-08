#ifndef TP_GLOBALCONST_H
#define TP_GLOBALCOSNT_H

#include <QString>

namespace TP
{

static const unsigned mainWindow        = 0;
static const unsigned playlistWindow    = 1;
static const unsigned lyricsWindow      = 2;

static const int borderSize = 5;
static const int snapRange  = 8;

static const QString configDirectoryPath    { "./userData" };
static const QString configFilePath         { configDirectoryPath + QString { "/config.ini" } };
static const QString playlistFilePath       { configDirectoryPath + QString { "/playlists.xml" } };

static const int role_Duration      = 0x0100;   // int
static const int role_Bitrate       = 0x0101;   // int
static const int role_SampleRate    = 0x0102;   // int
static const int role_BitDepth      = 0x0103;   // int
static const int role_Artist        = 0x0104;   // QString
static const int role_Title         = 0x0105;   // QString
static const int role_Album         = 0x0106;   // QString
static const int role_FileName      = 0x0107;   // QString
static const int role_URL           = 0x0108;   // QUrl
static const int role_SourceType    = 0x0109;   // TP::SourceType
static const int role_Description   = 0x010A;   // QString


static const int iconSize_Play          = 20;
static const int iconSize_Pause         = 18;

static const int iconSize_SingleTime    = 26;
static const int iconSize_Repeat        = 28;
static const int iconSize_Sequential    = 22;
static const int iconSize_Shuffle       = 29;

}       //namespace TP

#endif // TP_GLOBALCONST_H
