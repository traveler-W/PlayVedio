#include "rtspsream.h"

RtspSream::RtspSream(QString RtspStr,WId id)
{
    handle_windows=id;
    pAVFormatContext = avformat_alloc_context();    // 分配
    pAVPacket = av_packet_alloc();                  // 分配
    pAVFrame = av_frame_alloc();                    // 分配
    pAVFrameRGB32 = av_frame_alloc();               // 分配

    if(!pAVFormatContext || !pAVPacket || !pAVFrame || !pAVFrameRGB32)
    {
        return;
    }
}

void RtspSream::testDecodeRtsp()
{
    ret = avformat_open_input(&pAVFormatContext, RtspStr.toUtf8().data(), 0, 0);
    if(ret)
    {
        qDebug() << "Failed";
        return;
    }
    // 步骤三：探测流媒体信息
    ret = avformat_find_stream_info(pAVFormatContext, 0);
    if(ret < 0)
    {
        qDebug() << "Failed to avformat_find_stream_info(pAVFormatContext, 0)";
        return;
    }
    for(int index = 0; index < pAVFormatContext->nb_streams; index++)
    {
        pAVCodecParameters=pAVFormatContext->streams[index]->codecpar;
        pAVStream = pAVFormatContext->streams[index];
        switch (pAVStream->codecpar->codec_type)
        {
        case AVMEDIA_TYPE_UNKNOWN:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_UNKNOWN";
            break;
        case AVMEDIA_TYPE_VIDEO:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_VIDEO";
            videoIndex = index;
            qDebug();
            break;
        case AVMEDIA_TYPE_AUDIO:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_AUDIO";
            break;
        case AVMEDIA_TYPE_DATA:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_DATA";
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_SUBTITLE";
            break;
        case AVMEDIA_TYPE_ATTACHMENT:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_ATTACHMENT";
            break;
        case AVMEDIA_TYPE_NB:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_NB";
            break;
        default:
            break;
        }
        // 已经找打视频品流
        if(videoIndex != -1)
        {
            break;
        }
    }
    //对解码器上下文赋值
    pAVCodecContext=avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(pAVCodecContext,pAVCodecParameters);

    pAVCodec=avcodec_find_decoder(pAVCodecContext->codec_id);

    // 步骤六：打开解码器
    // 设置缓存大小 1024000byte
    av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
    // 设置超时时间 20s
    av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
    // 设置最大延时 3s
    av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
    // 设置打开方式 tcp/udp
    av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);
    ret = avcodec_open2(pAVCodecContext, pAVCodec, &pAVDictionary);
    if(ret)
    {
        qDebug()<< "Failed to avcodec_open2(pAVCodecContext, pAVCodec, pAVDictionary)";
        return;
    }

    // 没有总时长的时候，使用分子和分母计算
    fps = pAVStream->avg_frame_rate.num * 1.0f / pAVStream->avg_frame_rate.den;
    interval = 1 * 1000 / fps;
    qDebug()<< "平均帧率:" << fps;
    qDebug() << "帧间隔:" << interval << "ms";
    // 步骤七：对拿到的原始数据格式进行缩放转换为指定的格式高宽大小
    pSwsContext = sws_getContext(pAVCodecContext->width,
                                 pAVCodecContext->height,
                                 pAVCodecContext->pix_fmt,
                                 pAVCodecContext->width,
                                 pAVCodecContext->height,
                                 AV_PIX_FMT_RGBA,
                                 SWS_FAST_BILINEAR,
                                 0,
                                 0,
                                 0);


    numBytes=av_image_get_buffer_size(AV_PIX_FMT_RGBA, pAVCodecContext->width,
                                      pAVCodecContext->height,1);
    outBuffer = (uchar *)av_malloc(numBytes);
    //pAVFrame32的data指针指向了outBuffer
    uchar*pixle[4];
    int pitch[4];
    av_image_fill_arrays(pixle, pitch,outBuffer,
                         AV_PIX_FMT_RGBA,
                         pAVCodecContext->width,
                         pAVCodecContext->height,1);
    openSDL();
}

