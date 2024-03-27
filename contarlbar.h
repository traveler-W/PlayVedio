#ifndef CONTARLBAR_H
#define CONTARLBAR_H

#include <QWidget>
#include "help.h"
#include "voice_top.h"
#include <QTimer>
#include <QMutex>
#include "frame_nums.h"

# pragma warning (disable:4819)
namespace Ui {
class contarlbar;
}

class contarlbar : public QWidget
{
    Q_OBJECT

public:
    explicit contarlbar(QWidget *parent = nullptr);
    ~contarlbar();

    QTime change_time(int ss,double aqv);

signals:
    void play_start();
    void button_pos();

    void send_seek_play(double s);
public slots:
    void on_stop_clicked();

    void on_pushButton_clicked();

    void get_total_tmie(int secondsall,int secondstrat);

    void set_volumn(double value);

    void change_seek(double s);

    void set_pslider();

private:
    Ui::contarlbar *ui;
    bool button_styl=false;
    bool is_showtop_voice=false;

    int64_t all_time;
    int64_t start_time;
    QTime av_play_starttime;
    QTime av_play_totaltime;
    QTime av_play_settime;


    bool is_play=false;
    QTimer *timeer;//改变进度条
    QMutex change_slidermutex;


};

#endif // CONTARLBAR_H
