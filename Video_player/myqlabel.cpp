#include "myqlabel.h"
#include <QMouseEvent>
#include <QPainter>

MyQLabel::MyQLabel(QWidget *parent) : QLabel(parent)
{

}

void MyQLabel::setNewPainter(std::vector<std::vector<int>> rect)
{
    if(rect.size()>0){
        for(int i = 0; i < rect.size(); i++){
            QPoint leftTopPoint=QPoint(rect[i][0], rect[i][1]);
            QPoint rightBottomPoint=QPoint(rect[i][2], rect[i][3]);
            areaList.append(QRect(leftTopPoint,rightBottomPoint));
        }
    }
    update();
}

void MyQLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QPainter painter(this);
    if(this->areaList.size()>0){
        for(int i = 0; i != this->areaList.size(); i++)
        {
            painter.setPen(QPen(Qt::white,1));
            painter.drawRect(areaList.at(i).x(), areaList.at(i).y(), areaList.at(i).width(), areaList.at(i).height());
        }
    }
}

