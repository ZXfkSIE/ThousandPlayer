#ifndef TP_GLOBALCONST_H
#define TP_GLOBALCOSNT_H

#include <string>

namespace TP
{

static const int borderSize = 5;

static const std::string configDirectoryPath    = "config";
static const std::string playlistFilePath       = configDirectoryPath + "/playlists.xml";

static const int role_Duration      = 0x0100;
static const int role_Artist        = 0x0101;
static const int role_Title         = 0x0102;
static const int role_Album         = 0x0103;
static const int role_FileName      = 0x0104;
static const int role_FileType      = 0x0105;
static const int role_Path          = 0x0106;
static const int role_Description   = 0x0107;

static const int iconSize_Repeat = 28;
static const int iconSize_Play = 20;
static const int iconSize_Pause = 17;

}       //namespace TP

#endif // TP_GLOBALCONST_H
