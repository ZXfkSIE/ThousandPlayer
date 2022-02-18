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

public slots:
    void slot_readLyricsFile( const QUrl &I_URL );

private slots:
    void on_pushButton_Return_clicked();
    void on_pushButton_Open_clicked();
    void on_pushButton_Save_clicked();
    void on_pushButton_InsertTimestamp_clicked();

private:
    Ui::TP_LyricsEditor *ui;

    void initializeUI();

    QUrl    currentFileURL;
    qint64  currentPosition;
};

#endif // TP_LYRICSEDITOR_H
