#include "logondialog.h"


TESTER_MESSBOX_S        tmplogonMessbox;

//显示弹框
bool __stdcall LogonGetCallExternalAPI(const wchar_t* pcwcAPIName, const wchar_t* pcwcAgrs, wchar_t* pwcResValueList, int iValueBufferSize, void* pobj)
{
    //这里可以直接显示弹框效果
    LogonDiaLog *tmpLogonDiaLog = static_cast<LogonDiaLog *>(pobj);


    if(nullptr==pcwcAPIName || nullptr==pcwcAgrs || nullptr ==pwcResValueList)
    {
        qDebug()<<"pcwcAPIName is NULL";
        return false;
    }

    tmplogonMessbox.pcwcAPIName=pcwcAPIName;
    tmplogonMessbox.pcwcAgrs = pcwcAgrs;
    tmplogonMessbox.iValueBufferSize = iValueBufferSize;

    ::SendMessage((HWND)tmpLogonDiaLog->m_hWnd_View, WM_LOGON_MESSAGR_TS_MSG, (WPARAM)&tmplogonMessbox, (LPARAM)0);

    wcscpy_s(pwcResValueList,iValueBufferSize,tmpLogonDiaLog->m_cla_logon_gfc.QString2Wchar(tmpLogonDiaLog->m_Query_Messbox_Result));


}

//显示进度节点
void __stdcall GetLogonResultProcess(double msg, void* pobj)
{

    LogonDiaLog *tmpLogonDiaLog = static_cast<LogonDiaLog *>(pobj);


    //m_barvalue = (double*)wParam;


    tmpLogonDiaLog->m_bar_current_value = (int)(msg);

    //richedit
   // ::SendMessage((HWND)tmpLogonDiaLog->m_hWnd_View, WM_LOGON_PROCESS_MSG_TS_MSG, (WPARAM)&msg, (LPARAM)0);

}

void LOGON_OP_EXECUTE(LPVOID);

LogonDiaLog::LogonDiaLog(QWidget *parent) :
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

    //选择变化，触发事件
    connect(m_TestModecombox,SIGNAL(currentIndexChanged(int)),this,SLOT(TestModeCurrentIndexChanged(int)));

    //release_Customer
#if Release_Customer
    qInfo()<<"Release_Customer";
    Release_Customer_Init();
#endif

    //for test

//    m_quermessbox.SetShowQueryResult("error","AAAAAAAAAAA");
//    m_quermessbox.exec();
//    m_quermessbox.SetShowQueryResult("error","AAAAAAAAAAAAAAAAAAAAAAA");
//    m_quermessbox.exec();
//    m_quermessbox.SetShowQueryResult("error","AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
//    m_quermessbox.exec();
//    m_quermessbox.SetShowQueryResult("error","AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
//    m_quermessbox.exec();
//    m_quermessbox.SetShowQueryResult("error","AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
//    m_quermessbox.exec();
//    m_quermessbox.SetShowQueryResult("error","AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
//    m_quermessbox.exec();
//    m_quermessbox.SetShowQueryResult("error","AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
//    m_quermessbox.exec();
//    m_quermessbox.SetShowQueryResult("error","AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
//    m_quermessbox.exec();
}


LogonDiaLog::~LogonDiaLog()
{
    //close时候调用一下

}

void LogonDiaLog::Init()
{
    //定义无边框
   // this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setWindowFlag(Qt::FramelessWindowHint);

    //设置大小
    this->setFixedSize(800,600);

    //设置背景色
    this->setStyleSheet("LogonDiaLog{border-image: url(./res/Logon/Logon_Background.png);}");

    //

    //
    InitLogon();


}

void LogonDiaLog::InitExternalAPI()
{
    //弹出框
    Q_ATE_CONTROLLER_LoadCallExternalAPICallBack(0,LogonGetCallExternalAPI,this);

    Q_ATE_CONTROLLER_LoadResultProcessCallBack(0,GetLogonResultProcess,this);
    m_hWnd_View = this->winId();

}

