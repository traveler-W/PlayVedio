﻿#ifndef DEXCODECTHTO_AUDIO_H
#define DEXCODECTHTO_AUDIO_H

#include <QObject>
#include <QThread>
#include "myqueue.h"
#include "frame_nums.h"



class dexcodecthto_audio:public QThread
{
public:
    dexcodecthto_audio();
    ~dexcodecthto_audio(){

    }
    void run() override;
    int abort;
    int is_create=0;
    void init(AVCodecParameters *par);


private:
    AVDictionary *pAVDictionary = 0;//配置编解码器的一些相关配置
    StreamType type;
    AVCodecContext* codec_ctx;//解码器相关结构体
    int Packet_type;
    const AVCodec *codec;//解码器
};

#endif // DEXCODECTHTO_AUDIO_H
