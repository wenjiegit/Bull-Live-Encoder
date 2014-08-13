#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QTranslator>
#include <QFontDatabase>

#include "BleMainWindow.hpp"
#include "BleLog.hpp"

#include "BleAudioEncoder_AAC.hpp"
#include "BleAudioCapture.hpp"

int main(int argc, char *argv[])
{
    BleAudioCapture cp;
    cp.startCapture();
    QApplication a(argc, argv);

    // not supported color printf on win
#ifdef Q_OS_WIN
    g_logCtx->setEnableColorPrint(false);
#endif
    g_logCtx->setFilePath("ble.log.txt");
    g_logCtx->setLog2File(true);

    QTranslator translator;
    if(translator.load(QApplication::applicationDirPath()+"/zh_CN.qm"))
            QApplication::installTranslator(&translator);

    QString fontPath = QApplication::applicationDirPath() + "/font/msyh.ttf";

    int fontId = QFontDatabase::addApplicationFont(fontPath);
    if(fontId != -1)
    {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).first();
        QFont font(fontFamily, 9);
        a.setFont(font);
    }

    QFile styleSheetFile(QApplication::applicationDirPath() + "/stylesheet/default.qss");
    if(styleSheetFile.open(QIODevice::ReadOnly))
    {
        QString styleSheetString = styleSheetFile.readAll();
        a.setStyleSheet(styleSheetString);
        styleSheetFile.close();
    }

    BleMainWindow w;
    w.show();

    // update the theme to adaptable font color,
    updateTheme();

    return a.exec();
}