void LogonDiaLog::ReadCfg()
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

         //STATION_NAME-----------------------------------------------------------
         memset(wcHar,0x00,sizeof(wcHar));
         pStationcfg->GetParaValue(L"STATION_NAME", wcHar, 256);
         str = m_cla_logon_gfc.Wchar2QString(wcHar);
         qDebug()<<"STATION_NAME ="<<str;

         //TEST_FLOW_STATION_INFO
         memset(wcHar,0x00,sizeof(wcHar));
         pStationcfg->GetParaValue(L"TEST_FLOW_STATION_INFO", wcHar, 256);
         ShowStation(wcHar);

         //Used_Station_Name
         memset(wcHar,0x00,sizeof(wcHar));
         pStationcfg->GetParaValue(L"UsedStationName", wcHar, 256);
         GetUsedStation(wcHar);


         //
         int itriangle=0;
         for( itriangle=0;itriangle<15;itriangle++)
         {
             if(str == m_triangleLabellist.at(itriangle)->text())
             {
                 m_triangleBtnlist.at(itriangle)->ChangeBackGround("#0F5040","#27E29E");
                 if(!str.isEmpty())
                 {
                     m_current_triangle_Index = itriangle;
                 }
                 break;
             }
         }

         if(15==itriangle)
         {
              if(!str.isEmpty())
              {
                  m_triangleBtnlist.at(0)->setHidden(false);
                  m_triangleLabellist.at(0)->setText(str);
                  m_triangleBtnlist.at(0)->ChangeBackGround("#0F5040","#27E29E");

                  m_current_triangle_Index = 0;
              }
         }



//         for(iindex= 0; iindex < m_Stationcombox->count(); iindex++)
//         {
//             if(m_Stationcombox->itemText(iindex) == str)
//             {
//                //
//                m_Stationcombox->setCurrentIndex(iindex);
//                break;
//             }
//         }

//         QIcon StationIcon("./res/Logon/icon_station.png");
//         if(iindex == m_Stationcombox->count())
//         {
//             m_Stationcombox->addItem(StationIcon,str,m_Stationcombox->count());

//             m_Stationcombox->setCurrentIndex(m_Stationcombox->count()-1);
//         }

         //


         //TESTMODE--------------------------------------------------------------
         memset(wcHar,0x00,sizeof(wcHar));
         pStationcfg->GetParaValue(L"TEST_PROCESS_DES", wcHar, 256);
         str = m_cla_logon_gfc.Wchar2QString(wcHar);
         qDebug()<<"TESTMODE ="<<str;

         for(iindex= 0; iindex < m_TestModecombox->count(); iindex++)
         {
             if(m_TestModecombox->itemText(iindex) == str)
             {
                //
                m_TestModecombox->setCurrentIndex(iindex);
                break;
             }
         }

         QIcon TestmodeIcon ("./res/Logon/icon_testmode.png");
         if(iindex == m_TestModecombox->count())
         {
             m_TestModecombox->addItem(TestmodeIcon,str,m_TestModecombox->count());

             m_TestModecombox->setCurrentIndex(m_TestModecombox->count()-1);
         }

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

void LogonDiaLog::SetCfg()
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

        //STATION_NAME
        if(m_current_triangle_Index!=-1)
        {
            wstr=m_cla_logon_gfc.QString2Wchar(m_triangleLabellist.at(m_current_triangle_Index)->text());
            //wstr=m_cla_logon_gfc.QString2Wchar(m_Stationcombox->currentText());
            qDebug()<<"Set STATION_NAME"<<m_cla_logon_gfc.Wchar2QString(wstr);
            pStationcfg->SetParaValue(L"STATION_NAME", wstr);

        }

        //TESTMODE
        wstr=m_cla_logon_gfc.QString2Wchar(m_TestModecombox->currentText());
        qDebug()<<"Set TEST_PROCESS"<<m_cla_logon_gfc.Wchar2QString(wstr);
        pStationcfg->SetParaValue(L"TEST_PROCESS_DES", wstr);

        //
        wstr=m_cla_logon_gfc.QString2Wchar(m_OfflineModecombox->currentText());
        qDebug()<<"Set OfflineMode"<<m_cla_logon_gfc.Wchar2QString(wstr);
        pStationcfg->SetParaValue(L"OFFLINE_MODE", wstr);

        //


        pStationcfg->Save();
    }
    else
    {
        qDebug()<<"Q_ATE_TESTSCRIPT_PARSER_STATIONCONFIG NULL";
        MyMessageBox message_box(this,"Error","Q_ATE_TESTSCRIPT_PARSER_STATIONCONFIG() is NULL");
        message_box.exec();
    }

}

