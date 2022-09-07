#ifndef TP_GLOBALENUM_H
#define TP_GLOBALENUM_H

namespace TP
{

enum class AudioFormat
{
    NotSupported,
    FLAC,
    ALAC,
    AAC,
    MP3,
    WAV,
    OGG
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
