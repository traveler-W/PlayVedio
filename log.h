#ifndef LOG_H
#define LOG_H
#include <QMutex>
#include <QFile>
#include <QTime>
#include <QMutexLocker>

# pragma execution_character_set("utf-8")
//# pragma warning (disable:4819)
enum class logtype{
    LOGTO,
    ERRORLOG,
    WARINGLOG
};
void LOG(logtype t,const char*fomrt,...);


#endif // LOG_H
