/*
The MIT License (MIT)

Copyright (c) wenjie.zhao

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QTranslator>
#include <QFontDatabase>

#include "BleMainWindow.hpp"
#include "BleLog.hpp"
#include "MOption.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // not supported color printf on win
#ifdef Q_OS_WIN
    g_logCtx->setEnableColorPrint(false);
#endif
    g_logCtx->setFilePath(a.applicationDirPath() + "/logs");
    g_logCtx->setLog2File(true);

    // install language
    QTranslator translator;
    QString lastqm = MOption::instance()->option("qm", "language").toString();
    if (!lastqm.isEmpty()) {
        if(translator.load(QApplication::applicationDirPath() + "/locale/" + lastqm)) {
                QApplication::installTranslator(&translator);
        }
    }

    // install font
    QString fontPath = QApplication::applicationDirPath() + "/font/msyh.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    if(fontId != -1) {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).first();
        QFont font(fontFamily, 9);
        a.setFont(font);
    }

    // install style sheet
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