void RtspSream::openSDL()
{
    ret = SDL_Init(SDL_INIT_VIDEO);
    if(ret)
    {
        qDebug() << "Failed";
        return;
    }
    pSDLWindow = SDL_CreateWindowFrom((const void*)handle_windows);
    if(!pSDLWindow)
    {
        qDebug() << "Failed";
        return;
    }
    pSDLRenderer = SDL_CreateRenderer(pSDLWindow, -1, 0);
    if(!pSDLRenderer)
    {
        qDebug() << "Failed";
        return;
    }

    startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    currentFrame = 0;

    pSDLTexture = SDL_CreateTexture(pSDLRenderer,
                                    //                                  SDL_PIXELFORMAT_IYUV,
                                    SDL_PIXELFORMAT_YV12,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    pAVCodecContext->width,
                                    pAVCodecContext->height);
    while(av_read_frame(pAVFormatContext, pAVPacket) >= 0)
       {
           if(pAVPacket->stream_index == videoIndex)
           {
               // 步骤八：对读取的数据包进行解码
               ret = avcodec_send_packet(pAVCodecContext, pAVPacket);
               if(ret)
               {
                   qDebug() << "Failed to avcodec_send_packet(pAVCodecContext, pAVPacket) ,ret =" << ret;
                   break;
               }
               while(!avcodec_receive_frame(pAVCodecContext, pAVFrame))
               {
                   sws_scale(pSwsContext,
                             (const uint8_t * const *)pAVFrame->data,
                             pAVFrame->linesize,
                             0,
                             pAVCodecContext->height,
                             pAVFrameRGB32->data,
                             pAVFrameRGB32->linesize);
                   // 格式为RGBA=8:8:8:8”
                   // rmask 应为 0xFF000000  但是颜色不对 改为 0x000000FF 对了
                   // gmask     0x00FF0000                  0x0000FF00
                   // bmask     0x0000FF00                  0x00FF0000
                   // amask     0x000000FF                  0xFF000000
                   // 测试了ARGB，也是相反的，而QImage是可以正确加载的
                   // 暂时只能说这个地方标记下，可能有什么设置不对什么的
                   qDebug() << __FILE__ << __LINE__  << pSDLTexture;
                   SDL_UpdateYUVTexture(pSDLTexture,
                                        NULL,
                                        pAVFrame->data[0], pAVFrame->linesize[0],
                                        pAVFrame->data[1], pAVFrame->linesize[1],
                                        pAVFrame->data[2], pAVFrame->linesize[2]);
                   qDebug() << __FILE__ << __LINE__  << pSDLTexture;

                   SDL_RenderClear(pSDLRenderer);
                   // Texture复制到Renderer
                   SDL_Rect        sdlRect;
                   sdlRect.x = 0;
                   sdlRect.y = 0;
                   sdlRect.w = pAVFrame->width;
                   sdlRect.h = pAVFrame->height;
                   qDebug() << __FILE__ << __LINE__ << SDL_RenderCopy(pSDLRenderer, pSDLTexture, 0, &sdlRect) << pSDLTexture;
                   // 更新Renderer显示
                   SDL_RenderPresent(pSDLRenderer);
                   // 事件处理
                   SDL_PollEvent(&event);
               }
               // 下一帧
               currentFrame++;
               while(QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime < currentFrame * interval)
               {
                   SDL_Delay(1);
               }
               qDebug() << "current:" << currentFrame <<"," << time << (QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime);
           }
       }
       qDebug() << "释放回收资源";
       if(outBuffer)
       {
           av_free(outBuffer);
           outBuffer = 0;
       }
       if(pSwsContext)
       {
           sws_freeContext(pSwsContext);
           pSwsContext = 0;
           qDebug() << "sws_freeContext(pSwsContext)";
       }
       if(pAVFrameRGB32)
       {
           av_frame_free(&pAVFrameRGB32);
           pAVFrame = 0;
           qDebug() << "av_frame_free(pAVFrameRGB888)";
       }
       if(pAVFrame)
       {
           av_frame_free(&pAVFrame);
           pAVFrame = 0;
           qDebug() << "av_frame_free(pAVFrame)";
       }
       if(pAVPacket)
       {
           av_packet_free(&pAVPacket);
           pAVPacket = 0;
           qDebug() << "av_free_packet(pAVPacket)";
       }
       if(pAVCodecContext)
       {
           avcodec_close(pAVCodecContext);
           pAVCodecContext = 0;
           qDebug() << "avcodec_close(pAVCodecContext);";
       }
       if(pAVFormatContext)
       {
           avformat_close_input(&pAVFormatContext);
           avformat_free_context(pAVFormatContext);
           pAVFormatContext = 0;
           qDebug() << "avformat_free_context(pAVFormatContext)";
       }

       // 步骤五：销毁渲染器
       SDL_DestroyRenderer(pSDLRenderer);
       // 步骤六：销毁窗口
       SDL_DestroyWindow(pSDLWindow);
       // 步骤七：退出SDL
       SDL_Quit();
}
