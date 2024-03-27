#include "help.h"
#include <QDebug>
help::help()
{

}

QString help::Get_Qss(QString path)
{
    QFile file(path);
    QString ret_str;

    if( file.open(QIODevice::ReadOnly))
    {
        ret_str=file.readAll();

    }else
    {
        LOG(logtype::ERRORLOG,"%s","qss读取失败");
    }
    file.close();
    return ret_str;
}
