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

#include "TitleWidget.h"
#include "ui_TitleWidget.h"
#include "MThemeAdaptor.h"
#include "MOption.h"

static void setVisible(QList<QAbstractButton*>& buttons, bool action)
{
    foreach(QAbstractButton *b, buttons) b->setVisible(action);
}

TitleWidget::TitleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleWidget)
  , user1Btn(0)
  , user2Btn(0)
  , user3Btn(0)
{
    ui->setupUi(this);

    ui->closeButton->setPixmap(MThemeAdaptor::instance()->pixmap("Main_Title_Close"));
    ui->maxButton->setPixmap(MThemeAdaptor::instance()->pixmap("Main_Title_Restore"));
    ui->minButton->setPixmap(MThemeAdaptor::instance()->pixmap("Main_Title_Min"));
    ui->menuButton->setPixmap(MThemeAdaptor::instance()->pixmap("Main_Title_Menu"));
    ui->skinButton->setPixmap(MThemeAdaptor::instance()->pixmap("Main_Title_Skin"));

    buttons << ui->closeButton << ui->maxButton << ui->helpButton
            << ui->minButton << ui->menuButton << ui->skinButton;
    ::setVisible(buttons, false);
    setHints(CLOSE_HINT | MAX_HINT | Min_HINT);

    ui->logo->setPixmap(QPixmap(":/images/logo.png"));

    applyNewAdapter();
}

TitleWidget::~TitleWidget()
{
    delete ui;
}

void TitleWidget::setHints(int hints)
{
    ::setVisible(buttons, false);
    hint = hints;

    if(hints & Min_HINT)
    {
        ui->minButton->setVisible(true);
    }

    if(hints & SKIN_HINT)
    {
        ui->skinButton->setVisible(true);
    }

    if(hints & MENU_HINT)
    {
        ui->menuButton->setVisible(true);
    }

    if(hints & CLOSE_HINT)
    {
        ui->closeButton->setVisible(true);
    }

    if(hints & HELP_HINT)
    {
        ui->helpButton->setVisible(true);
    }

    if(hints & MAX_HINT)
    {
        ui->maxButton->setVisible(true);
    }
}

int TitleWidget::hints()
{
    return hint;
}

void TitleWidget::addWidget(QWidget *w)
{
    //ui->mainLayout->removeItem(ui->verticalSpacer);
    ui->mainLayout->addWidget(w);
    //ui->mainLayout->addItem(ui->verticalSpacer);
}

void TitleWidget::addButton(QAbstractButton* btn)
{
    // TODO 增加此代码
}

void TitleWidget::setTitle(const QString &title)
{
    ui->TitleLabel->setText(title);
}

QString TitleWidget::title()
{
    return ui->TitleLabel->text();
}

void TitleWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClicked();
}

void TitleWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
}

void TitleWidget::applyNewAdapter()
{
    QColor color = MOption::instance()->option(OPTION_AVERAGE_COLOR, OPTION_GROUP_Theme).value<QColor>();

    int averageValue = (color.red()+color.green()+color.blue())/3;

    if(averageValue <= 100)
    {
        ui->TitleLabel->setStyleSheet("color: rgb(255, 255, 255);");
        //ui->TitleLabel->setForegroundRole(QPalette::HighlightedText);
    } else {
        ui->TitleLabel->setStyleSheet("color: rgb(10, 10, 10);");
        //ui->TitleLabel->setForegroundRole(QPalette::Light);
    }
}

QAbstractButton* TitleWidget::button(int buttonRole)
{
    QAbstractButton *ret = 0;

    if(buttonRole == Min_HINT)
    {
        ret = ui->minButton;
    }

    if(buttonRole == SKIN_HINT)
    {
        ret = ui->skinButton;
    }

    if(buttonRole == MENU_HINT)
    {
        ret = ui->menuButton;
    }

    if(buttonRole == CLOSE_HINT)
    {
        ret = ui->closeButton;
    }

    if(buttonRole == HELP_HINT)
    {
        ret = ui->helpButton;
    }

    if(buttonRole == MAX_HINT)
    {
        ret = ui->maxButton;
    }

    return ret;
}
