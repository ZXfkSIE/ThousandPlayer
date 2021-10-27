﻿#ifndef TP_AUDIOINFOLABEL_H
#define TP_AUDIOINFOLABEL_H

#include <QLabel>

class QTimer;

class TP_AudioInfoLabel : public QLabel
{
    Q_OBJECT

public:
    explicit TP_AudioInfoLabel ( QWidget *parent );

    void initialize();
    void setStrings ( std::vector< QString > &&I_vec_Qstr );

private slots:
    void slot_timerIsTriggered();

private:
    void paintEvent ( QPaintEvent * ) override;

    std::vector< QString > vec_Qstr;

    QTimer *timer;

    bool b_isInitialPainting;
    bool b_isScrolling;
    bool b_isTimerTriggered;

    int initialX, initialMiddleY;
    int x, y;
    unsigned currentIdx;
};

#endif // TP_AUDIOINFOLABEL_H
