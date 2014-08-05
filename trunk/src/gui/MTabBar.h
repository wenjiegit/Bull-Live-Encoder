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

#ifndef MTabBar_H
#define MTabBar_H

#include <QWidget>
#include <QPushButton>
#include <QTimeLine>

class QPixmap;
class QMouseEvent;
class QEvent;
class QPainter;
class MTabBarPushButton;
class QPropertyAnimation ;
class QSignalMapper;

namespace Ui {
class MTabBar;
}

class MTabBar : public QWidget
{
    Q_OBJECT
    
public:
    explicit MTabBar(QWidget *parent = 0);
    ~MTabBar();

    void addTab(const QPixmap &darwPixmap,const QString &text = "");
    inline int count() { return buttonList.size(); }
    inline int currentIndex() { return endNUm; }

public slots:
    void onSelect(int num);
    
private:
    Ui::MTabBar *ui;
    QList<MTabBarPushButton *> buttonList;

    MTabBarPushButton *m_pBtnAnima;
    QPropertyAnimation *m_pProAnima;
    QSignalMapper *m_pSignalMapper;

    int pushWidth;
    int pushHeight;
    int startNum;
    int endNUm;
    const int buttonspace;

private slots:
    void SetAnimation(int nCurveType);
    void animaFinished();

protected:
    void resizeEvent(QResizeEvent *);

signals:
    void currentChanged(int num);
};


/****************************MTabBarPushButton***************************/

class MTabBarPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MTabBarPushButton(QWidget *parent = 0);

    inline void setPixmap(const QPixmap &pixmap,const QString &text)
    {
        drawPixmap = pixmap;
        drawPixmap = pixmap.scaled(45,45);
        m_text = text;
        update();
    }

    void setPress(bool isPress);

    inline void setNum(int num)
    {
        this->num = num;
    }

    inline void setAnimationButton(int width,int height)
    {
        setFixedSize(width,height);
        isAnimation = true;
        update();
    }


protected:
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void leaveEvent(QEvent *e);

private:
    QPixmap drawPixmap;
    int num;
    int m_height;

    bool isEnter;
    bool isPress;
    bool isAnimation;

    QTimeLine timeLine;
    int RGBA;
    int averageRGBA;
    int currentRGBA;
    int maxFrameRange;
    QString m_text;
    int m_distance;

private:
    void drawColor(int nTopPartOpacity,int nBottomPartOpacity,QPainter *pPainter);

signals:
    void select(int num);

private slots:
    void frameChanged(int frame);
    void finished();
};

#endif // QKTABBAR_H
