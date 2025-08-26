#include "mymessagebox.h"

MyMessageBox::MyMessageBox(QDialog *parent,QString strmsg,QString strtext) : QDialog(parent)
{


    m_strmsg = strmsg;
    m_strtext = strtext;

   //
   //定义无边框
   this->setWindowFlag(Qt::FramelessWindowHint);
   this->setAutoFillBackground(true);
   //设置大小
    //判断text长度，如果大于40，则继续增加
    if(strtext.length()>40)
    {
        this->setFixedSize(650,64);
    }
    else
    {
        this->setFixedSize(450,64);
    }

    //
    //定义无边框

   logolab    = new QLabel(this);
   titlelab   = new QLabel(this);
   contentlab = new QLabel(this);
   closebtn   = new QPushButton(this);

   logolab->move(18,24);
   logolab->setMinimumWidth(18);
   logolab->setMinimumHeight(18);

   titlelab->move(50,5);

   titlelab->setAlignment(Qt::AlignLeft);

   contentlab->move(50,31);
   contentlab->setAlignment(Qt::AlignLeft);

   closebtn->move(422,26);

   InitUI();

   connect(closebtn, SIGNAL(clicked()), this, SLOT(closeWidget()));
}


MyMessageBox::~MyMessageBox()
{
    //close时候调用一下

}

void MyMessageBox::InitUI()
{

    qInfo()<<"m_strtext.length()=="<<m_strtext.length();
    qInfo()<<"m_strtext"<<m_strtext;

    if(m_strtext.length()>100)
    {
        this->setMinimumWidth(1200);
        this->setMaximumWidth(1200);
        closebtn->move(1172,26);
    }
    else if(m_strtext.length()>90)
    {
        this->setMinimumWidth(1100);
        this->setMaximumWidth(1100);
        closebtn->move(1072,26);
    }
    else if(m_strtext.length()>80)
    {
        this->setMinimumWidth(1000);
        this->setMaximumWidth(1000);
        closebtn->move(972,26);
    }
    else if(m_strtext.length()>70)
    {
        this->setMinimumWidth(900);
        this->setMaximumWidth(900);
        closebtn->move(872,26);
    }
    else if(m_strtext.length()>60)
    {
        this->setMinimumWidth(800);
        this->setMaximumWidth(800);
        closebtn->move(772,26);
    }
    else if(m_strtext.length()>50)
    {
        this->setMinimumWidth(700);
        this->setMaximumWidth(700);
        closebtn->move(672,26);
    }
    else if(m_strtext.length()>40)
    {
        this->setMinimumWidth(600);
        this->setMaximumWidth(600);
        closebtn->move(572,26);
    }
    else if(m_strtext.length()>30)
    {
        this->setMinimumWidth(500);
        this->setMaximumWidth(500);
        closebtn->move(472,26);
    }
    else if(m_strtext.length()>20)
    {
        this->setMinimumWidth(400);
        this->setMaximumWidth(400);
        closebtn->move(372,26);
    }
    else if(m_strtext.length()>10)
    {
        this->setMinimumWidth(300);
        this->setMaximumWidth(300);
        closebtn->move(272,26);
    }
    else
    {
        this->setMinimumWidth(250);
        this->setMaximumWidth(250);
        closebtn->move(222,26);
    }

    //根据不同的类型显示不同的界面
    if(m_strmsg=="Error")
    {

        this->setStyleSheet("QDialog { background-color: rgba(255,161,161,1);}");


        logolab->setStyleSheet("QLabel{border-image: url(./res/messagebox/Error-icon.png);}");

        titlelab->setText("Error");
        titlelab->setStyleSheet("QLabel{color: #ec213c;font-size: 22px;font-weight: 700; font-family: \"Microsoft YaHei\";}");

        contentlab->setStyleSheet("QLabel{color: #ec213c;font-size: 16px;min-width: 200px;font-weight: 400; font-family: \"Microsoft YaHei\";}");

        closebtn->setStyleSheet("QPushButton{border-image: url(./res/messagebox/Error_close_icon.png);width: 12px;height: 12px;}"
                                //"QPushButton:hover{border-image: url(./res/icon_minimize_Hover.png);}"
                                //"QPushButton:pressed{border-image: url(./res/icon_minimize_Click.png);}"
                                );

    }
    else if(m_strmsg=="Success")
    {
        this->setStyleSheet("QDialog { background-color: #006B59;}");

        logolab->setStyleSheet("QLabel{border-image: url(./res/messagebox/Success_icon.png);}");

        titlelab->setText("Success");
        titlelab->setStyleSheet("QLabel{ color: #00ffac;font-size: 22px;font-weight: 700; font-family: \"Microsoft YaHei\";}");

        contentlab->setStyleSheet("QLabel{ color: #00ffac;font-size: 14px;font-weight: 400; font-family: \"Microsoft YaHei\";}");

        closebtn->setStyleSheet("QPushButton{border-image: url(./res/messagebox/Success_close_icon.png);width: 12px;height: 12px;}"
                                //"QPushButton:hover{border-image: url(./res/icon_minimize_Hover.png);}"
                                //"QPushButton:pressed{border-image: url(./res/icon_minimize_Click.png);}"
                                );
    }
    else //info
    {
        this->setStyleSheet("QDialog { background-color: #A4C8FF;}");

        logolab->setStyleSheet("QLabel{border-image: url(./res/messagebox/info_icon.png);}");

        titlelab->setText(m_strmsg);
        titlelab->setStyleSheet("QLabel{ color: #116EFD;font-size: 22px;font-weight: 700; font-family: \"Microsoft YaHei\";}");

        contentlab->setStyleSheet("QLabel{ color: #116EFD;font-size: 14px;font-weight: 400; font-family: \"Microsoft YaHei\";}");

        closebtn->setStyleSheet("QPushButton{border-image: url(./res/messagebox/Info_close_icon.png);width: 12px;height: 12px;}"
                                //"QPushButton:hover{border-image: url(./res/icon_minimize_Hover.png);}"
                                //"QPushButton:pressed{border-image: url(./res/icon_minimize_Click.png);}"
                                );
    }

    contentlab->setText(m_strtext);
    contentlab->adjustSize();
}

void MyMessageBox::SetShowResult(QString strmsg,QString strtext)
{
    //
    m_strmsg = strmsg;
    m_strtext = strtext;
    InitUI();

}

void MyMessageBox::closeWidget()
{
    close();
}
//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void MyMessageBox::mousePressEvent(QMouseEvent *event)
{
    if( (event->button() == Qt::LeftButton) )
    {
        mouse_press = true;
        mousePoint = event->globalPos() - this->pos();

    // event->accept();
    }

}
void MyMessageBox::mouseMoveEvent(QMouseEvent *event)
{
    // if(event->buttons() == Qt::LeftButton){ //如果这里写这行代码，拖动会有点问题
    if(mouse_press && (mousePoint!=QPoint()))
    {
    move(event->globalPos() - mousePoint);
    // event->accept();
    }
}
void MyMessageBox::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    mousePoint = QPoint();
}


