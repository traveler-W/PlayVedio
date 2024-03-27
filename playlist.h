#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include "help.h"
#include <QFileDialog>
#include <QListWidget>
#if _MSC_VER >=1600 //VS2010版本号是1600
#pragma execution_character_set("utf-8")
#endif
namespace Ui {
class playlist;
}

class playlist : public QWidget
{
    Q_OBJECT

public:
    explicit playlist(QWidget *parent = nullptr);
    ~playlist();
    void init();
    void contextMenuEvent(QContextMenuEvent *event);
    void setqss_active();
    void connect_signal();
    void addfile_item(QString item);

signals:
    void sendfilename(QString filename,QString file_info);
private:
    Ui::playlist *ui;
    QMenu mymenu;
    QAction add_av;
    QAction remove_av;
    QAction remove_all;
public slots:
    void addfilename();
    void remove_index();
    void remove_allfile();
    void double_list(QListWidgetItem *item);
};

#endif // PLAYLIST_H
