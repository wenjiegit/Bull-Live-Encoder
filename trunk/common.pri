PROJECT_BINDIR = $$PWD/bin
PROJECT_LIBDIR = $$PWD/lib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# lib - gui
LIB_GUILIB_PATH=$$PWD/gui
TEMPLATE += fakelib
LIB_GUILIB_NAME = $$qtLibraryTarget(ble_gui)
TEMPLATE -= fakelib
