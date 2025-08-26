#include "myinputmessbox.h"

MyInputMessBox::MyInputMessBox(QDialog *parent,QString strmsg,QString strtext):MyQueryMessageBox(parent,strmsg,strtext)
{

    //
    m_inputLine_edit = new QLineEdit(this);
    m_inputLine_edit->setStyleSheet("QLineEdit{width: 418px;height: 32px;border-radius: 4px;color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: #252e39;border: 1px solid #98989a;}");


    m_StationLine = new QLineEdit(this);
    m_StationLine->setStyleSheet("QLineEdit{width: 450px;height: 1px;background: #808080;}");

    InitUI();


    //信号与槽
    connect(m_inputLine_edit, SIGNAL(returnPressed()), this, SLOT(on_inputLineedit_returnPressed()));
}

void MyInputMessBox::InitUI()
{
    //设置大小
    this->setFixedSize(450,174);

    //
    m_inputLine_edit->move(16,74);

    m_StationLine->move(0,52);

    logolab->setHidden(true);

    titlelab->setHidden(true);

    contentlab->move(16,14);

    okbtn->move(114,126);
    cancelbtn->move(240,126);

    closebtn->move(420,18);

}

void MyInputMessBox::showInputMessBox(QString strmsg,QString strtext)
{
    //
    m_inputLine_edit->setText("");
    this->setStyleSheet("QDialog {border-radius: 3px;border: 1px solid #808080;background: #2a2a2a;box-shadow: 0 4px 10px 0 #0000004d;}");

    //logolab->setStyleSheet("QLabel{border-image: url(./res/messagebox/info_icon.png);}");

    //titlelab->setText(strmsg);
    //titlelab->setStyleSheet("QLabel{ color: #116EFD;font-size: 28px;font-weight: 700; font-family: \"Microsoft YaHei\";}");


    contentlab->setStyleSheet("QLabel{ color: #ffffff;font-size: 14px;font-weight: 400; font-family: \"Microsoft YaHei\";line-height: 24px;background: #2a2a2a;}");

    closebtn->setStyleSheet("QPushButton{border-image: url(./res/messagebox/Success_close_icon.png);width: 12px;height: 12px;}"
                            );

    contentlab->setText(strtext);
    contentlab->adjustSize();
}


void MyInputMessBox::on_inputLineedit_returnPressed()
{
    //引用OK

    okbtn->click();
}
