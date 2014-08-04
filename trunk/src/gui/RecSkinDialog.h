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

#ifndef RECSKINDIALOG_H
#define RECSKINDIALOG_H

#include <QDialog>

class QPainter;
class QKSkinSimulationWidget;
class QKSkinImageWidget;
class QMouseEvent;
class QMenu;

namespace Ui {
class RecSkinDialog;
}

class RecSkinDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RecSkinDialog(const QImage &mainImage,const QImage &skinImage,QWidget *parent = 0);
    ~RecSkinDialog();
    void setImage(const QImage &mainImage, const QImage &skinImage);

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::RecSkinDialog *ui;
    QKSkinSimulationWidget *SkinSimulationWidget;
    QImage skinImage;
    int red;
    int green;
    int blue;
    int increase;

private slots:
    void onSave();
    void onCancel();

signals:
    void sigSavePix(const QString &fileName,const QPixmap &pix,const QColor &averageColor);
};


class QKSkinSimulationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QKSkinSimulationWidget(QWidget *parent = 0);
    void setWidget(QWidget *child);
    void setImage(const QImage &image);

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    //bool eventFilter(QObject *, QEvent *);

private:
       QPoint dragPosition;
       QImage image;
       QMenu *menu;

signals:
       void sigSave();
       void sigCancel();
};

#endif // RECSKINDIALOG_H
