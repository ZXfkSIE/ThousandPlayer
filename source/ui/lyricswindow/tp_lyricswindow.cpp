#include "tp_lyricswindow.h"
#include "ui_tp_lyricswindow.h"

TP_LyricsWindow::TP_LyricsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TP_LyricsWindow)
{
    ui->setupUi(this);
}

TP_LyricsWindow::~TP_LyricsWindow()
{
    delete ui;
}
