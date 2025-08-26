#ifndef MYINPUTMESSBOX_H
#define MYINPUTMESSBOX_H

#include "myquerymessagebox.h"
#include <qlineedit.h>

class MyInputMessBox: public MyQueryMessageBox
{
    Q_OBJECT

public:
    explicit MyInputMessBox(QDialog *parent = nullptr,QString strmsg="",QString strtext="");

    QLineEdit *m_inputLine_edit;
    QLineEdit  *m_StationLine ;

    void InitUI();

    void showInputMessBox(QString strmsg,QString strtext);

private slots:
    void on_inputLineedit_returnPressed(); //输入框响应函数;
};

#endif // MYINPUTMESSBOX_H
