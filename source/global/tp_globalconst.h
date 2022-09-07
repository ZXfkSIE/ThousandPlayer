#ifndef TP_GLOBALCONST_H
#define TP_GLOBALCONST_H

#include <QStandardPaths>
#include <QString>

namespace TP
{

static const unsigned numberOfWindows   = 3;
static const unsigned mainWindow        = 0;
static const unsigned playlistWindow    = 1;
static const unsigned lyricsWindow      = 2;

static const int borderSize = 5;
static const int snapRange  = 8;

static const int role_Duration          = Qt::UserRole;         // int              must have
static const int role_Bitrate           = Qt::UserRole + 1;     // int
static const int role_SampleRate        = Qt::UserRole + 2;     // int
static const int role_BitDepth          = Qt::UserRole + 3;     // int
static const int role_Artist            = Qt::UserRole + 4;     // QString
static const int role_Title             = Qt::UserRole + 5;     // QString
static const int role_Album             = Qt::UserRole + 6;     // QString
static const int role_FileName          = Qt::UserRole + 7;     // QString          must have
static const int role_AudioFormat         = Qt::UserRole + 8;     // TP::AudioFormat    must have
static const int role_URL               = Qt::UserRole + 9;     // QUrl             must have
static const int role_SourceType        = Qt::UserRole + 10;    // TP::SourceType   must have
static const int role_Description       = Qt::UserRole + 11;    // QString          must have
static const int role_LastModified      = Qt::UserRole + 12;    // QDateTime        must have
static const int role_ReplayGainTrack   = Qt::UserRole + 13;    // float
static const int role_ReplayGainAlbum   = Qt::UserRole + 14;    // float

static const int role_FileListAddress   = Qt::UserRole;         // TP_FileListWidget * (stored as quintptr)

static const int iconSize_Play          = 20;
static const int iconSize_Pause         = 18;

static const int iconSize_SingleTime    = 26;
static const int iconSize_Repeat        = 28;
static const int iconSize_Sequential    = 22;
static const int iconSize_Shuffle       = 29;

static const QString playlistFilePath {
    QStandardPaths::writableLocation(
#ifdef Q_OS_LINUX
                QStandardPaths::AppConfigLocation
#endif
#ifdef Q_OS_WIN
                QStandardPaths::AppDataLocation
#endif
                )
            + "/playlists.json"
};

}       //namespace TP

#endif // TP_GLOBALCONST_H
