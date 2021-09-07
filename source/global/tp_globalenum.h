#ifndef TP_GLOBALENUM_H
#define TP_GLOBALENUM_H

namespace TP
{

// First window snaps at the left/right/bottom/top of second window.
enum SnapType
{
    notAdjacent,
    toLeft,
    toRight,
    toBottom,
    toTop
};

enum ResizeType
{
    atLeft,
    atRight,
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

enum SourceType
{
    singleFile,
    cueSheet,
    network
};

enum PlayMode
{
    singleTime,
    repeat,
    sequential,
    shuffle
};

}       //namespace TP

#endif // TP_GLOBALENUM_H
