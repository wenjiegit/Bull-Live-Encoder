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

#include "MTabBar.h"
#include "ui_MTabBar.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QEvent>
#include <QPropertyAnimation>
#include <QSignalMapper>
#include <QDebug>
#include "MOption.h"

MTabBar::MTabBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MTabBar),
    startNum(0),
    buttonspace(10),
    pushWidth(70),
    pushHeight(70)
{
    ui->setupUi(this);
    m_pProAnima = 0;
    m_pBtnAnima = 0;
    m_pSignalMapper = new QSignalMapper(this);
    connect(m_pSignalMapper, SIGNAL(mapped(int)),this, SLOT(SetAnimation(int)));
}

MTabBar::~MTabBar()
{
    delete ui;
}

void MTabBar::addTab(const QPixmap &darwPixmap,const QString &text)
{
    MTabBarPushButton *tabBarPushButton = new MTabBarPushButton(this);
    tabBarPushButton->setNum(buttonList.length());
    tabBarPushButton->setPixmap(darwPixmap,text);

    pushWidth = tabBarPushButton->width();
    pushHeight = tabBarPushButton->height();


    m_pSignalMapper->setMapping(tabBarPushButton, QEasingCurve::OutSine);
    connect(tabBarPushButton,SIGNAL(select(int)),this,SLOT(onSelect(int)));
    connect(tabBarPushButton, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));

    buttonList << tabBarPushButton;

    ui->horizontalLayout->addWidget(tabBarPushButton);
    if(!buttonList.isEmpty())
        buttonList.value(0)->setPress(true);

    if(m_pBtnAnima != 0)
        m_pBtnAnima->deleteLater();

    if(m_pProAnima != 0)
        m_pProAnima->deleteLater();

    m_pBtnAnima = new MTabBarPushButton(this);
    m_pBtnAnima->setAnimationButton(pushWidth,pushHeight);
    m_pBtnAnima->hide();

    m_pProAnima = new QPropertyAnimation(m_pBtnAnima,"geometry");
    connect(m_pProAnima,SIGNAL(finished()),this,SLOT(animaFinished()));
}

void MTabBar::resizeEvent(QResizeEvent *)
{
    if(!buttonList.isEmpty())
    {
        pushWidth = buttonList.value(0)->width();
        pushHeight = buttonList.value(0)->height();
    }
    if(m_pBtnAnima)
        m_pBtnAnima->setAnimationButton(pushWidth,pushHeight);
}

void MTabBar::onSelect(int num)
{
    endNUm = num;
    for(int i = 0; i < buttonList.length(); ++i)
    {
        if(i != num)
            buttonList.value(i)->setPress(false);
        else
            buttonList.value(i)->setPress(true);
    }
    emit currentChanged(num);
}

void MTabBar::SetAnimation(int nCurveType)
{
    if(m_pProAnima)
    {
        if(m_pProAnima->state()==QPropertyAnimation::Running)
        {
            m_pProAnima->stop();
        }
    }
    if(m_pBtnAnima)
        m_pBtnAnima->show();

    if(m_pProAnima)
    {
        m_pProAnima->setDuration(350);
        m_pProAnima->setStartValue(QRect(startNum * pushWidth,0,pushWidth,pushHeight));
        m_pProAnima->setEndValue(QRect(pushWidth * endNUm,0,pushWidth,pushHeight));
        m_pProAnima->setEasingCurve(QEasingCurve::Type(nCurveType));
        m_pProAnima->start();
    }
    startNum = endNUm;
}

void MTabBar::animaFinished()
{
    m_pBtnAnima->hide();
}




/*****************************MTabBarPushButton***************************/

MTabBarPushButton::MTabBarPushButton(QWidget *parent) :
    QPushButton(parent),
    isEnter(false),
    isPress(false),
    isAnimation(false),
    currentRGBA(0),
    averageRGBA(0),
    maxFrameRange(10),
    m_height(75),
    m_distance(5)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedHeight(m_height);
    timeLine.setDuration(500);
    connect(&timeLine, SIGNAL(frameChanged(int)), this, SLOT(frameChanged(int)));
    //connect(&timeLine, SIGNAL(finished()), this, SLOT(finished()));
    timeLine.setFrameRange(0, maxFrameRange);
}

void MTabBarPushButton::frameChanged(int frame)
{
    currentRGBA = averageRGBA*frame;
    update();
}

void MTabBarPushButton::finished()
{
    //    int direction = timeLine.direction();
    //    switch(direction)
    //    {
    //    case QTimeLine::Backward:
    //        setIcon(m_normal);
    //    }
    qDebug() << "finished";
}


void MTabBarPushButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if(isAnimation)
    {
        drawColor(70,100,&painter);
        return;
    }

    if(isPress)
    {
        drawColor(80,100,&painter);
    }
    else if(isEnter)
    {
        drawColor(currentRGBA,100,&painter);
    }
    else
    {
        drawColor(currentRGBA,100,&painter);
    }

    if(!drawPixmap.isNull() && !m_text.isEmpty())
    {
//        QFont font(this->font());
//        font.setPointSize(11);
//        painter.setFont(font);
        QFontMetrics fm(this->font());
        int textHeight = fm.height();
        int PixHeight = drawPixmap.height();
        int PixWidth = drawPixmap.width();
        int drawX = this->width()/2 - PixWidth/2;
        int drawY = this->height()/2 - (PixHeight + textHeight + m_distance)/2;
        QRect imageRect(drawX,drawY,PixWidth,PixHeight);
        painter.drawPixmap(imageRect,drawPixmap);

        QColor color = MOption::instance()->option(OPTION_AVERAGE_COLOR, OPTION_GROUP_Theme).value<QColor>();

        int averageValue = (color.red()+color.green()+color.blue())/3;

        if(averageValue <= 100)
        {
            painter.setPen(Qt::white);
            painter.setBrush(QBrush(Qt::white));
        }
        else
        {
            painter.setPen(Qt::black);
            painter.setBrush(QBrush(Qt::black));
        }
        QRect textRect(0,drawY + drawPixmap.height() + m_distance,this->width(),textHeight);
        painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, m_text);

    }
}

void MTabBarPushButton::drawColor(int nTopPartOpacity,int nBottomPartOpacity,QPainter *painter)
{
    QRect r = rect();
    r.setWidth(r.width() - 1);
    r.setHeight(r.height() - 1);

    QPainterPath path;
    path.addRoundedRect(r, 4, 4);
    path.closeSubpath();

    QColor color0(247,255,255,nTopPartOpacity);
    QColor color1(242,255,255,nTopPartOpacity);
    QColor color2(245,255,255,nTopPartOpacity);
    QLinearGradient knobGradientTmp(0, 0, 0, height());
    knobGradientTmp.setColorAt(0.0, color0);
    knobGradientTmp.setColorAt(0.1, color1);
    knobGradientTmp.setColorAt(0.5, color2);
    knobGradientTmp.setColorAt(0.9, color1);
    knobGradientTmp.setColorAt(1.0, color0);

    QColor bl(0,0,0,100);
    QPen pen(bl);
    pen.setWidthF(0.5);
    if(isPress)
        painter->setPen(pen);
    else
        painter->setPen(Qt::NoPen);
    painter->setBrush(knobGradientTmp);
    painter->drawPath(path);

}

void MTabBarPushButton::enterEvent(QEvent *)
{
    if(isPress)
        return;
    isEnter = true;
    RGBA = 70;
    averageRGBA = RGBA/maxFrameRange;
    currentRGBA = 0;
    timeLine.stop();
    timeLine.setDirection(QTimeLine::Forward);
    timeLine.start();
}

void MTabBarPushButton::mousePressEvent(QMouseEvent *event)
{
    if(isPress)
        return;
    QPushButton::mousePressEvent(event);
    RGBA = 200;
    averageRGBA = RGBA/maxFrameRange;
    currentRGBA = 0;
    isPress = true;
    emit select(num);
    timeLine.stop();
    timeLine.setDirection(QTimeLine::Forward);
    timeLine.start();
}

void MTabBarPushButton::mouseMoveEvent(QMouseEvent *e)
{
    int x = e->x();
    int y = e->y();
    if(x >= 0 && x <= width() && y >= 0 && y <= height())
    {
        ;
    }
    else
    {
        return;
    }
}

void MTabBarPushButton::leaveEvent(QEvent *)
{
    if(isPress)
        return;
    RGBA = 70;
    averageRGBA = RGBA/maxFrameRange;
    currentRGBA = 70;
    isEnter = false;
    timeLine.stop();
    timeLine.setDirection(QTimeLine::Backward);
    timeLine.start();
}

void MTabBarPushButton::setPress(bool isInPress)
{
    if(isPress)
    {
        RGBA = 200;
        averageRGBA = RGBA/maxFrameRange;
        currentRGBA = 200;
        isPress = isInPress;
        timeLine.stop();
        timeLine.setDirection(QTimeLine::Backward);
        timeLine.start();
    }
    if(isInPress)
    {
        RGBA = 200;
        averageRGBA = RGBA/maxFrameRange;
        currentRGBA = 0;
        isPress = isInPress;
        timeLine.stop();
        timeLine.setDirection(QTimeLine::Forward);
        timeLine.start();
    }
}
