#include "audio_play.h"
#include <QDebug>
extern Myqueue<AVFrame> vedio_frame;
extern Myqueue<AVPacket> vedio_packet;
extern Myqueue<AVPacket> audio_packet;
extern Myqueue<AVFrame> audio_frame;

extern Frame_nums<AVFrame> faudio_nums;
audio_play::audio_play(av_sync*s,AVRational time_base,const AVCodecParameters*audio_params)
{
    src_tgt.ch_layout=audio_params->ch_layout;
    //src_tgt.channels=audio_params->channels;
    //src_tgt.channel_layout=audio_params->channel_layout;
    src_tgt.fmt=(enum AVSampleFormat)audio_params->format;
    src_tgt.freq=audio_params->sample_rate;
    src_tgt.frame_size=audio_params->frame_size;
    sync=s;
    ration=time_base;

}

//更新音量的大小
void audio_play::UpdateVolume(int sign, double step)
{
    double volume_level = audio_value ? (20 * log(audio_value / (double)SDL_MIX_MAXVOLUME) / log(10)) : -1000.0;
    int new_volume = lrint(SDL_MIX_MAXVOLUME * pow(10.0, (volume_level + sign * step) / 20.0));
    audio_value = av_clip(audio_value == new_volume ? (audio_value + sign) : new_volume, 0, SDL_MIX_MAXVOLUME);

    //emit SigVideoVolume(m_CurStream->audio_volume * 1.0 / SDL_MIX_MAXVOLUME);
}

void audio_play::set_Volume(double volume_value)
{
    temp_voice_value=volume_value*SDL_MIX_MAXVOLUME;
    audio_value=temp_voice_value;
}

void audio_play::setaudio_index(double ss)
{
    audio_index_mutex.lock();
    faudio_nums.v_index=qRound(faudio_nums.V.size()*ss);
    audio_index_mutex.unlock();
}

