#ifndef TP_GLOBALVARIABLE_H
#define TP_GLOBALVARIABLE_H

#include "tp_config.h"

#include <QMediaPlayer>

#include <random>

class QListWidgetItem;

namespace TP
{
    TP_Config &                     config();
    std::mt19937 &                  randomEngine();
    QListWidgetItem *&              currentItem();
    QMediaPlayer::PlaybackState &   playbackState();
}

#endif // TP_GLOBALVARIABLE_H