void LogonDiaLog::CheckAndSetStatus()
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

    m_DownloadFW ="";

    //
    m_Stationcombox->setHidden(true);
    //初始化状态是false 并且不能选
   // m_OfflineModecombox->setCurrentIndex(1);
   // m_OfflineModecombox->setEnabled(false);


}

void LogonDiaLog::Release_Customer_Init()
{
    qInfo()<<"======Release_Customer_Init() Start======";
    //
#ifdef Release_Customer
    //
    m_IPlabel->setVisible(false); //隐藏
    m_IPLineEdit->setVisible(false);
    m_IPTestbtn->setVisible(false);
    m_Searchbtn->setVisible(false);
    m_OfflineModecombox->setCurrentIndex(0);
    m_OfflineModelabel->setVisible(false);
    m_OfflineModecombox->setVisible(false);
    m_testModelabel->setVisible(false);
    m_TestModecombox->setVisible(false);
    m_MOlabel->setText("OC");
     m_MOLineEdit->setEnabled(true);
#endif
    qInfo()<<"======Release_Customer_Init() End======";

}
void LogonDiaLog::InitLogon()
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
    logoncontinuelabel->setText("Login to continue");
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
    m_IPlabel = new QLabel(logonlabel);
    m_IPlabel->move(25,76);
    m_IPlabel->setText("IP");
    m_IPlabel->setStyleSheet("width: 17px;height: 17px;font:18px \"Microsoft YaHei UI\";font-weight:400;color: rgb(255,255,255);");

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
    m_MOlabel = new QLabel(logonlabel);
    m_MOlabel->move(25,146);
    m_MOlabel->setText("MO");
    m_MOlabel->setStyleSheet("width: 15px;height: 15px;font:18px \"Microsoft YaHei UI\";font-weight:400 ;color: rgb(255,255,255);");

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

    //test Mode label
    m_testModelabel = new QLabel(logonlabel);
    m_testModelabel->move(25,220);
    m_testModelabel->setText("Test Process");
    m_testModelabel->setStyleSheet("width: 15px;height: 15px;font:18px \"Microsoft YaHei UI\";font-weight:400;color: rgb(255,255,255);");

    //Test Mode
    m_TestModecombox  = new AutoWidthComboBox(logonlabel);
    m_TestModecombox->move(25,240);
    m_TestModecombox->setFixedSize(342,32);

    QIcon TestmodeIcon ("./res/Logon/icon_testmode.png");

    m_TestModecombox->setStyleSheet(
                              "QComboBox{background: #252e39;color: rgb(255,255,255);font:16px \"Microsoft YaHei UI\";font-weight:400;}"
                              //"QComboBox::down-arrow{width: 24px;height: 24px;border-image: url(./res/Logon/arrow.png);}"
                             // "QComboBox::drop-down{background: #252e39;}"//设置下拉框按钮背景及边框
                              //"QComboBox QAbstractItemView{ background-color: #252e39;color: white; }"
                              "QComboBox QAbstractItemView{ background-color:#252e39;color: white; }"
                              "QComboBox QAbstractItemView::item:hover{color: #27E29E;background-color: #27E29E4D;}"
                              "QComboBox QAbstractItemView::item:selected{color: #27E29E;background-color: #27E29E4D;} "
                              );

    //加载combox的新样式
     QStyledItemDelegate *TestModedelegate = new QStyledItemDelegate();
     m_TestModecombox ->setItemDelegate(TestModedelegate);


     for(int i=0;i<15;i++)
     {
         //生成groupbox
         TrianglePushButton *tptriangleBtn = new TrianglePushButton(logonlabel);
         tptriangleBtn->setGeometry(0,0,90,50);
         m_triangleBtnlist.append(tptriangleBtn);

         //
         connect(tptriangleBtn, SIGNAL(clicked()), this, SLOT(TptriangleBtnClick()));

         QLabel  *tplabel = new QLabel(tptriangleBtn);
         tplabel->move(8,0);
         m_triangleLabellist.append(tplabel);

         tplabel->setGeometry(10,0,80,30);
         tplabel->setStyleSheet("QLabel{font-weight: 400;font: 12px \"Microsoft YaHei UI\";color: #ffffff;background: transparent;}");
         tplabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

         if(i<5)
         {
             tptriangleBtn->move(30+i*90,320);
         }
         else if(i<10 && i>=5)
         {
             tptriangleBtn->move(30+(i-5)*90,370);
         }
         else
         {
             tptriangleBtn->move(30+(i-10)*90,420);
         }

         tptriangleBtn->setHidden(true);
     }


    //选择变化，触发事件
    //connect(m_TestModecombox,SIGNAL(currentIndexChanged(int)),this,SLOT(TestModeCurrentIndexChanged(int)));

    //Station  label
    QLabel *Stationlabel = new QLabel(logonlabel);
    Stationlabel->move(23,290);
    Stationlabel->setText("Select Station");
    Stationlabel->setStyleSheet("width: 15px;height: 15px;font:18px \"Microsoft YaHei UI\";;font-weight:400;color: rgb(255,255,255);");

    //Station
    m_Stationcombox  = new QComboBox(logonlabel);
    m_Stationcombox->move(150,280);
    m_Stationcombox->setFixedSize(184,32);

    QIcon StationIcon ("./res/Logon/icon_station.png");

    m_Stationcombox->setStyleSheet(
                              "QComboBox{background: #252e39;color: rgb(255,255,255);font:16px \"Microsoft YaHei UI\";font-weight:400;}"
                              //"QComboBox::down-arrow{width: 24px;height: 24px;border-image: url(./res/Logon/arrow.png);}"
                             // "QComboBox::drop-down{background: #252e39;}"//设置下拉框按钮背景及边框
                              //"QComboBox QAbstractItemView{ background-color: #252e39;color: white; }"
                              "QComboBox QAbstractItemView{ background-color:#252e39;color: white; }"
                              "QComboBox QAbstractItemView::item:hover{color: #27E29E;background-color: #27E29E4D;}"
                               "QComboBox QAbstractItemView::item:selected{color: #27E29E;background-color: #27E29E4D;} "
                              );

    //加载combox的新样式
     QStyledItemDelegate *Stationdelegate = new QStyledItemDelegate();
     m_Stationcombox ->setItemDelegate(Stationdelegate);

    //OFFLine Mode label
    m_OfflineModelabel = new QLabel(logonlabel);
    m_OfflineModelabel->move(376,220);
    m_OfflineModelabel->setText("Offline Mode");
    m_OfflineModelabel->setStyleSheet("width: 15px;height: 15px;font:18px \"Microsoft YaHei UI\";;font-weight:400;color: rgb(255,255,255);");

    //OFFLine Mode
    m_OfflineModecombox  = new QComboBox(logonlabel);
    m_OfflineModecombox->move(376,240);
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
    m_Enterbtn->move(110,462);
    m_Enterbtn->setStyleSheet("QPushButton{width: 136px;height: 40px;font:20px \"Microsoft YaHei UI\";font-weight: 400;color: #27E29E;border-radius: 4px 4px 4px 4px;background: rgba(39,226,158,0.8);}"
                              "QPushButton:hover{background: rgba(39,226,158,0.4);}"
                              "QPushButton:pressed{background: rgba(39,226,158,0.1);}");
    connect(m_Enterbtn, SIGNAL(clicked()), this, SLOT(EnterLogon()));

    //reselect
    m_ReSelectbtn = new QPushButton(logonlabel);
    m_ReSelectbtn->setText("ReSelect");
    m_ReSelectbtn->move(266,462);
    m_ReSelectbtn->setStyleSheet("QPushButton{width: 136px;height: 40px;font:20px \"Microsoft YaHei UI\";font-weight: 400;color: #27E29E;border-radius: 4px 4px 4px 4px;background: rgba(39,226,158,0.8);}"
                                 "QPushButton:hover{background: rgba(39,226,158,0.2);}"
                                 "QPushButton:pressed{background: rgba(39,226,158,0.0);}");

    connect(m_ReSelectbtn, SIGNAL(clicked()), this, SLOT(ReSelect()));

}

