#include "mychannelnumdlg.h"

MyChannelNumDlg::MyChannelNumDlg(QWidget *parent) :
    QDialog(parent)
{
    //初始化
    UIinit();
}

void MyChannelNumDlg::UIinit()
{

    this->setWindowFlag(Qt::FramelessWindowHint);

    //设置大小
    this->setFixedSize(420,120);

    this->setStyleSheet("MyChannelNumDlg{ background-color: #2A2A2A;}");

    //设置一个登录label
    QLabel *logonlabel = new QLabel(this);
    logonlabel->setFixedSize(420,120);
    logonlabel->move(0,0);
    logonlabel->setStyleSheet("QLabel{border: 1px solid #808080;background-color:#2A2A2A;}");

    channelnumlab    = new QLabel(logonlabel);
    channelnumlab->move(66,33);
    channelnumlab->setText("Set Channel Num:");
    channelnumlab->setStyleSheet("QLabel{color: #ffffff;font-size: 18px;border: 0px solid #808080;font-weight: 400; font-family: \"Microsoft YaHei\";}");

    channelnumBox    = new QComboBox(logonlabel);
    channelnumBox->move(235,27);

    channelnumBox->setFixedSize(120,32);

    channelnumBox->setMaxVisibleItems(33); //先设置33个
    QString stmpnum="";
    for(int i=4;i<34;i=i+2)
    {

        stmpnum=QString("%1").arg(i);
        channelnumBox->addItem(stmpnum,(i-4)/2);
    }

    channelnumBox->setStyleSheet(
                  "QComboBox {background: #2a2a2a;border: 1px solid #808080; color: #FFFFFF;font:16px \"Microsoft YaHei UI\";font-weight:400;}"
                       "QComboBox QAbstractItemView{ background-color:#252e39;color: white; }"
                       "QComboBox QAbstractItemView::item:hover{color: #27E29E;background-color: #27E29E4D;}"
                       "QComboBox QAbstractItemView::item:selected{color: #27E29E;background-color: #27E29E4D;} "
                              );


    //加载combox的新样式
     QStyledItemDelegate *channledelegate = new QStyledItemDelegate();
     channelnumBox ->setItemDelegate(channledelegate);

     //OK 按钮
     okBtn = new QPushButton(logonlabel);
     okBtn->setText("OK");

     okBtn->move(100,78);
     okBtn->setStyleSheet("QPushButton{color: #27e29e;width: 96px;height: 28px;border: 1px solid #98989a;font-size: 14px;font-weight: 400;background: rgba(15,80,64,1.0);border-radius: 4px 4px 4px 4px;font: 18px \"Microsoft YaHei UI\";}"
                                     "QPushButton:hover{background: rgba(15,80,64,0.6);}"
                                     "QPushButton:pressed{background: rgba(15,80,64,0.2);}"
                                     );

     connect(okBtn, SIGNAL(clicked()), this, SLOT(Okclicked()));

     //取消按钮
     cancelBtn = new QPushButton(logonlabel);
     cancelBtn->setText("Cancel");
     cancelBtn->move(218,78);
     cancelBtn->setStyleSheet("QPushButton{color: #27e29e;width: 96px;height: 28px;border: 1px solid #98989a;font-size: 14px;font-weight: 400;background: rgba(89,89,89,1.0);border-radius: 4px 4px 4px 4px;font: 18px \"Microsoft YaHei UI\";}"
                                     "QPushButton:hover{background: rgba(89,89,89,0.6);}"
                                     "QPushButton:pressed{background: rgba(89,89,89,0.2);}"
                                     );

     connect(cancelBtn, SIGNAL(clicked()), this, SLOT(Cancelclicked()));

}

void MyChannelNumDlg::Okclicked()
{
    //取得值，发送给主窗口

   emit sendData(channelnumBox->currentText());

   this->close();

}

void MyChannelNumDlg::Cancelclicked()
{
   this->close();
}


//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void MyChannelNumDlg::mousePressEvent(QMouseEvent *event)
{
    if( (event->button() == Qt::LeftButton) )
    {
        mouse_press = true;
        mousePoint = event->globalPos() - this->pos();

    // event->accept();
    }

}
void MyChannelNumDlg::mouseMoveEvent(QMouseEvent *event)
{
    // if(event->buttons() == Qt::LeftButton){ //如果这里写这行代码，拖动会有点问题
    if(mouse_press && (mousePoint!=QPoint()))
    {
    move(event->globalPos() - mousePoint);
    // event->accept();
    }
}
void MyChannelNumDlg::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    mousePoint = QPoint();

    (void) *event;
}
