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

#ifndef RECSKINWIDGET_H
#define RECSKINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "gui_global.h"

class QLabel;
class QPainter;
class RecSkinPushButton;
class QMouseEvent;
class QTimer;

namespace Ui {
class RecSkinWidget;
}

class GUISHARED_EXPORT RecSkinWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit RecSkinWidget(QWidget *parent = 0);
    ~RecSkinWidget();
    
private:
    Ui::RecSkinWidget *ui;
    QList<RecSkinPushButton *> buttons;
    QPixmap onePix;
    QColor oneColor;
    QString oneFileName;
    int nowPixNum;
    int m_rowMaxPix;

private slots:
    void addItem(const QString &fileName,const QPixmap &pix,
                 const QColor &average,bool isColor,bool isSkin);
    void onSavePix(const QString &fileName,const QPixmap &pix,const QColor &average);
    void getRgb();
    void onCloseNum(int num,const QString &fileName);
    void onClickNum(int num);

signals:
    void currentPixmap(const QString &fileName,const QPixmap &pix,const QColor &averageColor);
    void definePixmapResult();
};

/*******************************RecSkinPushButton**************************************/
class GUISHARED_EXPORT RecSkinPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit RecSkinPushButton(QWidget *parent = 0);

    inline void setSkin(bool type)
    {
        isSkin = type;
    }

    inline void setCanDelete(bool del) { canDelete = del; }

    inline void setPixmap(const QString &fileName,const QPixmap &pixmap,const QColor &averageColor,int size,int isColorTrue = false)
    {
        this->fileName = fileName;
        originalPixmap = pixmap;
        drawPixmap = pixmap.scaled(size,size);
        this->averageColor = averageColor;
        this->isColorTrue = isColorTrue;
        setFixedSize(drawPixmap.width(),drawPixmap.height());
        update();
    }

    inline void setNum(int num)
    {
        m_num = num;
    }
    inline QString getFileName()
    {
        return fileName;
    }

protected:
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *);

private:
    QPixmap originalPixmap;
    QPixmap drawPixmap;
    QColor averageColor;
    bool isEnter;
    bool isSkin;
    bool isColorTrue;
    bool canDelete; // �������Ƿ�����ɾ��
    int m_num;
    QString fileName;
    QPushButton *m_closeButton;

private slots:
    void onClose();

signals:
    void currentPixmap(const QString &fileName,const QPixmap &pix,const QColor &averageColor);
    void clickResult();
    void closeNum(int num,const QString &fileName);
    void clickNum(int num);
};

class GUISHARED_EXPORT RecSkinColorPalette : public QWidget
{
    Q_OBJECT
public:
    explicit RecSkinColorPalette(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QImage drawImage;
    bool isMousePress;
    bool isDrawEllipse;
    QPoint mousePoint;
    int hsvValue;

private slots:
    void horizontalSliderValueChanged(int);
    void getColor(const QColor &averageColor);

signals:
    void sigMousePressRGB(const QRgb &rgb);
    void currentColor(const QColor &color);
};

class GUISHARED_EXPORT RecSkinColorBrightnessRange :public QWidget
{
    Q_OBJECT

public:
    explicit RecSkinColorBrightnessRange(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    QImage drawImage;
    bool isMousePress;
    bool isDrawEllipse;
    QPoint mousePoint;

private slots:
    void onGetColor(const QRgb &rgb);

signals:
    void sigMousePressSize(int);
    void currentColor(const QColor &color);
};
#endif // RECSKINWIDGET_H
