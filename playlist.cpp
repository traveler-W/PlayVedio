#include "playlist.h"
#include "ui_playlist.h"

playlist::playlist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::playlist)
{
    ui->setupUi(this);
    init();
    connect_signal();
}

playlist::~playlist()
{
    delete ui;
}

void playlist::init()
{
    add_av.setText("添加");
    mymenu.addAction(&add_av);
    mymenu.addSeparator();
    remove_av.setText("移除");
    mymenu.addAction(&remove_av);
    mymenu.addSeparator();
    remove_all.setText("清空列表");
    mymenu.addAction(&remove_all);
    mymenu.addSeparator();
    RtspStream.setText("rtsp拉流");
    mymenu.addAction(&RtspStream);
    setqss_active();
}

void playlist::contextMenuEvent(QContextMenuEvent *event)
{
    mymenu.exec(event->globalPos());
}

void playlist::setqss_active()
{
    mymenu.setProperty("class", "blackMenu");
    mymenu.setWindowFlags(mymenu.windowFlags() | Qt::FramelessWindowHint);
    mymenu.setAttribute(Qt::WA_TranslucentBackground);
    QString str=help::Get_Qss("./menu.txt");
    mymenu.setStyleSheet(str);
}

void playlist::connect_signal()
{
    connect(&add_av,&QAction::triggered,this,&playlist::addfilename);
    connect(&remove_av,&QAction::triggered,this,&playlist::remove_index);
    connect(&remove_all,&QAction::triggered,this,&playlist::remove_allfile);
    connect(&RtspStream,&QAction::triggered,this,&playlist::RtspstreamGet);
    connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&playlist::double_list);

}

void playlist::addfile_item(QString item)
{
        bool bSupportMovie = item.endsWith(".mkv", Qt::CaseInsensitive) ||
            item.endsWith(".rmvb", Qt::CaseInsensitive) ||
            item.endsWith(".mp4", Qt::CaseInsensitive) ||
            item.endsWith(".avi", Qt::CaseInsensitive) ||
            item.endsWith(".flv", Qt::CaseInsensitive) ||
            item.endsWith(".wmv", Qt::CaseInsensitive) ||
            item.endsWith(".3gp", Qt::CaseInsensitive);
        if(bSupportMovie)
        {
            QFileInfo info(item);
            QList<QListWidgetItem*> listitem=ui->listWidget->findItems(info.fileName(),Qt::MatchExactly);
            QListWidgetItem*pitem=nullptr;
            if(listitem.isEmpty())
            {
                pitem=new QListWidgetItem(ui->listWidget);
                pitem->setText(info.fileName());
                pitem->setToolTip(info.filePath());
                ui->listWidget->addItem(pitem);
            }else{
                pitem=listitem.at(0);
            }

        }

}
void playlist::addfilename()
{
    QStringList filenames=QFileDialog::getOpenFileNames(this,"选择文件",QDir::homePath(),"视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");

    for(QString item:filenames)
    {
        addfile_item(item);
    }

}

void playlist::remove_index()
{
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

void playlist::remove_allfile()
{
    ui->listWidget->clear();
}

void playlist::RtspstreamGet()
{
    QString rtspUrl=QInputDialog::getText(this,"拉流框","输入rtsp流地址",QLineEdit::Normal,"",nullptr);
    emit sendfilename(rtspUrl,rtspUrl);
}

void playlist::double_list(QListWidgetItem *item)
{
    emit sendfilename(item->toolTip(),item->text());
}
