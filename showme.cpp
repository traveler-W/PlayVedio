#include "showme.h"
#include "ui_showme.h"

showme::showme(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::showme)
{
    ui->setupUi(this);
}

showme::~showme()
{
    delete ui;
}

void showme::ChangeShow()
{

    g_show_rect_mutex.lock();

    if (screen_width == 0 && screen_height == 0)
    {
        ui->label->setGeometry(0, 0, width(), height());
    }
    else
    {
        float aspect_ratio;
        int width, height, x, y;
        int scr_width = this->width();
        int scr_height = this->height();

        aspect_ratio = (float)screen_width / (float)screen_height;

        height = scr_height;
        width = lrint(height * aspect_ratio) & ~1;
        if (width > scr_width)
        {
            width = scr_width;
            height = lrint(width / aspect_ratio) & ~1;
        }
        x = (scr_width - width) / 2;
        y = (scr_height - height) / 2;


        ui->label->setGeometry(x, y, width, height);
    }

    g_show_rect_mutex.unlock();
}

void showme::width_height(int w, int h)
{
    screen_width=w;
    screen_height=h;
    ChangeShow();

}
void showme::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    ChangeShow();
}

void showme::create_window()
{
   qDebug()<<"宽度:"<<width()<<"高度:"<<height();
   emit init_window(width(),height(),ui->label->winId());
}
