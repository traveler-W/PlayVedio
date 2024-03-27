#ifndef INCLUDE_HEAD_H
#define INCLUDE_HEAD_H
extern "C"
{

    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libavformat/version.h>
    #include <libavutil/time.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/imgutils.h>
    #include "libswresample/swresample.h"

}
struct play_time{
  int time_all;
  int time_start;
};
#endif // INCLUDE_HEAD_H
