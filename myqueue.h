#ifndef MYQUEUE_H
#define MYQUEUE_H
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include "include_head.h"
#include "log.h"
#include <QDebug>
template <class T>
class Myqueue
{
public:
    Myqueue()=default;
    bool f_push;
    bool f_pop;
    void push(T* t)
    {
        pushmutex.lock();
        if(MyQ.size()>=20)
        {
            f_push=pushMywaitcondition.wait(&pushmutex);
            if(f_push==false)
            {
                pushmutex.unlock();
                LOG(logtype::ERRORLOG,"this is error wait....");
            }
        }
        MyQ.push_back(t);
        pushmutex.unlock();
        popMywaitcondition.wakeOne();
    }
    T* pop()
    {
        popmutex.lock();
        if(MyQ.isEmpty())
        {
            f_pop=popMywaitcondition.wait(&popmutex);
            if(f_pop==false)
            {
                popmutex.unlock();
                LOG(logtype::ERRORLOG,"this is error wait....");
            }

        }
        T*part;
        if(!MyQ.isEmpty())
        {
            part=MyQ.front();
            MyQ.pop_front();
        }
        popmutex.unlock();
        pushMywaitcondition.wakeOne();
        return part;


    }

    //唤醒所有堵塞的条件变量
    void wake_allthread(){
        if(f_push)
        {
              pushMywaitcondition.wakeAll();
        }

        if(f_pop)
        {
             popMywaitcondition.wakeAll();
        }

    }
private:
    QQueue<T*> MyQ;
    QMutex pushmutex;
    QMutex popmutex;
    QWaitCondition pushMywaitcondition;
    QWaitCondition popMywaitcondition;
};

#endif // MYQUEUE_H