void fill_audio_pcm(void *udata,Uint8*stream,int len)
{
    audio_play *is=(audio_play*)udata;
    int len1=0;
    int audio_size=0;

    if(faudio_nums.get_pause())
    {
        memset(stream,0,len);
        return;
    }
    while(len>0)
    {
        if(is->audio_buf_index==is->audio_buf_size){//判断这一帧数据是否读取完毕
            is->audio_buf_index=0;
            AVFrame*frame;

            if(vedio_frame.RtspStream)
            {
               frame =audio_frame.pop();
            }else{
               //数组中取出数据
               frame=faudio_nums.frame_pop();
               if(faudio_nums.v_index>=faudio_nums.V.size())
               {
                   return;
               }
            }

            if(frame)
            {

                is->pts=frame->pts;
                if(((frame->format!=is->dst_tgt.fmt)
                        ||(frame->sample_rate!=is->dst_tgt.freq)
                        ||av_channel_layout_compare(&frame->ch_layout,&is->dst_tgt.ch_layout))
                    &&(!is->swr_ctx)){
                    swr_alloc_set_opts2(&is->swr_ctx,//对重采样上下文环境进行参数设置
                                                   &is->dst_tgt.ch_layout,//要输出的采样声道
                                                   (enum AVSampleFormat)is->dst_tgt.fmt,//输出的采样格式
                                                   is->dst_tgt.freq,//输出的采样率
                                                   &frame->ch_layout,//输入的采样声道
                                                   (enum AVSampleFormat)frame->format,//输入的采样格式
                                                   frame->sample_rate,//输入采样率
                                                   0,NULL);//不用管日志相关
                    if(!is->swr_ctx||swr_init(is->swr_ctx)<0)//初始化采样器
                    {
                        swr_free((SwrContext**)(&is->swr_ctx));
                        return;
                    }
                }
                if(is->swr_ctx){//重采样
                    const uint8_t**in =(const uint8_t**)frame->extended_data;//要转换的数据---重采样
                    uint8_t**out =&is->audio_buf1;//采样后数据存放的位置
                    int out_samples =frame->nb_samples*is->dst_tgt.freq/frame->sample_rate+256;//计算样本数
                    //计算冲重采样后的参数所需空间大小
                    int out_byte =av_samples_get_buffer_size(NULL,is->dst_tgt.ch_layout.nb_channels,out_samples,is->dst_tgt.fmt,0);
                    if(out_byte<0)
                    {
                        return;
                    }
                    av_fast_malloc(&is->audio_buf1,&is->audio_buf1_size,out_byte);//为buf1快速分配内存
                    int len2 =swr_convert(is->swr_ctx,out,out_samples,in,frame->nb_samples);//将输入的音频按照定义的参数进转换并输出
                    if(len2<0)
                        return;
                    is->audio_buf=is->audio_buf1;
                    is->audio_buf_size=av_samples_get_buffer_size(NULL,is->dst_tgt.ch_layout.nb_channels,len2,is->dst_tgt.fmt,1);

                }else{//没有重新采样
                    //计算这个音频数据的大小
                    audio_size=av_samples_get_buffer_size(NULL,frame->ch_layout.nb_channels,frame->nb_samples,(enum AVSampleFormat)frame->format,1);
                    av_fast_malloc(&is->audio_buf1,&is->audio_buf1_size,audio_size);//为其宿主分配内存
                    is->audio_buf=is->audio_buf1;
                    is->audio_buf_size=audio_size;
                    memcpy(is->audio_buf,frame->data[0],audio_size);//填入数据
                }
                if(vedio_frame.RtspStream)
                {
                    av_frame_free(&frame);
                }
            }else{
                is->audio_buf=NULL;
                is->audio_buf_size=512;
            }
        }
        len1=is->audio_buf_size-is->audio_buf_index;//确保返回的数据不能大于回调函数可以接受的值
        if(len1>len)
        {
            len1=len;
        }
        if(is->audio_buf&&is->audio_value == SDL_MIX_MAXVOLUME)
        {
            //
            memcpy(stream,is->audio_buf+is->audio_buf_index,len1);
            //SDL_MixAudioFormat(stream,(uint8_t*)is->audio_buf+is->audio_buf_index,AUDIO_S16SYS,len1,is->audio_value);
        }else{
             memset(stream,0,len1);
             if(is->audio_buf)
             {
                 SDL_MixAudioFormat(stream,(uint8_t*)is->audio_buf+is->audio_buf_index,AUDIO_S16SYS,len1,is->audio_value);
             }


        }
//        if(!is->audio_buf)
//        {
//            memset(stream,0,len1);
//        }else{
//            memcpy(stream,is->audio_buf+is->audio_buf_index,len1);
//            //更新音量大小
//            SDL_MixAudioFormat(stream,(uint8_t*)is->audio_buf+is->audio_buf_index,AUDIO_S16SYS,len1,is->audio_value);
//        }
        len-=len1;
        stream+=len1;
        is->audio_buf_index+=len1;
    }
    if(is->pts!=AV_NOPTS_VALUE)
    {
        double pts_=is->pts*av_q2d(is->ration);
        //qDebug()<<pts_;
        is->sync->Setclock(pts_);
    }
}



int audio_play::Init()
{
    if(SDL_Init(SDL_INIT_AUDIO!=0))
    {
        qDebug("SDL_INIT error from audioinput.cpp");
        return -1;
    }
    SDL_AudioSpec wanted_spec;//想要的参数和实际输出的参数
    wanted_spec.channels=2;
    wanted_spec.freq=src_tgt.freq;
    wanted_spec.format=AUDIO_S16SYS;
    wanted_spec.silence=0;
    wanted_spec.callback=fill_audio_pcm;//回调函数的调用
    wanted_spec.userdata=this;
    wanted_spec.samples=1024;//采样数量
    int ret=SDL_OpenAudio(&wanted_spec,NULL);
    if(ret<0)
    {
        qDebug("SDL_openaudio erroe from audioinput.cpp 33");
        return -1;
    }
    av_channel_layout_default(&dst_tgt.ch_layout,wanted_spec.channels);
    //dst_tgt.channels=spec.channels;
    dst_tgt.fmt=AV_SAMPLE_FMT_S16;
    dst_tgt.freq=wanted_spec.freq;
    //dst_tgt.channel_layout=av_get_default_channel_layout(src_tgt.channels);
    dst_tgt.frame_size=wanted_spec.samples;
    SDL_PauseAudio(0);

    return 0;

}

int audio_play::DeInit()
{
    SDL_PauseAudio(0);
    SDL_CloseAudio();

    return 0;
}
