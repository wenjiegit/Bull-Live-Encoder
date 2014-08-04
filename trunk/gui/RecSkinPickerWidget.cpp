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

#include "RecSkinPickerWidget.h"
#include "ui_RecSkinPickerWidget.h"
#include "MOption.h"
#include "MThemeWidgetBase.h"

RecSkinPickerWidget::RecSkinPickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecSkinPickerWidget)
{
    ui->setupUi(this);

    connect(this,SIGNAL(PixColor(QColor)),
            ui->PickerWidget,SLOT(setColor(QColor)));

    connect(ui->PickerWidget,SIGNAL(newCol(int,int,int)),
            ui->LuminancePickerWidget,SLOT(setCol(int,int,int)));

    connect(ui->LuminancePickerWidget,SIGNAL(newVal(int)),
            ui->PickerWidget,SLOT(setVal(int)));

    connect(ui->PickerWidget,SIGNAL(newCol(QColor)),
            this,SLOT(onCurrentColorChanged(QColor)));


    QString themeType = MOption::instance()->option("WindowBGPixmapType", "theme").toString();

    if(themeType == "color")
    {
        QColor m_cachedColor = MOption::instance()->option("WindowBGColor", "theme").value<QColor>();
        emit PixColor(m_cachedColor);
    }
    else if(themeType == "bitmap")
    {
        QPixmap skinPix = MOption::instance()->option("WindowBGPixmap", "theme").toString();
        if(!skinPix.isNull())
        {
            QImage skinImage = skinPix.toImage();
            int num = 0;
            int red = 0;
            int green = 0;
            int blue = 0;
            for(int ii = 0; ii < skinImage.width(); ++ii)
            {
                for(int j = 0; j < skinImage.height(); ++j)
                {
                    ++num;
                    QRgb rgbValue(skinImage.pixel(ii, j));
                    red += qRed(rgbValue);
                    green += qGreen(rgbValue);
                    blue += qBlue(rgbValue);
                }
            }

            red = red/num;
            green = green/num;
            blue = blue/num;

            emit PixColor(QColor(red,green,blue,255));
        }
    }
}

RecSkinPickerWidget::~RecSkinPickerWidget()
{
    delete ui;
}

void RecSkinPickerWidget::onCurrentColorChanged(const QColor &c)
{
    MOption::instance()->setOption(QVariant(c), "WindowBGColor", "theme");
    MOption::instance()->setOption("color", "WindowBGPixmapType", "theme");
    MOption::instance()->setOption(QVariant(c), OPTION_AVERAGE_COLOR, OPTION_GROUP_Theme);
    updateThemedWidgets();
    updateTheme();
}

/*******************QColorLuminancePicker******************/
QColorLuminancePicker::QColorLuminancePicker(QWidget* parent)
    :QWidget(parent)
{
    hue = 100; sat = 100; val = 100;
    pix = 0;
    //    setAttribute(WA_NoErase, true);
    //    setCol(150, 255);
    //    setCol(180, 173,200);
}

QColorLuminancePicker::~QColorLuminancePicker()
{
    delete pix;
}

void QColorLuminancePicker::mouseMoveEvent(QMouseEvent *m)
{
    setVal(y2val(m->y()));
}
void QColorLuminancePicker::mousePressEvent(QMouseEvent *m)
{
    setVal(y2val(m->y()));
}

void QColorLuminancePicker::setVal(int v)
{
    if (val == v)
        return;
    val = qMax(0, qMin(v,255));
    delete pix; pix=0;
    repaint();
    emit newHsv(hue, sat, val);
    emit newVal(val);
}

//receives from a hue,sat chooser and relays.
void QColorLuminancePicker::setCol(int h, int s)
{
    setCol(h, s, val);
    //emit newHsv(h, s, val);
}

void QColorLuminancePicker::setCol(int h, int s , int v)
{
    val = v;
    hue = h;
    sat = s;
    delete pix; pix=0;
    //repaint();
    update();
}

void QColorLuminancePicker::paintEvent(QPaintEvent *)
{
    int w = width() - 5;

    QRect r(0, foff, w, height() - 2*foff);
    int wi = r.width() - 2;
    int hi = r.height() - 2;
    if (!pix || pix->height() != hi || pix->width() != wi) {
        delete pix;
        QImage img(wi, hi, QImage::Format_RGB32);
        int y;
        uint *pixel = (uint *) img.scanLine(0);
        for (y = 0; y < hi; y++) {
            const uint *end = pixel + wi;
            while (pixel < end) {
                QColor c;
                c.setHsv(hue, sat, y2val(y+coff));
                *pixel = c.rgb();
                ++pixel;
            }
        }
        pix = new QPixmap(QPixmap::fromImage(img));
    }
    QPainter p(this);
    p.drawPixmap(1, coff, *pix);
    const QPalette &g = palette();
    //qDrawShadePanel(&p, r, g, true);
    p.setPen(g.foreground().color());
    p.setBrush(g.foreground());
    QPolygon a;
    int y = val2y(val);
    a.setPoints(3, w, y, w+5, y+5, w+5, y-5);
    p.eraseRect(w, 0, 5, height());
    p.drawPolygon(a);
}

