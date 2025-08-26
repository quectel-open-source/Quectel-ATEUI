#include "myquerymessagebox.h"

MyQueryMessageBox::MyQueryMessageBox(QDialog *parent,QString strmsg,QString strtext):MyMessageBox(parent,strmsg,strtext)
{
    //


    okbtn      = new QPushButton(this);
    cancelbtn  = new QPushButton(this);

    okbtn->setText("OK");
    okbtn->setStyleSheet("QPushButton{width: 95;height: 30px;font:16px \"Microsoft YaHei UI\";border-radius: 4px;border: 1px solid #005ae5;background: #116efd; color: #ffffff;}"
                         "QPushButton:hover{background: rgba(17,110,253,0.5);}"
                         "QPushButton:pressed{background: rgba(17,110,253,0.3);}"
                         );

    cancelbtn->setText("Cancel");
    cancelbtn->setStyleSheet("QPushButton{width: 95;height: 30px;font:16px \"Microsoft YaHei UI\";border-radius: 4px;border: 1px solid #ffffff;background: transparent; color: #ffffff;}"
                         );

    //
    connect(okbtn, SIGNAL(clicked()), this, SLOT(OKWidget()));
    connect(cancelbtn, SIGNAL(clicked()), this, SLOT(CancelWidget()));


    InitUI();

}

void MyQueryMessageBox::OKWidget()
{
    //发送信号
    emit mySignal(true);

    close();
}

void MyQueryMessageBox::CancelWidget()
{
    close();
}

void MyQueryMessageBox::InitUI()
{

    //设置大小
    this->setFixedSize(450,120);

    //
    logolab->move(30,20);

    //
    titlelab->move(50,10);

    //
    closebtn->move(422,16);

    contentlab->move(50,44);

    okbtn->move(114,67);

    cancelbtn->move(240,67);


}

void MyQueryMessageBox::SetShowQueryResult(QString strmsg,QString strtext)
{
    //
    SetShowResult(strmsg,strtext);

    //调整 Button的位置,先取得确认框的长度
    //this->width();

    qInfo()<<"this->width()=="<<this->width();
    okbtn->move( (this->width()/2)-110,75);

    cancelbtn->move( (this->width()/2)+20,75);


}

