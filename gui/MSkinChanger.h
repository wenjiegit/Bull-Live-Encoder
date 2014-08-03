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

#ifndef RECCHOICESKINWIDGET_H
#define RECCHOICESKINWIDGET_H

#include <QWidget>
#include "gui_global.h"
#include "mutility.h"
#include "QjtCustomWidget.h"

class RecSkinWidget;
class RecSkinColorPalette;
class RecSkinDialog;
class RecSkinPickerWidget;

namespace Ui {
class MSkinChanger;
}

class GUISHARED_EXPORT MSkinChanger : public QWidget
{
    Q_OBJECT
    
public:
    explicit MSkinChanger(QWidget *parent = 0);
    ~MSkinChanger();
    
private slots:
    void onDefinePixmap();

    void onAeroTransparentValueChanged(int v);
    void onWidgetTransparentValueChanged(int v);
    void onCurrentPixmap(const QString &fileName,const QPixmap &pix,const QColor &averageColor);

private:
    Ui::MSkinChanger *ui;
    RecSkinWidget *SkinWidget;
    RecSkinPickerWidget *SkinPickerWidget;
    RecSkinDialog *SkinDialog;

signals:
    void currentColor(const QColor &color);
    void currentPixmap(const QString &fileName,const QPixmap &pix,const QColor &averageColor);
};

class GUISHARED_EXPORT MSkinChangerWidget : public QjtCustomWidget
{
    Q_OBJECT
public:
    explicit MSkinChangerWidget(QWidget *parent = 0);
};

#endif // RECCHOICESKINWIDGET_H
