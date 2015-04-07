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

#ifndef BLESETTINGETTING_H
#define BLESETTINGETTING_H

#include <QWidget>
#include <QStandardItemModel>

#include "MCustomDialog.h"

namespace Ui {
class BleSetting;
}

class BleSetting : public QWidget
{
    Q_OBJECT

public:
    explicit BleSetting(QWidget *parent = 0);
    ~BleSetting();

    void paintEvent(QPaintEvent *);

private slots:
    void onOKClicked();
    void onCancelClicked();
    void onApplyClicked();
    void onAudioBitrateChanged(const QString & text);
    void onQualityValueChanged(int value);
    void onBrowseClicked();
    void onEnableSaveStateChanged(int state);
    void onClicked(const QModelIndex & index);

private:
    void restore();

    void restoreLanguage();

signals:
    void start();
    void stop();
    void settingChanged();

private:
    Ui::BleSetting *ui;
    QStandardItemModel m_model;
};

class BleSettingDialog : public MCustomDialog
{
    Q_OBJECT
public:
    explicit BleSettingDialog(QWidget *parent = 0);
    ~BleSettingDialog();
};

#include <QStyledItemDelegate>

/*!
    why use ItemDelegate ?
    Qt will draw a dotted line rect which is ugly, ugly, ugly!
    so using ItemDelegate to avoid this.
    @see http://blog.sina.com.cn/s/blog_a6fb6cc90101i8it.html
*/
class StyleItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StyleItemDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent)
    {

    }

    ~StyleItemDelegate()
    {

    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem  view_option(option);
        if (view_option.state & QStyle::State_HasFocus) {
            view_option.state = view_option.state ^ QStyle::State_HasFocus;
        }

        QStyledItemDelegate::paint(painter, view_option, index);
    }
};

#endif // BLESETTINGETTING_H