void LogonDiaLog::CloseLogon()
{
//    MyQueryMessageBox qbx(this);
//    qbx.showMessBox("Info","OKKKKKKKKKKKKKKKKKKKK");
//    qbx.exec();

    SetCfg();
    exit(0);
}

void LogonDiaLog::EnterLogon()
{

    m_bar_current_value=1;

    // 配置参数修改
    SetCfg();

    qInfo()<<"EnterLogon() m_current_triangle_Index "<<m_current_triangle_Index;
    if(-1==m_current_triangle_Index)
    {
        //弹框提示失败
        MyMessageBox message_box(this,"Error","Not Select Staion Name Fail!!");
        message_box.exec();
        return;
    }



    //启动一个线程函数进行初始化
    //创建线程
    m_hThread_Logon_INIT_Handle = CreateThread( NULL,
        0,
        (LPTHREAD_START_ROUTINE)LOGON_OP_EXECUTE,
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
    dialog.setLabelText("Initializing...");
    dialog.setStyleSheet("QProgressDialog{background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);font: 14px \"Microsoft YaHei UI\";}");

    QLabel textlabel;

    textlabel.setText("Initializing.....");
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

void LogonDiaLog::ReSelect()
{
    QString qsTitle;
    QString qsContent;

    qsTitle = "title";
    qsContent ="content";

    wchar_t pwcResValueList[200];

    LogonGetCallExternalAPI(L"QueryMessageBox",L"--Title \"title\"  --Content \"content\"",pwcResValueList,200,this);
//    m_logon_messbox.SetShowResult(qsTitle,qsContent);
//    m_logon_messbox.exec();

//    m_quermessbox.SetShowResult(qsTitle,qsContent);
//    m_quermessbox.exec();
}

void LogonDiaLog::TptriangleBtnClick()
{
    //
    //判断是哪个button

    QObject* button = QObject::sender();

    int i=0;
    for(i=0;i<15;i++)
    {
        if(button == m_triangleBtnlist.at(i))
        {
            m_triangleBtnlist.at(i)->ChangeBackGround("#0F5040","#27E29E");
            qDebug() << "m_triangleBtnlist " << i;
            //break;
            m_current_triangle_Index = i;
        }
        else
        {
            //如果是不可点击的按钮.
            int j=0;
            for(j=0;j<m_stusedliststion.count();j++)
            {
                //判断是否存在，如果存在则可以修改，如果不存在，则不用修改
                if(m_triangleLabellist.at(i)->text().trimmed()==m_stusedliststion.at(j))
                {
                    break;
                }
            }

            if(m_stusedliststion.count()!=j)
            {
                m_triangleBtnlist.at(i)->ChangeBackGround("#252E39","#5F6268");
            }
        }
    }

    if(i==15)
    {
        qDebug() << "not m_triangleBtnlist  ";
        return;
    }

    //


}

void LogonDiaLog::SearchMO()
{

    //
    m_Stationcombox->clear();
    m_TestModecombox->clear();


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
    for(int i=0;i<stlisttestflow.count();i++)
    {
        m_TestModecombox->addItem(stlisttestflow.at(i));
    }

    SetCfg();

    memset(wcHar,0x00,sizeof(wcHar));
    Q_ATE_CONTROLLER_GetTestFlowStationInfo(wcHar,1024);

    ShowStation(wcHar);

    //Used_Station_Name
    memset(wcHar,0x00,sizeof(wcHar));
    pStationcfg->GetParaValue(L"UsedStationName", wcHar, 256);
    GetUsedStation(wcHar);

    // Q_ATE_CONTROLLER_GetTestFlowStationInfo，需要再save，才能保存UsedStationName，不然不能实现
    pStationcfg->Save();

}

void LogonDiaLog::IPTest()
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

        //判断是否是研发模式，研发模式可以离线测试，非研发模式必须在线测试
        if("192.168.10.38"==m_IPLineEdit->text().trimmed())
        {
            qInfo()<<" this is R&D mode ";
            m_OfflineModecombox->setEnabled(true);
        }
    }
}

