#include "tradgoldlogondiaLog.h"


TESTER_MESSBOX_S        tradtmplogonMessbox;

//显示弹框
bool __stdcall TradLogonGetCallExternalAPI(const wchar_t* pcwcAPIName, const wchar_t* pcwcAgrs, wchar_t* pwcResValueList, int iValueBufferSize, void* pobj)
{
    //这里可以直接显示弹框效果
    TradGoldLogonDiaLog *tmpTradGoldLogonDiaLog = static_cast<TradGoldLogonDiaLog *>(pobj);


    if(nullptr==pcwcAPIName || nullptr==pcwcAgrs || nullptr ==pwcResValueList)
    {
        qDebug()<<"pcwcAPIName is NULL";
        return false;
    }

    tradtmplogonMessbox.pcwcAPIName=pcwcAPIName;
    tradtmplogonMessbox.pcwcAgrs = pcwcAgrs;
    tradtmplogonMessbox.iValueBufferSize = iValueBufferSize;

    ::SendMessage((HWND)tmpTradGoldLogonDiaLog->m_hWnd_View, WM_LOGON_MESSAGR_TS_MSG, (WPARAM)&tradtmplogonMessbox, (LPARAM)0);

    wcscpy_s(pwcResValueList,iValueBufferSize,tmpTradGoldLogonDiaLog->m_cla_logon_gfc.QString2Wchar(tmpTradGoldLogonDiaLog->m_Query_Messbox_Result));


}

//显示进度节点
void __stdcall TradGetLogonResultProcess(double msg, void* pobj)
{

    TradGoldLogonDiaLog *tmpTradGoldLogonDiaLog = static_cast<TradGoldLogonDiaLog *>(pobj);


    //m_barvalue = (double*)wParam;


    tmpTradGoldLogonDiaLog->m_bar_current_value = (int)(msg);

    //richedit
   // ::SendMessage((HWND)tmpTradGoldLogonDiaLog->m_hWnd_View, WM_LOGON_PROCESS_MSG_TS_MSG, (WPARAM)&msg, (LPARAM)0);

}

void TradLOGON_OP_EXECUTE(LPVOID);

TradGoldLogonDiaLog::TradGoldLogonDiaLog(QWidget *parent) :
    QDialog(parent)
{

    LogFileInit();
    //初始化
    Q_ATE_CONTROLLER_InitFirst();

    Init();

    ReadCfg();

    //判断和设置状态
    CheckAndSetStatus();

    //初始化状态是false 并且不能选
    m_OfflineModecombox->setCurrentIndex(1);
    m_OfflineModecombox->setEnabled(false);

    connect(&m_quermessbox, SIGNAL(mySignal(bool)), this, SLOT(myQueryMessBoxSlot(bool)));

}


TradGoldLogonDiaLog::~TradGoldLogonDiaLog()
{
    //close时候调用一下

}

void TradGoldLogonDiaLog::Init()
{
    //定义无边框
   // this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setWindowFlag(Qt::FramelessWindowHint);

    //设置大小
    this->setFixedSize(800,600);

    //设置背景色
    this->setStyleSheet("TradGoldLogonDiaLog{border-image: url(./res/Logon/Logon_Background.png);}");

    //

    //
    InitLogon();


}

void TradGoldLogonDiaLog::InitExternalAPI()
{
    //弹出框
    Q_ATE_CONTROLLER_LoadCallExternalAPICallBack(0,TradLogonGetCallExternalAPI,this);

    Q_ATE_CONTROLLER_LoadResultProcessCallBack(0,TradGetLogonResultProcess,this);
    m_hWnd_View = this->winId();

}

