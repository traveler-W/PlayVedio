#include "contarlbar.h"
#include "ui_contarlbar.h"
#include <QDebug>

extern Frame_nums<AVFrame> fvedio_nums;
extern Frame_nums<AVFrame> faudio_nums;

contarlbar::contarlbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::contarlbar)
{
    ui->setupUi(this);
    QString qss=help::Get_Qss("./contral-qss.txt");
    setStyleSheet(qss);
    timeer=new QTimer(this);
    connect(ui->verticalSlider,&QSlider::sliderMoved,this,&contarlbar::change_seek);
    connect(timeer,&QTimer::timeout,[this](){
            set_pslider();
        ;});
    timeer->start(1000);

}

contarlbar::~contarlbar()
{

    delete ui;
}

QTime contarlbar::change_time(int ss,double aqv)
{
    int thh,tmm,tss;
    ss=qRound(ss*aqv);
    thh = ss / 3600;
    tmm = (ss % 3600) / 60;
    tss = (ss % 60);
    QTime TotalTime(thh, tmm, tss);
    return TotalTime;
}

void contarlbar::on_stop_clicked()
{
    if(button_styl==true)
    {
        ui->stop->setIcon(QIcon(":/src/start.png"));
        faudio_nums.change_pause(true);
        fvedio_nums.change_pause(true);
        //qDebug()<<faudio_nums.get_pause();
        button_styl=false;
    }
    else{
        ui->stop->setIcon(QIcon(":/src/stop.png"));
        fvedio_nums.change_pause(false);
        faudio_nums.change_pause(false);
        //qDebug()<<faudio_nums.get_pause();
        button_styl=true;
    }
    ui->stop->setIconSize(QSize(50,50));   

    //开始播放音视频
    if(!is_play)
    {
        emit play_start();
        is_play=true;
    }

}

void contarlbar::on_pushButton_clicked()
{
    emit button_pos();
}

//设置视频总时长
void contarlbar::get_total_tmie(int nSeconds,int start)
{
    all_time=nSeconds/1000000LL;
    start_time=start/1000000LL;


    av_play_totaltime=change_time(all_time,1);
    ui->time_end->setTime(av_play_totaltime);
}

//设置音量大小
void contarlbar::set_volumn(double value)
{
    Q_UNUSED(value);
}

void contarlbar::change_seek(double s)
{
    double ss=s*1.0/ui->verticalSlider->maximum();
    emit send_seek_play(ss);
    av_play_settime=av_play_totaltime;

    //qDebug()<<s;
    ui->time_start->setTime(change_time((all_time-start_time),ss));
}

void contarlbar::set_pslider()
{
    float temp=0.000;
    change_slidermutex.lock();
    temp=fvedio_nums.v_index*1.000/fvedio_nums.V.size();
    change_slidermutex.unlock();
    ui->verticalSlider->setValue(ui->verticalSlider->maximum()*temp);
    ui->time_start->setTime(change_time((all_time-start_time),temp));

}
