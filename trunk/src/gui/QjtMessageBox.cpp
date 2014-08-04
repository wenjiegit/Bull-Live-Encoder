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

#include "QjtMessageBox.h"

#include <QMessageBox>
#include <QLayout>
#include <QPushButton>
#include <QEvent>

static QMessageBox::StandardButton showMessageBox(enum QMessageBox::Icon icon, QWidget * parent, const QString & title, const QString & text,
                                                              QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                              QMessageBox::StandardButton defaultButton = QMessageBox::NoButton )
{
    Q_UNUSED(parent);
    QjtMessageBox box;
    box.boxWidget()->setText(text);
    box.boxWidget()->setIcon(icon);
    box.setTitle(title);
    box.boxWidget()->setStandardButtons(buttons);
    box.boxWidget()->setDefaultButton(defaultButton);

    box.exec();

    QMessageBox::StandardButton ret = QMessageBox::NoButton;
    QAbstractButton* clickedButton = box.boxWidget()->clickedButton();
    if(clickedButton)
        return box.boxWidget()->standardButton(clickedButton);
    else
        return ret;
}


QjtMessageBox::QjtMessageBox(QWidget *parent)
    : MCustomDialog(parent)
    , msg(new QMessageBox(this))
{
    msg->setWindowFlags(Qt::Widget);
    msg->setAttribute(Qt::WA_TranslucentBackground);
    msg->installEventFilter(this);

    addWidget(msg);
    setResizable(false);
}

QjtMessageBox::~QjtMessageBox()
{

}

bool QjtMessageBox::eventFilter(QObject *o, QEvent *e)
{
    if(o == msg)
    {
        switch(e->type())
        {
        case QEvent::Close:
        case QEvent::Hide:
            close();
            break;
        case QEvent::Resize:
            setFixedSize(msg->width(), msg->height() + 30);
            break;
        default: break;
        }
    }

    return o->eventFilter(o, e);
}


QMessageBox::StandardButton	QjtMessageBox::critical(QWidget *parent,
                                                    const QString &title,
                                                    const QString &text,
                                                    QMessageBox::StandardButtons buttons,
                                                    QMessageBox::StandardButton defaultButton)
{
    return showMessageBox(QMessageBox::Critical, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton	QjtMessageBox::information(QWidget *parent,
                                                       const QString &title,
                                                       const QString &text,
                                                       QMessageBox::StandardButtons buttons,
                                                       QMessageBox::StandardButton defaultButton)
{
    return showMessageBox(QMessageBox::Information, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton	QjtMessageBox::question(QWidget *parent,
                                                    const QString &title,
                                                    const QString &text,
                                                    QMessageBox::StandardButtons buttons,
                                                    QMessageBox::StandardButton defaultButton)
{
    return showMessageBox(QMessageBox::Question, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton	QjtMessageBox::warning (QWidget *parent,
                                                    const QString &title,
                                                    const QString &text,
                                                    QMessageBox::StandardButtons buttons,
                                                    QMessageBox::StandardButton defaultButton)
{
    return showMessageBox(QMessageBox::Warning, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton	QjtMessageBox::showMsg ( QWidget * parent,
                                                     const QString & title,
                                                     const QString & text,
                                          QMessageBox::StandardButtons buttons,
                                          QMessageBox::StandardButton defaultButton)
{
    return showMessageBox(QMessageBox::NoIcon, parent, title, text, buttons, defaultButton);
}
