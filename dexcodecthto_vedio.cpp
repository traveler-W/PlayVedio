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
                    //队列形式存储
                    //vedio_frame.push(frame);

                    //数组形式存储
                    fvedio_nums.fream_push(frame);
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

    //打开解码器
    ret=avcodec_open2(codec_ctx,codec,NULL);
    if(ret<0)
    {
        qDebug()<<"avcodec_open2 erroe......";
    }
}
