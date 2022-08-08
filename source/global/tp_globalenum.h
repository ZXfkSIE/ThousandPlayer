#ifndef TP_GLOBALENUM_H
#define TP_GLOBALENUM_H

namespace TP
{

// First window snaps at the left/right/bottom/top of second window.
enum class SnapType
{
    NonSnap,
    ToLeft,
    ToRight,
    ToBottom,
    ToTop
};

/*
enum class ResizeType
{
    Left,
    Right,
    Bottom
};
*/

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
