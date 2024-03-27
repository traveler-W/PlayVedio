#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include "showme.h"
#include "log.h"
#include "dexcodecth.h"
#include "dexcodecthto_vedio.h"
#include "dexcodecthto_audio.h"
#include "vedio_play.h"
#include "audio_play.h"
#include "av_sync.h"
#include <QMouseEvent>
#include "voice_top.h"
#include "frame_nums.h"
# pragma warning (disable:4819)
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    Dexcodecth *Dex;
    dexcodecthto *Dexto_vedio;
    dexcodecthto_audio*Dexto_audio;
    Vedio_play *play_v;
    audio_play *play_a;
    av_sync sysClock;

    void exit_play();
    void start_av(QString filename);

    void connect_siginal();

    //窗口发生大小变化
    void resizeEvent(QResizeEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::Widget *ui;
    QThread *vedio_thread;
    QString filename;
    voice_top *voice_w;
    bool is_strat=false;

    bool show_voice_w=false;


    //实现移动窗口
    QPoint window_pos;
    bool is_ok;
signals:
    void play_vedio();
    void send_titlename(QString filename);

    void Send_playtotaltime(int total_time,int start_time);


    //销毁所创建变量和内存
    void Destorsig();
public slots:
    void  btn_play();
    void exit_all();
    void min_show();
    void minmin_show();
    void max_show();

    void getfilename(QString filename,QString file_info);
    void send_title_name();

    //显示声音按键
    void get_buttonpos();
};
#endif // WIDGET_H
