#ifndef SHOWME_H
#define SHOWME_H

#include <QWidget>
#include <QDebug>
#include <QMutex>
# pragma warning (disable:4819)
namespace Ui {
class showme;
}

class showme : public QWidget
{
    Q_OBJECT

public:
    explicit showme(QWidget *parent = nullptr);
    ~showme();
    void resizeEvent(QResizeEvent *event);
    //重新赋值窗口大小
    void ChangeShow();
    //得到画面比例
    void width_height(int w,int h);

private:
    Ui::showme *ui;
    int screen_height=0;//记录窗口的高度
    int screen_width=0;//窗口的宽度
    QMutex g_show_rect_mutex;


public slots :
    void create_window();

signals:
    void init_window(int h,int w,WId handel);

};

#endif // SHOWME_H
