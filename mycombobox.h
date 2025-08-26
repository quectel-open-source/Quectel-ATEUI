#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QObject>
#include<QWidget>
#include<QComboBox>
#include<QStylePainter>
#include<QStandardItemModel>
#include<QAbstractItemView>

class MyComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit MyComboBox(QWidget *parent = nullptr);

    void setTextAlignment(Qt::Alignment alignment) const;

protected:
     void paintEvent(QPaintEvent *event);

signals:

};

#endif // MYCOMBOBOX_H
