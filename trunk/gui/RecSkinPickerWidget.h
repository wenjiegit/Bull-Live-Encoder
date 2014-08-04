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

#ifndef RECSKINPICKERWIDGET_H
#define RECSKINPICKERWIDGET_H

#include <QWidget>
#include <QtGui>


class QColorLuminancePicker;
class QColorPickerWidget;

namespace Ui {
class RecSkinPickerWidget;
}

class RecSkinPickerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit RecSkinPickerWidget(QWidget *parent = 0);
    ~RecSkinPickerWidget();
    
private:
    Ui::RecSkinPickerWidget *ui;

private slots:
    void onCurrentColorChanged(const QColor &c);

signals:
    void PixColor(const QColor &c);
};

class QColorLuminancePicker : public QWidget
{
    Q_OBJECT
public:
    QColorLuminancePicker(QWidget* parent=0);
    ~QColorLuminancePicker();

public slots:
    void setCol(int h, int s, int v);
    void setCol(int h, int s);

signals:
    void newHsv(int h, int s, int v);
    void newVal(int v);

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    enum { foff = 3, coff = 4 }; //frame and contents offset
    int val;
    int hue;
    int sat;

    int y2val(int y);
    int val2y(int val);
    void setVal(int v);

    QPixmap *pix;
};


class QColorPickerWidget : public QWidget
{
    Q_OBJECT
public:
    QColorPickerWidget(QWidget* parent);
    ~QColorPickerWidget();

public slots:
    void setColor(const QColor &color);
    void setCol(int h, int s);
    void setCol(int h, int s, int v);
    void setVal(int nval);

signals:
    void newCol(int h, int s);
    void newCol(int h, int s, int v);
    void newCol(const QRgb &rgb);
    void newCol(const QColor &color);

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    //void resizeEvent(QResizeEvent *);

private:
    int hue;
    int sat;
    int val;

    QPoint colPt();
    int huePt(const QPoint &pt);
    int satPt(const QPoint &pt);
    void setCol(const QPoint &pt);

    QPixmap pix;
};

#endif // RECSKINPICKERWIDGET_H