void LogonDiaLog::IPcomboxcurrentIndexChanged(int index)
{
    //

    {
        m_OfflineModecombox->setEnabled(false);
    }
}

void LogonDiaLog::onOffLineCurrentIndexChanged(int index)
{
    //
    CheckAndSetStatus();
}

void LogonDiaLog::TestModeCurrentIndexChanged(int index)
{
    qInfo()<<"TestModeCurrentIndexChanged() Start";

    for(int j=0;j<15;j++)
    {
        //隐藏
         m_triangleBtnlist.at(j)->setHidden(true);
    }
       wchar_t wcHar[1024] = { 0 };
     // SearchMO();
      SetCfg(); //设置配置参数给dll
      memset(wcHar,0x00,sizeof(wcHar));
      Q_ATE_CONTROLLER_GetTestFlowStationInfo(wcHar,1024); //从dll获取stion name

      ShowStation(wcHar); //显示Station name

      //Used_Station_Name
      memset(wcHar,0x00,sizeof(wcHar));
      pStationcfg->GetParaValue(L"UsedStationName", wcHar, 256); //显示主要显示项
      GetUsedStation(wcHar);

}

void LOGON_OP_EXECUTE(LPVOID x)
{

    LogonDiaLog * pSv = (LogonDiaLog *)x;

    pSv->DO_Logon_OP();

}

