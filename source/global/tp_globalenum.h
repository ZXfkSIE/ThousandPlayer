#ifndef TP_GLOBALENUM_H
#define TP_GLOBALENUM_H

namespace TP
{

enum WindowType
{
    mainWindow,
    playlistWindow,
    equalizerWindow,
    lyricsWindow
};

enum SnapStatus
{
    notSnapped,
    snapped,
    pending
};

// At some direction of the main window
enum SnapType
{
    atLeft,
    atRight,
    atTop,
    atBottom
};

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

enum PlayMode
{
    singleTime,
    repeat,
    sequential,
    sequentialLoop,
    shuffle
};

}       //namespace TP

#endif // TP_GLOBALENUM_H
