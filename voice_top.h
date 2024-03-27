#ifndef VOICE_TOP_H
#define VOICE_TOP_H

#include <QWidget>
#include <QDebug>
namespace Ui {
class voice_top;
}

class voice_top : public QWidget
{
    Q_OBJECT

public:
    explicit voice_top(QWidget *parent = nullptr);
    ~voice_top();

     void audio_sliderchange(int value);
private:
    Ui::voice_top *ui;


    void label_shownum(int);
signals:
    void Send_vulume_value(double value);
};

#endif // VOICE_TOP_H
