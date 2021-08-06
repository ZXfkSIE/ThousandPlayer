#ifndef TP_GLOBALCONST_H
#define TP_GLOBALCOSNT_H

#include <string>

namespace TP
{

static const int borderSize = 5;

static const std::string configDirectoryPath = "config";
static const std::string playlistFilePath = configDirectoryPath + "/playlists.xml";

// 0: No., 1: description, 2: duration, 3: artist,
// 4: title, 5: album, 6: filename, 7: path
static const int index_No = 0;
static const int index_Description = 1;
static const int index_Duration = 2;
static const int index_Artist = 3;
static const int index_Title = 4;
static const int index_Album = 5;
static const int index_Filename = 6;
static const int index_Path = 7;

}       //namespace TP

#endif // TP_GLOBALCONST_H
