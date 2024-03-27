#include "log.h"

void LOG(logtype type, const char *fomrt,...)
{
    va_list vlist;
    va_start(vlist, fomrt);
    char buf[100];
    vsprintf_s(buf, fomrt, vlist);
    va_end(vlist);

    QMutex log_mutex;
    QMutexLocker LOGmutex(&log_mutex);
    QString str2;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss:zzz-------");
    QString str=buf;

    QFile f;
    f.setFileName("./log.txt");
    f.open(QIODevice::WriteOnly|QIODevice::Append);
    switch (type) {
    case logtype::LOGTO:
        str2=current_date+"LOGTO:"+"["+str+"]\n";
        f.write(str2.toUtf8());
        break;
    case logtype::ERRORLOG:
        str2=current_date+"LOGERROR:"+"["+str+"]\n";
        f.write(str2.toUtf8());
        break;
    case logtype::WARINGLOG:
        str2=current_date+"LOGWARING:"+"["+str+"]\n";
        f.write(str2.toUtf8());
        break;
    default:
        str2=current_date+"write error.....\n";
        f.write(str2.toUtf8());
        break;
    }
    f.close();
}
