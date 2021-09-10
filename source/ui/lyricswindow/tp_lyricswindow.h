#ifndef TP_LYRICSWINDOW_H
#define TP_LYRICSWINDOW_H

#include <QWidget>

namespace Ui {
class TP_LyricsWindow;
}

class TP_LyricsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_LyricsWindow(QWidget *parent = nullptr);
    ~TP_LyricsWindow();

private:
    Ui::TP_LyricsWindow *ui;
};

#endif // TP_LYRICSWINDOW_H
