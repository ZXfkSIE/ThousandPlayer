#include "tp_globalvariable.h"

#include <QAudioDevice>
#include <QStandardPaths>

TP_Config &
TP::config()
{
    static TP_Config config;
    return config;
}


std::mt19937 &
TP::randomEngine()
{
    static std::random_device randomDevice;
    static std::mt19937 randomGenerator { randomDevice() };
    return randomGenerator;
}


QListWidgetItem *&
TP::currentItem()
{
    static QListWidgetItem *item { nullptr };
    return item;
}


QMediaPlayer::PlaybackState &
TP::playbackState()
{
    static QMediaPlayer::PlaybackState state {};
    return state;
}


QString
TP::playlistFilePath()
{
    static const QString path {
        QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation)
                + "/playlists.json"
    };
    return path;
}
