#ifndef AV_SYNC_H
#define AV_SYNC_H
#include <chrono>
#include <ctime>
#include <math.h>
class av_sync
{
public:
    av_sync();

    void init();
    void SetclockAt(double pst,double time);

    double GetClock();
    void Setclock(double pts);
    time_t GetMicroseconds();


    double pts_drift=0;
    double pts=0;


};

#endif // AV_SYNC_H
