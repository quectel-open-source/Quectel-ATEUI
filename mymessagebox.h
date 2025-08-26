#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QMouseEvent>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QDebug>



class MyMessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit MyMessageBox(QDialog *parent = nullptr,QString strmsg="",QString strtext="");
    ~MyMessageBox();

    void InitUI();

    void SetShowResult(QString strmsg,QString strtext);

    QPoint   mousePoint;
    bool     mouse_press;
    QString  m_strmsg;
    QString  m_strtext;

    QLabel *logolab;
    QLabel *titlelab;
    QLabel *contentlab;
    QPushButton *closebtn;


signals:

private slots:
    void closeWidget();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

};

#endif // MYMESSAGEBOX_H
