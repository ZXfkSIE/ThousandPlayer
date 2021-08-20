#ifndef TP_FILELISTWIDGET_H
#define TP_FILELISTWIDGET_H

#include <QListWidget>

class TP_Menu;

class TP_FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_FileListWidget(QWidget *parent, const QString &I_qstr);

    void setListName(const QString &I_qstr);
    QString getListName();

public slots:
    void slot_refreshShowingTitle(int idx_Min, int idx_Max);
    void slot_clearAllItems();

private slots:
    void slot_removeSelections();

private:
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    void initializeMenu();

    TP_Menu *menu_rightClick;
    QAction *act_remove;

    QString listName;
    bool    b_isConnected;
    bool    b_isLeftButtonPressed;
};

#endif // TP_FILELISTWIDGET_H
