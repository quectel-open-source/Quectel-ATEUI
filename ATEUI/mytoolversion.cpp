#include "mytoolversion.h"

MyToolVersion::MyToolVersion(QWidget *parent,QString testscript_version,QString dll_version) :
    QDialog(parent)
{
    //
    m_testscript_version   = testscript_version;
    m_function_dll_version = dll_version;
    UIinit();
}

void MyToolVersion::UIinit()
{
    //
    this->setWindowFlag(Qt::FramelessWindowHint);

    //设置大小
    this->setFixedSize(500,400);

    //
    this->setStyleSheet("MyToolVersion{ background-color: #2A2A2A;border: 1px solid #808080;border-radius: 4px;box-shadow: 0 4px 10px 0 #0000004d;}");

    m_ok_btn = new QPushButton(this);
    m_ok_btn->setText("OK");
    m_ok_btn->move(200,340);
    m_ok_btn->setStyleSheet("QPushButton{width: 95;height: 30px;font:20px \"Microsoft YaHei UI\";border-radius: 4px;border: 1px solid #27e29e;background: #0f5040; color: #27e29e;}"
                            "QPushButton:hover{background: rgba(39,226,158,0.4);}"
                            "QPushButton:pressed{background: rgba(39,226,158,0.1);}");

    connect(m_ok_btn, SIGNAL(clicked()), this, SLOT(OKBtnClicked()));

    //qlabel about
    QLabel *aboutlabel = new QLabel(this);
    aboutlabel->move(16,14);
    aboutlabel->setText("About");
    aboutlabel->setStyleSheet("font:18px \"Microsoft YaHei UI\";;font-weight:400; color: #ffffff;");

    //Qline
    QLineEdit *oneline = new QLineEdit(this);
    oneline->move(0,48);
    oneline->setStyleSheet("QLineEdit{width: 500px;height: 1.5px;background: #808080;border: 1px solid #808080;}");

    //Textedit
    QTextEdit *tpversionEdit = new QTextEdit(this);

    tpversionEdit->setGeometry(50, 60, 400, 260);
//            m_TestTextLogEditlist.at(i)->setText("2023-11-8_9:24:3urrent:3.000000\n"
//                                                          "2023-11-8_9:24:33\n <--> MANUAL OP");
    tpversionEdit->setStyleSheet("QTextEdit{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;background: #252e39;border-radius: 4px 4px 4px 4px;}"
                                               //"QTextEdit QScrollBar:vertical{width: 6px;background:red;border: none;}"
                                               );

    tpversionEdit->verticalScrollBar()->setStyleSheet(
                                             "QScrollBar:vertical{background:#0E1016;width:6px;padding-top:0px;padding-bottom:0px;padding-left:0px;padding-right:0px;}"    //左/右预留位置
                                             "QScrollBar::handle:vertical{background:#909090;border-radius:2px;min-height:32px;}"    //滑块最小高度
                                             "QScrollBar::handle:vertical:hover{background:#383838;}"//鼠标触及滑块样式/滑块颜色
                                             "QScrollBar::add-line:vertical{background:url(./res/down_arrow.png) center no-repeat;}"//向下箭头样式
                                             "QScrollBar::sub-line:vertical{background:url(./res/up_arrow.png) center no-repeat;}"//向上箭头样式
                                             "QScrollBar::add-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在垂直区域
                                             "QScrollBar::sub-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在区域
                                            );

    tpversionEdit->setEnabled(false);

    tpversionEdit->append("Script Version Information:");
    tpversionEdit->append(m_testscript_version);

    tpversionEdit->append(" ");
    tpversionEdit->append("DLL Version Information:");

    //根据分号区别不同软件的版本信息
    QStringList stlisttestflow;
    stlisttestflow=m_function_dll_version.split(";");


    // 清空QComboBox中的所有项目
//    m_TestModecombox->clear();
    for(int i=0;i<stlisttestflow.count();i++)
    {
        tpversionEdit->append(stlisttestflow.at(i));
    }

    tpversionEdit->append(" ");
    tpversionEdit->append("QATE Version Information:");

    tpversionEdit->append(QApplication::applicationVersion());



//    // QGroupBox
//    QGroupBox *scriptvergroup = new QGroupBox(this);
//    scriptvergroup->move(50,60);
//    scriptvergroup->setFixedSize(400,40);
//    //scriptvergroup->setStyleSheet("QGroupBox{width: 400px;height: 30px;}");
//    //脚本版本Script version
//    QLabel *scriptverlab = new QLabel(scriptvergroup);
//    scriptverlab->move(50,5);
//    scriptverlab->setText("脚本版本信息:");
//    scriptverlab->setStyleSheet("font:18px \"Microsoft YaHei UI\";;font-weight:400; color: #ffffff;");

//    QLabel *scshowlab = new QLabel(scriptvergroup);
//    scshowlab->move(180,5);
//    scshowlab->setText(m_testscript_version+"CommonScriptV1.1.1");
//    scshowlab->setStyleSheet("font:18px \"Microsoft YaHei UI\";;font-weight:400; color: #ffffff;");

//    // QGroupBox
//    QGroupBox *FunDllgroup2 = new QGroupBox(this);
//    FunDllgroup2->move(50,120);
//    FunDllgroup2->setFixedSize(400,180);
//    //
//    QLabel *FunDlllab = new QLabel(FunDllgroup2);
//    FunDlllab->move(50,5);
//    FunDlllab->setText("DLL 版本信息:");
//    FunDlllab->setStyleSheet("font:18px \"Microsoft YaHei UI\";;font-weight:400; color: #ffffff;");

//    QLabel *dllshowlab = new QLabel(FunDllgroup2);
//    dllshowlab->move(70,5);
//    dllshowlab->setText(m_function_dll_version);
//    dllshowlab->setStyleSheet("font:18px \"Microsoft YaHei UI\";;font-weight:400; color: #ffffff;");

    //功能DLL版本信息

}

void MyToolVersion::OKBtnClicked()
{
     close();
}


//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void MyToolVersion::mousePressEvent(QMouseEvent *event)
{
    if( (event->button() == Qt::LeftButton) )
    {
        mouse_press = true;
        mousePoint = event->globalPos() - this->pos();

    // event->accept();
    }

}
void MyToolVersion::mouseMoveEvent(QMouseEvent *event)
{
    // if(event->buttons() == Qt::LeftButton){ //如果这里写这行代码，拖动会有点问题
    if(mouse_press && (mousePoint!=QPoint()))
    {
    move(event->globalPos() - mousePoint);
    // event->accept();
    }
}
void MyToolVersion::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    mousePoint = QPoint();
}

