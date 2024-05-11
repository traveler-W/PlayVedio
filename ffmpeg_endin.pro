QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audio_play.cpp \
    av_sync.cpp \
    contarlbar.cpp \
    dexcodecth.cpp \
    dexcodecthto_audio.cpp \
    dexcodecthto_vedio.cpp \
    frame_nums.cpp \
    help.cpp \
    log.cpp \
    main.cpp \
    myqueue.cpp \
    playlist.cpp \
    rtspsream.cpp \
    showme.cpp \
    title.cpp \
    vedio_play.cpp \
    voice_top.cpp \
    widget.cpp

HEADERS += \
    audio_play.h \
    av_sync.h \
    contarlbar.h \
    dexcodecth.h \
    dexcodecthto_audio.h \
    dexcodecthto_vedio.h \
    frame_nums.h \
    help.h \
    include_head.h \
    log.h \
    myqueue.h \
    playlist.h \
    rtspsream.h \
    showme.h \
    title.h \
    vedio_play.h \
    voice_top.h \
    widget.h

FORMS += \
    contarlbar.ui \
    playlist.ui \
    showme.ui \
    title.ui \
    voice_top.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

//添加头文件所在位置
INCLUDEPATH += $$PWD/include \
               $$PWD/SDL2-2.28.5/include

//声明静态库的所在位置
LIBS    += $$PWD/lib/avcodec.lib \
            $$PWD/lib/avdevice.lib \
            $$PWD/lib/avfilter.lib \
            $$PWD/lib/avformat.lib \
            $$PWD/lib/avutil.lib \
            $$PWD/lib/postproc.lib \
            $$PWD/lib/swresample.lib \
            $$PWD/lib/swscale.lib
//解决乱码问题
win32{
QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
}
//添加SDL库的库文件
LIBS   +=$$PWD/SDL2-2.28.5/lib/x64/SDL2.lib

RESOURCES += \
    resure.qrc

DISTFILES += \
    src/最大化.png
