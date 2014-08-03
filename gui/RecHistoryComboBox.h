#ifndef RecHistoryComboBox_H
#define RecHistoryComboBox_H

#include <QComboBox>
#include "GUILib_global.h"
class GUILIBSHARED_EXPORT RecHistoryComboBox : public QComboBox
{
    Q_OBJECT
public:
    RecHistoryComboBox(QWidget *parent = 0);

    void leaveEvent(QEvent *);
    void focusOutEvent(QFocusEvent *e);
    void focusInEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void setFilePath(QString path);
signals:
    void stringChanged(QString);
private:
    void updateHistoryText();
    QString filePath;
private slots:
    void currentIndexChanged(int index);
protected:
    bool eventFilter(QObject *o, QEvent *e);
    void wheelEvent ( QWheelEvent * e );

};

#endif // RecHistoryComboBox_H