void TradGoldLogonDiaLog::ReadCfg()
{
    wchar_t wcHar[256] = { 0 };
    pStationcfg = (Q_ATE_Controller_STATIONCONFIG_Interface*)Q_ATE_CONTROLLER_GetStationInfo();
    if (pStationcfg != NULL)
    {
        int iindex=0;
        //IP------------------------------------------------------------------
        memset(wcHar,0x00,sizeof(wcHar));
        pStationcfg->GetParaValue(L"FDB_SERVER_IP", wcHar, 256);

         QString str;
         str = m_cla_logon_gfc.Wchar2QString(wcHar);
         qDebug()<<"FDB_SERVER_IP ="<<str;
        //遍历IPcombox 查找是否有相同的，没有则增加到最后---------------------------
        m_IPLineEdit->setText(str);

         //----------------------------------------------------------------------

         //MO--------------------------------------------------------------------
         memset(wcHar,0x00,sizeof(wcHar));
         pStationcfg->GetParaValue(L"MO", wcHar, 256);
         str = m_cla_logon_gfc.Wchar2QString(wcHar);
         m_MOLineEdit->setText(str);

         //-----------------------------------------------------------------------



        //LOG_LEVEL----------------------------------------------------
         memset(wcHar,0x00,sizeof(wcHar));
         pStationcfg->GetParaValue(L"DEBUG_LOG_INFO_LEVEL", wcHar, 256);
         str = m_cla_logon_gfc.Wchar2QString(wcHar);
         qDebug()<<"LOG_LEVEL ="<<str;

         //离线还是在线模式
         memset(wcHar,0x00,sizeof(wcHar));
         pStationcfg->GetParaValue(L"OFFLINE_MODE", wcHar, 256);
         str = m_cla_logon_gfc.Wchar2QString(wcHar);
         qDebug()<<"ENABLE_FDB "<<str;

         for(iindex= 0; iindex < m_OfflineModecombox->count(); iindex++)
         {
             if(m_OfflineModecombox->itemText(iindex) == str)
             {
                //说明有，设置成当前的IP地址
                m_OfflineModecombox->setCurrentIndex(iindex);
                break;
             }
         }

         QIcon OfflineIcon ("./res/Logon/icon_Offline.png");
         if(iindex == m_OfflineModecombox->count())
         {
             m_OfflineModecombox->addItem(OfflineIcon,str,m_OfflineModecombox->count());

             m_OfflineModecombox->setCurrentIndex(m_OfflineModecombox->count()-1);
         }
    }
    else
    {
        qDebug()<<"Q_ATE_TESTSCRIPT_PARSER_STATIONCONFIG NULL";
        MyMessageBox message_box(this,"Error","Q_ATE_TESTSCRIPT_PARSER_STATIONCONFIG() is NULL");
        message_box.exec();
        return;
    }
}

void TradGoldLogonDiaLog::SetCfg()
{
    if (pStationcfg != NULL)
    {
        //修改配置
        const wchar_t* wstr =m_cla_logon_gfc.QString2Wchar(m_IPLineEdit->text().trimmed());
        qDebug()<<"Set IP"<<m_cla_logon_gfc.Wchar2QString(wstr);
        pStationcfg->SetParaValue(L"FDB_SERVER_IP", wstr);

        //MO
        wstr=m_cla_logon_gfc.QString2Wchar(m_MOLineEdit->text());
        qDebug()<<"Set MO"<<m_cla_logon_gfc.Wchar2QString(wstr);
        pStationcfg->SetParaValue(L"MO", wstr);


        //
        wstr=m_cla_logon_gfc.QString2Wchar(m_OfflineModecombox->currentText());
        qDebug()<<"Set OfflineMode"<<m_cla_logon_gfc.Wchar2QString(wstr);
        pStationcfg->SetParaValue(L"OFFLINE_MODE", wstr);

        //金板点检标志位
        pStationcfg->SetParaValue(L"STATION_WORKMODE", L"1");


        pStationcfg->Save();
    }
    else
    {
        qDebug()<<"Q_ATE_TESTSCRIPT_PARSER_STATIONCONFIG NULL";
        MyMessageBox message_box(this,"Error","Q_ATE_TESTSCRIPT_PARSER_STATIONCONFIG() is NULL");
        message_box.exec();
    }

}

