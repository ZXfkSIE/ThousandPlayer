#ifndef TP_GLOBALENUM_H
#define TP_GLOBALENUM_H

namespace TP
{

enum CursorPositionType
{
    notAtBorder,
    leftBorder,
    rightBorder,
    bottomBorder
};

enum VisualContainerType
{
    albumCover,
    barVisualizer
};

enum VolumeStatus
{
    mute,
    sound
};

enum FileFormat
{
    FLAC,
    MP3
};

enum SourceType
{
    single,
    cueSheet,
    network
};

}       //namespace TP

#endif // TP_GLOBALENUM_H
