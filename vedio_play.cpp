#include "vedio_play.h"
#include "QDebug"
extern Myqueue<AVFrame> vedio_frame;

extern Frame_nums<AVFrame> fvedio_nums;
Vedio_play::Vedio_play(av_sync *sysnc,AVRational time_base,int width,int height):vedio_width(width),vedio_height(height)
{
    sync=sysnc;
    ration=time_base;
    abort=1;
    result_frame=av_frame_alloc();


}

Vedio_play::~Vedio_play()
{
    av_frame_free(&result_frame);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(renderer);


    yuv_buf=nullptr;
    delete yuv_buf;
    SDL_Quit();
}
//调节画面比列
void calculate_display_rect(SDL_Rect *rect,
    int scr_xleft, int scr_ytop, int scr_width, int scr_height,
    int pic_width, int pic_height, AVRational pic_sar)
{
    float aspect_ratio;
    int width, height, x, y;

    if (pic_sar.num == 0)
        aspect_ratio = 0;
    else
        aspect_ratio = av_q2d(pic_sar);

    if (aspect_ratio <= 0.0)
        aspect_ratio = 1.0;
    aspect_ratio *= (float)pic_width / (float)pic_height;

    /* XXX: we suppose the screen has a 1.0 pixel ratio */
    height = scr_height;
    width = lrint(height * aspect_ratio) & ~1;
    if (width > scr_width) {
        width = scr_width;
        height = lrint(width / aspect_ratio) & ~1;
    }
    x = (scr_width - width) / 2;
    y = (scr_height - height) / 2;
    rect->x = scr_xleft + x;
    rect->y = scr_ytop + y;
    rect->w = FFMAX(width, 1);
    rect->h = FFMAX(height, 1);
}

void Vedio_play::init(int w,int h,WId id)
{
     change_width=w;
     change_height=h;
     //为缩放图片设置缩放器和计算大小
     get_buff_frame();
     handle_windows=id;
    //qDebug()<<QThread::currentThreadId();


    if((SDL_Init(SDL_INIT_VIDEO)))
    {

    }
    win=SDL_CreateWindowFrom((const void*)handle_windows);
    //win=SDL_CreateWindow("Myplay",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,vedio_width,vedio_height,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    renderer=SDL_CreateRenderer(win,-1,0);
    texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_IYUV,SDL_TEXTUREACCESS_STREAMING,1920,1080);
}




//主循环函数
void Vedio_play::MainLoop()
{
    SDL_Event event;
    while(abort!=0)
    {
        RefreshLoopWaitEvent(&event);
        switch(event.type){
        case SDL_KEYDOWN:
            if(event.key.keysym.sym==SDLK_ESCAPE){

            }
            break;
        case SDL_WINDOWEVENT:
            //窗口大小改变事件
            switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                //重置窗口大小，并且触发lable的resizeevent
                change_width=event.window.data1;
                change_height=event.window.data2;
                get_buff_frame();
            case SDL_WINDOWEVENT_EXPOSED:

                break;

            }
            break;
        case SDL_QUIT:

            break;
        default:
            break;
        }
    }
}

void Vedio_play::RefreshLoopWaitEvent(SDL_Event *e)
{
    double reming_time=0.0;
    SDL_PumpEvents();
    while(!SDL_PeepEvents(e,1,SDL_GETEVENT,SDL_FIRSTEVENT,SDL_LASTEVENT))
    {   
//        if(reming_time>0.0)
//        {

//        }
//        reming_time=0.01;
        //刷新画面
        vedioRefresh(&reming_time);
        SDL_PumpEvents();
    }
}

void Vedio_play::vedioRefresh(double *time_this)
{
    Q_UNUSED(time_this);

    if(fvedio_nums.get_pause())
    {
        return;
    }
    AVFrame *frame;
    //frame=vedio_frame.pop();

    //数组中取出数据
    frame=fvedio_nums.frame_pop();


    //qDebug()<<fvedio_nums.si();
    if(frame)
    {
        double pts=frame->pts*av_q2d(ration);
        double diff=pts-sync->GetClock();

        if(diff>0)
        {
            //*time_this=FFMIN(*time_this,diff);
            std::this_thread::sleep_for(std::chrono::milliseconds(int64_t(diff*1000.0)));
            //return;
        }
        //渲染
        calculate_display_rect(&rect,0,0,change_width,change_height,frame->width,frame->height,frame->sample_aspect_ratio);


        //缩放帧图片到合适大小
        if(result_frame==NULL)
        {
           //qDebug()<<"返回函数了";
           return;
        }

        sws_scale(img_convert_ctx, (const unsigned char* const*)frame->data, frame->linesize, 0,
                                        frame->height, result_frame->data, result_frame->linesize);
        //SDL_UpdateTexture(texture, &rect, result_frame->data[0], result_frame->linesize[0]);


        if(roation_width!=frame->width||roation_height!=frame->height)
        {
            roation_width=frame->width;
            roation_height=frame->height;
            emit siginal_rect(roation_width,roation_height);
        }


        SDL_UpdateYUVTexture(texture,&rect,result_frame->data[0],result_frame->linesize[0],
                result_frame->data[1],result_frame->linesize[1],
                result_frame->data[2],result_frame->linesize[2]);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,texture,&rect,&rect);
        SDL_RenderPresent(renderer);
        //av_frame_free(&frame);

    }
}

void Vedio_play::Destroy()
{

    av_frame_free(&result_frame);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(renderer);

    yuv_buf=nullptr;
    delete yuv_buf;
    SDL_Quit();

}

void Vedio_play::change_rect(int w, int h)
{
    change_width=w;
    change_height=h;
}

void Vedio_play::setvedio_index(double s)
{
    vedio_index_mutex.lock();
    fvedio_nums.v_index=qRound(fvedio_nums.V.size()*s);
    vedio_index_mutex.unlock();
}

void Vedio_play::get_buff_frame()
{
//    result_frame=av_frame_alloc();
    //计算缩放后的图片大小并且填入数组中
    int buff_size=av_image_get_buffer_size(AV_PIX_FMT_YUV420P,change_width,change_height,1);
    unsigned char*buffer=(unsigned char*)av_malloc(buff_size);
    av_image_fill_arrays(result_frame->data,result_frame->linesize,buffer,AV_PIX_FMT_YUV420P,change_width,change_height,1);

    //配置缩放器
    img_convert_ctx =sws_getCachedContext(img_convert_ctx,vedio_width,vedio_height,
                                               AV_PIX_FMT_YUV420P,change_width,change_height,
                                              AV_PIX_FMT_YUV420P,SWS_BICUBIC, NULL, NULL, NULL);

}