void TradGoldLogonDiaLog::CheckAndSetStatus()
{
    //检查状态
    if(m_OfflineModecombox->currentText().trimmed()=="true")
    {
        m_IPLineEdit->setEnabled(false);
        m_IPTestbtn->setEnabled(false);
        m_MOLineEdit->setEnabled(false);
        m_Searchbtn->setEnabled(false);
    }
    else
    {
        m_IPLineEdit->setEnabled(true);
        m_IPTestbtn->setEnabled(true);
        m_MOLineEdit->setEnabled(true);
        m_Searchbtn->setEnabled(true);

    }

    //初始化状态是false 并且不能选

}

void TradGoldLogonDiaLog::InitLogon()
{
    //设置一个登录label
    QLabel *logonlabel = new QLabel(this);
    logonlabel->setFixedSize(512,520);
    logonlabel->move(258,40);
    logonlabel->setStyleSheet("QLabel{background-color:#090C13;}");

    //logon to continue
    QLabel *logoncontinuelabel = new QLabel(logonlabel);
    //logoncontinuelabel->setFixedSize(135,17);
    logoncontinuelabel->move(22,20);
    logoncontinuelabel->setText("Traditional Gold Plate Tools");
    //logoncontinuelabel->setStyleSheet("font-weight: bold;width: 135px;height: 17px;font: 16px "Microsoft YaHei UI";text-align: left;color: #FFFFFF 100%;");
    logoncontinuelabel->setStyleSheet("font:16px \"Microsoft YaHei UI\";font-weight:400;color: rgb(255,255,255);");

    //关闭按钮
    m_closebtn  = new QPushButton(logonlabel);
    m_closebtn->setFixedSize(24,24);
    m_closebtn->move(471,24);
    m_closebtn->setStyleSheet("QPushButton{border-image: url(./res/Logon/exit.png);}"
                              "QPushButton:hover{border-image: url(./res/Logon/exit_hover.png);}"
                              "QPushButton:pressed{border-image: url(./res/Logon/exit_click.png);}");

    connect(m_closebtn, SIGNAL(clicked()), this, SLOT(CloseLogon()));

    //一条线
    QLineEdit  *logonLine = new QLineEdit(logonlabel);
    logonLine->move(0,55);
    logonLine->setStyleSheet("QLineEdit{width: 512px;height: 1.5px;background: #27e29e;border: 1px solid #27e29e;}");

    //IP
    QLabel *IPlabel = new QLabel(logonlabel);
    IPlabel->move(25,76);
    IPlabel->setText("IP");
    IPlabel->setStyleSheet("width: 17px;height: 17px;font:18px \"Microsoft YaHei UI\";font-weight:400;color: rgb(255,255,255);");

    //IP选择框

    m_IPLineEdit = new QLineEdit(logonlabel);
    m_IPLineEdit->move(25,95);
    m_IPLineEdit->setStyleSheet("width: 367px;height: 32px;border-radius: 2.78px 2.78px 2.78px 2.78px;background: #252e39;font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF");

    QAction *IPaction = new QAction(this);
    IPaction->setIcon(QIcon("./res/Logon/icon_IP.png"));
    m_IPLineEdit->addAction(IPaction,QLineEdit::LeadingPosition);

    //Test
    m_IPTestbtn = new QPushButton(logonlabel);
    m_IPTestbtn->setText("Test");
    m_IPTestbtn->move(400,95);
    m_IPTestbtn->setStyleSheet("QPushButton{width: 87px;height: 32px;font-size: 16px;font-weight: 400;color: #27E29E;border-radius: 4px 4px 4px 4px;background: rgba(39,226,158,0.8);}"
                               "QPushButton:hover{background: rgba(39,226,158,0.4);}"
                               "QPushButton:pressed{background: rgba(39,226,158,0.1);}");

    connect(m_IPTestbtn, SIGNAL(clicked()), this, SLOT(IPTest()));

    //MO label
    QLabel *MOlabel = new QLabel(logonlabel);
    MOlabel->move(25,146);
    MOlabel->setText("OC");
    MOlabel->setStyleSheet("width: 15px;height: 15px;font:18px \"Microsoft YaHei UI\";font-weight:400 ;color: rgb(255,255,255);");

    //MO
    m_MOLineEdit = new QLineEdit(logonlabel);
    m_MOLineEdit->move(25,167);
    m_MOLineEdit->setStyleSheet("width: 367px;height: 32.56px;border-radius: 2.78px 2.78px 2.78px 2.78px;background: #252e39;font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF");

    QAction *action = new QAction(this);
    action->setIcon(QIcon("./res/Logon/icon_MO.png"));
    m_MOLineEdit->addAction(action,QLineEdit::LeadingPosition);

    //Search
    m_Searchbtn = new QPushButton(logonlabel);
    m_Searchbtn->setText("Search");
    m_Searchbtn->move(400,167);
    m_Searchbtn->setStyleSheet("QPushButton{width: 87px;height: 32px;font: 16px \"Microsoft YaHei UI\";font-weight: 400;color: #27E29E;border-radius: 4px 4px 4px 4px;background: rgba(39,226,158,0.8);}"
                               "QPushButton:hover{background: rgba(39,226,158,0.4);}"
                               "QPushButton:pressed{background: rgba(39,226,158,0.1);}");

    connect(m_Searchbtn, SIGNAL(clicked()), this, SLOT(SearchMO()));

    //加载combox的新样式

    //OFFLine Mode label
    QLabel *OfflineModelabel = new QLabel(logonlabel);
    OfflineModelabel->move(25,260);
    OfflineModelabel->setText("Offline Mode");
    OfflineModelabel->setStyleSheet("width: 15px;height: 15px;font:18px \"Microsoft YaHei UI\";;font-weight:400;color: rgb(255,255,255);");

    //OFFLine Mode
    m_OfflineModecombox  = new QComboBox(logonlabel);
    m_OfflineModecombox->move(140,255);
    m_OfflineModecombox->setFixedSize(112,32);

    QIcon OfflineIcon ("./res/Logon/icon_Offline.png");


    m_OfflineModecombox->addItem(OfflineIcon,"true",0);
    m_OfflineModecombox->addItem(OfflineIcon,"false",1);

    m_OfflineModecombox->setStyleSheet(
                              "QComboBox{background: #252e39;color: rgb(255,255,255);font:16px \"Microsoft YaHei UI\";font-weight:400;}"
                              //"QComboBox::down-arrow{width: 24px;height: 24px;border-image: url(./res/Logon/arrow.png);}"
                             // "QComboBox::drop-down{background: #252e39;}"//设置下拉框按钮背景及边框
                              //"QComboBox QAbstractItemView{ background-color: #252e39;color: white; }"
                              "QComboBox QAbstractItemView{ background-color:#252e39;color: white; }"
                              "QComboBox QAbstractItemView::item:hover{color: #27E29E;background-color: #27E29E4D;}"
                               "QComboBox QAbstractItemView::item:selected{color: #27E29E;background-color: #27E29E4D;} "
                              );

    connect(m_OfflineModecombox,SIGNAL(currentIndexChanged(int)),this,SLOT(onOffLineCurrentIndexChanged(int)));

    //加载combox的新样式
     QStyledItemDelegate *OfflineModdelegate = new QStyledItemDelegate();
     m_OfflineModecombox ->setItemDelegate(OfflineModdelegate);

    //
    m_Enterbtn = new QPushButton(logonlabel);
    m_Enterbtn->setText("Enter");
    m_Enterbtn->move(100,362);
    m_Enterbtn->setStyleSheet("QPushButton{width: 136px;height: 40px;font:20px \"Microsoft YaHei UI\";font-weight: 400;color: #27E29E;border-radius: 4px 4px 4px 4px;background: rgba(39,226,158,0.8);}"
                              "QPushButton:hover{background: rgba(39,226,158,0.4);}"
                              "QPushButton:pressed{background: rgba(39,226,158,0.1);}");
    connect(m_Enterbtn, SIGNAL(clicked()), this, SLOT(EnterLogon()));

    //reselect
    m_ReSelectbtn = new QPushButton(logonlabel);
    m_ReSelectbtn->setText("ReSelect");
    m_ReSelectbtn->move(256,362);
    m_ReSelectbtn->setStyleSheet("QPushButton{width: 136px;height: 40px;font:20px \"Microsoft YaHei UI\";font-weight: 400;color: #27E29E;border-radius: 4px 4px 4px 4px;background: rgba(39,226,158,0.8);}"
                                 "QPushButton:hover{background: rgba(39,226,158,0.2);}"
                                 "QPushButton:pressed{background: rgba(39,226,158,0.0);}");

   // connect(m_ReSelectbtn, SIGNAL(clicked()), this, SLOT(ReSelect()));

}