int QColorLuminancePicker::y2val(int y)
{
    int d = height() - 2*coff - 1;
    return 255 - (y - coff)*255/d;
}

int QColorLuminancePicker::val2y(int v)
{
    int d = height() - 2*coff - 1;
    return coff + (255-v)*d/255;
}


/*********************QColorPickerWidget************/
QColorPickerWidget::QColorPickerWidget(QWidget* parent) :
    QWidget(parent)
{
    hue = 100; sat = 100; val = 100;
    //    setCol(150, 255);
    //    setCol(180, 173,200);
}

QColorPickerWidget::~QColorPickerWidget()
{
}

QPoint QColorPickerWidget::colPt()
{
    QRect r = contentsRect();
    return QPoint((360 - hue) * (r.width() - 1) / 360, (255 - sat) * (r.height() - 1) / 255);
}

int QColorPickerWidget::huePt(const QPoint &pt)
{
    QRect r = contentsRect();
    return 360 - pt.x() * 360 / (r.width() - 1);
}

int QColorPickerWidget::satPt(const QPoint &pt)
{
    QRect r = contentsRect();
    return 255 - pt.y() * 255 / (r.height() - 1);
}

void QColorPickerWidget::setCol(const QPoint &pt)
{
    setCol(huePt(pt), satPt(pt));
}

void QColorPickerWidget::setCol(int h, int s)
{
    setCol(h, s, val);
}

void QColorPickerWidget::setColor(const QColor &color)
{
    int h = color.hsvHue();
    int s = color.hsvSaturation();
    int v = color.value();
    int nhue = qMin(qMax(0,h), 359);
    int nsat = qMin(qMax(0,s), 255);
    int nval = qMin(qMax(0,v), 255);
    if (nhue == hue && nsat == sat && nval == val)
        return;

    QRect r(colPt(), QSize(20,20));
    hue = nhue; sat = nsat; val = nval;
    r = r.united(QRect(colPt(), QSize(20,20)));
    r.translate(contentsRect().x()-9, contentsRect().y()-9);
    //update(r);
    //repaint(r);
    //repaint();
    update();
    emit newCol(hue, sat, val);
}


void QColorPickerWidget::setCol(int h, int s, int v)
{
    int nhue = qMin(qMax(0,h), 359);
    int nsat = qMin(qMax(0,s), 255);
    int nval = qMin(qMax(0,v), 255);
    if (nhue == hue && nsat == sat && nval == val)
        return;

    QRect r(colPt(), QSize(20,20));
    hue = nhue; sat = nsat; val = nval;
    r = r.united(QRect(colPt(), QSize(20,20)));
    r.translate(contentsRect().x()-9, contentsRect().y()-9);
    //update(r);
    //repaint(r);
    //repaint();
    update();
    emit newCol(hue, sat);
    emit newCol(hue, sat, val);
    QColor color;
    color.setHsv(hue,sat,val);
    emit newCol(color);
    //emit newCol(color.toRgb());
}

void QColorPickerWidget::setVal(int nval)
{
    val = nval;
    QRect r(colPt(), QSize(20,20));
    r = r.united(QRect(colPt(), QSize(20,20)));
    r.translate(contentsRect().x()-9, contentsRect().y()-9);
    //update(r);
    //repaint(r);
    //repaint();
    update();
    QColor color;
    color.setHsv(hue,sat,val);
    emit newCol(color);
}

void QColorPickerWidget::mouseMoveEvent(QMouseEvent *m)
{
    QPoint p = m->pos() - contentsRect().topLeft();
    setCol(p);
}

void QColorPickerWidget::mousePressEvent(QMouseEvent *m)
{
    QPoint p = m->pos() - contentsRect().topLeft();
    setCol(p);
}

void QColorPickerWidget::paintEvent(QPaintEvent* )
{
    int w = this->width();
    int h = this->height();
    QImage img(w, h, QImage::Format_RGB32);
    int x, y;
    uint *pixel = (uint *) img.scanLine(0);
    for (y = 0; y < h; y++) {
        const uint *end = pixel + w;
        x = 0;
        while (pixel < end) {
            QPoint p(x, y);
            QColor c;
            c.setHsv(huePt(p), satPt(p), val);
            *pixel = c.rgb();
            ++pixel;
            ++x;
        }
    }
    pix = QPixmap::fromImage(img);

    QPainter p(this);
    QRect r = contentsRect();

    //p.drawPixmap(rect(), pix);
    p.drawPixmap(r.topLeft(), pix);
    QPoint pt = colPt() + r.topLeft();
    p.setPen(Qt::black);

    QColor color;
    if(val >= 255/2)
    {
        color = QColor(0,0,0,255);
    }
    else
    {
        color = QColor(255,255,255,255);
    }

    p.fillRect(pt.x()-9, pt.y(), 20, 2, color);
    p.fillRect(pt.x(), pt.y()-9, 2, 20, color);

}
