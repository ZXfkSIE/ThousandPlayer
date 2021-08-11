#ifndef TP_MAINCLASS_H
#define TP_MAINCLASS_H

#include <QWidget>

class TP_MainWindow;
class TP_PlaylistWindow;

class TP_MainClass : public QObject
{
    Q_OBJECT

public:
    explicit TP_MainClass();
    ~TP_MainClass();

public slots:
    void initializePosition();

private:
    void connect();

    TP_MainWindow       *tp_MainWindow;
    TP_PlaylistWindow   *tp_PlaylistWindow;
};

#endif // TP_MAINCLASS_H