void TradGoldLogonDiaLog::CloseLogon()
{
//    MyQueryMessageBox qbx(this);
//    qbx.showMessBox("Info","OKKKKKKKKKKKKKKKKKKKK");
//    qbx.exec();

    SetCfg();
    exit(0);
}

void TradGoldLogonDiaLog::EnterLogon()
{

    m_bar_current_value=1;

    // 配置参数修改
    SetCfg();

//    qInfo()<<"EnterLogon() m_current_triangle_Index "<<m_current_triangle_Index;
//    if(-1==m_current_triangle_Index)
//    {
//        //弹框提示失败
//        MyMessageBox message_box(this,"Error","Not Select Staion Name Fail!!");
//        message_box.exec();
//        return;
//    }



    //启动一个线程函数进行初始化
    //创建线程
    m_hThread_Logon_INIT_Handle = CreateThread( NULL,
        0,
        (LPTHREAD_START_ROUTINE)TradLOGON_OP_EXECUTE,
        (void *)this,
        0,
        &m_dwThread_Logon_INIT_ID );



    QProgressDialog  dialog(this);

    //去除对话框内的Cancel按钮
    dialog.setCancelButtonText(nullptr);
    dialog.setWindowFlag(Qt::FramelessWindowHint);
    dialog.setWindowModality(Qt::WindowModal);  // 将对话框设置为模态
    dialog.setMinimumWidth(500);
    dialog.setMaximumHeight(50);
    dialog.setLabelText("初始化中...");
    dialog.setStyleSheet("QProgressDialog{background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);font: 14px \"Microsoft YaHei UI\";}");

    QLabel textlabel;

    textlabel.setText("初始化中.....");
    textlabel.setAlignment(Qt::AlignCenter);
    textlabel.setStyleSheet("QLabel{border-radius:5px;color: #FFFFFF;}");
    dialog.setLabel(&textlabel);


    QProgressBar initbar;
    initbar.setStyleSheet("QProgressBar{font-weight: 700;border-radius: 4px 4px 4px 4px;background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);text-align: center;color: #000000;font: 14px \"Microsoft YaHei UI\";}"
                                         "QProgressBar::chunk{background-color: #FFFFFF;width: 10px;margin: 0.5px;text-align: center;border-radius:5px;}"
                                         );

    dialog.setBar(&initbar);


    //pass 进度条变成100

    dialog.show();
    dialog.setValue(1);
    while(1)
    {
        _sleep(100);
        if(m_bar_current_value<=99)
        {
            dialog.setValue(m_bar_current_value);
        }

        QCoreApplication::processEvents();      // 避免界面冻结
        //qInfo()<<"m_bar_current_value="<<m_bar_current_value;

        if(true==m_Init_WorkResource_Flag)
        {
            dialog.setValue(100);
            break;
        }

        //qInfo()<<"m_bar_current_value="<<m_bar_current_value;
    }

    // 判断Init是否成功，如果成功，则关闭对话框，如果失败，则弹框提示

}