void LogonDiaLog::DO_Logon_OP()
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

void LogonDiaLog::LogFileInit()
{

    //QString strLogFile = QCoreApplication::applicationDirPath();
//    QString strLogFile = QDir::currentPath();
//    QLogOutput::install(strLogFile);


//    qDebug()<<strLogFile;
//    qInfo()<<QCoreApplication::applicationFilePath();
//    qInfo()<<QDir::currentPath();

    //判断exe程序是否已经打开，如果打开则报错
    m_current_triangle_Index=-1;
}

void LogonDiaLog::ShowStation(const wchar_t *stationinfo)
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
    for(int i=0;i<stliststion.count();i++)
    {
        //显示
        m_triangleBtnlist.at(i)->ChangeBackGround("#252E39","#5F6268");
        m_triangleBtnlist.at(i)->setHidden(false);
        m_triangleLabellist.at(i)->setText(stliststion.at(i));
        //
    }
    for(int j=stliststion.count();j<15;j++)
    {
        //隐藏
         m_triangleBtnlist.at(j)->setHidden(true);
    }

    //
}

void LogonDiaLog::GetUsedStation(const wchar_t *usedstationinfo)
{
    //
    m_stusedliststion.clear();
    m_stusedliststion = m_cla_logon_gfc.Wchar2QString(usedstationinfo).split(";");

    //
    //双重循环
    for(int i=0;i<m_triangleLabellist.count();i++)
    {
        int j=0;
        for(;j<m_stusedliststion.count();j++)
        {
            if(m_triangleLabellist.at(i)->text().trimmed()== m_stusedliststion.at(j))
            {
                break;
            }
        }
        //说明没有找到
        if(j==m_stusedliststion.count())
        {
            m_triangleBtnlist.at(i)->setEnabled(false);
            //设置成灰色
            m_triangleBtnlist.at(i)->ChangeBackGround("#171C23","#3D4144");
            m_triangleLabellist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 12px \"Microsoft YaHei UI\";color: #3D4144;background: transparent;}");

        }
        else
        {
            //找到设置成可使用状态
            m_triangleBtnlist.at(i)->setEnabled(true);
            m_triangleLabellist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 12px \"Microsoft YaHei UI\";color: #ffffff;background: transparent;}");

        }
    }
}


//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void LogonDiaLog::mousePressEvent(QMouseEvent *event)
{
    if( (event->button() == Qt::LeftButton) )
    {
        mouse_press = true;
        mousePoint = event->globalPos() - this->pos();

    // event->accept();
    }

}
void LogonDiaLog::mouseMoveEvent(QMouseEvent *event)
{
    // if(event->buttons() == Qt::LeftButton){ //如果这里写这行代码，拖动会有点问题
    if(mouse_press && (mousePoint!=QPoint()))
    {
    move(event->globalPos() - mousePoint);
    // event->accept();
    }
}
void LogonDiaLog::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    mousePoint = QPoint();
}

void LogonDiaLog::myQueryMessBoxSlot(bool value)
{
    //
    m_query_current_Value = value;
}


bool LogonDiaLog::nativeEvent(const QByteArray &eventType, void *message, long *result)
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

void LogonDiaLog::OnGetMsg_Load_MessageBox_State_TS(WPARAM wParam, LPARAM lParam)
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
            //SetShowQueryResult
            //m_quermessbox.SetShowResult(qsTitle,qsContent);
            m_quermessbox.SetShowQueryResult(qsTitle,qsContent);

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

void LogonDiaLog::OnGetMsg_Process_State_TS( WPARAM wParam, LPARAM lParam)
{
     m_barvalue = (double*)wParam;


     m_bar_current_value = (int)(*m_barvalue);

}

void LogonDiaLog::PrintDebugLog()
{
}
