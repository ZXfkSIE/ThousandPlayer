#ifndef TP_GLOBALENUM_H
#define TP_GLOBALENUM_H

namespace TP
{

enum class AudioType
{
    NotSupported,
    FLAC,
    ALAC,
    AAC,
    MP3,
    WAV,
    OGG
};

// First window snaps to the left/right/bottom/top of second window.
enum class SnapType
{
    NonSnap,
    ToLeft,
    ToRight,
    ToBottom,
    ToTop
};


enum class CursorPositionType
{
    NonBorder,
    Left,
    Right,
    Bottom
};

enum class SourceType
{
    SingleFile,
    CueSheet,
    Network
};

enum class PlayMode
{
    SingleTime,
    Repeat,
    Sequential,
    Shuffle
};

enum class ReplayGainMode
{
    Disabled,
    Track,
    Album
};

}       //namespace TP

#endif // TP_GLOBALENUM_H
