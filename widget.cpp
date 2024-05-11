#include "widget.h"
#include "ui_widget.h"
#include "include_head.h"
# pragma warning (disable:4819)
extern Myqueue<AVFrame> vedio_frame;
extern Myqueue<AVPacket> vedio_packet;
extern Myqueue<AVPacket> audio_packet;
extern Myqueue<AVFrame> audio_frame;


extern Frame_nums<AVFrame> faudio_nums;
extern Frame_nums<AVFrame> fvedio_nums;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    qRegisterMetaType<WId>("WId");
    ui->setupUi(this);
    //setWindowFlags(Qt::FramelessWindowHint /*| Qt::WindowSystemMenuHint*/ | Qt::WindowMinimizeButtonHint);

    //设置为自定义窗口
    this->setWindowFlags(Qt::CustomizeWindowHint);


    //删除dockwidget的标题栏
    QWidget* TitleBar = ui->dockWidget->titleBarWidget();
    QWidget* EmptyWidget = new QWidget();
    ui->dockWidget->setTitleBarWidget(EmptyWidget);
    delete TitleBar;

    voice_w=new voice_top(this);
    voice_w->close();

//    //解复用
//    Dex=new Dexcodecth();
//    Dex->init("C:\\Users\\DELL.DESKTOP-EBC04UD\\Documents\\Tencent Files\\2477438674\\FileRecv\\MobileFile\\vedio\\ss.mp4");
//    Dex->start();
//    //音视频同步
//    sysClock.init();

//    //视频解码
//    Dexto_vedio=new dexcodecthto();
//    Dexto_vedio->init(Dex->VedioCodecParamters());
//    Dexto_vedio->start();

//    //声音解码
//    Dexto_audio=new dexcodecthto_audio();
//    Dexto_audio->init(Dex->AudioCodecParamters());
//    Dexto_audio->start();

//    //播放音频
//    play_a=new audio_play(&sysClock,Dex->Audio_timebase(),Dex->AudioCodecParamters());
//    play_a->Init();

//    //播放视频
//    vedio_thread=new QThread();
//    play_v=new Vedio_play(&sysClock,Dex->Vedio_timebase(),Dex->VedioCodecParamters()->width,Dex->VedioCodecParamters()->height);
//    play_v->moveToThread(vedio_thread);
//    vedio_thread->start();




//    connect(ui->widget,&showme::init_window,play_v,&Vedio_play::init);

//    connect(play_v,&Vedio_play::siginal_rect,ui->widget,&showme::width_height);


    //标题的信号连接
    connect(ui->title_widget,&title::min_siginal,this,&Widget::min_show);
    connect(ui->title_widget,&title::max_siginal,this,&Widget::max_show);
    connect(ui->title_widget,&title::minmin_siginal,this,&Widget::minmin_show);
    connect(ui->title_widget,&title::exit_app,this,&Widget::exit_all);
    connect(this,&Widget::send_titlename,ui->title_widget,&title::set_playtitle);



//    //控制按键信号连接
//    connect(ui->contralbar,&contarlbar::play_start,play_a,&audio_play::Init);
//    connect(ui->contralbar,&contarlbar::play_start,ui->widget,&showme::create_window);
//    connect(ui->contralbar,&contarlbar::play_start,play_v,&Vedio_play::MainLoop);
    connect(ui->contralbar,&contarlbar::button_pos,this,&Widget::get_buttonpos);
    connect(ui->contralbar,&contarlbar::play_start,this,&Widget::send_title_name);

    //列表信号连接
    connect(ui->playlistwidget,&playlist::sendfilename,this,&Widget::getfilename);


    exit_play();



}

