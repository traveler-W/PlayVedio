#include "voice_top.h"
#include "ui_voice_top.h"

voice_top::voice_top(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::voice_top)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    connect(ui->horizontalSlider,&QSlider::valueChanged,this,&voice_top::audio_sliderchange);
    connect(ui->horizontalSlider,&QSlider::valueChanged,this,&voice_top::label_shownum);
    ui->horizontalSlider->setValue(50);
}

voice_top::~voice_top()
{
    delete ui;
}

void voice_top::audio_sliderchange(int value)
{
    double dsecond=value*1.0/ui->horizontalSlider->maximum();
    emit Send_vulume_value(dsecond);
}

void voice_top::label_shownum(int value)
{
    ui->label->setText(QString::number(value)+"%");
}