void TradGoldLogonDiaLog::ReSelect()
{
    QString qsTitle;
    QString qsContent;

    qsTitle = "title";
    qsContent ="content";

    wchar_t pwcResValueList[200];

    TradLogonGetCallExternalAPI(L"QueryMessageBox",L"--Title \"title\"  --Content \"content\"",pwcResValueList,200,this);
//    m_logon_messbox.SetShowResult(qsTitle,qsContent);
//    m_logon_messbox.exec();

//    m_quermessbox.SetShowResult(qsTitle,qsContent);
//    m_quermessbox.exec();
}


void TradGoldLogonDiaLog::SearchMO()
{

    //


    SetCfg();

    //查找MO工单是否存在
    wchar_t wcHar[1024] = { 0 };
    pMoCfg = (Q_ATE_Controller_MOInfo_Interface*)Q_ATE_CONTROLLER_GetMOInfo();

    if(pMoCfg==nullptr)
    {
        qDebug()<<"Q_ATE_Controller_MOInfo_Interface NULLPTR";
        MyMessageBox message_box(this,"Error","Q_ATE_CONTROLLER_GetMOInfo() is NULL");
        message_box.exec();
        return;
    }


    for(int i=0;i<pMoCfg->GetParaCout();i++)
    {
        //打印MO的信息
        memset(wcHar,0x00,sizeof(wcHar));

        QString strname;
        strname = m_cla_logon_gfc.Wchar2QString(pMoCfg->GetParaName(i));

        QString strvlaue;
        strvlaue = m_cla_logon_gfc.Wchar2QString(pMoCfg->GetParaValue(i));

        qInfo()<<strname<<"="<<strvlaue;

    }

    //软件版本号
    memset(wcHar,0x00,sizeof(wcHar));
    m_DownloadFW=m_cla_logon_gfc.Wchar2QString(pMoCfg->GetParaValueByName(L"DownloadFW"));
    qInfo()<<"m_DownloadFW="<<m_DownloadFW;

    //TestFlow
    memset(wcHar,0x00,sizeof(wcHar));
    QString stTestFlow;
    stTestFlow = m_cla_logon_gfc.Wchar2QString(pMoCfg->GetParaValueByName(L"TestFlowDes"));
    qInfo()<<"stTestFlow="<<stTestFlow;

    //
    QStringList stlisttestflow;
    stlisttestflow=stTestFlow.split(";");


    // 清空QComboBox中的所有项目

    SetCfg();


    memset(wcHar,0x00,sizeof(wcHar));
    Q_ATE_CONTROLLER_GetTestFlowStationInfo(wcHar,1024);

    ShowStation(wcHar);


}

