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

#include "MThemeWidgetBase.h"

#include <QWidget>
#include <QPainter>
#include <QApplication>
#include <QMainWindow>
#include <QFile>

#include "MOption.h"
#include "mutility.h"

static QList<ThemedWidgetBase*> g_themedWidgets;
static QList<WidgetColorAdapter*> g_colorAdapter;

ThemedWidgetBase::ThemedWidgetBase(QWidget *parent)
    : m_titleHeight(0)
    , m_linearHeight(0)
    , m_statusHeight(0)
    , m_themedWidget(parent)
{
    applyNewTheme();
    regThemedWidget(this);
}

ThemedWidgetBase::~ThemedWidgetBase()
{
    removeThemedWidget(this);
}

void ThemedWidgetBase::applyNewTheme()
{
    m_aeroTransparent = MOption::instance()->option("AeroTransparent", "theme").toInt();
    m_widgetTransparent = MOption::instance()->option("WidgetTransparent", "theme").toInt();

    m_cachedPixmap = QPixmap(QCoreApplication::applicationDirPath() + MOption::instance()->option("WindowBGPixmap", "theme").toString());
    m_cachedPixmap = setAlphaPixmap(m_cachedPixmap, m_aeroTransparent);

    m_cachedColor = MOption::instance()->option("WindowBGColor", "theme").value<QColor>();
    m_cachedColor.setAlpha(m_aeroTransparent);

    m_themedWidget->update();
}

void ThemedWidgetBase::drawThemedStyle(QPainter &p, bool drawLinar)
{
    QPainterPath path;
    QPoint bottomRight = m_themedWidget->rect().bottomRight();
    QRect pathRect = m_themedWidget->rect();
    pathRect.setBottomRight(QPoint(bottomRight.x()-1, bottomRight.y()-1));
    path.addRoundedRect(pathRect, 0, 0);

    QString themeType = MOption::instance()->option("WindowBGPixmapType", "theme").toString();
    if(themeType == "bitmap")
    {
        if(!m_cachedPixmap.isNull())
        {
            p.fillPath(path, QBrush(m_cachedPixmap));
        }
        else
        {
            QColor color(100,178,226,255);
            MOption::instance()->setOption(QVariant(color), "WindowBGColor", "theme");
            MOption::instance()->setOption("color", "WindowBGPixmapType", "theme");
            MOption::instance()->setOption(QVariant(color), OPTION_AVERAGE_COLOR, OPTION_GROUP_Theme);

            p.fillPath(path, color);
        }
    } else if(themeType == "color")
    {
        QImage image(QSize(100, 100), QImage::Format_ARGB32);
        image.fill(MOption::instance()->option("WindowBGColor", "theme").value<QColor>());
        m_cachedPixmap = setAlphaPixmap(QPixmap::fromImage(image), m_aeroTransparent);
        p.fillPath(path, QBrush(m_cachedPixmap));
    }

    QRect linearRect(0, m_titleHeight, m_themedWidget->width(), m_linearHeight);
    QLinearGradient linear(0, m_titleHeight, 0, m_linearHeight+m_titleHeight);

    QColor c1(255, 255, 255, m_widgetTransparent);
    QColor c2(255, 255, 255, m_widgetTransparent / 2);

    linear.setColorAt(0.0, Qt::transparent);
    //linear.setColorAt(0.5, c2);
    linear.setColorAt(1.0, c1);

    p.setBrush(QBrush(linear));
    p.fillRect(linearRect, QBrush(linear));


    QPainterPath clientPath;
    QRect clientRect(0, m_titleHeight + m_linearHeight,
                     m_themedWidget->width(), m_themedWidget->height() - m_titleHeight - m_linearHeight - m_statusHeight);
    clientPath.addRect(clientRect);
    p.fillPath(clientPath, QBrush(QColor(255, 255, 255, m_widgetTransparent)));

    p.strokePath(path, QPen(QColor(0, 0, 0, 150)));
}

WidgetColorAdapter::WidgetColorAdapter()
{
    regColorAdapterWidget(this);
}

WidgetColorAdapter::~WidgetColorAdapter()
{
    removeColorAdapterWidget(this);
}

void WidgetColorAdapter::applyNewAdapter()
{

}

void regThemedWidget(ThemedWidgetBase *w)
{
    g_themedWidgets.append(w);
}

void removeThemedWidget(ThemedWidgetBase *w)
{
    g_themedWidgets.removeAll(w);
}

void updateThemedWidgets()
{
    foreach (ThemedWidgetBase *w, g_themedWidgets) {
        w->applyNewTheme();
    }
}

