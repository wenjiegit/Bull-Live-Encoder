#-------------------------------------------------
#
# Project created by QtCreator 2012-09-18T15:16:30
#
#-------------------------------------------------
BLE_BINDIR = $$PWD/bin

QT       += core gui winextras multimedia multimediawidgets

TARGET = ble
TEMPLATE = app

# for RtAudio defines
DEFINES += __WINDOWS_DS__

INCLUDEPATH +=  src/gui
INCLUDEPATH +=  src/core \
                3rdparty/librtmp \
                3rdparty/libx264 \
                3rdparty/libopencv/include \
                3rdparty/libRtAudio/ \
                3rdparty/libfaac/include \
                3rdparty/libmp3lame/include

LIBS += -L$$PWD/3rdparty/librtmp/ -lrtmp \
        -L$$PWD/3rdparty/libx264/ -lx264 \
        -L$$PWD/3rdparty/libopenssl/ -lssl \
        -L$$PWD/3rdparty/libopenssl/ -lcrypto \
        -L$$PWD/3rdparty/libopencv/lib/ -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248 \
        -L$$PWD/3rdparty/libfaac/lib/ -lfaac \
        -L$$PWD/3rdparty/libmp3lame/lib/ -lmp3lame

LIBS += -ldsound -lole32 -lwinmm -lgdi32 -lws2_32 -lz

DESTDIR = $$BLE_BINDIR

# copy files to rundir
win32:system($$PWD/script/copy_file.bat)

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
    src/gui/TitleWidget.cpp \
    3rdparty/libRtAudio/RtAudio.cpp \
    src/BleAudioCapture.cpp \
    src/BleAudioEncoder_AAC.cpp \
    src/BleAVQueue.cpp \
    src/BleAVUtil.cpp \
    src/BleAudioEncoderAbstract.cpp \
    src/BleFileSource.cpp \
    src/core/mstream.cpp \
    src/BleContext.cpp \
    src/BleTextSource.cpp \
    src/BleAudioEncoder_MP3.cpp \
    src/BleImageCaptureThread.cpp \
    src/BleAVContext.cpp \
    src/BleMediaSelector.cpp

HEADERS  += \
    src/BleMainWindow.hpp \
    src/core/mglobal.hpp \
    src/core/mstring.hpp \
    src/core/mstringlist.hpp \
    src/BleX264Encoder.hpp \
    src/BleSetting.hpp \
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
    src/gui/TitleWidget.h \
    3rdparty/libRtAudio/RtAudio.h \
    3rdparty/libRtAudio/RtError.h \
    src/BleAudioCapture.hpp \
    src/BleAudioEncoder_AAC.hpp \
    src/BleAVQueue.hpp \
    src/BleAVUtil.hpp \
    src/BleAudioEncoderAbstract.hpp \
    src/BleVersion.hpp \
    src/BleFileSource.hpp \
    src/core/mstream.hpp \
    src/BleContext.hpp \
    src/BleTextSource.hpp \
    src/BleAudioEncoder_MP3.hpp \
    src/BleImageCaptureThread.hpp \
    src/BleAVContext.hpp \
    src/BleMediaSelector.hpp

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
    src/gui/TitleWidget.ui \
    src/BleMediaSelector.ui

win32:RC_FILE = src/ble.rc

RESOURCES += src/res.qrc \
    src/gui/gui.qrc