void TradGoldLogonDiaLog::IPTest()
{
    //
    SetCfg();

    bool bret=false;
    //判断是否连通
    bret=Q_ATE_CONTROLLER_InitFDB();
    if(false==bret)
    {
        qInfo()<<"Q_ATE_CONTROLLER_InitFDB false";
    }
    else
    {
        qInfo()<<"Q_ATE_CONTROLLER_InitFDB() true";

        //判断是否是研发模式
        if("192.168.10.38"==m_IPLineEdit->text().trimmed())
        {
            m_OfflineModecombox->setEnabled(true);
        }
    }
}

void TradGoldLogonDiaLog::IPcomboxcurrentIndexChanged(int index)
{
    //

    {
        m_OfflineModecombox->setEnabled(false);
    }
}

void TradGoldLogonDiaLog::onOffLineCurrentIndexChanged(int index)
{
    //
    CheckAndSetStatus();
}

void TradGoldLogonDiaLog::TestModeCurrentIndexChanged(int index)
{
    wchar_t wcHar[1024] = { 0 };
    //判断
    //if(stlisttestflow.count()>=1)
    {
        SetCfg();
    }

    memset(wcHar,0x00,sizeof(wcHar));
    Q_ATE_CONTROLLER_GetTestFlowStationInfo(wcHar,1024);

    qInfo()<<"TestFlowStationInfo="<<m_cla_logon_gfc.Wchar2QString(wcHar);
    QStringList stliststion;
    stliststion=m_cla_logon_gfc.Wchar2QString(wcHar).split("->");
}

