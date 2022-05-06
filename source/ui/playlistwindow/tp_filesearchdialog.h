#ifndef TP_FILESEARCHDIALOG_H
#define TP_FILESEARCHDIALOG_H

#include <QDialog>

namespace Ui { class TP_FileSearchDialog; }

class TP_FileSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TP_FileSearchDialog( QWidget *parent = nullptr );
    ~TP_FileSearchDialog();

    QString getKeyword() const;
    bool    isFilenameSearched() const;
    bool    isAlbumSearched() const;
    bool    isArtistSearched() const;
    bool    isTitleSearched() const;

private:
    Ui::TP_FileSearchDialog *ui;

    void accept() override;

    QString qstr_keyword;
    bool    b_isFilenameSearched;
    bool    b_isAlbumSearched;
    bool    b_isArtistSearched;
    bool    b_isTitleSearched;
};

#endif // TP_FILESEARCHDIALOG_H
