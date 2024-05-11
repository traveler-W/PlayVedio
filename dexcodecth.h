#ifndef DEXCODECTH_H
#define DEXCODECTH_H
#include "include_head.h"
#include <QObject>
#include <QDebug>
#include <QThread>
#include "log.h"
#include "myqueue.h"
# pragma execution_character_set("utf-8")
class Dexcodecth:public QThread
{
    //Q_OBJECT
public:
    Dexcodecth();
    ~Dexcodecth()
    {       
        f_ctx=nullptr;
        delete f_ctx;
    }
    void run() override;
    void init(const char*filename);
    AVCodecParameters* AudioCodecParamters();
    AVCodecParameters* VedioCodecParamters();

    AVRational Audio_timebase();
    AVRational Vedio_timebase();

    play_time get_seconds()
    {
        play_time t;
        t.time_all=Secondall;
        t.time_start=Secondstart;
        return t;
    }
    int abort;

    bool RtspStream=false;

private:
    int vedio_index=0;
    int audio_index=0;
    QString file_url;
      //解复用器
    AVFormatContext *f_ctx;
    int Secondall;
    int Secondstart;



};

#endif // DEXCODECTH_H
