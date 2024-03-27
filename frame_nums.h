#ifndef FRAME_NUMS_H
#define FRAME_NUMS_H

#include <QVector>
#include <QMutex>
#include <QDebug>
#include <QWaitCondition>
#include "include_head.h"
template <class T>
class Frame_nums
{
public:
    int v_index=0;
    QVector<T*> V;
    Frame_nums()=default;
    void fream_push(T* packet)
    {
        push_mymutex.lock();
        V.push_back(packet);
        push_mymutex.unlock();
        pop_waitcon.wakeOne();
    }

    int si()
    {
        int temp=0;
        pop_mymutex.lock();
        temp=V.size();
        pop_mymutex.unlock();
        return temp;
    }
    T*frame_pop()
    {
        pop_mymutex.lock();
        if(v_index>V.size())
        {
            qDebug()<<"数组停止了";
            pop_waitcon.wait(&pop_mymutex);

        }
        T* part;
        if(v_index<=V.size()-1)
        {
            part=V[v_index];
        }else
        {
            v_index=V.size()-1;
        }
        v_index++;

        pop_mymutex.unlock();
        return part;
    }

    void change_index(int s)
    {
        index_mutex.lock();
        v_index=s;
        index_mutex.unlock();
    }

    void change_pause(bool is_or)
    {
        pause_mutex.lock();
        is_pause=is_or;
        pause_mutex.unlock();
    }

    bool get_pause()
    {
       bool temp;
       pause_mutex.lock();
       temp=is_pause;
       pause_mutex.unlock();
       return temp;
    }
private:


    bool is_pause=false;
    QMutex pause_mutex;

    QMutex push_mymutex;
    QMutex index_mutex;
    QMutex pop_mymutex;
    QWaitCondition push_waitcon;
    QWaitCondition pop_waitcon;
};

#endif // FRAME_NUMS_H
