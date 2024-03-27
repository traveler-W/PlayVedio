#ifndef VEDIO_PLAY_H
#define VEDIO_PLAY_H

#include <QWidget>
#include <chrono>
#include <QObject>
#include "myqueue.h"
#include "av_sync.h"
#include "frame_nums.h"
#include <QMutex>
# pragma warning (disable:4819)
extern "C"
{
#include "SDL.h"
#include "SDL_render.h"
#include "SDL_test.h"

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"

}
class Vedio_play:public QObject
{
    Q_OBJECT
public:
    Vedio_play(av_sync *sync_,AVRational time_base,int width,int height);
    ~Vedio_play();
    void init(int h,int w,WId id);
    void MainLoop();
    int abort;
    void RefreshLoopWaitEvent(SDL_Event *e);
    void vedioRefresh(double*time_this);
    void Destroy();
    //重置窗口大小
    void change_rect(int w,int h);
    int realloc_texture(SDL_Texture **texture, Uint32 new_format, int new_width, int new_height, SDL_BlendMode blendmode, int init_texture);

    void setvedio_index(double s);
private:
    SDL_Event event;
    SDL_Rect rect;
    SDL_Window *win=NULL;
    SDL_Renderer*renderer=NULL;
    SDL_Texture*texture =NULL;
    SDL_Thread *sdl_thread;
    int vedio_width;
    int vedio_height;

    //设置画面高度
    int change_width;
    int change_height;
    //设置画面比列
    int roation_width=0;
    int roation_height=0;
    uint8_t *yuv_buf=NULL;
    int yuv_buf_size;

    //音视频同步时钟设置
    av_sync*sync;
    AVRational ration;

    //转换后的frame大小
    AVFrame *result_frame;
    SwsContext*img_convert_ctx=NULL;//图片大下转换器

    //显示屏幕句柄
    WId handle_windows;
signals:
    void siginal_rect(int w,int h);

private:
    void get_buff_frame();
    QMutex vedio_index_mutex;

};

#endif // VEDIO_PLAY_H
