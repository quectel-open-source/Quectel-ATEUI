#ifndef TRIANGLEPUSHBUTTON_H
#define TRIANGLEPUSHBUTTON_H

#include <QPainter>
#include <QWidget>
#include <QPushButton>
#include <QRectF>
#include<math.h>
#include<QPen>

class TrianglePushButton : public QPushButton
{
public:
    TrianglePushButton(QWidget *parent = nullptr) : QPushButton(parent)
    {
        m_qscolor = "#252E39";
        m_qsolidscolor="#5F6268";

        setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    }

    QString m_qscolor;
    QString m_qsolidscolor;


    void ChangeBackGround(QString qsbackcolor,QString qsolidcolor)
    {
        //
        m_qscolor = qsbackcolor;
        m_qsolidscolor = qsolidcolor;
        update();
    }
protected:
    void paintEvent(QPaintEvent *event) //override
    {

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QPolygonF polygon;

        polygon << QPointF(88, 15);
        polygon << QPointF(80, 30);
        polygon << QPointF(0, 30);
        polygon << QPointF(8, 15);
        polygon << QPointF(0, 0);
        polygon << QPointF(80, 0);

        QPen pen; // 创建一个黑色描边
        pen.setBrush(QColor(m_qsolidscolor));
        pen.setWidth(1);
        painter.setPen(pen);
        painter.setBrush(QColor(m_qscolor));
        painter.drawPolygon(polygon);

        //
        (void)*event;
    }    

};

#endif // TRIANGLEPUSHBUTTON_H
