#include "av_sync.h"

av_sync::av_sync()
{

}

void av_sync::init()
{
    Setclock(NAN);
}

void av_sync::SetclockAt(double pst, double time)
{
    pts=pst;
    pts_drift=pts-time;
}

double av_sync::GetClock()
{
    double time=GetMicroseconds()/1000000.0;
    return pts_drift+time;
}

void av_sync::Setclock(double pts)
{
    double time=GetMicroseconds()/1000000.0;
    SetclockAt(pts,time);
}

time_t av_sync::GetMicroseconds()
{
   std::chrono::system_clock::time_point time_point=std::chrono::system_clock::now();
   std::chrono::system_clock::duration duration =time_point.time_since_epoch();
   time_t time_temp=std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
   return time_temp;
}
