#ifndef MYQLABEL_H
#define MYQLABEL_H

#include "mainwindow.h"
#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QDebug>

class MyQLabel : public QLabel{
    Q_OBJECT
public:
    MyQLabel(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent* event);
    void setNewPainter(std::vector<std::vector<int>> rect);
    void clearAreaList(){this->areaList.clear();};

private:
    QList<QRect> areaList;
};


#endif // MYQLABEL_H