Widget::~Widget()
{
    for(auto i:faudio_nums.V)
    {
        av_frame_free(&i);
    }
    for(auto j:fvedio_nums.V)
    {
        av_frame_free(&j);
    }

    if(is_strat)
    {
        Dex->abort=0;
        Dexto_vedio->abort=0;
        Dexto_audio->abort=0;
        play_v->abort=0;
        vedio_packet.wake_allthread();
        audio_packet.wake_allthread();
        vedio_frame.wake_allthread();
        audio_frame.wake_allthread();
        emit Destorsig();
    }
    if(Dex)
    {
        Dex=nullptr;
        delete Dex;
    }

    if(Dexto_audio)
    {
        Dexto_audio=nullptr;
        delete Dexto_audio;
    }
    if(Dexto_vedio)
    {

        Dexto_vedio=nullptr;
        delete Dexto_vedio;

    }

    if(play_v)
    {

        play_v=nullptr;
         delete play_v;

    }


    if(play_a)
    {
         play_a->DeInit();
         play_a=nullptr;
         delete play_a;

    }

   if(vedio_thread)
   {
       vedio_thread=nullptr;
       delete vedio_thread;

   }


    delete ui;
}
//这里是设置窗口的拖拽
void Widget::exit_play()
{

    connect(this,&Widget::Destorsig,play_v,&Vedio_play::Destroy);
}

void Widget::start_av(QString filename)
{
    is_strat=true;
    //解复用
    Dex=new Dexcodecth();
    Dex->init(filename.toStdString().c_str());
    Dex->start();
    //音视频同步
    sysClock.init();

    //视频解码
    Dexto_vedio=new dexcodecthto();
    Dexto_vedio->init(Dex->VedioCodecParamters());
    Dexto_vedio->start();

    //声音解码
    Dexto_audio=new dexcodecthto_audio();
    Dexto_audio->init(Dex->AudioCodecParamters());
    Dexto_audio->start();

    //播放音频
    play_a=new audio_play(&sysClock,Dex->Audio_timebase(),Dex->AudioCodecParamters());

    //播放视频
    vedio_thread=new QThread();
    play_v=new Vedio_play(&sysClock,Dex->Vedio_timebase(),Dex->VedioCodecParamters()->width,Dex->VedioCodecParamters()->height);
    play_v->moveToThread(vedio_thread);
    vedio_thread->start();
    connect_siginal();

}

void Widget::connect_siginal()
{
    connect(ui->widget,&showme::init_window,play_v,&Vedio_play::init);

    connect(play_v,&Vedio_play::siginal_rect,ui->widget,&showme::width_height);


    connect(voice_w,&voice_top::Send_vulume_value,[this](double scends){
            play_a->set_Volume(scends);
        ;});


    connect(this,&Widget::Send_playtotaltime,ui->contralbar,&contarlbar::get_total_tmie);

    //控制按键信号连接
    //实现拖拽到某个为止的功能
    connect(ui->contralbar,&contarlbar::send_seek_play,[this](double ss){
       play_v->setvedio_index(ss);
       play_a->setaudio_index(ss);
        ;});
    connect(ui->contralbar,&contarlbar::play_start,play_a,&audio_play::Init);
    connect(ui->contralbar,&contarlbar::play_start,ui->widget,&showme::create_window);
    connect(ui->contralbar,&contarlbar::play_start,play_v,&Vedio_play::MainLoop);
    //发出总视频时长大小给控制条
    emit Send_playtotaltime(Dex->get_seconds().time_all,Dex->get_seconds().time_start);
    emit voice_w->audio_sliderchange(50);


}

void Widget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    voice_w->close();
    show_voice_w=false;
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{

    if(is_ok)
    {
        move(event->globalPos()-window_pos);
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()&Qt::LeftButton)
    {
        if(ui->title_widget->geometry().contains(event->pos()))
        {
            is_ok=true;
            window_pos=event->globalPos()-this->pos();
        }

    }

}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    is_ok=false;
}



void Widget::btn_play()
{
    emit play_vedio();
}

//退出
void Widget::exit_all()
{
    this->close();
}
//最小化
void Widget::min_show()
{
    this->showNormal();

}
//最小最小化
void Widget::minmin_show()
{
    this->showMinimized();
}

//最大化
void Widget::max_show()
{
    this->showMaximized();
}

void Widget::getfilename(QString filename,QString file_info)
{
    start_av(filename);
    this->filename=file_info;
}

void Widget::send_title_name()
{
    emit send_titlename(this->filename);
}

void Widget::get_buttonpos()
{
    if(!show_voice_w)
    {
        voice_w->move(ui->contralbar->pos().x()+ui->contralbar->width()-50,ui->contralbar->pos().y()-220);
        voice_w->show();
        show_voice_w=true;

    }
    else{
        voice_w->close();
        show_voice_w=false;
    }

}

