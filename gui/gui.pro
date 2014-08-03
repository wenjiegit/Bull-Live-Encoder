#-------------------------------------------------
#
# Project created by Wenjie 2012-09-19T15:45:48
#
#-------------------------------------------------
QT += core gui  winextras printsupport  widgets
TEMPLATE = lib

include(../common.pri)

TARGET = $$LIB_GUILIB_NAME
DESTDIR = $$PROJECT_LIBDIR

win32{
    DLLDESTDIR = $$PROJECT_BINDIR
    QMAKE_DISTCLEAN += $$PROJECT_BINDIR/$${LIB_GUILIB_NAME}.dll
}
DEFINES += GUILIB_LIBRARY

HEADERS += \
    RecSkinWidget.h \
    RecSkinDialog.h \
    TitleWidget.h \
    NcFramelessHelper.h \
    QjtCustomWidget.h \
    QjtMessageBox.h \
    RecSkinPickerWidget.h \
    QjtTabBar.h \
    MToolButton.h \
    MOption.h \
    MSkinChanger.h \
    MThemeAdaptor.h \
    MTabBar.h \
    MThemeWidgetBase.h \
    mutility.h \
    gui_global.h \
    MCustomDialog.h \
    BleWinAero.hpp

SOURCES += \
    RecSkinDialog.cpp \
    RecSkinWidget.cpp \
    TitleWidget.cpp \
    NcFramelessHelper.cpp \
    QjtCustomWidget.cpp \
    RecSkinPickerWidget.cpp \
    QjtTabBar.cpp \
    MToolButton.cpp \
    QjtMessageBox.cpp \
    MOption.cpp \
    MSkinChanger.cpp \
    MThemeAdaptor.cpp \
    MTabBar.cpp \
    MThemeWidgetBase.cpp \
    mutility.cpp \
    MCustomDialog.cpp \
    BleWinAero.cpp

FORMS += \
    RecSkinDialog.ui \
    RecSkinWidget.ui \
    TitleWidget.ui \
    RecSkinPickerWidget.ui \
    QjtShadeWindow.ui \
    MSkinChanger.ui \
    MTabBar.ui

RESOURCES += \
    res.qrc
