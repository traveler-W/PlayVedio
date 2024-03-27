#ifndef DEXCODECTHTO_H
#define DEXCODECTHTO_H
#include "include_head.h"
#include "myqueue.h"
#include <QThread>
#include "frame_nums.h"
class dexcodecthto:public QThread
{
public:
    dexcodecthto();
    ~dexcodecthto()
    {
        codec=nullptr;

        codec_ctx=nullptr;
        delete codec_ctx;
        delete codec;
    }
    void run() override;
    int abort;
    int is_create=0;
    void init(AVCodecParameters *par);

private:
    AVCodecContext* codec_ctx;//解码器相关结构体
    int Packet_type;
    const AVCodec *codec;
};

#endif // DEXCODECTHTO_H
