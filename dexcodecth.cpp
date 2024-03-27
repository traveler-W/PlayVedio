#include "dexcodecth.h"
extern Myqueue<AVPacket> vedio_packet;
extern Myqueue<AVPacket> audio_packet;
Dexcodecth::Dexcodecth()
{
    abort=1;
}
//读取解封装后的包进队列
void Dexcodecth::run()
{
    while(abort!=0)
    {
        AVPacket *pkt =av_packet_alloc();
        av_read_frame(f_ctx,pkt);

        if(pkt->stream_index==vedio_index)
        {
            vedio_packet.push(pkt);
            //av_packet_unref(pkt);
        }
        else if(pkt->stream_index==audio_index)
        {
            audio_packet.push(pkt);

        }
        else{
            av_packet_unref(pkt);
        }

    }

}


void Dexcodecth::init(const char *filename)
{
    f_ctx=avformat_alloc_context();
    int ret =0;
    ret = avformat_open_input(&f_ctx,filename,NULL,NULL);
    if(ret!=0)
    {
        LOG(logtype::ERRORLOG,"error this open filename %d",ret);

    }
    if((ret=avformat_find_stream_info(f_ctx,NULL))!=0)
    {
        LOG(logtype::ERRORLOG,"error this file filename %d",ret);

    }
    //将视频的总长度发送给控制面板

    Secondall=f_ctx->duration;
    Secondstart=f_ctx->start_time;


    //寻找流信息
    for(unsigned i=0;i<f_ctx->nb_streams;i++)
    {
        AVStream *pstream=f_ctx->streams[i];
        if(pstream->codecpar->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            vedio_index=i;
            qDebug()<<"视频下标:"<<vedio_index;
            qDebug()<<"width:"<<pstream->codecpar->width;
            qDebug()<<"heigth:"<<pstream->codecpar->height;
        }
        else if(pstream->codecpar->codec_type==AVMEDIA_TYPE_AUDIO)
        {
            audio_index=i;
            qDebug()<<"音频下标:"<<audio_index;
        }
    }
}

AVCodecParameters *Dexcodecth::AudioCodecParamters()
{
    if(audio_index!=-1)
    {
       return f_ctx->streams[audio_index]->codecpar;
    }else{
        return NULL;
    }
}

AVCodecParameters *Dexcodecth::VedioCodecParamters()
{
    if(vedio_index!=-1)
    {
       return f_ctx->streams[vedio_index]->codecpar;
    }else{
        return NULL;
    }
}

AVRational Dexcodecth::Audio_timebase()
{
    if(audio_index!=-1)
    {
       return f_ctx->streams[audio_index]->time_base;
    }else{
        return AVRational{0,0};
    }
}

AVRational Dexcodecth::Vedio_timebase()
{
    if(vedio_index!=-1)
    {
       return f_ctx->streams[vedio_index]->time_base;
    }else{
        return AVRational{0,0};
    }
}
