#-------------------------------------------------
#
# Project created by QtCreator 2012-09-18T15:16:30
#
#-------------------------------------------------
PROJECT_BINDIR = $$PWD/bin
PROJECT_LIBDIR = $$PWD/lib

QT       += core gui winextras multimedia multimediawidgets

TARGET = ble
TEMPLATE = app

INCLUDEPATH +=  src/gui
INCLUDEPATH +=  $$PWD/src/core \
                $$PWD/3rdparty/librtmp \
                $$PWD/3rdparty/libx264 \
                $$PWD/3rdparty/libopencv/include

LIBS += -L$$PWD/3rdparty/librtmp -lrtmp \
        -L$$PWD/3rdparty/libx264/ -lx264 \
        -L$$PWD/3rdparty/libopenssl/ -lssl \
        -L$$PWD/3rdparty/libopenssl/ -lcrypto \
        -L$$PWD/3rdparty/libopencv/lib/ -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248
win32:LIBS +=  -lws2_32 -lgdi32 -lwinmm -lz

DESTDIR = $$PROJECT_BINDIR
unix:QMAKE_RPATHDIR+=$$PROJECT_LIBDIR

SOURCES += \
    src/main.cpp \
    src/BleMainWindow.cpp \
    src/core/mglobal.cpp \
    src/core/mstring.cpp \
    src/core/mstringlist.cpp \
    src/BleX264Encoder.cpp \
    src/BleSetting.cpp \
    src/BleEncoderThread.cpp \
    src/BleUtil.cpp \
    src/BleRtmpMuxer.cpp \
    src/BleErrno.cpp \
    src/BleLog.cpp \
    src/BleThread.cpp \
    src/BleRtmpSendThread.cpp \
    src/BleImageProcess.cpp \
    src/BleSourceAbstract.cpp \
    src/BleCameraSource.cpp \
    src/BleWindowsCaptureSource.cpp \
    src/BleImageProcessThread.cpp \
    src/BleImageDisplayArea.cpp \
    src/BleCameraSelector.cpp \
    src/BleDesktopAreaSelector.cpp \
    src/BleTimestampBulider.cpp \
    src/BlePictureSource.cpp \
    src/gui/BleWinAero.cpp \
    src/gui/MCustomDialog.cpp \
    src/gui/MOption.cpp \
    src/gui/MSkinChanger.cpp \
    src/gui/MTabBar.cpp \
    src/gui/MThemeAdaptor.cpp \
    src/gui/MThemeWidgetBase.cpp \
    src/gui/MToolButton.cpp \
    src/gui/mutility.cpp \
    src/gui/NcFramelessHelper.cpp \
    src/gui/QjtCustomWidget.cpp \
    src/gui/QjtMessageBox.cpp \
    src/gui/QjtTabBar.cpp \
    src/gui/RecSkinDialog.cpp \
    src/gui/RecSkinPickerWidget.cpp \
    src/gui/RecSkinWidget.cpp \
    src/gui/TitleWidget.cpp

HEADERS  += \
    src/BleMainWindow.hpp \
    src/core/mglobal.hpp \
    src/core/mstring.hpp \
    src/core/mstringlist.hpp \
    src/BleX264Encoder.hpp \
    src/BleSettingetting.hpp \
    src/BleEncoderThread.hpp \
    src/BleUtil.hpp \
    src/BleRtmpMuxer.hpp \
    src/BleErrno.hpp \
    src/BleLog.hpp \
    src/BleThread.hpp \
    src/BleRtmpSendThread.hpp \
    src/BleImageProcess.hpp \
    src/BleSourceAbstract.hpp \
    src/BleCameraSource.hpp \
    src/BleWindowsCaptureSource.hpp \
    src/BleImageProcessThread.hpp \
    src/BleImageDisplayArea.hpp \
    src/BleCameraSelector.hpp \
    src/BleDesktopAreaSelector.hpp \
    src/BleTimestampBulider.hpp \
    src/BlePictureSource.hpp \
    src/gui/BleWinAero.hpp \
    src/gui/MCustomDialog.h \
    src/gui/MOption.h \
    src/gui/MSkinChanger.h \
    src/gui/MTabBar.h \
    src/gui/MThemeAdaptor.h \
    src/gui/MThemeWidgetBase.h \
    src/gui/MToolButton.h \
    src/gui/mutility.h \
    src/gui/NcFramelessHelper.h \
    src/gui/QjtCustomWidget.h \
    src/gui/QjtMessageBox.h \
    src/gui/QjtTabBar.h \
    src/gui/RecSkinDialog.h \
    src/gui/RecSkinPickerWidget.h \
    src/gui/RecSkinWidget.h \
    src/gui/TitleWidget.h

FORMS    += \
    src/BleMainWindow.ui \
    src/BleSetting.ui \
    src/BleImageProcess.ui \
    src/BleImageDisplayArea.ui \
    src/BleCameraSelector.ui \
    src/BleDesktopAreaSelector.ui \
    src/gui/MSkinChanger.ui \
    src/gui/MTabBar.ui \
    src/gui/QjtShadeWindow.ui \
    src/gui/RecSkinDialog.ui \
    src/gui/RecSkinPickerWidget.ui \
    src/gui/RecSkinWidget.ui \
    src/gui/TitleWidget.ui

win32:RC_FILE = src/ble.rc

RESOURCES += src/res.qrc \
    src/gui/gui.qrc
