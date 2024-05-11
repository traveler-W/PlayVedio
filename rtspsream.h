#ifndef RTSPSREAM_H
#define RTSPSREAM_H

#include "include_head.h"
#include <QObject>
#include "SDL.h"
#include "log.h"
#include <QDebug>
#include <QWidget>
class RtspSream
{
public:
    RtspSream();
public:
    RtspSream(QString RtspStr,WId id);
    ~RtspSream();

    void testDecodeRtsp();
    void openSDL();
private:
    QString RtspStr;


    // SDL相关变量预先定义
    SDL_Window *pSDLWindow = 0;
    SDL_Renderer *pSDLRenderer = 0;
    SDL_Surface *pSDLSurface = 0;
    SDL_Texture *pSDLTexture = 0;
    SDL_Event event;

    qint64 startTime = 0;                           // 记录播放开始
    int currentFrame = 0;                           // 当前帧序号
    double fps = 0;                                 // 帧率
    double interval = 0;                            // 帧间隔


    AVFormatContext *pAVFormatContext = 0;          // ffmpeg的全局上下文，所有ffmpeg操作都需要
    AVCodecParameters*pAVCodecParameters=0;         //新的流信息载体
    AVStream *pAVStream = 0;                        // ffmpeg流信息
    AVCodecContext *pAVCodecContext = 0;            // ffmpeg编码上下文
    const AVCodec *pAVCodec = 0;                          // ffmpeg编码器
    AVPacket *pAVPacket = 0;                        // ffmpag单帧数据包
    AVFrame *pAVFrame = 0;                          // ffmpeg单帧缓存
    AVFrame *pAVFrameRGB32 = 0;                     // ffmpeg单帧缓存转换颜色空间后的缓存
    struct SwsContext *pSwsContext = 0;             // ffmpeg编码数据格式转换
    AVDictionary *pAVDictionary = 0;                // ffmpeg数据字典，用于配置一些编码器属性等

    int ret = 0;                                    // 函数执行结果
    int videoIndex = -1;                            // 音频流所在的序号
    int numBytes = 0;                               // 解码后的数据长度
    uchar *outBuffer = 0;

    //显示屏幕句柄
    WId handle_windows;
};

#endif // RTSPSREAM_H
