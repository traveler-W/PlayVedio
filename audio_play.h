#ifndef AUDIO_PLAY_H
#define AUDIO_PLAY_H

#include <QObject>
#include <SDL.h>
#include "include_head.h"
#include "myqueue.h"
#include "frame_nums.h"
#include "av_sync.h"
#include <QMutex>
typedef struct AudioParams
{
    int freq;//采样率
    AVChannelLayout ch_layout;//音频通道数
    //int channels;
    //int64_t channel_layout;
    enum AVSampleFormat fmt;//采样方式
    int frame_size;
}AudioParams;

class audio_play:public QObject
{
    //Q_OBJECT
public:
    audio_play(av_sync *s,AVRational time_base,const AVCodecParameters*audio_params);

    int Init();
    int DeInit();

    AudioParams src_tgt;//解码后的参数
    AudioParams dst_tgt;//sdl实际输出的参数
    int64_t pts=AV_NOPTS_VALUE;

    struct SwrContext *swr_ctx=NULL;//重采样上下文还清
    uint8_t*audio_buf=nullptr;
    uint8_t*audio_buf1=nullptr;
    uint32_t audio_buf_size=0;
    uint32_t audio_buf1_size=0;
    uint32_t audio_buf_index=0;

     double temp_voice_value;

    av_sync*sync;
    AVRational ration;
    double audio_value;
    void set_Volume(double volume_value);

    void setaudio_index(double ss);
private:


    void UpdateVolume(int sign, double step);
    QMutex audio_index_mutex;




};

#endif // AUDIO_PLAY_H
