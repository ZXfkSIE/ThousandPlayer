#ifndef TP_LYRICSEDITOR_H
#define TP_LYRICSEDITOR_H

#include <QUrl>
#include <QWidget>

namespace Ui {
class TP_LyricsEditor;
}

class TP_LyricsEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TP_LyricsEditor( QWidget *parent = nullptr );
    ~TP_LyricsEditor();

    void setCurrentPosition( qint64 I_ms );
    void refreshFont();
    void readLyricsFile( const QUrl &I_url );

    bool saveLyricsFileBeforeQuit();

signals:
    void signal_switchToLyricsViewer( const QUrl &url );

private slots:
    void on_pushButton_Return_clicked();
    void on_pushButton_Open_clicked();
    void on_pushButton_Save_clicked();
    void on_pushButton_InsertTimestamp_clicked();

private:
    Ui::TP_LyricsEditor *ui;

    void initializeUI();

    void returnToLyricsViewer( const QUrl &I_url );
    bool openSaveFileDialog();

    QUrl    currentFileURL;
    qint64  currentPosition;
    QShortcut *shortcut_F8;
};

#endif // TP_LYRICSEDITOR_H
