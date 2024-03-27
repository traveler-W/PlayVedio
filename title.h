#ifndef TITLE_H
#define TITLE_H

#include <QWidget>
#include "help.h"

namespace Ui {
class title;
}

class title : public QWidget
{
    Q_OBJECT

public:
    explicit title(QWidget *parent = nullptr);
    ~title();
    void set_playtitle(QString str);
signals:
    void min_siginal();
    void max_siginal();
    void minmin_siginal();
    void exit_app();
private slots:
    void on_min_show_clicked();


    void on_max_show_clicked();

    void on_exit_clicked();


private:
    Ui::title *ui;
    bool max_min=false;
};

#endif // TITLE_H
