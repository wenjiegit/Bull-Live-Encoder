#-------------------------------------------------
#
# Project created by QtCreator 2012-09-18T15:16:30
#
#-------------------------------------------------

QT       += core gui winextras multimedia multimediawidgets

TARGET = ble
TEMPLATE = app

include(../common.pri)

INCLUDEPATH += $$LIB_GUILIB_PATH
INCLUDEPATH +=  $$PWD/core \
                $$PWD/3rdparty/librtmp \
                $$PWD/3rdparty/libx264 \
                $$PWD/3rdparty/libopencv/include

LIBS += -L$$PROJECT_LIBDIR -l$$LIB_GUILIB_NAME
LIBS += -L$$PWD/3rdparty/librtmp -lrtmp \
        -L$$PWD/3rdparty/libx264/ -lx264 \
        -L$$PWD/3rdparty/libopenssl/ -lssl \
        -L$$PWD/3rdparty/libopenssl/ -lcrypto \
        -L$$PWD/3rdparty/libopencv/lib/ -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248
win32:LIBS +=  -lws2_32 -lgdi32 -lwinmm -lz

DESTDIR = $$PROJECT_BINDIR
unix:QMAKE_RPATHDIR+=$$PROJECT_LIBDIR

SOURCES += main.cpp\
    BleMainWindow.cpp \
    core/mglobal.cpp \
    core/mstring.cpp \
    core/mstringlist.cpp \
    BleX264Encoder.cpp \
    BleSetting.cpp \
    BleEncoderThread.cpp \
    BleUtil.cpp \
    BleRtmpMuxer.cpp \
    BleErrno.cpp \
    BleLog.cpp \
    BleThread.cpp \
    BleRtmpSendThread.cpp \
    BleImageProcess.cpp \
    BleSourceAbstract.cpp \
    BleCameraSource.cpp \
    BleWindowsCaptureSource.cpp \
    BleImageProcessThread.cpp \
    BleImageDisplayArea.cpp \
    BleCameraSelector.cpp \
    BleDesktopAreaSelector.cpp \
    BleTimestampBulider.cpp \
    BlePictureSource.cpp

HEADERS  += \
    BleMainWindow.hpp \
    core/mglobal.hpp \
    core/mstring.hpp \
    core/mstringlist.hpp \
    BleX264Encoder.hpp \
    BleSettingetting.hpp \
    BleEncoderThread.hpp \
    BleUtil.hpp \
    BleRtmpMuxer.hpp \
    BleErrno.hpp \
    BleLog.hpp \
    BleThread.hpp \
    BleRtmpSendThread.hpp \
    BleImageProcess.hpp \
    BleSourceAbstract.hpp \
    BleCameraSource.hpp \
    BleWindowsCaptureSource.hpp \
    BleImageProcessThread.hpp \
    BleImageDisplayArea.hpp \
    BleCameraSelector.hpp \
    BleDesktopAreaSelector.hpp \
    BleTimestampBulider.hpp \
    BlePictureSource.hpp

FORMS    += \
    BleMainWindow.ui \
    BleSetting.ui \
    BleImageProcess.ui \
    BleImageDisplayArea.ui \
    BleCameraSelector.ui \
    BleDesktopAreaSelector.ui

win32:RC_FILE = ble.rc

RESOURCES += \
    res.qrc