void TradLOGON_OP_EXECUTE(LPVOID x)
{

    TradGoldLogonDiaLog * pSv = (TradGoldLogonDiaLog *)x;

    pSv->DO_Logon_OP();

}

void TradGoldLogonDiaLog::DO_Logon_OP()
{
    m_bar_current_value =1;
    //判断是否是在线，
    if(m_OfflineModecombox->currentText()=="false")
    {

    }

    m_Init_WorkResource_Flag=false;
    qDebug()<<"Q_ATE_CONTROLLER_InitFirst()";

    m_bar_current_value =2;


    if(Q_ATE_CONTROLLER_Init_WorkResource()==false)
    {
        qDebug("Q_ATE_CONTROLLER_Init_WorkResource() fail");
        m_Init_WorkResource_Flag=true;

        //弹框提示失败
        //return;
    }
    else
    {
        qDebug("Q_ATE_CONTROLLER_Init_WorkResource() successful");
        m_Init_WorkResource_Flag=true;

         close();
    }

}

void TradGoldLogonDiaLog::LogFileInit()
{

    //QString strLogFile = QCoreApplication::applicationDirPath();

    //判断exe程序是否已经打开，如果打开则报错
    m_current_triangle_Index=-1;
}

void TradGoldLogonDiaLog::ShowStation(const wchar_t *stationinfo)
{
    //
    m_current_triangle_Index= -1;
//    wchar_t wcHar[1024] = { 0 };

//    memset(wcHar,0x00,sizeof(wcHar));
//    Q_ATE_CONTROLLER_GetTestFlowStationInfo(wcHar,1024);

    qInfo()<<"TestFlowStationInfo="<<m_cla_logon_gfc.Wchar2QString(stationinfo);
    QStringList stliststion;
    stliststion=m_cla_logon_gfc.Wchar2QString(stationinfo).split("->");

    if(stliststion.count()==1)
    {
        if(stliststion.at(0).isEmpty())
        {
            return;
        }
    }

    //
}

void TradGoldLogonDiaLog::ShowMessageBox(QString messtype,QString messTitle,QString messContent)
{
    //
    if("MessageBox"==messtype)
    {
        m_logon_messbox.SetShowResult(messTitle,messContent);
        m_logon_messbox.exec();
    }
    else if("QueryMessageBox"==messtype)
    {
         m_quermessbox.SetShowResult(messTitle,messContent);
         m_quermessbox.exec();

    }
}

//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void TradGoldLogonDiaLog::mousePressEvent(QMouseEvent *event)
{
    if( (event->button() == Qt::LeftButton) )
    {
        mouse_press = true;
        mousePoint = event->globalPos() - this->pos();

    // event->accept();
    }

}
void TradGoldLogonDiaLog::mouseMoveEvent(QMouseEvent *event)
{
    // if(event->buttons() == Qt::LeftButton){ //如果这里写这行代码，拖动会有点问题
    if(mouse_press && (mousePoint!=QPoint()))
    {
    move(event->globalPos() - mousePoint);
    // event->accept();
    }
}
void TradGoldLogonDiaLog::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    mousePoint = QPoint();
}

void TradGoldLogonDiaLog::myQueryMessBoxSlot(bool value)
{
    //
    m_query_current_Value = value;
}


bool TradGoldLogonDiaLog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG") //windows平台
    {
        MSG* msg = reinterpret_cast<MSG*>(message); //

        if(msg->message == WM_LOGON_MESSAGR_TS_MSG)//消息类型 runlog
        {
                //
            OnGetMsg_Load_MessageBox_State_TS(msg->wParam,msg->lParam);
        }
        else if(msg->message == WM_LOGON_PROCESS_MSG_TS_MSG)
        {
            //
            OnGetMsg_Process_State_TS(msg->wParam,msg->lParam);
        }
    }

     return QWidget::nativeEvent(eventType, message, result);//交给Qt处理
}

