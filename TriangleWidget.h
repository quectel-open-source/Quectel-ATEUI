#ifndef TRIANGLEWIDGET_H
#define TRIANGLEWIDGET_H

#include <QPainter>
#include <QWidget>

class TriangleWidget : public QWidget
{
public:
    TriangleWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        m_qscolor = "#909090";

        setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    }

    QString m_qscolor;

    void ChangeBackGround(QString qsbackcolor)
    {
        //
        m_qscolor = qsbackcolor;

        update();
    }
protected:
    void paintEvent(QPaintEvent *event) override
    {

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QPolygonF polygon;
        polygon << QPointF(0, 0);
        polygon << QPointF(0, 0);
        polygon << QPointF(0, 40);
        polygon << QPointF(40, 0);
        //painter.setBrush(QColor("#909090"));
        painter.setBrush(QColor(m_qscolor));
        painter.drawPolygon(polygon);

        //
        (void)*event;
    }    

};

#endif // TRIANGLEWIDGET_H
