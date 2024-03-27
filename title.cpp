#include "title.h"
#include "ui_title.h"

title::title(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::title)
{
    ui->setupUi(this);
//    QString title_qss=help::Get_Qss("./title.txt");
//    setStyleSheet(title_qss);

}

title::~title()
{
    delete ui;
}

void title::on_min_show_clicked()
{
    emit minmin_siginal();
}

void title::set_playtitle(QString str)
{
    ui->filename->setText(u8"正在播放:"+str);
}

void title::on_max_show_clicked()
{
    if(max_min==false)
    {
        emit max_siginal();
        ui->max_show->setIcon(QIcon(":/src/minshow.png"));
        max_min=true;
    }
    else{
        emit min_siginal();
        ui->max_show->setIcon(QIcon(":/src/max.png"));
        max_min=false;
    }

}

void title::on_exit_clicked()
{
    emit exit_app();
}