void updateTheme()
{
    QColor c = MOption::instance()->option(OPTION_AVERAGE_COLOR,
                                             OPTION_GROUP_Theme).value<QColor>();


    //int widgetTransparent = MOption::instance()->option(OPTION_WidgetTransparent, OPTION_GROUP_Theme).toInt();
    int aeroTransparent = MOption::instance()->option(OPTION_AeroTransparent, OPTION_GROUP_Theme).toInt();
    QString sheetPath = QApplication::applicationDirPath() + "/stylesheet";
    // 读取所有的style sheet
    QStringList styleSheetList;
    styleSheetList << sheetPath + "/default.qss"
                   << sheetPath + "/hbar.qss"
                   << sheetPath + "/menu.qss"
                   << sheetPath + "/tabview.qss"
                   << sheetPath + "/vbar.qss"
                   << sheetPath + "/checkbox.qss";

    QString sheetStr;
    for(int i = 0; i < styleSheetList.size(); ++i)
    {
        const QString &str = styleSheetList.at(i);

        QFile file(str);
        if(file.open(QIODevice::ReadOnly))
        {
            sheetStr += file.readAll();
        }
    }

    sheetStr.replace("menu_background-color_red",   QString::number(c.red()));
    sheetStr.replace("menu_background-color_green", QString::number(c.green()));
    sheetStr.replace("menu_background-color_blue",  QString::number(c.blue()));
    sheetStr.replace("menu_background-color_alpha", QString::number(aeroTransparent));

    sheetStr.replace("horizontal_handle_1", QString::number(c.red()));
    sheetStr.replace("horizontal_handle_2", QString::number(c.green()));
    sheetStr.replace("horizontal_handle_3", QString::number(c.blue()));
    sheetStr.replace("horizontal_handle_4", QString::number(60));

    sheetStr.replace("horizontal_handle_hover_1", QString::number(c.red()));
    sheetStr.replace("horizontal_handle_hover_2", QString::number(c.green()));
    sheetStr.replace("horizontal_handle_hover_3", QString::number(c.blue()));
    sheetStr.replace("horizontal_handle_hover_4", QString::number(100));

    sheetStr.replace("horizontal_sub-line_1", QString::number(c.red()));
    sheetStr.replace("horizontal_sub-line_2", QString::number(c.green()));
    sheetStr.replace("horizontal_sub-line_3", QString::number(c.blue()));
    sheetStr.replace("horizontal_sub-line_4", QString::number(140));

    sheetStr.replace("horizontal_add-line_1", QString::number(c.red()));
    sheetStr.replace("horizontal_add-line_2", QString::number(c.green()));
    sheetStr.replace("horizontal_add-line_3", QString::number(c.blue()));
    sheetStr.replace("horizontal_add-line_4", QString::number(140));

    sheetStr.replace("vertical_handle_1", QString::number(c.red()));
    sheetStr.replace("vertical_handle_2", QString::number(c.green()));
    sheetStr.replace("vertical_handle_3", QString::number(c.blue()));
    sheetStr.replace("vertical_handle_4", QString::number(60));

    sheetStr.replace("vertical_handle_hover_1", QString::number(c.red()));
    sheetStr.replace("vertical_handle_hover_2", QString::number(c.green()));
    sheetStr.replace("vertical_handle_hover_3", QString::number(c.blue()));
    sheetStr.replace("vertical_handle_hover_4", QString::number(100));

    sheetStr.replace("vertical_sub-line_1", QString::number(c.red()));
    sheetStr.replace("vertical_sub-line_2", QString::number(c.green()));
    sheetStr.replace("vertical_sub-line_3", QString::number(c.blue()));
    sheetStr.replace("vertical_sub-line_4", QString::number(140));

    sheetStr.replace("vertical_add-line_1", QString::number(c.red()));
    sheetStr.replace("vertical_add-line_2", QString::number(c.green()));
    sheetStr.replace("vertical_add-line_3", QString::number(c.blue()));
    sheetStr.replace("vertical_add-line_4", QString::number(140));

    sheetStr.replace("QTableView_background_color_red",     QString::number(c.red()));
    sheetStr.replace("QTableView_background_color_green",   QString::number(c.green()));
    sheetStr.replace("QTableView_background_color_blue",    QString::number(c.blue()));
    sheetStr.replace("QTableView_background_color_alpha",   QString::number(aeroTransparent));

    qApp->setStyleSheet(sheetStr);

    foreach (WidgetColorAdapter* w, g_colorAdapter) {
        w->applyNewAdapter();
    }
}

void regColorAdapterWidget(WidgetColorAdapter *w)
{
    g_colorAdapter.append(w);
}

void removeColorAdapterWidget(WidgetColorAdapter *w)
{
    g_colorAdapter.removeAll(w);
}

QColor adapterColor()
{
    QColor color = MOption::instance()->option(OPTION_AVERAGE_COLOR, OPTION_GROUP_Theme).value<QColor>();

    int averageValue = (color.red()+color.green()+color.blue())/3;

    QColor ret;
    if(averageValue <= 100)
    {
        ret = QColor(255, 255, 255);
    } else {
        ret = QColor(10, 10, 10);
    }

    return ret;
}
