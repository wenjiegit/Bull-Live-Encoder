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

#ifndef BLESCENEWIDGET_HPP
#define BLESCENEWIDGET_HPP

#include <QWidget>
#include <QHash>
#include <QTimer>

namespace Ui {
class BleSceneWidget;
}

class BleImageProcess;
class BleImageProcessThread;

class BleSceneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BleSceneWidget(QWidget *parent = 0);
    ~BleSceneWidget();

    static BleSceneWidget *instance();

    void addWidget(QWidget *widget);
    void addScene();

    BleImageProcess *currentImageProcessWidget();
    BleImageProcessThread *currentImageProcessThread();

private slots:
    void onSceneChanged(int index);
    void onTimeout();

private:
    struct ScenePair
    {
        BleImageProcess *imageProcessWidget;
        BleImageProcessThread *imageProcessThread;
    };

private:
    Ui::BleSceneWidget *ui;
    QHash<int, ScenePair> m_scenesPair;
    QTimer m_timer;

signals:
    void sizeChanged();
};

#endif // BLESCENEWIDGET_HPP
