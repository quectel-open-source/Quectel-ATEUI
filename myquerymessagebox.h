#ifndef MYQUERYMESSAGEBOX_H
#define MYQUERYMESSAGEBOX_H

#include <QObject>
#include "mymessagebox.h"
#include <QPushButton>
#include <QDebug>

class MyQueryMessageBox: public MyMessageBox
{

    Q_OBJECT

public:
    explicit MyQueryMessageBox(QDialog *parent = nullptr,QString strmsg="",QString strtext="");

    void InitUI();

    void SetShowQueryResult(QString strmsg,QString strtext);

    QPushButton *okbtn;
    QPushButton *cancelbtn;

signals:
    void mySignal(bool value);

private slots:
    void OKWidget();
    void CancelWidget();
};

#endif // MYQUERYMESSAGEBOX_H
