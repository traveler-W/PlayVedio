#include "dexcodecthto_vedio.h"
#include <QDebug>
extern Myqueue<AVFrame> vedio_frame;
extern Myqueue<AVPacket> vedio_packet;
extern Myqueue<AVPacket> audio_packet;
extern Myqueue<AVFrame> audio_frame;

//数组形式
extern Frame_nums<AVFrame> fvedio_nums;

dexcodecthto::dexcodecthto()
{
    if(vedio_packet.RtspStream)
    {
        type=StreamType::RTSP_STREAM;
    }
    abort=1;
}

void dexcodecthto::run()
{
   AVFrame *frame;
    while(abort!=0)
    {
        if(is_create==0)
        {
            frame =av_frame_alloc();
        }
        AVPacket *pkt=vedio_packet.pop();
        if(pkt)
        {            
            int ret=avcodec_send_packet(codec_ctx,pkt);
            av_packet_free(&pkt);
            if(ret<0)
            {
                break;
            }
            for(;;)
            {     
                is_create=avcodec_receive_frame(codec_ctx,frame);
                if(is_create==0)
                {
                    if(type==StreamType::RTSP_STREAM)
                    {
                        //队列形式存储
                        //qDebug()<<"放入了数据包";
                        vedio_frame.push(frame);

                    }else{
                        //数组形式存储
                        fvedio_nums.fream_push(frame);
                    }
                    break;
                }
                else if(AVERROR(EAGAIN)==is_create)
                {
                    break;
                }
                else{
                    abort=1;
                    break;
                }
            }
        }
    }

}

void dexcodecthto::init(AVCodecParameters *par)
{
    codec=avcodec_find_decoder(par->codec_id);
    codec_ctx=avcodec_alloc_context3(codec);//可以理解为为解码器结构体开辟空间，这里指定解码器

    int ret =avcodec_parameters_to_context(codec_ctx,par);//将相应解码器信息拷贝到结构体中
    if(ret<0)
    {
        qDebug()<<"avcodec_parameters_to_context error....";
    }
    //查找相应流的解码器
    if(type==StreamType::RTSP_STREAM)
    {
        //以rtsp拉流的方式打开解码器
        // 设置缓存大小 1024000byte
           av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
           // 设置超时时间 20s
           av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
           // 设置最大延时 3s
           av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
           // 设置打开方式 tcp/udp
           av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);

           ret=avcodec_open2(codec_ctx,codec,&pAVDictionary);
           goto end;
    }
    //打开解码器
    ret=avcodec_open2(codec_ctx,codec,NULL);
    end:
    if(ret<0)
    {
        qDebug()<<"avcodec_open2 erroe......";
    }
}