void TradGoldLogonDiaLog::OnGetMsg_Load_MessageBox_State_TS(WPARAM wParam, LPARAM lParam)
{


    m_query_current_Value =false;

    TESTER_MESSBOX_S  *tmpMessbox = (TESTER_MESSBOX_S*)wParam;

    m_Query_Messbox_Result ="";
    //界面累加
    QString qstmpAgrs  = m_cla_logon_gfc.Wchar2QString(tmpMessbox->pcwcAgrs);

     //判断类型
    QString tmpAPIName = m_cla_logon_gfc.Wchar2QString(tmpMessbox->pcwcAPIName);

    qInfo()<<"tmpAPIName=="<<tmpAPIName;

    //标志和内容
    QString tmpAgrs  = m_cla_logon_gfc.Wchar2QString(tmpMessbox->pcwcAgrs);

    qInfo()<<"tmpAgrs=="<<tmpAgrs;

    QStringList tmpAgrsList = tmpAgrs.split("--");

    QString qsTitle="";
    QString qsContent="";
    QString qsBoardType="";

    for(int i=0;i<tmpAgrsList.count();i++)
    {
        QString tmpqs = tmpAgrsList.at(i);
        //标题
        if(tmpqs.contains("Title"))
        {
            //先去掉"Title "
            tmpqs=tmpqs.remove("Title");
            tmpqs= tmpqs.trimmed(); //删除空格

            //去掉首尾字符
            tmpqs=tmpqs.remove(0, 1);   // 去除首字符 ''
            tmpqs.chop(1);        // 去除尾字符 ''

            qsTitle = tmpqs;

        }
        //内容
        if(tmpqs.contains("Content"))
        {
            //
            //先去掉"Content "
            tmpqs=tmpqs.remove("Content");
            tmpqs= tmpqs.trimmed(); //删除空格

            //去掉首尾字符
            tmpqs=tmpqs.remove(0, 1);   // 去除首字符 ''
            tmpqs.chop(1);        // 去除尾字符 ''

            qsContent = tmpqs;
        }
        //Board type
         if(tmpqs.contains("BoardType"))
         {
             tmpqs=tmpqs.remove("BoardType");
             tmpqs= tmpqs.trimmed(); //删除空格

             //去掉首尾字符
             tmpqs=tmpqs.remove(0, 1);   // 去除首字符 ''
             tmpqs.chop(1);        // 去除尾字符 ''

             qsBoardType = tmpqs;
         }
    }

    qInfo()<<"qsContent=="<<qsContent;
    qInfo()<<"qsTitle=="<<qsTitle;

    if("MessageBox"==tmpAPIName)
    {
        if(!qsTitle.isEmpty())
        {
            m_logon_messbox.SetShowResult(qsTitle,qsContent);
            m_logon_messbox.exec();
        }

    }
    if("QueryMessageBox"==tmpAPIName)
    {
        //
        if(!qsTitle.isEmpty())
        {
            m_quermessbox.SetShowResult(qsTitle,qsContent);

            m_quermessbox.exec();

            if(true==m_query_current_Value)
            {
                //
                m_Query_Messbox_Result = "--Result \"OK\"";
            }
            else
            {
                //
                m_Query_Messbox_Result= "--Result \"Cancel\"";
            }
        }
    }
    else if("QueryOSBarcode"==tmpAPIName)
    {

    }
    else if("MainControl"==tmpAPIName)
    {
        //
    }

}

void TradGoldLogonDiaLog::OnGetMsg_Process_State_TS( WPARAM wParam, LPARAM lParam)
{
     m_barvalue = (double*)wParam;


     m_bar_current_value = (int)(*m_barvalue);

}
