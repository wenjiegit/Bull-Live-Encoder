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

#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QAbstractButton>
#include "gui_global.h"
#include "MThemeWidgetBase.h"

namespace Ui {
    class TitleWidget;
}

class GUISHARED_EXPORT TitleWidget : public QWidget, public WidgetColorAdapter
{
    Q_OBJECT

public:
    explicit TitleWidget(QWidget *parent = 0);
    ~TitleWidget();

    enum{
        Min_HINT    = 0x00000001,
        SKIN_HINT   = 0x00000002,
        MENU_HINT   = 0x00000004,
        CLOSE_HINT  = 0x00000008,
        HELP_HINT   = 0x00000010,
        MAX_HINT    = 0x00000020,
        USER_HINT1  = 0x00000080,
        USER_HINT2  = 0x00000100,
        USER_HINT3  = 0x00000200
    };

    void setHints(int hints);
    int hints();

    void addWidget(QWidget *w);
    void addButton(QAbstractButton* btn);

    QAbstractButton* button(int buttonRole);

    void setTitle(const QString &title);
    QString title();

protected:
    void mouseDoubleClickEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *e);
    void applyNewAdapter();

private:
    Ui::TitleWidget *ui;
    QList<QAbstractButton*> buttons;
    QAbstractButton *user1Btn;
    QAbstractButton *user2Btn;
    QAbstractButton *user3Btn;
    int hint;

signals:
    void doubleClicked();
};

#endif // TITLEWIDGET_H
