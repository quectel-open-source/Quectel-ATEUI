#include "ateuimainwindow.h"
#include <QStatusBar>



//定义32个channel的数组

int channelnum[32]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

int g_iActiveChannelNums=1;
int g_channelnum=1;
// CMIView construction/destruction
void OP_EXECUTE(LPVOID);
void OP_EXECUTE_TIMER(LPVOID);

void OP_START_TS_SERVER( LPVOID );

//DL  初始化
void OP_DL_INIT_EXECUTE(LPVOID);

//像总控回传消息
void OP_EXECUTE_SEND_RESULT_LAN_CMD_TO_MC_PC(LPVOID);


struct threadInfo
{
    ATEUIMainWindow  *CAteMainWgt;  //窗口句柄
    int  channelnum;                //通道数
};

threadInfo tmpthreadInfoList[MAX_CHANNEL_NUM];

struct threadMCBCInfo
{
    ATEUIMainWindow  *CAteMainWgt;  //窗口句柄
    int  channelnum;                //通道数
    QString state;  //状态
    int robot_LOSS_Status;
};

threadMCBCInfo tmpthreadMCBCInfo[MAX_CHANNEL_NUM];

TESTER_RESULT_S g_tmpResult;

ATEUIMainWindow::ATEUIMainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,m_DL_Current_Channel(0)
    ,m_rdretrynum(0)
    ,m_Max_Normal_Flag(true)
{

    //
    // LogFileInit(); //log init
    //
    LogonInit();

    //初始化第三方lib
    ATE_Ctrol_Lib_Init();

    UIinit();
    OnlyOneInit();              //只需要一次初始化的
    UIDataInit();


}

ATEUIMainWindow::~ATEUIMainWindow()
{
    //close时候调用一下
    Q_ATE_CONTROLLER_ReleaseAll();

    //关闭TS服务器
    ::TS_LAN_STOP_TCP_SERVER();
}

void ATEUIMainWindow::LogonInit()
{
    QString tmpLog;
    qInfo("LogonInit()");
    wchar_t wcHar[256] = { 0 };
    m_ateStationcfg = (Q_ATE_Controller_STATIONCONFIG_Interface*)Q_ATE_CONTROLLER_GetStationInfo();
    if (m_ateStationcfg != NULL)
    {
        //STATION_NAME-----------------------------------------------------------
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"STATION_NAME", wcHar, 256);
        m_StationName = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"LogonInit STATION_NAME ="<<m_StationName;

        //根据Stationnanme转换成自动线的站位名
        StationNameToRoboticStationName();

        //MO_NAME-----------------------------------------------------------
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"MO", wcHar, 256);
        m_MoName = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"LogonInit m_MoName ="<<m_MoName;

        //TESTMODE--------------------------------------------------------------
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"TEST_PROCESS_DES", wcHar, 256);
        m_Test_Process = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"m_Test_Process ="<<m_Test_Process;

        //读取
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"OFFLINE_MODE", wcHar, 256);
        m_Offline_Status = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"OFFLINE_MODE "<<m_Offline_Status;

        //读取DownLoadScript
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"DownLoadScript", wcHar, 256);
        m_DownLoadScript_Status = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"m_DownLoadScript_Status "<<m_DownLoadScript_Status;

        //读取DownLoadFunctionDll
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"DownLoadFunctionDll", wcHar, 256);
        m_DLFunctionDll_Status = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"m_DLFunctionDll_Status "<<m_DLFunctionDll_Status;


        //ESTSCRIPT_VERSION   脚本版本信息
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"TESTSCRIPT_VERSION", wcHar, 256);
        m_testscript_version = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"TESTSCRIPT_VERSION:"<<m_testscript_version;

        //FUNCTION_DLL_VERSION DLL 版本信息
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"FUNCTION_DLL_VERSION", wcHar, 256);
        m_function_dll_version = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"FUNCTION_DLL_VERSION:"<<m_function_dll_version;

        //ECN信息
        memset(wcHar,0x00,sizeof(wcHar));
        m_ateStationcfg->GetParaValue(L"IS_ECN", wcHar, 256);
        m_IS_ECN = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"IS_ECN "<<m_IS_ECN;

    }
    else
    {
        qDebug()<<"Q_ATE_CONTROLLER_GetStationInfo is null";
        m_message_box.SetShowResult("Error","Q_ATE_CONTROLLER_GetStationInfo is NULL");
        m_message_box.exec();
        return;
    }

//    m_logonDlg = new LogonDiaLog(this);
//    int ret = m_logonDlg->exec();

//    if (QDialog::Accepted == ret)
//    {

//    }

    //根据选择的结果生成界面
    //m_current_station = m_logonDlg->m_Stationcombox->currentIndex();


   // if(m_logonDlg->m_Stationcombox->currentText().trimmed()=="DL_FW")
    if((m_StationName.trimmed()=="DL_FW") || (m_StationName.trimmed()=="SecondDL"))
    {
        m_current_station = TEST_STATION_DL;
    }
    else
    {
        m_current_station= TEST_STATION_BT;
    }

    tmpLog= QString("m_current_station = %1").arg(m_current_station);
    qInfo()<<tmpLog;
}

void ATEUIMainWindow::LogFileInit()
{

    QString strLogFile = QCoreApplication::applicationDirPath();
    QLogOutput::install(strLogFile);


    qDebug()<<strLogFile;
    qInfo()<<QCoreApplication::applicationFilePath();
    qInfo()<<QDir::currentPath();

}

void ATEUIMainWindow::UIinit()
{
    //大小800,600
   // this->setFixedSize(1200,800);  

    //定义无边框
    this->setWindowFlag(Qt::FramelessWindowHint);

    //this->setStyleSheet(QString::fromUtf8("border:5px solid red"));

    this->setMinimumWidth(1034);
    this->setMinimumHeight(778);

    this->statusBar()->setSizeGripEnabled(true);

    //设置背景色
    //this->setStyleSheet("QWidget{background-color: #202020;}");
    QPalette pal = this->palette(); // 获取当前窗口的调色板
    pal.setColor(QPalette::Background, "#202020"); // 设置背景颜色为蓝色
    this->setPalette(pal); // 应用新的调色板

    SetMainWigdetUI();
    //CenterWigdetInit();

    //生成setting界面和测试界面相互切换
    SetCfgRFWgtUI();

    //
    SetCfgPageWidgetUI();

    SetPublicWgtUI();

    SetChannelInterfaceUI();
    SetDownLoadWgtUI();

    SetTestMainWgtUI();

    //设置为四通道的界面
    //g_channelnum =4;

    if(m_current_station==TEST_STATION_DL)
    {
        g_channelnum=6;
        ChooseDLWgtUI(g_channelnum);
    }
    else
    {
        g_channelnum =4;
        ChooseTestWgtUI(g_channelnum);
    }

    //登录框
}

void ATEUIMainWindow::OnlyOneInit()
{
    //提取PC名
    m_csLocal_PC_Name = QHostInfo::localHostName();

    //初始化
    myprocess = new QProcess(this);
    connect(myprocess, SIGNAL(readyRead()), this, SLOT(ProcessreadAllStandardOutput()));

    //
   connect(&m_querymess_box, SIGNAL(mySignal(bool)), this, SLOT(myQueryMessBoxSlot(bool)));
   connect(&m_inputmess_box, SIGNAL(mySignal(bool)), this, SLOT(myQueryMessBoxSlot(bool)));

    //为了测试
   qInfo()<<"this->winId()="<<this->winId();

   //自动线初始化
   TS_LAN_INIT_FIRST((HWND)this->winId());

   for(int i=0;i<MAX_CHANNEL_NUM;i++)
   {
       m_hThread_DL_Handle_List[i]=NULL;
       m_hThread_Timer_Handle_List[i]=NULL;
   }

   //离线和在线模式是否需要显示
   if(m_Offline_Status=="true")
   {
       m_OfflineModelabel->setVisible(true);
       m_showOfflineModelabel->setVisible(true);
       //m_showOfflineModelabel->setText("TRUE");
   }
   else
   {
       m_OfflineModelabel->setVisible(false);
       m_showOfflineModelabel->setVisible(false);
   }
   //DownLoadScript 不下载显示在界面
   if(m_DownLoadScript_Status=="false")
   {
       m_DLScriptlabel->setVisible(true);
       m_showDLScriptlabel->setVisible(true);
   }
   else
   {
       m_DLScriptlabel->setVisible(false);
       m_showDLScriptlabel->setVisible(false);
   }

   //DownLoadFunctionDll 不下载显示在界面
   if(m_DLFunctionDll_Status=="false")
   {
       m_DLFunctionDll->setVisible(true);
       m_showDLFunctionDll->setVisible(true);
   }
   else
   {
       m_DLFunctionDll->setVisible(false);
       m_showDLFunctionDll->setVisible(false);
   }

}

void ATEUIMainWindow::UIDataInit()
{
    //设置值
    qDebug()<<"pCommonScript->GetCommonParaCount()"<<pCommonScript->GetCommonParaCount();

    m_CommsetTableWgt->clearContents();
    while (m_CommsetTableWgt->rowCount() > 0)
    {
        m_CommsetTableWgt->removeRow(0);
    }

    for (int j=0; j<pCommonScript->GetCommonParaCount(); j++)
    {

        {
            m_CommsetTableWgt->insertRow(j);
            m_CommsetTableWgt->setItem(j,0,new QTableWidgetItem(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaName(j))));
            m_CommsetTableWgt->item(j,0)->setTextAlignment(Qt::AlignCenter);
            m_CommsetTableWgt->item(j,0)->setFlags(m_CommsetTableWgt->item(j,0)->flags() & (~Qt::ItemIsEditable));

            if(pCommonScript->GetCommonParaType(j)==1)  //下拉框
            {
                //下拉框来设置
                MyComboBox *comboxtype = new MyComboBox(m_CommsetTableWgt);
                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)comboxtype);

                //
                QStringList stlistcombox;
                stlistcombox=m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaItem(j)).split(",");
                //取得值分号结束
                comboxtype->addItems(stlistcombox);

                for(int i=0;i<stlistcombox.count();i++)
                {
                    comboxtype->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
                }

                //border: 0px solid gray;
                comboxtype->setStyleSheet(
                                          "QComboBox{background: rgba(37,46,57,1);color: rgb(255,255,255);font:16px \"Microsoft YaHei UI\";font-weight:400;}"
                                          "QComboBox QAbstractItemView{background: rgba(37,46,57,1);color: white; text-align: center;}"
                                          "QComboBox QAbstractItemView::item:hover{color: #27E29E;background: rgba(37,46,57,1)}"
                                          "QComboBox QAbstractItemView::item:selected{color: #27E29E;background: rgba(37,46,57,1)} "
                                          );
                //加载combox的新样式
                 QStyledItemDelegate *Stationdelegate = new QStyledItemDelegate();
                 comboxtype ->setItemDelegate(Stationdelegate);

                 comboxtype->setMaxVisibleItems(20); //先设置20个

                 //设置当前的值
                 comboxtype->setCurrentText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaValue(j)));
            }
            else
            {
                QLineEdit   *lineedit = new QLineEdit(m_CommsetTableWgt);
                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)lineedit);
                lineedit->setText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaValue(j)));
                lineedit->setAlignment(Qt::AlignCenter);
                lineedit->setStyleSheet("QLineEdit{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: #252e39;border-radius: 2.78px 2.78px 2.78px 2.78px;}");

            }

            m_CommsetTableWgt->setItem(j,2,new QTableWidgetItem(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaDes(j))));
            m_CommsetTableWgt->item(j,2)->setTextAlignment(Qt::AlignCenter);
            m_CommsetTableWgt->item(j,2)->setFlags(m_CommsetTableWgt->item(j,2)->flags() & (~Qt::ItemIsEditable));

            m_CommsetTableWgt->setRowHeight(j,50);
        }
    }

    m_currentTreeIndex=0;

    //读取Tsnum和Fixtruecode
    wchar_t wcHar[256] = { 0 };

    for(int i=0;i<g_channelnum;i++)
    {
        //TestNum
        memset(wcHar,0x00,sizeof(wcHar));
        pCommonScript->GetTchPara(i+1,L"TSNum",wcHar,256);
        if(m_current_station==0) //DL
        {
            m_DLShowtestNolablist.at(i)->setText(m_cla_ate_gfc.Wchar2QString(wcHar));
        }
        else
        {
            m_TestShowtestNolablist.at(i)->setText(m_cla_ate_gfc.Wchar2QString(wcHar));
        }

        //FixtureNo
        memset(wcHar,0x00,sizeof(wcHar));
        pCommonScript->GetTchPara(i+1,L"FixtureCode",wcHar,256);
        if(m_current_station==0) //DL
        {
            m_DLShowFixNolist.at(i)->setText(m_cla_ate_gfc.Wchar2QString(wcHar));
        }
        else
        {
            m_TestShowFixNolist.at(i)->setText(m_cla_ate_gfc.Wchar2QString(wcHar));
        }
    }

    //读取Line的值
    memset(wcHar,0x00,sizeof(wcHar));
    pCommonScript->GetCommonPara(L"Line",wcHar,256);    
    showLinelab->setText(m_cla_ate_gfc.Wchar2QString(wcHar));

    //读取operator的值
    memset(wcHar,0x00,sizeof(wcHar));
    pCommonScript->GetCommonPara(L"Operator",wcHar,256);
    showOperatorlab->setText(m_cla_ate_gfc.Wchar2QString(wcHar));

    //读取MO的值
    showMOlab->setText(m_MoName);

    //初始化统计
    if(m_current_station==0) //DL
    {
        m_statistics_ini = new QSettings(QDir::currentPath()+"/DataBases/DL_tongji.ini",QSettings::IniFormat);
    }
    else
    {
        m_statistics_ini = new QSettings(QDir::currentPath()+"/DataBases/Test_tongji.ini",QSettings::IniFormat);
    }
    //设置文件编码，配置文件中使用中文时，这是必须的，否则乱码
    m_statistics_ini->setIniCodec(QTextCodec::codecForName("UTF8"));


    //
    QString fileName = QDir::currentPath() + "/DataBases/yanfa.ini";
    qInfo()<<fileName;
    if(QFile::exists(fileName))
    {
        m_yanfa_ini = new QSettings(QDir::currentPath()+"/DataBases/yanfa.ini",QSettings::IniFormat);
        m_yanfa_ini->setIniCodec(QTextCodec::codecForName("UTF-8"));

        //取得RertyFlag次数
        QString tmpstrflag= "Config/RertyFlag";
        m_rdretrynum  = m_yanfa_ini->value(tmpstrflag).toInt();
        qInfo()<<"m_rdretrynum ="<<m_rdretrynum;

    }
    else
    {
        qInfo()<<"yanfa.ini no exsit";
    }

    //
    for(int i=0;i<g_iActiveChannelNums;i++)
    {
        Update_Statistics_Data(i);
    }

    //读取OC的值
    QString qstOC;
    qstOC= m_cla_ate_gfc.Wchar2QString(pProductInfoScript->GetParaValueByName(L"OC"));
    showOClab->setText(qstOC);

    //打开Active的
    for(int i=0;i<g_iActiveChannelNums;i++)
    {
        if(m_current_station==0) //DL
        {
            m_DLThreadGrouplist.at(i)->setDisabled(false);
            m_DLThreadGrouplist.at(i)->setStyleSheet("QGroupBox{border-radius: 4px;border: 1px solid #808080;}");
            m_DLTrianglelablist.at(i)->ChangeBackGround("#909090");
        }
        else
        {
            m_TestThreadGrouplist.at(i)->setDisabled(false);
            m_TestThreadGrouplist.at(i)->setStyleSheet("QGroupBox{border-radius: 4px;border: 1px solid #808080;}");

            //清空
            m_TestLogTableWgtlist.at(i)->clearContents();

            while (m_TestLogTableWgtlist.at(i)->rowCount() > 0)
            {
                m_TestLogTableWgtlist.at(i)->removeRow(0);
            }
            m_TestTextLogEditlist.at(i)->clear();
        }
    }
    //根据active数据 disable
    for(int i=g_iActiveChannelNums;i<g_channelnum;i++)
    {
        if(m_current_station==0) //DL
        {
            m_DLTrianglelablist.at(i)->ChangeBackGround("#909090");
            m_DLChanNumlist.at(i)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #909090;}");
            m_DL_StartBtnlist.at(i)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(144,144,144,1.0);;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                     "QPushButton:hover{background: rgba(144,144,144,0.5);}"
                                                     "QPushButton:pressed{background: rgba(144,144,144,0.3);}"
                                                     );
            m_DLStatuslablist.at(i)->setText("IDLE");
            m_DLStatuslablist.at(i)->setStyleSheet("QLabel{font-weight: 700;font: 12px \"Microsoft YaHei UI\";color:#0E1016;background-color:#909090;text-align: center;}");


            m_DLThreadGrouplist.at(i)->setDisabled(true);
            m_DLThreadGrouplist.at(i)->setStyleSheet("QGroupBox{border-radius: 4px;border: 1px solid #808080;background:rgba(128,128,128,0.4)}");

        }
        else
        {
            m_TestTrianglelablist.at(i)->ChangeBackGround("#909090");
            m_TestChanNumlist.at(i)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #909090;}");
            m_Test_StartBtnlist.at(i)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(144,144,144,1.0);;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                     "QPushButton:hover{background: rgba(144,144,144,0.5);}"
                                                     "QPushButton:pressed{background: rgba(144,144,144,0.3);}"
                                                     );
            m_TestStatuslablist.at(i)->setText("IDLE");
            m_TestStatuslablist.at(i)->setStyleSheet("QLabel{font-weight: 700;font: 12px \"Microsoft YaHei UI\";color:#0E1016;background-color:#909090;text-align: center;}");

            m_TestThreadGrouplist.at(i)->setDisabled(true);
            m_TestThreadGrouplist.at(i)->setStyleSheet("QGroupBox{border-radius: 4px;border: 1px solid #808080;background:rgba(128,128,128,0.5)}");


        }
    }

    //读取tooltype的值
    memset(wcHar,0x00,sizeof(wcHar));
    pCommonScript->GetCommonPara(L"Operation",wcHar,256);
    if(m_cla_ate_gfc.Wchar2QString(wcHar).trimmed()=="Manual")
    {
        m_iOP_Multi_Option = LOAD_MANUAL;
    }
    else if(m_cla_ate_gfc.Wchar2QString(wcHar).trimmed()=="Semi-Auto")
    {
        m_iOP_Multi_Option = LOAD_SEMI_FIXTURE;
    }
    else if(m_cla_ate_gfc.Wchar2QString(wcHar).trimmed()=="Robotic Automation")
    {
        m_iOP_Multi_Option = LOAD_AUTO_ROBOT;

        //读取Local_Port的值
        memset(wcHar,0x00,sizeof(wcHar));
        pCommonScript->GetCommonPara(L"Local_Port",wcHar,256);
        int localport;
        bool ok;
        localport =m_cla_ate_gfc.Wchar2QString(wcHar).toInt(&ok);
        //载入“TS测试台IP与端口”数据
        memset(wcHar,0x00,sizeof(wcHar));
        pCommonScript->GetCommonPara(L"Local_IP",wcHar,256);
        TS_LAN_LOAD_TS_PC_LAN_IP_AND_PORT(wcHar, localport);//LAN Port: 5000

        //读取Robot_Port的值
        memset(wcHar,0x00,sizeof(wcHar));
        pCommonScript->GetCommonPara(L"Robot_Port",wcHar,256);
        int port;
        port =m_cla_ate_gfc.Wchar2QString(wcHar).toInt(&ok);
        //读取Robot_IP的值
        memset(wcHar,0x00,sizeof(wcHar));
        pCommonScript->GetCommonPara(L"Robot_IP",wcHar,256);
        TS_LAN_LOAD_MAIN_CTRL_SERVER_LAN_IP_AND_PORT(wcHar, port);//LAN Port: 5000

        //创建线程 (TS服务器 监听)
        this->m_hThread_Handle_5 = CreateThread( NULL,
            0,
            (LPTHREAD_START_ROUTINE)OP_START_TS_SERVER,
            (void *)this,
            0,
            &this->m_dwThread_ID_5 );
    }
    else
    {
        m_iOP_Multi_Option = LOAD_MANUAL;
    }

    qInfo()<<"Operation"<<m_cla_ate_gfc.Wchar2QString(wcHar).trimmed()<<m_iOP_Multi_Option;


    //电源标志位
    for(int i=0;i<g_iActiveChannelNums;i++)
    {
        memset(wcHar,0x00,sizeof(wcHar));
        pCommonScript->GetTchPara(i+1,L"PowerSupplyCtrl",wcHar,256);

        //
        if("1"==m_cla_ate_gfc.Wchar2QString(wcHar))
        {
            ChannelPowerFlag[i]=true;
        }
        else
        {
            ChannelPowerFlag[i]=false;
        }
    }

    //
    for(int i=0;i<TEST_USE_CHANNEL_NUM;i++)
    {
        m_bLoad_OS_Open_Ref_Finish_Flag[i]=false;
    }


    this->m_Id1 = startTimer(1000);     // 毫秒级

    //
    //ATEProcessEvents();
}

void ATEUIMainWindow::ATE_Ctrol_Lib_Init()
{

    //登入界面执行确认按钮执行如下，在线开始下载运行脚本，测试功能DLL，离线初始化默认脚本。
    qDebug()<<"ATE_Ctrol_Lib_Init()";
    // 启动定时器

    //commset
    wchar_t wcHar[256] = { 0 };


    pCommonScript = (Q_ATE_Controller_CommonScript_Interface*)Q_ATE_CONTROLLER_GetCommonSetting();
    if (pCommonScript != NULL)
    {
         memset(wcHar,0x00,sizeof(wcHar));
         pCommonScript->GetCommonPara(L"ActivateChannel", wcHar, 256);

         QString str;
         str = m_cla_ate_gfc.Wchar2QString(wcHar);
         qDebug()<<"ActivateChannel num="<<str;

         bool ok;
         g_iActiveChannelNums = str.toInt(&ok,10);
         qDebug()<<"g_iActiveChannelNums ="<<g_iActiveChannelNums;

         if(g_iActiveChannelNums<1 ||g_iActiveChannelNums>32)
         {
             g_iActiveChannelNums=1;
         }

    }
    else
    {
        qDebug()<<"Q_ATE_CONTROLLER_GetCommonSetting NULL";
        return;
    }

    //
    pProductInfoScript = (Q_ATE_Controller_ProductInfo_Interface*)Q_ATE_CONTROLLER_GetProductInfo();
    if(pProductInfoScript==nullptr)
    {
        qDebug()<<"Q_ATE_Controller_ProductInfo_Interface is null";
        return;
    }
}

void ATEUIMainWindow::MenuInit()
{

    //this->setFixedSize(1200,800);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFocusPolicy(Qt::NoFocus);

    //创建菜单栏
    menuBar = new QMenuBar(m_menuBarwgt);

    //创建菜单（用addMenu方法添加入菜单栏）
    filename = menuBar->addMenu(tr("设置与选项"));

    //创建菜单项
    setnumchannels = new QAction(tr("通道数量设置"));
    commonsetdlg = new QAction();
    commonsetdlg->setText(tr("通道参数设置"));
    testlineset = new QAction(tr("测试线信息设置"));
    //给菜单项添入图标
    setnumchannels->setIcon(QIcon(":/D:/image/Luffy.png"));
    commonsetdlg->setIcon(QIcon(":/D:/image/LuffyQ.png"));

    //用addAction加入菜单项
    filename->addAction(commonsetdlg);
    //添加菜单项之间的分割线
    filename->addSeparator();
    filename->addAction(setnumchannels);
    filename->addSeparator();
    filename->addAction(testlineset);


    connect(commonsetdlg, SIGNAL(triggered()), this, SLOT(CommonSetting()));
    connect(setnumchannels, SIGNAL(triggered()), this, SLOT(MainSetting()));



    //工单
//    menuBar->addSeparator();
//    QMenu* workorder = menuBar->addMenu(QStringLiteral("工单"));
//    workorder->addSeparator();

    //金版校准 GOLDENSAMPLE：
    menuBar->addSeparator();
    Goldplatecalibration = menuBar->addMenu(tr("金板校准"));

    //通道，校准1通道线损
    for(int i=0;i<4;i++)
    {
        QString tmpstr;
        tmpstr=QString("校准%1通道线损").arg(i+1);
        QAction* setchannelLineLoss;
        if(0==i)
        {
            setchannelLineLoss = new QAction(tr("校准1通道线损"));
        }
        else if(1==i)
        {
            setchannelLineLoss = new QAction(tr("校准2通道线损"));
        }
        else if(2==i)
        {
            setchannelLineLoss = new QAction(tr("校准3通道线损"));
        }
        else if(3==i)
        {
            setchannelLineLoss = new QAction(tr("校准4通道线损"));
        }

        setchannelLineLoss->setObjectName(QString::number(i));
        m_channelLineLosslist.append(setchannelLineLoss);

        connect(setchannelLineLoss, SIGNAL(triggered()), this, SLOT(ChannelLineLoss()));

        Goldplatecalibration->addSeparator();
        Goldplatecalibration->addAction(setchannelLineLoss);

    }
    //

    //统计复位
    menuBar->addSeparator();
    StatisticalReset = menuBar->addMenu(tr("统计复位"));
    actSResetdlg = new QAction(tr("激活通道复位"));
    //用addAction加入菜单项
    StatisticalReset->addAction(actSResetdlg);
    connect(actSResetdlg, SIGNAL(triggered()), this, SLOT(ActiveStaticReset()));

    //半自动夹具测试
//    menuBar->addSeparator();
//    QMenu* Sautofixture = menuBar->addMenu(QStringLiteral("半自动夹具设置"));
//    Sautofixture->addSeparator();

//    //权限切换
//    menuBar->addSeparator();
//    QMenu* Permswitch = menuBar->addMenu(QStringLiteral("权限切换"));
//    Permswitch->addSeparator();

//    //在线升级
//    menuBar->addSeparator();
//    QMenu* onlineup = menuBar->addMenu(QStringLiteral("在线升级"));
//    onlineup->addSeparator();

    //语言
    menuBar->addSeparator();
    QMenu* language = menuBar->addMenu(QStringLiteral("语言Language"));
    chinaesedlg = new QAction(tr("中文"));
    language->addAction(chinaesedlg);
    englishdlg = new QAction(tr("英文"));
    language->addAction(englishdlg);
    connect(chinaesedlg, SIGNAL(triggered()), this, SLOT(LanguageForChinese()));
    connect(englishdlg, SIGNAL(triggered()), this, SLOT(LanguageForEnglish()));

    //help
    menuBar->addSeparator();
    QMenu* help = menuBar->addMenu(QStringLiteral("Help"));
    QAction* aboutTools = new QAction(QStringLiteral("About This Tool"));

    help->addAction(aboutTools);

    connect(aboutTools, SIGNAL(triggered()), this, SLOT(AboutThisToolVer()));
    /*
    */
    //设置颜色
    menuBar->setStyleSheet(
                           //"QMenuBar{background-color: yellow;}"
                           "QMenuBar{background-color: #202020;}"
                           "QMenuBar::item{color: #FFFFFF;font-weight: 400;width: 85px;height: 13px;font: 12px \"Microsoft YaHei UI\";}"
                           "QMenuBar::item:pressed{ background-color:#252E39;color: #FFFFFF;}"
                           "QMenuBar::item:selected{ background-color:#252E39;color: #FFFFFF;}"
                           "QMenu{background: #27e29e4d;border-radius: 2.78px 2.78px 2.78px 2.78px;color: #FFFFFF;}"
                           );


    QString tmpstylesheet = menuBar->styleSheet();
    qDebug()<<tmpstylesheet;

//    menuBar->setMaximumWidth(675);
//    menuBar->setMinimumWidth(675);

}

void ATEUIMainWindow::SetMainWigdetUI()
{
    //设置菜单栏widget
    m_menuBarwgt  = new QWidget(this);
    m_menuBarwgt->move(0,6);
    m_menuBarwgt->setMinimumWidth(530);
    m_menuBarwgt->setMaximumWidth(530);
    m_menuBarwgt->setMinimumHeight(30);
    m_menuBarwgt->setMaximumHeight(30);

    //设置菜单栏
    MenuInit();

    //创建一个lab为了距离使用
//    QLabel *barlab = new QLabel(m_menuBarwgt);
//    barlab->setMinimumWidth(158);
//    barlab->setMaximumWidth(158);

    //创建最小化和关闭按钮
    m_minBtn = new QPushButton(this);
    m_minBtn->setGeometry(888,8,24,24);
    m_minBtn->setStyleSheet("QPushButton{border-image: url(./res/icon_minimize.png);}"
                            "QPushButton:hover{border-image: url(./res/icon_minimize_Hover.png);}"
                            "QPushButton:pressed{border-image: url(./res/icon_minimize_Click.png);}"
                            );

//    //创建一个lab为了距离使用
//    QLabel *minlab = new QLabel(m_menuBarwgt);
//    minlab->setMinimumWidth(20);

    connect(m_minBtn, SIGNAL(clicked()), this, SLOT(MinMainWidget()));

    m_maxBtn = new QPushButton(this);
    m_maxBtn->setGeometry(932,8,24,24);
    m_maxBtn->setStyleSheet("QPushButton{border-image: url(./res/icon_maximize.png);width: 24px;height: 24px;}"
                            );

    connect(m_maxBtn, SIGNAL(clicked()), this, SLOT(MaxMainWidget()));

//    //创建一个lab为了距离使用
//    QLabel *maxlab = new QLabel(m_menuBarwgt);
//    maxlab->setMinimumWidth(20);

    m_closeBtn = new QPushButton(this);
    m_closeBtn->setGeometry(976,8,24,24);
    m_closeBtn->setStyleSheet("QPushButton{border-image: url(./res/icon_exit.png);width: 24px;height: 24px;}"
                              "QPushButton:hover{border-image: url(./res/icon_exit_hover.png);}"
                              "QPushButton:pressed{border-image: url(./res/icon_exit_click.png);}"
                              );

    connect(m_closeBtn, SIGNAL(clicked()), this, SLOT(CloseMainWidget()));
//    //close bar
//    QLabel *closelab = new QLabel(m_menuBarwgt);
//    closelab->setMinimumWidth(24);

    QGridLayout *menuBarpLayout = new QGridLayout();

    menuBarpLayout->setMenuBar(menuBar);
//    menuBarpLayout->addWidget(menuBar,0,0);
//    menuBarpLayout->addWidget(barlab,0,1);
//    menuBarpLayout->addWidget(m_minBtn,0,2);
//    menuBarpLayout->addWidget(minlab,0,3);
//    menuBarpLayout->addWidget(m_maxBtn,0,4);
//    menuBarpLayout->addWidget(maxlab,0,5);
//    menuBarpLayout->addWidget(m_closeBtn,0,6);
//    menuBarpLayout->addWidget(closelab,0,7);
//    menuBarpLayout->setContentsMargins(0,0,0,0);
//    menuBarpLayout->setSpacing(0);
    m_menuBarwgt->setLayout(menuBarpLayout);

    //设置背景色为#090C13
    m_menuBarwgt->setStyleSheet("QWidget{background-color:#202020;}");

}

void ATEUIMainWindow::SetPublicWgtUI()
{
    m_publicWgt = new QWidget(RFPageWidget);

    m_publicWgt->move(8,0);
    m_publicWgt->setMinimumWidth(236);
    m_publicWgt->setMinimumHeight(719);

    //设置背景色#5E4100 #0e1016
    if("true"== m_IS_ECN)
    {
        m_publicWgt->setStyleSheet("QWidget{background-color:#5E4100;}");
    }
    else
    {
        m_publicWgt->setStyleSheet("QWidget{background-color:#0e1016;}");
    }


    //lab ate logo
    QLabel *logolab = new QLabel(m_publicWgt);
    logolab->move(0,0);
    logolab->setMinimumWidth(236);
    logolab->setMinimumHeight(63);
    logolab->setStyleSheet("QLabel{border-image: url(./res/Ate_Logo.png);}");

    //
    m_showVersionlab = new QLabel(m_publicWgt);
    m_showVersionlab->move(85,45);
    m_showVersionlab->setText(QApplication::applicationVersion());
    m_showVersionlab->setStyleSheet("QLabel{width: 42px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");


    //Station lab
    QLabel *Stationlab = new QLabel(m_publicWgt);
    Stationlab->move(13,112);
    Stationlab->setText("Station");
    Stationlab->setStyleSheet("QLabel{width: 42px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");

    //show Station
    showStationlab = new QLabel(m_publicWgt);
    showStationlab->move(11,135);
    showStationlab->setText(m_StationName.trimmed());
    showStationlab->setStyleSheet("QLabel{font-weight: 300;width: 236px;font-weight: 700;height: 33px;font: 32px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

    //一条线
    QLineEdit  *StationLine = new QLineEdit(m_publicWgt);
    StationLine->move(8,194);
    StationLine->setStyleSheet("QLineEdit{width: 220px;height: 2px;border-radius: 1px 1px 1px 1px;background: #212121;}");

    //MO lab
    QLabel *MOlab = new QLabel(m_publicWgt);
    MOlab->move(13,217);
    MOlab->setText("MO");
    MOlab->setStyleSheet("QLabel{font-weight: 300;width: 23px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");

    //show MO
    showMOlab = new QLabel(m_publicWgt);
    showMOlab->move(11,237);
    showMOlab->setText(m_MoName);
    showMOlab->setStyleSheet("QLabel{font-weight: 700;width: 236px;font-weight: 700;height: 14px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

    //一条线
    QLineEdit  *MOLine = new QLineEdit(m_publicWgt);
    MOLine->move(8,272);
    MOLine->setStyleSheet("QLineEdit{width: 220px;height: 2px;border-radius: 1px 1px 1px 1px;background: #212121;}");

    //OC lab
    QLabel *OClab = new QLabel(m_publicWgt);
    OClab->move(13,296);
    OClab->setText("OC");
    OClab->setStyleSheet("QLabel{font-weight: 300;width: 23px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");

    //show OC
    showOClab = new QLabel(m_publicWgt);
    showOClab->move(11,316);
    //showOClab->setText("EG800QEULC-N03-SNNSA");
    showOClab->setStyleSheet("QLabel{font-weight: 700;width: 236px;font-weight: 700;height: 15px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

    //一条线
    QLineEdit  *OCLine = new QLineEdit(m_publicWgt);
    OCLine->move(8,351);
    OCLine->setStyleSheet("QLineEdit{width: 220px;height: 2px;border-radius: 1px 1px 1px 1px;background: #212121;}");

    //Line lab
    QLabel *Linelab = new QLabel(m_publicWgt);
    Linelab->move(13,375);
    Linelab->setText("Line");
    Linelab->setStyleSheet("QLabel{font-weight: 300;width: 23px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");

    //show line lab
    showLinelab = new QLabel(m_publicWgt);
    showLinelab->move(11,395);
    showLinelab->setText("B0902");
    showLinelab->setStyleSheet("QLabel{font-weight: 700;width: 236px;font-weight: 700;height: 15px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

    //一条线
    QLineEdit  *LineLine = new QLineEdit(m_publicWgt);
    LineLine->move(8,430);
    LineLine->setStyleSheet("QLineEdit{width: 220px;height: 2px;border-radius: 1px 1px 1px 1px;background: #212121;}");

    //Operator lab
    QLabel *Operatorlab = new QLabel(m_publicWgt);
    Operatorlab->move(13,454);
    Operatorlab->setText("Operator");
    Operatorlab->setStyleSheet("QLabel{font-weight: 300;width: 23px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");

    //show Operator lab
    showOperatorlab = new QLabel(m_publicWgt);
    showOperatorlab->move(11,474);
    showOperatorlab->setText("Vicky.Lin");
    showOperatorlab->setStyleSheet("QLabel{font-weight: 700;width: 236px;height: 17px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

    //一条线
    QLineEdit  *OperatorLine = new QLineEdit(m_publicWgt);
    OperatorLine->move(8,511);
    OperatorLine->setStyleSheet("QLineEdit{width: 220px;height: 2px;border-radius: 1px 1px 1px 1px;background: #212121;}");

    //test mode lab
    QLabel *TestModelab = new QLabel(m_publicWgt);
    TestModelab->move(13,513);
    TestModelab->setText("Test Process");
    TestModelab->setStyleSheet("QLabel{font-weight: 300;width: 23px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");


    //show Test Mode lab
//    showTestModelab = new QLabel(m_publicWgt);
//    showTestModelab->move(11,553);
//    showTestModelab->setText(m_Test_Process.trimmed());
//    showTestModelab->setStyleSheet("QLabel{font-weight: 700;width: 236px;height: 17px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

    showtestModeEdit = new QTextEdit(m_publicWgt);
    showtestModeEdit->move(11,533);
    showtestModeEdit->setText(m_Test_Process.trimmed());
    showtestModeEdit->setFixedSize(230, 60);
    showtestModeEdit->setStyleSheet("QTextEdit{font-weight: 300;font: 14px \"Microsoft YaHei UI\";color: #FFFFFF;border: 0px solid #808080;}");

    showtestModeEdit->verticalScrollBar()->setStyleSheet(
                                                         "QScrollBar:vertical{background:#0E1016;width:6px;padding-top:0px;padding-bottom:0px;padding-left:0px;padding-right:0px;}"    //左/右预留位置
                                                         "QScrollBar::handle:vertical{background:#909090;border-radius:2px;min-height:32px;}"    //滑块最小高度
                                                         "QScrollBar::handle:vertical:hover{background:#383838;}"//鼠标触及滑块样式/滑块颜色
                                                         "QScrollBar::add-line:vertical{background:url(./res/down_arrow.png) center no-repeat;}"//向下箭头样式
                                                         "QScrollBar::sub-line:vertical{background:url(./res/up_arrow.png) center no-repeat;}"//向上箭头样式
                                                         "QScrollBar::add-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在垂直区域
                                                         "QScrollBar::sub-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在区域
                                                         );
    // 设置为只读模式
    showtestModeEdit->setReadOnly(true);
    // 允许选择和复制
    showtestModeEdit->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    //下拉按钮优化

    //一条线
    QLineEdit  *TestModeLine = new QLineEdit(m_publicWgt);
    TestModeLine->move(8,591);
    TestModeLine->setStyleSheet("QLineEdit{width: 220px;height: 2px;border-radius: 1px 1px 1px 1px;background: #212121;}");

    //OFFLine Mode label
    m_OfflineModelabel = new QLabel(m_publicWgt);
    m_OfflineModelabel->move(13,595);
    m_OfflineModelabel->setText("Offline Mode");
    m_OfflineModelabel->setStyleSheet("QLabel{font-weight: 300;width: 23px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");

    //show OFFLine Mode label
    m_showOfflineModelabel = new QLabel(m_publicWgt);
    m_showOfflineModelabel->move(13,615);
    m_showOfflineModelabel->setText("TRUE");
    m_showOfflineModelabel->setStyleSheet("QLabel{font-weight: 700;width: 236px;height: 17px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

    //DownLoadScript
    m_DLScriptlabel = new QLabel(m_publicWgt);
    m_DLScriptlabel->move(13,635);
    m_DLScriptlabel->setText("DownLoadScript");
    m_DLScriptlabel->setStyleSheet("QLabel{font-weight: 300;width: 23px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");

    m_showDLScriptlabel = new QLabel(m_publicWgt);
    m_showDLScriptlabel->move(13,655);
    m_showDLScriptlabel->setText("FALSE");
    m_showDLScriptlabel->setStyleSheet("QLabel{font-weight: 700;width: 236px;height: 17px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

    //DownLoadFunctionDll
    m_DLFunctionDll = new QLabel(m_publicWgt);
    m_DLFunctionDll->move(13,675);
    m_DLFunctionDll->setText("DownLoadFunctionDll");
    m_DLFunctionDll->setStyleSheet("QLabel{font-weight: 300;width: 23px;height: 11px;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;}");

    m_showDLFunctionDll = new QLabel(m_publicWgt);
    m_showDLFunctionDll->move(13,695);
    m_showDLFunctionDll->setText("FALSE");
    m_showDLFunctionDll->setStyleSheet("QLabel{font-weight: 700;width: 236px;height: 17px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");


}

void ATEUIMainWindow::SetDownLoadWgtUI()
{
    //生成一个共通的窗口，生成32个不同的类型的控制，根据不同的通道显示效果
    m_dlcommlab = new QLabel(DownLoadPageWidget);
    m_dlcommlab->move(8,0);
    m_dlcommlab->setMinimumWidth(764);
    m_dlcommlab->setMinimumHeight(48);
    m_dlcommlab->setStyleSheet("QLabel{border-radius: 4px 4px 4px 4px;background: #090C13;}");

    //QGMR lab
    QLabel *QGMRlab = new QLabel(m_dlcommlab);
    QGMRlab->move(16,16);
    QGMRlab->setText("QGMR:");
    QGMRlab->setStyleSheet("QLabel{font-weight: 700;height: 16px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;background: #090c13;}");

    QGMRSRCEdit =new QLineEdit(m_dlcommlab);
    QGMRSRCEdit->move(94,16);

    //QGMRSRCEdit->setText(m_logonDlg->m_DownloadFW);
    //QGMRSRCEdit->setText("D:\\quectel\\Software_package\\RG500LEUACR04A03M8G_OCPU_30.200.30.200_V06\\EG800GEULCR01A04M04 01.001.01.001");
    QGMRSRCEdit->setStyleSheet("QLineEdit{font-weight: 700;width: 620px;height: 18px;font: 16px \"Microsoft YaHei UI\";text-align: left;color: #00FEA6;border: 1px solid #090c13;background: #090c13;}");

    //设置一个滚动条区域的大小

    DLscrollAreawgt = new QWidget();
    //scrollAreawgt->move(0,0);
//    scrollAreawgt->setMinimumWidth(664);
    DLscrollAreawgt->setMinimumHeight(564); //这个地方动态设置，如果大于4，则动态变大，如果小于等于4则不变
    DLscrollAreawgt->setStyleSheet("QWidget{background-color:#090c13;}");



    m_dlscrollArea = new QScrollArea(DownLoadPageWidget);
    m_dlscrollArea->setWidgetResizable(true);
    m_dlscrollArea->setMinimumWidth(765);
    m_dlscrollArea->setMinimumHeight(664);
    m_dlscrollArea->setWidget(DLscrollAreawgt);
    m_dlscrollArea->move(8,56);
    m_dlscrollArea->setFrameShape(QFrame::NoFrame); //去边框
    m_dlscrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_dlscrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_dlscrollArea->setStyleSheet(
        "QScrollBar:vertical{background-color:#090c13;width:8px;padding-top:16px;padding-bottom:8px;padding-left:2px;padding-right:2px;}"    //左/右预留位置
        "QScrollBar::handle:vertical{background:#383838;border-radius:2px;min-height:520px;}"    //滑块最小高度
        "QScrollBar::handle:vertical:hover{background:#383838;}"//鼠标触及滑块样式/滑块颜色
        "QScrollBar::add-line:vertical{background:url(./res/down_arrow.png) center no-repeat;}"//向下箭头样式
        "QScrollBar::sub-line:vertical{background:url(./res/up_arrow.png) center no-repeat;}"//向上箭头样式
        "QScrollBar::add-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在垂直区域
        "QScrollBar::sub-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在区域
        );  //很难也很重要



    for(int i=0;i<32;i++)
    {
        //生成groupbox
        QGroupBox *tpgroupBx = new QGroupBox(DLscrollAreawgt);
        m_DLThreadGrouplist.append(tpgroupBx);
        //生成按钮
        QPushButton *tpbtn = new QPushButton(tpgroupBx);
        tpbtn->setText("Start");
        m_DL_StartBtnlist.append(tpbtn);

        //设置信号和槽
        connect(tpbtn, SIGNAL(clicked()), this, SLOT(DL_Btn_Start()));

        //生成编辑框 输入二维码
        QLineEdit *tplineedit = new QLineEdit(tpgroupBx);
        m_DLQRLineeditlist.append(tplineedit);

        //信号与槽
        connect(tplineedit, SIGNAL(returnPressed()), this, SLOT(on_QRLineedit_returnPressed()));

        //进度条
        QProgressBar *tpprogressbar = new QProgressBar(tpgroupBx);
        m_DLProgBarlist.append(tpprogressbar);


        //状态lab
        QLabel *tplabel = new QLabel(tpgroupBx);
        m_DLStatuslablist.append(tplabel);


        //显示百分比lab
        QLabel *tppercentlab = new QLabel(tpgroupBx);
        tppercentlab->setText("0%");
        m_DLPercentlablist.append(tppercentlab);

        //三角形
        TriangleWidget *tpTrianglelab = new TriangleWidget(tpgroupBx);
        m_DLTrianglelablist.append(tpTrianglelab);

        //显示channel num lab
        QLabel *tpchannumlab = new QLabel(tpTrianglelab);
        m_DLChanNumlist.append(tpchannumlab);

        //IMEI tip lab
        QLabel *tptipIMEIlab = new QLabel(tpgroupBx);
        m_DLTipIMEIlist.append(tptipIMEIlab);

        //show  IMEI lab
        QLabel *tpshowIMEIlab = new QLabel(tpgroupBx);
        m_DLShowIMEIlist.append(tpshowIMEIlab);

        //Fix No
        QLabel *tptipFixNolab = new QLabel(tpgroupBx);
        m_DLTipFixNolist.append(tptipFixNolab);

        //show Fix No
        QLabel *tpshowFixNolab = new QLabel(tpgroupBx);
        m_DLShowFixNolist.append(tpshowFixNolab);

        // pass num
        QLabel *tptippassnumlab = new QLabel(tpgroupBx);
        m_DLTippassnumlablist.append(tptippassnumlab);

        //show pass num
        QLabel *tpshowpassnumlab = new QLabel(tpgroupBx);
        m_DLShowpassnumlablist.append(tpshowpassnumlab);

        //fail num
        QLabel *tptipfailnumlab = new QLabel(tpgroupBx);
        m_DLTipfailnumlablist.append(tptipfailnumlab);

        //show fail num
        QLabel *tpshowfailnumlab = new QLabel(tpgroupBx);
        m_DLShowfailnumlablist.append(tpshowfailnumlab);

        //testtime
        QLabel *tptiptestimelab = new QLabel(tpgroupBx);
        m_DLTiptesttimelablist.append(tptiptestimelab);

        //show testtime
        QLabel *tpshowtestimelab = new QLabel(tpgroupBx);
        m_DLShowtesttimelablist.append(tpshowtestimelab);

        //test No.
        QLabel *tptiptestNolab = new QLabel(tpgroupBx);
        m_DLTiptestNolablist.append(tptiptestNolab);

        //show Test No.
        QLabel *tpshowtestNolab = new QLabel(tpgroupBx);
        m_DLShowtestNolablist.append(tpshowtestNolab);

        //Rate
        QLabel *tptiprateNolab = new QLabel(tpgroupBx);
        m_DLTipRatelablist.append(tptiprateNolab);

        //show Rate
        QLabel *tpshowrateNolab = new QLabel(tpgroupBx);
        m_DLShowRatelablist.append(tpshowrateNolab);

        //lineedit
        QTextEdit *tptextEdit = new QTextEdit(tpgroupBx);
        tptextEdit->setFocusPolicy(Qt::NoFocus); // 控件不会获取焦点
        m_DLTextLogEditlist.append(tptextEdit);

    }

    //设置滚动条
}

void ATEUIMainWindow::SetTestMainWgtUI()
{
    //一个tab控件，包含32个控件
    m_TestRfTabWgt = new QTabWidget(TestPageWidget);
    m_TestRfTabWgt->setGeometry(8,8,756,664+48);

    m_Testchannelwgt = new QWidget(TestPageWidget);
    m_TestRfTabWgt->addTab(m_Testchannelwgt,"Channel");

    //background:#0E1116;
    m_TestRfTabWgt->setStyleSheet(
                                  "QTabWidget::pane{border:0px;border-radius: 3px;background:#0E1116;margin-top:-1px;}"
                                  "QTabBar::tab{width:150px;height:40;background: #2A2A2A;font: 14px \"Microsoft YaHei UI\";border:0px solid;color: #FFFFFF;}"
                                  "QTabBar::tab:selected{background: #0E1116;color: #00FEA6 ;}"
                                   "");


    //生产4个控件

    for(int i=0;i<TEST_USE_CHANNEL_NUM;i++)
    {
        //先生成groupbox
        //生成groupbox
        QGroupBox *tpgroupBx = new QGroupBox(m_Testchannelwgt);
        m_TestThreadGrouplist.append(tpgroupBx);

        //生成log界面
        QWidget  *log1wgt = new QWidget(TestPageWidget);
        m_TestShowLogWgtlist.append(log1wgt);
        m_TestRfTabWgt->addTab(log1wgt,"Log"+QString::number(i+1));

        //生成
        QTableWidget *tptablewgt = new QTableWidget(log1wgt);
        m_TestLogTableWgtlist.append(tptablewgt);

        //Textedit
        QTextEdit *tptextEdit = new QTextEdit(log1wgt);
        m_TestTextLogEditlist.append(tptextEdit);

        //
        //生成按钮
        QPushButton *tpbtn = new QPushButton(tpgroupBx);
        tpbtn->setText("Start");
        m_Test_StartBtnlist.append(tpbtn);

        //设置信号和槽
        //connect(tpbtn, SIGNAL(clicked()), this, SLOT(Test_Btn_Start()));
        // Qt5风格（推荐）
        connect(tpbtn, &QPushButton::clicked, this, &ATEUIMainWindow::Test_Btn_Start);

        //生成编辑框 输入二维码
        QLineEdit *tplineedit = new QLineEdit(tpgroupBx);
        m_TestQRLineeditlist.append(tplineedit);

        //信号与槽
        connect(tplineedit, SIGNAL(returnPressed()), this, SLOT(on_QRLineedit_returnPressed()));

        //状态lab
        QLabel *tplabel = new QLabel(tpgroupBx);
        m_TestStatuslablist.append(tplabel);

        //三角形
        TriangleWidget *tpTrianglelab = new TriangleWidget(tpgroupBx);
        m_TestTrianglelablist.append(tpTrianglelab);

        //显示channel num lab
        QLabel *tpchannumlab = new QLabel(tpTrianglelab);
        m_TestChanNumlist.append(tpchannumlab);

        //IMEI tip lab
        QLabel *tptipIMEIlab = new QLabel(tpgroupBx);
        m_TestTipIMEIlist.append(tptipIMEIlab);

        //show  IMEI lab
        QLabel *tpshowIMEIlab = new QLabel(tpgroupBx);
        m_TestShowIMEIlist.append(tpshowIMEIlab);

        //Fix No
        QLabel *tptipFixNolab = new QLabel(tpgroupBx);
        m_TestTipFixNolist.append(tptipFixNolab);

        //show Fix No
        QLabel *tpshowFixNolab = new QLabel(tpgroupBx);
        m_TestShowFixNolist.append(tpshowFixNolab);

        // pass num
        QLabel *tptippassnumlab = new QLabel(tpgroupBx);
        m_TestTippassnumlablist.append(tptippassnumlab);

        //show pass num
        QLabel *tpshowpassnumlab = new QLabel(tpgroupBx);
        m_TestShowpassnumlablist.append(tpshowpassnumlab);

        //fail num
        QLabel *tptipfailnumlab = new QLabel(tpgroupBx);
        m_TestTipfailnumlablist.append(tptipfailnumlab);

        //show fail num
        QLabel *tpshowfailnumlab = new QLabel(tpgroupBx);
        m_TestShowfailnumlablist.append(tpshowfailnumlab);

        //testtime
        QLabel *tptiptestimelab = new QLabel(tpgroupBx);
        m_TestTiptesttimelablist.append(tptiptestimelab);

        //show testtime
        QLabel *tpshowtestimelab = new QLabel(tpgroupBx);
        m_TestShowtesttimelablist.append(tpshowtestimelab);

        //test No.
        QLabel *tptiptestNolab = new QLabel(tpgroupBx);
        m_TestTiptestNolablist.append(tptiptestNolab);

        //show Test No.
        QLabel *tpshowtestNolab = new QLabel(tpgroupBx);
        m_TestShowtestNolablist.append(tpshowtestNolab);

        //Rate
        QLabel *tptiprateNolab = new QLabel(tpgroupBx);
        m_TestTipRatelablist.append(tptiprateNolab);

        //show Rate
        QLabel *tpshowrateNolab = new QLabel(tpgroupBx);
        m_TestShowRatelablist.append(tpshowrateNolab);

        //error code
        QLabel *tpshowerrorlab = new QLabel(tpgroupBx);
        m_TestShowErrorcodelist.append(tpshowerrorlab);

    }


}

void ATEUIMainWindow::SetCfgRFWgtUI()
{
    m_pcfgRFStackedWidget = new QStackedWidget(this);
    m_pcfgRFStackedWidget->move(0,41);

    m_pcfgRFStackedWidget->setMinimumWidth(1024);
    m_pcfgRFStackedWidget->setMinimumHeight(728);

    //
    CfgPageWidget = new QWidget(this);
    RFPageWidget  = new QWidget(this);

    m_pcfgRFStackedWidget->addWidget(RFPageWidget);
    m_pcfgRFStackedWidget->addWidget(CfgPageWidget);

    m_pcfgRFStackedWidget->setCurrentIndex(0);  //默认RF界面

}

void ATEUIMainWindow::SetCfgPageWidgetUI()
{
    //whole label

    m_cfgwholelabel = new QLabel(CfgPageWidget);
    m_cfgwholelabel->move(12,0);
    m_cfgwholelabel->setMinimumWidth(993);
//    wholelabel->setMaximumWidth(993);
    m_cfgwholelabel->setMinimumHeight(708);
//    wholelabel->setMaximumHeight(708);
    m_cfgwholelabel->setStyleSheet("QLabel{background-color: #0e1116;}");


    //Common Setting label
    m_cfgcomsetlabel = new QLabel(m_cfgwholelabel);
    m_cfgcomsetlabel->setText("Common Setting");
    m_cfgcomsetlabel->setStyleSheet("QLabel{font: 16px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");
    m_cfgcomsetlabel->move(834,20);

    //back button
    commonbackBtn = new QPushButton(m_cfgwholelabel);
    commonbackBtn->setText("back");
    commonbackBtn->move(24,15);
    commonbackBtn->setStyleSheet("QPushButton{background: #0e1116;color: #76797C;font: 16px \"Microsoft YaHei UI\";}"
                                 "QPushButton:hover{color:rgba(255,255,255,1.0);}"
                                 "QPushButton:pressed{color:rgba(255,255,255,0.5);}");
    QIcon icon("./res/icon_back.png");
    commonbackBtn->setIcon(icon);
    connect(commonbackBtn, SIGNAL(clicked()), this, SLOT(CommsetBackBtnClicked()));



    //一条线
    m_cfgStationLine = new QLineEdit(m_cfgwholelabel);
    m_cfgStationLine->move(0,56);
    m_cfgStationLine->setStyleSheet("QLineEdit{width: 993;height: 2px;border-radius: 1px 1px 1px 1px;background: #27e29e;}");

    //
    m_CommsetTree =new QTreeWidget(m_cfgwholelabel);
    m_CommsetTree->move(7,65);
    m_CommsetTree->setMinimumWidth(236);
    m_CommsetTree->setMinimumHeight(643);

    m_CommsetTree->setColumnCount(1);//设置树结构列的数量
    m_CommsetTree->setHeaderLabel(QStringLiteral("Common Setting"));//设置这一列的
    m_CommsetTree->header()->setDefaultAlignment(Qt::AlignLeft); // 设置头部默认居中
    m_CommsetTree->header()->hide();
    m_CommsetTree->header()->setMinimumHeight(30);//设置表头高度


    QIcon headericon;
    headericon.addPixmap(QPixmap("./res/icon_setting.png"));
    m_CommsetTree->headerItem()->setIcon(0,headericon);

    //信号和槽
    connect(m_CommsetTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(CommsetTreesClicked(QTreeWidgetItem*,int)));
    connect(m_CommsetTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(CommsetTreesItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

    SetCommsetTreeItem();

    //QTableWight
    m_CommsetTableWgt = new QTableWidget(m_cfgwholelabel);

    m_CommsetTableWgt->setGeometry(273,84,710,530);
    //设置表格中每一行的表头
    m_CommsetTableWgt->setColumnCount(3);
    m_CommsetTableWgt->setHorizontalHeaderLabels(QStringList() << "Parameter Name" << "Value" << "Description");

    m_CommsetTableWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_CommsetTableWgt->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    m_CommsetTableWgt->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_CommsetTableWgt->setColumnWidth(0, 230);
    m_CommsetTableWgt->setColumnWidth(1, 230);


    //去掉行号
    m_CommsetTableWgt->verticalHeader()->setHidden(true);

    //去掉网格线
    m_CommsetTableWgt->setShowGrid(false);

    //颜色交叉进行
    //m_CommsetTableWgt ->setAlternatingRowColors(true);

    m_CommsetTableWgt->horizontalHeader()->setStyleSheet("QHeaderView::section,QTableCornerButton::section{background:#2a2a2a;color: #909090;font-weight: 700;font: 16px \"Microsoft YaHei UI\";}");
    m_CommsetTableWgt->setStyleSheet("QTableWidget{background: #0E1116;border: 0px solid;color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";}"
                                               //"QTableView::item:alternate:!selected, QTableWidget::item:alternate:!selected, QListView::item:alternate:!selected { background: #2a2a2a; }"
                                               //"QTableView::item:!alternate:!selected, QTableWidget::item:!alternate:!selected, QListView::item:!alternate:!selected { background: #2a2a2a; }"
                                               "QTableView::item {border-bottom: 2px solid #0E1116;background: #2a2a2a;}"
                                               );

    //取出滚动条，然后再设置
    m_CommsetTableWgt->verticalScrollBar()->setStyleSheet( "QScrollBar:vertical{background:#2B2B2B;width:6px;padding-top:0px;padding-bottom:0px;padding-left:0px;padding-right:0px;}"    //左/右预留位置
                                                                     "QScrollBar::handle:vertical{background:#383838;border-radius:2px;min-height:96px;}"    //滑块最小高度
                                                                     "QScrollBar::handle:vertical:hover{background:#383838;}"//鼠标触及滑块样式/滑块颜色
                                                                     "QScrollBar::add-line:vertical{background:url(./res/down_arrow.png) center no-repeat;}"//向下箭头样式
                                                                     "QScrollBar::sub-line:vertical{background:url(./res/up_arrow.png) center no-repeat;}"//向上箭头样式
                                                                     "QScrollBar::add-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在垂直区域
                                                                     "QScrollBar::sub-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在区域
                                                                      );


    //去掉行号
    m_CommsetTableWgt->verticalHeader()->setHidden(true);

    //save
    m_CommsetSaveBtn = new QPushButton(m_cfgwholelabel);
    m_CommsetSaveBtn->setText("Save");
    m_CommsetSaveBtn->setGeometry(480,645,136,40);
    m_CommsetSaveBtn->setStyleSheet("QPushButton{color: #27E29E;width: 80px;height: 30px;border: 1px solid #27E29E;font-weight: 400;background: rgba(15,80,64,1.0);border-radius: 4px 4px 4px 4px;font: 18px \"Microsoft YaHei UI\";}"
                                    "QPushButton:hover{background: rgba(15,80,64,0.6);}"
                                    "QPushButton:pressed{background: rgba(15,80,64,0.2);}"
                                    );

    connect(m_CommsetSaveBtn, SIGNAL(clicked()), this, SLOT(CommsetSaveBtnClicked()));

    //cancel
    m_CommsetCancelBtn = new QPushButton(m_cfgwholelabel);
    m_CommsetCancelBtn->setText("Back");
    m_CommsetCancelBtn->setGeometry(480+136+20,645,136,40);
    m_CommsetCancelBtn->setStyleSheet("QPushButton{color: #FFFFFF;width: 80px;height: 30px;font-weight: 400;background: #0e1116;border-radius: 4px 4px 4px 4px;font: 18px \"Microsoft YaHei UI\";border: 1px solid #FFFFFF;}");
    connect(m_CommsetCancelBtn, SIGNAL(clicked()), this, SLOT(CommsetBackBtnClicked()));

}

void ATEUIMainWindow::SetChannelInterfaceUI()
{

    m_pStackedWidget = new QStackedWidget(RFPageWidget);

    m_pStackedWidget->move(245,0);
    m_pStackedWidget->setMinimumWidth(764+8+8);
    m_pStackedWidget->setMinimumHeight(664+48+8);

    //设置主界面
    TestPageWidget         = new QWidget(this);
    DownLoadPageWidget     = new QWidget(this);

    m_pStackedWidget->addWidget(TestPageWidget);
    m_pStackedWidget->addWidget(DownLoadPageWidget);

    //判断选择是下载类还是测试类
    if(m_current_station==TEST_STATION_DL)
    {
        m_pStackedWidget->setCurrentIndex(1); //下载类
    }
    else
    {
         m_pStackedWidget->setCurrentIndex(0);  //测试类
    }


    //设置背景色
    //TestPageWidget->setStyleSheet("QWidget{background-color:#2A2A2A;}");
}

void ATEUIMainWindow::ChooseDLWgtUI(int channelnum)
{
    //根据通道数，选择下拉框是否出现
    if(channelnum<4)
    {
        DLscrollAreawgt->setMinimumHeight(663);
    }
    else if(channelnum==4)
    {
        DLscrollAreawgt->setMinimumHeight(665);
    }
    else
    {
        DLscrollAreawgt->setMinimumHeight(665+168*(channelnum-4));
    }

    //设计不同channel的界面
    if(channelnum==1)
    {

    }
    else if(channelnum==2)
    {

    }
    else if(channelnum==3)
    {

    }
    else if(channelnum>=4)
    {
        //
        for(int i=0;i<32;i++)
        {
            //groupbox的大小和样式
            m_DLThreadGrouplist.at(i)->setMinimumWidth(754);
            //m_DLThreadGrouplist.at(i)->setMaximumWidth(754);
            m_DLThreadGrouplist.at(i)->setMinimumHeight(160);
            //m_DLThreadGrouplist.at(i)->setMaximumHeight(160);


            m_DLThreadGrouplist.at(i)->move(0,i*160+i*8);
            m_DLThreadGrouplist.at(i)->setStyleSheet("QGroupBox{border-radius: 4px;border: 1px solid #909090;}");



            //button在哪里
            m_DL_StartBtnlist.at(i)->move(412,10);
            m_DL_StartBtnlist.at(i)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(144,144,144,1.0);border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                   "QPushButton:hover{background: rgba(144,144,144,0.5);}"
                                                   "QPushButton:pressed{background: rgba(144,144,144,0.3);}"
                                                   );

            //lineedit
            m_DLQRLineeditlist.at(i)->move(60,10);
            m_DLQRLineeditlist.at(i)->setStyleSheet("QLineEdit{width: 345px;height: 30px;border-radius: 4px 4px 4px 4px;background: #252e39;font-weight: 400;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

            //Qprogressbar
            m_DLProgBarlist.at(i)->move(60,50);
            m_DLProgBarlist.at(i)->setStyleSheet("QProgressBar{font-weight: 700;width: 435px;height: 32px;border-radius: 4px 4px 4px 4px;background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);text-align: center;color: #0E1016;font: 14px \"Microsoft YaHei UI\";}"
                                                 "QProgressBar::chunk{background-color: #E91737;width: 10px;margin: 0.5px;text-align: center;border-radius:5px;}"
                                                 );
            m_DLProgBarlist.at(i)->setTextVisible(false);
            m_DLProgBarlist.at(i)->setValue(0);


            //状态
            m_DLStatuslablist.at(i)->setMinimumWidth(74);
            m_DLStatuslablist.at(i)->setMinimumHeight(32);
            m_DLStatuslablist.at(i)->move(680,1);
            m_DLStatuslablist.at(i)->setText("IDLE");
            m_DLStatuslablist.at(i)->setAlignment(Qt::AlignCenter);
            m_DLStatuslablist.at(i)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#909090;text-align: center;}");

            //显示百分比lab
            m_DLPercentlablist.at(i)->setMinimumWidth(45);
            m_DLPercentlablist.at(i)->setMinimumHeight(32);
            m_DLPercentlablist.at(i)->move(15,50);
            m_DLPercentlablist.at(i)->setText("0%");
            m_DLPercentlablist.at(i)->setAlignment(Qt::AlignCenter);
            m_DLPercentlablist.at(i)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color: #FFFFFF;background-color: transparent;}");

            //三角形
            m_DLTrianglelablist.at(i)->setMinimumWidth(40);
            m_DLTrianglelablist.at(i)->setMaximumWidth(40);
            m_DLTrianglelablist.at(i)->setMinimumHeight(40);
            m_DLTrianglelablist.at(i)->setMaximumHeight(40);
            m_DLTrianglelablist.at(i)->move(2,2);

            //三角形里面的数字
            m_DLChanNumlist.at(i)->setText(QString::number(i+1));
            m_DLChanNumlist.at(i)->move(1,1);
            m_DLChanNumlist.at(i)->setMinimumWidth(18);
            m_DLChanNumlist.at(i)->setAlignment(Qt::AlignCenter);
            m_DLChanNumlist.at(i)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #909090;}");

            //IMEI Tip lab
            m_DLTipIMEIlist.at(i)->setText("IMEI");
            m_DLTipIMEIlist.at(i)->move(17,87);
            m_DLTipIMEIlist.at(i)->setMinimumWidth(45);
            m_DLTipIMEIlist.at(i)->setAlignment(Qt::AlignRight);
            m_DLTipIMEIlist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;background-color: transparent;}");

            //show IMEI lab
            //m_DLShowIMEIlist.at(i)->setText("8600000054186021234567");
            m_DLShowIMEIlist.at(i)->move(75,87);
            m_DLShowIMEIlist.at(i)->setMinimumWidth(180);
            m_DLShowIMEIlist.at(i)->setAlignment(Qt::AlignLeft);
            m_DLShowIMEIlist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 18px \"Microsoft YaHei UI\";color: #FFFFFF;background-color: transparent;}");


            //Fix No lab
            m_DLTipFixNolist.at(i)->setText("Fix No.");
            m_DLTipFixNolist.at(i)->move(17,110);
            m_DLTipFixNolist.at(i)->setMinimumWidth(45);
            m_DLTipFixNolist.at(i)->setAlignment(Qt::AlignRight);
            m_DLTipFixNolist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;background-color: transparent;}");

            //show Fix No lab
            //m_DLShowFixNolist.at(i)->setText("EG800Q-PT001");
            m_DLShowFixNolist.at(i)->move(75,110);
            m_DLShowFixNolist.at(i)->setMinimumWidth(150);
            m_DLShowFixNolist.at(i)->setAlignment(Qt::AlignLeft);
            m_DLShowFixNolist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 18px \"Microsoft YaHei UI\";color: #FFFFFF;background-color: transparent;}");


            //pass
            m_DLTippassnumlablist.at(i)->setText("PASS");
            m_DLTippassnumlablist.at(i)->move(17,132);
            m_DLTippassnumlablist.at(i)->setMinimumWidth(45);
            m_DLTippassnumlablist.at(i)->setAlignment(Qt::AlignRight);
            m_DLTippassnumlablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;background-color: transparent;}");

            //show pass
            //m_DLShowpassnumlablist.at(i)->setText("233");
            m_DLShowpassnumlablist.at(i)->move(75,132);
            m_DLShowpassnumlablist.at(i)->setMinimumWidth(50);
            m_DLShowpassnumlablist.at(i)->setAlignment(Qt::AlignLeft);
            m_DLShowpassnumlablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 18px \"Microsoft YaHei UI\";color: #FFFFFF;background-color: transparent;}");


            //fail
            m_DLTipfailnumlablist.at(i)->setText("FAIL");
            m_DLTipfailnumlablist.at(i)->move(133,132);
            m_DLTipfailnumlablist.at(i)->setMinimumWidth(45);
            m_DLTipfailnumlablist.at(i)->setAlignment(Qt::AlignRight);
            m_DLTipfailnumlablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;background-color: transparent;}");


            //show fail
            //m_DLShowfailnumlablist.at(i)->setText("100");
            m_DLShowfailnumlablist.at(i)->move(190,132);
            m_DLShowfailnumlablist.at(i)->setMinimumWidth(50);
            m_DLShowfailnumlablist.at(i)->setAlignment(Qt::AlignLeft);
            m_DLShowfailnumlablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 18px \"Microsoft YaHei UI\";color: #FFFFFF;background-color: transparent;}");



            //test time
            m_DLTiptesttimelablist.at(i)->setText("Test Time");
            m_DLTiptesttimelablist.at(i)->move(330,87);
            m_DLTiptesttimelablist.at(i)->setMinimumWidth(65);
            m_DLTiptesttimelablist.at(i)->setAlignment(Qt::AlignRight);
            m_DLTiptesttimelablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;background-color: transparent;}");

            //show  test time
            //m_DLShowtesttimelablist.at(i)->setText("0");
            m_DLShowtesttimelablist.at(i)->move(410,87);
            m_DLShowtesttimelablist.at(i)->setMinimumWidth(100);
            m_DLShowtesttimelablist.at(i)->setAlignment(Qt::AlignLeft);
            m_DLShowtesttimelablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 18px \"Microsoft YaHei UI\";color: #FFFFFF;background-color: transparent;}");

            // test TsNum.
            m_DLTiptestNolablist.at(i)->setText("Test No.");
            m_DLTiptestNolablist.at(i)->move(330,110);
            m_DLTiptestNolablist.at(i)->setMinimumWidth(65);
            m_DLTiptestNolablist.at(i)->setAlignment(Qt::AlignRight);
            m_DLTiptestNolablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;background-color: transparent;}");

            //show Test Num.
           // m_DLShowtestNolablist.at(i)->setText("TS1");
            m_DLShowtestNolablist.at(i)->move(410,110);
            m_DLShowtestNolablist.at(i)->setMinimumWidth(100);
            m_DLShowtestNolablist.at(i)->setAlignment(Qt::AlignLeft);
            m_DLShowtestNolablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 18px \"Microsoft YaHei UI\";color: #FFFFFF;background-color: transparent;}");

            //循环读取值，显示在界面上

            //rate
            m_DLTipRatelablist.at(i)->setText("Rate");
            m_DLTipRatelablist.at(i)->move(330,132);
            m_DLTipRatelablist.at(i)->setMinimumWidth(65);
            m_DLTipRatelablist.at(i)->setAlignment(Qt::AlignRight);
            m_DLTipRatelablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;background-color: transparent;}");

            // show Rate
            //m_DLShowRatelablist.at(i)->setText("100.00%");
            m_DLShowRatelablist.at(i)->move(410,132);
            m_DLShowRatelablist.at(i)->setMinimumWidth(100);
            m_DLShowRatelablist.at(i)->setAlignment(Qt::AlignLeft);
            m_DLShowRatelablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 18px \"Microsoft YaHei UI\";color: #FFFFFF;background-color: transparent;}");

            //logshow QTextEdit
            m_DLTextLogEditlist.at(i)->setGeometry(505, 45, 240, 100);
            m_DLTextLogEditlist.at(i)->setStyleSheet("QTextEdit{font-weight: 400;font: 12px \"Microsoft YaHei UI\";color: #FFFFFF;background: #252E39;border: 0.5px solid #98989A;}");
        }
    }
    else if(channelnum>8)
    {

    }
    else
    {
        return;
    }

    //大于4的都隐藏
    for(int i=channelnum;i<32;i++)
    {
        m_DLThreadGrouplist.at(i)->setHidden(true);
        m_DL_StartBtnlist.at(i)->setHidden(true);
    }

    //显示出来
    for(int i=0;i<channelnum;i++)
    {
        m_DLThreadGrouplist.at(i)->setHidden(false);
        m_DL_StartBtnlist.at(i)->setHidden(false);
    }



}

void ATEUIMainWindow::ChooseTestWgtUI(int channelnum)
{
    //设计不同channel的界面
    if(channelnum==1)
    {

    }
    else if(channelnum==2)
    {

    }
    else if(channelnum==3)
    {

    }
    else if(channelnum==4)
    {
        //
        for(int i=0;i<channelnum;i++)
        {
            //groupbox的大小和样式
            m_TestThreadGrouplist.at(i)->setMinimumWidth(725);
            m_TestThreadGrouplist.at(i)->setMinimumHeight(155);
            //m_TestThreadGrouplist.at(i)->setMaximumHeight(155);


            m_TestThreadGrouplist.at(i)->move(16,i*155+i*8+16);
            m_TestThreadGrouplist.at(i)->setStyleSheet("QGroupBox{border-radius: 4px;border: 1px solid #909090;}");

            //
            //button在哪里
            m_Test_StartBtnlist.at(i)->move(555,15);
            m_Test_StartBtnlist.at(i)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(144,144,144,1.0);;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                     "QPushButton:hover{background: rgba(144,144,144,0.5);}"
                                                     "QPushButton:pressed{background: rgba(144,144,144,0.3);}"
                                                     );

            //lineedit
            m_TestQRLineeditlist.at(i)->move(40,15);
            m_TestQRLineeditlist.at(i)->setStyleSheet("QLineEdit{width: 510px;height: 30px;border-radius: 4px 4px 4px 4px;background: #252e39;font-weight: 400;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");

            //状态
            m_TestStatuslablist.at(i)->setMinimumWidth(73);
            m_TestStatuslablist.at(i)->setMinimumHeight(45);
            m_TestStatuslablist.at(i)->move(651,1);
            m_TestStatuslablist.at(i)->setText("IDLE");
            m_TestStatuslablist.at(i)->setAlignment(Qt::AlignCenter);
            m_TestStatuslablist.at(i)->setStyleSheet("QLabel{font-weight: 700;font: 12px \"Microsoft YaHei UI\";color:#0E1016;background-color:#909090;text-align: center;}");

            //三角形
            m_TestTrianglelablist.at(i)->setMinimumWidth(40);
            m_TestTrianglelablist.at(i)->setMaximumWidth(40);
            m_TestTrianglelablist.at(i)->setMinimumHeight(40);
            m_TestTrianglelablist.at(i)->setMaximumHeight(40);
            m_TestTrianglelablist.at(i)->move(2,2);

            //三角形里面的数字
            m_TestChanNumlist.at(i)->setText(QString::number(i+1));
            m_TestChanNumlist.at(i)->move(1,1);
            m_TestChanNumlist.at(i)->setMinimumWidth(18);
            m_TestChanNumlist.at(i)->setAlignment(Qt::AlignCenter);
            m_TestChanNumlist.at(i)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #909090;}");

            //IMEI Tip lab
            m_TestTipIMEIlist.at(i)->setText("IMEI");
            m_TestTipIMEIlist.at(i)->move(32,57);
            m_TestTipIMEIlist.at(i)->setMinimumWidth(45);
            m_TestTipIMEIlist.at(i)->setAlignment(Qt::AlignRight);
            m_TestTipIMEIlist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;}");

            //show IMEI lab
            m_TestShowIMEIlist.at(i)->setText("");
            m_TestShowIMEIlist.at(i)->move(95,57);
            m_TestShowIMEIlist.at(i)->setMinimumWidth(180);
            m_TestShowIMEIlist.at(i)->setAlignment(Qt::AlignLeft);
            m_TestShowIMEIlist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;}");

            //Fix No lab
            m_TestTipFixNolist.at(i)->setText("Fix No.");
            m_TestTipFixNolist.at(i)->move(32,83);
            m_TestTipFixNolist.at(i)->setMinimumWidth(45);
            m_TestTipFixNolist.at(i)->setAlignment(Qt::AlignRight);
            m_TestTipFixNolist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;}");

            //show Fix No lab
            m_TestShowFixNolist.at(i)->setText("");
            m_TestShowFixNolist.at(i)->move(95,83);
            m_TestShowFixNolist.at(i)->setMinimumWidth(150);
            m_TestShowFixNolist.at(i)->setAlignment(Qt::AlignLeft);
            m_TestShowFixNolist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;}");


            //pass
            m_TestTippassnumlablist.at(i)->setText("PASS");
            m_TestTippassnumlablist.at(i)->move(32,110);
            m_TestTippassnumlablist.at(i)->setMinimumWidth(45);
            m_TestTippassnumlablist.at(i)->setAlignment(Qt::AlignRight);
            m_TestTippassnumlablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;}");

            //show pass
            m_TestShowpassnumlablist.at(i)->setText("233");
            m_TestShowpassnumlablist.at(i)->move(95,110);
            m_TestShowpassnumlablist.at(i)->setMinimumWidth(50);
            m_TestShowpassnumlablist.at(i)->setAlignment(Qt::AlignLeft);
            m_TestShowpassnumlablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;}");


            //fail
            m_TestTipfailnumlablist.at(i)->setText("FAIL");
            m_TestTipfailnumlablist.at(i)->move(150,110);
            m_TestTipfailnumlablist.at(i)->setMinimumWidth(45);
            m_TestTipfailnumlablist.at(i)->setAlignment(Qt::AlignRight);
            m_TestTipfailnumlablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;}");


            //show fail
            m_TestShowfailnumlablist.at(i)->setText("100");
            m_TestShowfailnumlablist.at(i)->move(205,110);
            m_TestShowfailnumlablist.at(i)->setMinimumWidth(50);
            m_TestShowfailnumlablist.at(i)->setAlignment(Qt::AlignLeft);
            m_TestShowfailnumlablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;}");


            //test time
            m_TestTiptesttimelablist.at(i)->setText("Test Time");
            m_TestTiptesttimelablist.at(i)->move(405,57);
            m_TestTiptesttimelablist.at(i)->setMinimumWidth(67);
            m_TestTiptesttimelablist.at(i)->setAlignment(Qt::AlignRight);
            m_TestTiptesttimelablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;}");

            //show  test time
            //m_TestShowtesttimelablist.at(i)->setText("00:48:32");
            m_TestShowtesttimelablist.at(i)->move(485,57);
            m_TestShowtesttimelablist.at(i)->setMinimumWidth(100);
            m_TestShowtesttimelablist.at(i)->setAlignment(Qt::AlignLeft);
            m_TestShowtesttimelablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;}");

            // test No.
            m_TestTiptestNolablist.at(i)->setText("Test No.");
            m_TestTiptestNolablist.at(i)->move(405,83);
            m_TestTiptestNolablist.at(i)->setMinimumWidth(67);
            m_TestTiptestNolablist.at(i)->setAlignment(Qt::AlignRight);
            m_TestTiptestNolablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;}");

            //show test No.
            m_TestShowtestNolablist.at(i)->setText("");
            m_TestShowtestNolablist.at(i)->move(485,83);
            m_TestShowtestNolablist.at(i)->setMinimumWidth(100);
            m_TestShowtestNolablist.at(i)->setAlignment(Qt::AlignLeft);
            m_TestShowtestNolablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;}");


            //rate
            m_TestTipRatelablist.at(i)->setText("Pass Rate");
            m_TestTipRatelablist.at(i)->move(403,110);
            m_TestTipRatelablist.at(i)->setMinimumWidth(67);
            m_TestTipRatelablist.at(i)->setAlignment(Qt::AlignRight);
            m_TestTipRatelablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: #909090 ;}");

            // show Rate
            m_TestShowRatelablist.at(i)->setText("");
            m_TestShowRatelablist.at(i)->move(485,110);
            m_TestShowRatelablist.at(i)->setMinimumWidth(100);
            m_TestShowRatelablist.at(i)->setAlignment(Qt::AlignLeft);
            m_TestShowRatelablist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;}");

            //errorcode
            //m_TestShowErrorcodelist.at(i)->setText("ErrorCode1234567891234567891234500");
            m_TestShowErrorcodelist.at(i)->move(42,135);
            m_TestShowErrorcodelist.at(i)->setMinimumWidth(600);
            m_TestShowErrorcodelist.at(i)->setAlignment(Qt::AlignLeft);
            m_TestShowErrorcodelist.at(i)->setStyleSheet("QLabel{font-weight: 400;font: 14px \"Microsoft YaHei UI\";color: red ;}");

            //
            m_TestLogTableWgtlist.at(i)->setGeometry(16,20,720,392);
            //设置表格中每一行的表头
            m_TestLogTableWgtlist.at(i)->setColumnCount(7);
            m_TestLogTableWgtlist.at(i)->setHorizontalHeaderLabels(QStringList() << "ID" << "Test Item" << "Low"<<"High"<<"Data"<<"Result"<<"Unit");

            m_TestLogTableWgtlist.at(i)->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            m_TestLogTableWgtlist.at(i)->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
            m_TestLogTableWgtlist.at(i)->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
            m_TestLogTableWgtlist.at(i)->setColumnWidth(0, 60);
            m_TestLogTableWgtlist.at(i)->setColumnWidth(1, 320);
            m_TestLogTableWgtlist.at(i)->setColumnWidth(6, 10);

            //去掉行号
            m_TestLogTableWgtlist.at(i)->verticalHeader()->setHidden(true);

            //去掉网格线
            m_TestLogTableWgtlist.at(i)->setShowGrid(false);

            //颜色交叉进行
            //m_TestLogTableWgtlist.at(i) ->setAlternatingRowColors(true);

            m_TestLogTableWgtlist.at(i)->horizontalHeader()->setStyleSheet("QHeaderView::section,QTableCornerButton::section{background:#2a2a2a;color: #909090;font-weight: 700;font: 16px \"Microsoft YaHei UI\";}");
            m_TestLogTableWgtlist.at(i)->setStyleSheet("QTableWidget{background: #0E1116;border: 0px solid;color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";}"
                                                       //"QTableView::item:alternate:!selected, QTableWidget::item:alternate:!selected, QListView::item:alternate:!selected { background: #2a2a2a; }"
                                                       //"QTableView::item:!alternate:!selected, QTableWidget::item:!alternate:!selected, QListView::item:!alternate:!selected { background: #2a2a2a; }"
                                                       "QTableView::item {border-bottom: 2px solid #0E1116;background: #2a2a2a;}"
                                                       );

            //取出滚动条，然后再设置
            m_TestLogTableWgtlist.at(i)->verticalScrollBar()->setStyleSheet( "QScrollBar:vertical{background:#2B2B2B;width:6px;padding-top:0px;padding-bottom:0px;padding-left:0px;padding-right:0px;}"    //左/右预留位置
                                                                             "QScrollBar::handle:vertical{background:#383838;border-radius:2px;min-height:96px;}"    //滑块最小高度
                                                                             "QScrollBar::handle:vertical:hover{background:#383838;}"//鼠标触及滑块样式/滑块颜色
                                                                             "QScrollBar::add-line:vertical{background:url(./res/down_arrow.png) center no-repeat;}"//向下箭头样式
                                                                             "QScrollBar::sub-line:vertical{background:url(./res/up_arrow.png) center no-repeat;}"//向上箭头样式
                                                                             "QScrollBar::add-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在垂直区域
                                                                             "QScrollBar::sub-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在区域
                                                                              );


            //去掉行号
            m_TestLogTableWgtlist.at(i)->verticalHeader()->setHidden(true);

            //

            //设置值
//            for (int j=0; j<100; j++)
//            {
//                m_TestLogTableWgtlist.at(i)->insertRow(j);
//                m_TestLogTableWgtlist.at(i)->setItem(j,0,new QTableWidgetItem(QString::number(j+10000)));
//                m_TestLogTableWgtlist.at(i)->setItem(j,1,new QTableWidgetItem("Hello QTableWidget!"));
//                m_TestLogTableWgtlist.at(i)->setItem(j,2,new QTableWidgetItem("0"));
//                m_TestLogTableWgtlist.at(i)->setItem(j,3,new QTableWidgetItem("1000"));
//                m_TestLogTableWgtlist.at(i)->setItem(j,4,new QTableWidgetItem("50"));
//                m_TestLogTableWgtlist.at(i)->setItem(j,5,new QTableWidgetItem("PASS"));
//                m_TestLogTableWgtlist.at(i)->item(j,5)->setTextColor(QColor(Qt::red));
//                m_TestLogTableWgtlist.at(i)->setItem(j,6,new QTableWidgetItem("OK"));

//            }

            //logshow QTextEdit
            m_TestTextLogEditlist.at(i)->setGeometry(16, 430, 720, 224);
//            m_TestTextLogEditlist.at(i)->setText("2023-11-8_9:24:3urrent:3.000000\n"
//                                                          "2023-11-8_9:24:33\n <--> MANUAL OP");
            m_TestTextLogEditlist.at(i)->setStyleSheet("QTextEdit{font-weight: 400;font: 16px \"Microsoft YaHei UI\";color: #FFFFFF;background: #252e39;border-radius: 4px 4px 4px 4px;}"
                                                       //"QTextEdit QScrollBar:vertical{width: 6px;background:red;border: none;}"
                                                       );

            m_TestTextLogEditlist.at(i)->verticalScrollBar()->setStyleSheet(
                                                                             "QScrollBar:vertical{background:#0E1016;width:6px;padding-top:0px;padding-bottom:0px;padding-left:0px;padding-right:0px;}"    //左/右预留位置
                                                                             "QScrollBar::handle:vertical{background:#909090;border-radius:2px;min-height:32px;}"    //滑块最小高度
                                                                             "QScrollBar::handle:vertical:hover{background:#383838;}"//鼠标触及滑块样式/滑块颜色
                                                                             "QScrollBar::add-line:vertical{background:url(./res/down_arrow.png) center no-repeat;}"//向下箭头样式
                                                                             "QScrollBar::sub-line:vertical{background:url(./res/up_arrow.png) center no-repeat;}"//向上箭头样式
                                                                             "QScrollBar::add-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在垂直区域
                                                                             "QScrollBar::sub-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在区域
                                                                            );
        }
    }
    else if(channelnum>4)
    {

    }
    else
    {
        return;
    }

    //大于4的都隐藏
    for(int i=channelnum;i<4;i++)
    {
         m_TestThreadGrouplist.at(i)->setHidden(true);
    }

}

void ATEUIMainWindow::CloseMainWidget()
{
    //close时候调用一下
    Q_ATE_CONTROLLER_ReleaseAll();
    exit(0);
}

void ATEUIMainWindow::MinMainWidget()
{
    this->showMinimized();
}

void ATEUIMainWindow::MaxMainWidget()
{
   // this->showMaximized();
    if(m_Max_Normal_Flag==false)
    {
        this->showNormal();
        m_Max_Normal_Flag=true;
    }
    else
    {
        this->showMaximized();
        m_Max_Normal_Flag =false;
    }

}


void ATEUIMainWindow::CommonSetting()
{
    qDebug("CommonSetting");
    m_pcfgRFStackedWidget->setCurrentIndex(1);  //配置界面

    //设置当前的current Tree
    m_topLevelItem=m_CommsetTree->topLevelItem(0);
    if(nullptr!=m_topLevelItem)
    {
        m_CommsetTree->setCurrentItem(m_topLevelItem);
    }


}

void ATEUIMainWindow::SetCommsetTreeItem()
{

    disconnect(m_CommsetTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(CommsetTreesClicked(QTreeWidgetItem*,int)));
    disconnect(m_CommsetTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(CommsetTreesItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

    // 获取根节点
    QTreeWidgetItem *root = m_CommsetTree->invisibleRootItem();
    // 遍历删除所有节点
    while (root->childCount() > 0) {
        QTreeWidgetItem *item = root->takeChild(0);  // 获取第一个子节点
        delete item;  // 删除子节点
    }

    for(int i=0;i<g_iActiveChannelNums+1;i++)
    {
        QString str;

        if(i==0)
        {
             str = QString("Common Setting").arg(i);
        }
        else
        {
             str = QString("Test Channel %1").arg(i);
        }


        QStringList strlist;
        strlist.append(str);
        QTreeWidgetItem *province = new QTreeWidgetItem(m_CommsetTree,strlist);

        QIcon icon1;
        icon1.addPixmap(QPixmap("./res/icon_channel.png"));
        province->setIcon(0,icon1);
    }

    m_CommsetTree->expandAll();//全部展开

    //设置背景色
    m_CommsetTree->setStyleSheet("QTreeView{border:none; outline:0px; background: #000000;show-decoration-selected: 1;color:#FFFFFF;font-size: 14px;height: 56px;}"
                                 "QTreeView::item{height: 60px;}");

    m_CommsetTree->header()->setStyleSheet("QHeaderView::section { background-color: #20AA7A;font-size: 14px;color: #FFFFFF;}");

    m_CommsetTree->setCurrentIndex(m_CommsetTree->rootIndex());

    connect(m_CommsetTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(CommsetTreesClicked(QTreeWidgetItem*,int)));
    connect(m_CommsetTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(CommsetTreesItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

}

void ATEUIMainWindow::ActiveStaticReset()
{

    //判断是DL还是Test
    //if(m_current_station==TEST_STATION_DL)
    {
        //
        for(int i=0;i<g_iActiveChannelNums;i++)
        {
            //情况 fail ，Pass，rate的值
            Clear_Statistics_Data(i);

        }
    }
}

void ATEUIMainWindow::MainSetting()
{
    //如果是测试界面，则不能修改
    if(m_current_station!=TEST_STATION_DL)
    {
        return;
    }

    //创建遮罩窗口
//    QWidget* mask = new QWidget();
//    mask->setWindowFlag(Qt::FramelessWindowHint);
//    mask->setStyleSheet("{background:rgba(0,0,0,0)}");
//    mask->setGeometry(rect());
//    mask->show();

    MyChannelNumDlg *setchannlenum = new MyChannelNumDlg(this);
    connect(setchannlenum,SIGNAL(sendData(QString)),this,SLOT(updatechannelnumData(QString)));
    if(setchannlenum->exec()==QDialog::Accepted)
    {
    }
    delete setchannlenum;

}


void ATEUIMainWindow::ChannelLineLoss()
{

    qInfo()<<"Channel1LineLoss() Start";
    //判断，如果是DL，直接返回
    if(m_current_station==TEST_STATION_DL)
    {
        qDebug()<<"Station is TEST_STATION_DL";
        return;
    }

    //
    QObject* button = QObject::sender();

    qInfo()<<"channelLineLoss QAction"<<button->objectName();

    int Ts_num= button->objectName().toInt();
    //判断是否running状态
    if(TS_OBJ_RUN == this->m_irTS_DL_OBJ_State[Ts_num])
    {
        qDebug()<<"OBJ_State[ is TS_OBJ_RUN";
        return;
    }

    //判断是否激活状态
    if((Ts_num+1)>g_iActiveChannelNums)
    {
        qDebug()<<"ActiveChannelNums < Ts_num";
        return;
    }

    //判断是否是在线的，如果是在线的，需要弹框输入密码
    if(m_Offline_Status=="false")
    {
        m_query_current_Value =false; //初始化值
        //弹框输入密码12345678
        m_inputmess_box.showInputMessBox("","Password");
        m_inputmess_box.exec();

        //判断是否是点击确认按钮
        if(true == m_query_current_Value)
        {
            //是确认按钮，则需要跟12345678进行比较是否正确
            QString tmpstr; //取得输入框的值
            tmpstr = m_inputmess_box.m_inputLine_edit->text().trimmed();

            if(L"12345678"==tmpstr)
            {

            }
            else
            {
                //打印输入密码错误
                m_TestTextLogEditlist.at(Ts_num)->append("Input Pwd Error!!");
                return;
            }
        }
        else //不是确认按钮，则直接返回
        {
            m_TestTextLogEditlist.at(Ts_num)->append("Cancel LineLoss!!");
            //打印取消做金板
            return;
        }


    }

    //调用Test start button
    m_TestQRLineeditlist.at(Ts_num)->setText("GOLDENSAMPLE:");
    m_Test_StartBtnlist.at(Ts_num)->clicked(true);

    qInfo()<<"Channel1LineLoss() End";
}

void ATEUIMainWindow::on_QRLineedit_returnPressed()
{
    QLineEdit *obj=(QLineEdit*)sender();

    //先判断是DL还是Test

    if(TEST_STATION_DL == m_current_station)
    {
        //判断是哪个lineedit
        int i=0;
        for(i=0;i<MAX_CHANNEL_NUM;i++)
        {
            if(obj == m_DLQRLineeditlist.at(i))
            {
                qInfo() << "m_DLQRLineeditlist= " << i;
                break;
            }
        }

        if(i==MAX_CHANNEL_NUM)
        {
             qDebug() << "not m_DLQRLineeditlist  ";
            return;
        }

        //根据发送信号
        m_DL_StartBtnlist.at(i)->click();

        if(m_iOP_Multi_Option != LOAD_AUTO_ROBOT)
        {

            for(int i=0;i<g_iActiveChannelNums;i++)
            {
                qInfo()<<"m_irTS_DL_OBJ_State"<<TS_OBJ_RUN<<"i"<<i;
                if(this->m_irTS_DL_OBJ_State[i]!=TS_OBJ_RUN)
                {
                    m_DLQRLineeditlist.at(i)->setFocus();
                    break;
                }
            }
        }
    }
    else
    {
        //判断是哪个lineedit
        int i=0;
        for(i=0;i<MAX_CHANNEL_NUM;i++)
        {
            if(obj == m_TestQRLineeditlist.at(i))
            {
                qInfo() << "m_TestQRLineeditlist= " << i;
                break;
            }
        }

        if(i==MAX_CHANNEL_NUM)
        {
             qDebug() << "not m_TestQRLineeditlist  ";
            return;
        }

        //根据发送信号
        m_Test_StartBtnlist.at(i)->click();

        //判断是否是自动线

        if(m_iOP_Multi_Option != LOAD_AUTO_ROBOT)
        {

            for(int i=0;i<g_iActiveChannelNums;i++)
            {
                qInfo()<<"m_irTS_DL_OBJ_State"<<TS_OBJ_RUN<<"i"<<i;
                if(this->m_irTS_DL_OBJ_State[i]!=TS_OBJ_RUN)
                {
                    m_TestQRLineeditlist.at(i)->setFocus();
                    break;
                }
            }
        }
    }
}

void ATEUIMainWindow::AboutThisToolVer()
{
    //弹框显示
    MyToolVersion mytoolver(this,m_testscript_version,m_function_dll_version);
    if(mytoolver.exec()==QDialog::Accepted)
    {
        //

    }
}

void ATEUIMainWindow::LanguageForTest()
{
    qDebug()<<"LanguageForTest() Start";

    //
    //richedit 句柄不能给自己发消息
    int ret = ::SendMessage((HWND)this->winId(), WM_GET_LAN_BARCODE_MSG, (WPARAM)"869206050002735;D1Q21KD12000026", 0 );

    qDebug()<<"ret"<<ret;

    qDebug()<<"LanguageForTest() End";
}

void ATEUIMainWindow::LanguageForChinese()
{
    bool  bload=false;
    qInfo()<<"LanguageForChinese() Start";

    QDir dir = QDir::current();
    QString relativePath = "/translations/chinese.qm";
    QString absolutePath = QCoreApplication::applicationDirPath()+relativePath;
   // translator.load(absolutePath);
   qInfo()<<"absolutePath=="<<absolutePath;
    bload=mTranslator.load(absolutePath);
    if(bload)
    {
        qInfo()<<"bload  OK";
    }
    else
    {
        qInfo()<<"bload  Fail";
    }
    qApp->installTranslator(&mTranslator); //再加载完翻译文件之后更新文本信息
}

void ATEUIMainWindow::LanguageForEnglish()
{
     bool  bload=false;
     qInfo()<<"LanguageForEnglish() Start";



     QDir dir = QDir::current();
     QString relativePath = "/translations/english.qm";
     QString absolutePath = QCoreApplication::applicationDirPath()+relativePath;
    // translator.load(absolutePath);
    qInfo()<<"absolutePath=="<<absolutePath;
     bload=mTranslator.load(absolutePath);
     if(bload)
     {
         qInfo()<<"bload  OK";
     }
     else
     {
         qInfo()<<"bload  Fail";
     }
     //下面这条语句会触发语言改变事件，调用FunSetText()函数
     qApp->installTranslator(&mTranslator); //再加载完翻译文件之后更新文本信息
}

//调用installTranslator后，系统会给窗体发送信号将产生了changeEvent槽产生event
void ATEUIMainWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            RetranslateUI();
            break;
        default:
            break;
    }
}

void ATEUIMainWindow::RetranslateUI()
{
    qInfo()<<"RetranslateUI() Start";
    commonsetdlg->setText(tr("通道参数设置"));
    setnumchannels->setText(tr("通道数量设置"));
    testlineset->setText(tr("测试线信息设置"));
    filename->setTitle(tr("设置与选项"));
    Goldplatecalibration->setTitle(tr("金版校准"));
    m_channelLineLosslist.at(0)->setText(tr("校准1通道线损"));
    m_channelLineLosslist.at(1)->setText(tr("校准2通道线损"));
    m_channelLineLosslist.at(2)->setText(tr("校准3通道线损"));
    m_channelLineLosslist.at(3)->setText(tr("校准4通道线损"));

    StatisticalReset->setTitle(tr("统计复位"));
    actSResetdlg->setText(tr("激活通道复位"));

    chinaesedlg->setText(tr("中文"));
    englishdlg->setText(tr("英文"));
}

void ATEUIMainWindow::CommsetTreesClicked(QTreeWidgetItem* item,int index)
{
//    qDebug("CommsetTreesClicked begin");
//    qDebug()<<item->text(0);
    (void) *item;

    Q_UNUSED(index);
}

void ATEUIMainWindow::CommsetTreesItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    //
    Q_UNUSED(previous);
    //首先清除common table 里面所有的内容 contents可以删除
    m_CommsetTableWgt->clearContents();

    while (m_CommsetTableWgt->rowCount() > 0) {

        //先删除表格里面的控件，在删除表格内容
        m_CommsetTableWgt->removeRow(0);
    }

    //
    int tmpTHIRDPARTY_NUM=0;
    for(int i=0;i<MAX_THIRDPARTY_NUM;i++)
    {
        thirdpartybtn[i]=nullptr;
    }
    m_thirdpartyIndexNum=0;
    //选择的index来显示值
    int index= m_CommsetTree->indexOfTopLevelItem(current);
    qDebug()<<"index"<<index;

    m_currentTreeIndex = index;

    if(index==0)
    {
        //显示commset的值
        for (int j=0; j<pCommonScript->GetCommonParaCount(); j++)
        {
            {
                m_CommsetTableWgt->insertRow(j);
                m_CommsetTableWgt->setItem(j,0,new QTableWidgetItem(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaName(j))));
                m_CommsetTableWgt->item(j,0)->setTextAlignment(Qt::AlignCenter);
                m_CommsetTableWgt->item(j,0)->setFlags(m_CommsetTableWgt->item(j,0)->flags() & (~Qt::ItemIsEditable));

                if(pCommonScript->GetCommonParaType(j)==1)  //下拉框
                {

                    //下拉框来设置
                    MyComboBox *comboxtype = new MyComboBox(m_CommsetTableWgt);
                    m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)comboxtype);

                    //
                    QStringList stlistcombox;
                    stlistcombox=m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaItem(j)).split(",");
                    //取得值分号结束
                    comboxtype->addItems(stlistcombox);

                    for(int i=0;i<stlistcombox.count();i++)
                    {
                        comboxtype->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
                    }

                    //border: 0px solid gray;
                    comboxtype->setStyleSheet(
                                              "QComboBox{background: rgba(37,46,57,1);color: rgb(255,255,255);font:16px \"Microsoft YaHei UI\";font-weight:400;}"
                                              "QComboBox QAbstractItemView{ background: rgba(37,46,57,1);color: white; text-align: center;}"
                                              "QComboBox QAbstractItemView::item:hover{color: #27E29E;background: rgba(37,46,57,1)}"
                                              "QComboBox QAbstractItemView::item:selected{color: #27E29E;background: rgba(37,46,57,1)} "
                                              );
                    //加载combox的新样式
                     QStyledItemDelegate *Stationdelegate = new QStyledItemDelegate();
                     comboxtype ->setItemDelegate(Stationdelegate);

                     comboxtype->setMaxVisibleItems(20); //先设置20个

                     //设置当前的值
                     comboxtype->setCurrentText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaValue(j)));
                }
                else if(3==pCommonScript->GetCommonParaType(j))  //按钮，调用第三方函数
                {
                    //
                    thirdpartybtn[tmpTHIRDPARTY_NUM] = new QPushButton(m_CommsetTableWgt);
                    m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)thirdpartybtn[tmpTHIRDPARTY_NUM]);
                    thirdpartybtn[tmpTHIRDPARTY_NUM]->setText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaValue(j)));
                    thirdpartybtn[tmpTHIRDPARTY_NUM]->setStyleSheet("QPushButton{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: rgba(37,46,57,1);border-radius: 2.78px 2.78px 2.78px 2.78px;}"
                                               "QPushButton:hover{background: rgba(37,46,57,0.5);}"
                                               "QPushButton:pressed{background: rgba(37,46,57,0.0);}");
                    //参数赋值到全局变量
                    m_thirdpartyTsNum = index;
                    m_thirdpartyName[tmpTHIRDPARTY_NUM]  =  m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaName(j));
                    m_thirdpartyItem[tmpTHIRDPARTY_NUM]  =  m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaItem(j));
                    //信号与槽
                    connect(thirdpartybtn[tmpTHIRDPARTY_NUM], SIGNAL(clicked()), this, SLOT(thirdpartybtnClicked()));
                    tmpTHIRDPARTY_NUM++;
                }
                else
                {
                    QLineEdit   *lineedit = new QLineEdit(m_CommsetTableWgt);
                    m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)lineedit);
                    lineedit->setText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaValue(j)));
                    lineedit->setAlignment(Qt::AlignCenter);
                    lineedit->setStyleSheet("QLineEdit{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: #252e39;border-radius: 2.78px 2.78px 2.78px 2.78px;}");

                }

                m_CommsetTableWgt->setItem(j,2,new QTableWidgetItem(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetCommonParaDes(j))));
                m_CommsetTableWgt->item(j,2)->setTextAlignment(Qt::AlignCenter);
                m_CommsetTableWgt->item(j,2)->setFlags(m_CommsetTableWgt->item(j,2)->flags() & (~Qt::ItemIsEditable));

                m_CommsetTableWgt->setRowHeight(j,50);
            }
        }
    }
    else
    {
        for (int j=0; j<pCommonScript->GetTchParaCount(index); j++)
        {
            m_CommsetTableWgt->insertRow(j);
            m_CommsetTableWgt->setItem(j,0,new QTableWidgetItem(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaName(index,j))));
            m_CommsetTableWgt->item(j,0)->setTextAlignment(Qt::AlignCenter);
            m_CommsetTableWgt->item(j,0)->setFlags(m_CommsetTableWgt->item(j,0)->flags() & (~Qt::ItemIsEditable));


            //先判断类型，根据类型生产不同的控件
            if(pCommonScript->GetTchParaType(index,j)==1)  //下拉框
            {
                //下拉框来设置
                MyComboBox *comboxtype = new MyComboBox(m_CommsetTableWgt);
                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)comboxtype);


                QStringList stlistcombox;
                stlistcombox=m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaItem(index,j)).split(",");
                comboxtype->addItems(stlistcombox);

                for(int i=0;i<stlistcombox.count();i++)
                {
                    comboxtype->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
                }

                //border: 0px solid gray;
                comboxtype->setStyleSheet(
                                          "QComboBox{background: rgba(37,46,57,1);color: rgb(255,255,255);font:16px \"Microsoft YaHei UI\";font-weight:400;}"
                                          "QComboBox QAbstractItemView{background: rgba(37,46,57,1);color: white; text-align: center;}"
                                          "QComboBox QAbstractItemView::item:hover{color: #27E29E;background: rgba(37,46,57,1)}"
                                          "QComboBox QAbstractItemView::item:selected{color: #27E29E;background: rgba(37,46,57,1)} "
                                          );
                //加载combox的新样式
                 QStyledItemDelegate *Stationdelegate = new QStyledItemDelegate();
                 comboxtype ->setItemDelegate(Stationdelegate);

                 //设置当前的值
                 comboxtype->setCurrentText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaValue(index,j)));

            }
            else if(pCommonScript->GetTchParaType(index,j)==2) //按钮 线损
            {
                QPushButton *pathlossbtn = new QPushButton(m_CommsetTableWgt);
                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)pathlossbtn);
                pathlossbtn->setText("....");
                pathlossbtn->setStyleSheet("QPushButton{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: rgba(37,46,57,1);border-radius: 2.78px 2.78px 2.78px 2.78px;}"
                                           "QPushButton:hover{background: rgba(37,46,57,0.5);}"
                                           "QPushButton:pressed{background: rgba(37,46,57,0.0);}");
                //信号与槽
                connect(pathlossbtn, SIGNAL(clicked()), this, SLOT(ShowEidtPathLoss()));
            }
            else if(3==pCommonScript->GetTchParaType(index,j))  //按钮，调用第三方函数
            {
                //
                thirdpartybtn[tmpTHIRDPARTY_NUM] = new QPushButton(m_CommsetTableWgt);
                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)thirdpartybtn[tmpTHIRDPARTY_NUM]);
                thirdpartybtn[tmpTHIRDPARTY_NUM]->setText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaValue(index,j)));
                thirdpartybtn[tmpTHIRDPARTY_NUM]->setStyleSheet("QPushButton{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: rgba(37,46,57,1);border-radius: 2.78px 2.78px 2.78px 2.78px;}"
                                           "QPushButton:hover{background: rgba(37,46,57,0.5);}"
                                           "QPushButton:pressed{background: rgba(37,46,57,0.0);}");
                //参数赋值到全局变量
                m_thirdpartyTsNum = index;
                m_thirdpartyName[tmpTHIRDPARTY_NUM]  =  m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaName(index,j));
                m_thirdpartyItem[tmpTHIRDPARTY_NUM]  =  m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaItem(index,j));
                //信号与槽
                connect(thirdpartybtn[tmpTHIRDPARTY_NUM], SIGNAL(clicked()), this, SLOT(thirdpartybtnClicked()));
                tmpTHIRDPARTY_NUM++;
            }
            else if(4 == pCommonScript->GetTchParaType(index,j))  //按钮，弹框选择文件
            {
                //
                m_filepathbtn = new QPushButton(m_CommsetTableWgt);
                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)m_filepathbtn);
                m_filepathbtn->setText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaValue(index,j)));
                m_filepathbtn->setStyleSheet("QPushButton{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: rgba(37,46,57,1);border-radius: 2.78px 2.78px 2.78px 2.78px;}"
                                           "QPushButton:hover{background: rgba(37,46,57,0.5);}"
                                           "QPushButton:pressed{background: rgba(37,46,57,0.0);}");
                //信号与槽
                connect(m_filepathbtn, SIGNAL(clicked()), this, SLOT(ShowFilePath()));
            }
            else  //文本框
            {
                QLineEdit   *lineedit = new QLineEdit(m_CommsetTableWgt);
                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)lineedit);
                lineedit->setText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaValue(index,j)));
                lineedit->setAlignment(Qt::AlignCenter);
                lineedit->setStyleSheet("QLineEdit{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: #252e39;border-radius: 2.78px 2.78px 2.78px 2.78px;}");

            }

//            //判断是否等于PathLossFile，如果是，则生成显示线损的button
//            if(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaName(index,j))=="PathLossFile")
//            {
//                QPushButton *pathlossbtn = new QPushButton(m_CommsetTableWgt);
//                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)pathlossbtn);
//                pathlossbtn->setText("....");
//                pathlossbtn->setStyleSheet("QPushButton{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: rgba(37,46,57,1);border-radius: 2.78px 2.78px 2.78px 2.78px;}"
//                                           "QPushButton:hover{background: rgba(37,46,57,0.5);}"
//                                           "QPushButton:pressed{background: rgba(37,46,57,0.0);}");
//                //信号与槽
//                connect(pathlossbtn, SIGNAL(clicked()), this, SLOT(ShowEidtPathLoss()));
//            }
//            else if(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaName(index,j))=="Open_Debug_Log")
//            {
//                //下拉框来设置
//                QComboBox *debugcombox = new QComboBox(m_CommsetTableWgt);
//                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)debugcombox);

//                debugcombox->addItem("False",0);
//                debugcombox->addItem("TRUE",1);

//                //border: 0px solid gray;
//                debugcombox->setStyleSheet(
//                                          "QComboBox{padding-left: 50px;background: rgba(37,46,57,1);color: rgb(255,255,255);font:16px \"Microsoft YaHei UI\";font-weight:400;}"
//                                          "QComboBox QAbstractItemView{padding-left: 50px; background: rgba(37,46,57,1);color: white; text-align: center;}"
//                                          "QComboBox QAbstractItemView::item:hover{color: #27E29E;background: rgba(37,46,57,1)}"
//                                          "QComboBox QAbstractItemView::item:selected{color: #27E29E;background: rgba(37,46,57,1)} "
//                                          );
//                //加载combox的新样式
//                 QStyledItemDelegate *Stationdelegate = new QStyledItemDelegate();
//                 debugcombox ->setItemDelegate(Stationdelegate);

//            }
//            else
//            {
//                QLineEdit   *lineedit = new QLineEdit(m_CommsetTableWgt);
//                m_CommsetTableWgt->setCellWidget(j,1,(QWidget*)lineedit);
//                lineedit->setText(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaValue(index,j)));
//                lineedit->setAlignment(Qt::AlignCenter);
//                lineedit->setStyleSheet("QLineEdit{color: #FFFFFF;font: 16px \"Microsoft YaHei UI\";background: #252e39;border-radius: 2.78px 2.78px 2.78px 2.78px;}");

//            }

            m_CommsetTableWgt->setItem(j,2,new QTableWidgetItem(m_cla_ate_gfc.Wchar2QString(pCommonScript->GetTchParaDes(index,j))));
            m_CommsetTableWgt->item(j,2)->setTextAlignment(Qt::AlignCenter);
            m_CommsetTableWgt->item(j,2)->setFlags(m_CommsetTableWgt->item(j,2)->flags() & (~Qt::ItemIsEditable));


            m_CommsetTableWgt->setRowHeight(j,50);
        }
    }

   // m_CommsetTableWgt->scrollToBottom();
}

void ATEUIMainWindow::CommsetSaveBtnClicked()
{
    const wchar_t* wstrname=nullptr;
    const wchar_t* wstValue=nullptr;
    QString     tmpName;
    QString     tmpvalue;
    QLineEdit   *lineedit;
    QComboBox   *Combobox;
    QPushButton *tmpPushBtn;
    QWidget     *tmpwgt;
    //根据index来选择保存
    pCommonScript = (Q_ATE_Controller_CommonScript_Interface*)Q_ATE_CONTROLLER_GetCommonSetting();
    if(pCommonScript!=nullptr)
    {
        //
        for (int j=0; j<m_CommsetTableWgt->rowCount(); j++)
        {
            //取得第一行
          // QString tmpName  = m_CommsetTableWgt->item(j,0)->text();
          // QString tmpvalue = m_CommsetTableWgt->item(j,1)->text();

            tmpName  = m_CommsetTableWgt->item(j,0)->text();

            //判断是否是PathLossFile，如果是，则需要不需要保存
            if(tmpName.trimmed()=="PathLossFile")
            {
                continue;
            }

            tmpwgt =(QWidget*) m_CommsetTableWgt->cellWidget(j,1);

            if(tmpwgt->inherits("QPushButton"))
            {
                if("PathLossFile"==tmpName.trimmed())
                {
                    continue;
                }
                else
                {
                    //
                    tmpPushBtn = (QPushButton*)tmpwgt;
                    tmpvalue = tmpPushBtn->text();
                }
            }
            else if(tmpwgt->inherits("QComboBox"))
            {
                Combobox = (QComboBox*)tmpwgt;
                tmpvalue = Combobox->currentText();
            }
            else
            {
                lineedit = (QLineEdit*)tmpwgt;
                tmpvalue = lineedit->text();
            }


            wstrname =m_cla_ate_gfc.QString2Wchar(tmpName);
            wstValue = m_cla_ate_gfc.QString2Wchar(tmpvalue);

            if(m_currentTreeIndex==0)
            {
                pCommonScript->SetCommonPara(wstrname, wstValue);
            }
            else
            {
                pCommonScript->SetTchPara(m_currentTreeIndex,wstrname,wstValue);
            }

        }

        pCommonScript->Save();

        if(m_currentTreeIndex==0)
        {
            m_message_box.SetShowResult("Success","Save Common Setting Success");
        }
        else
        {
            m_message_box.SetShowResult("Success","Save Test Channel Success");
        }
        m_message_box.exec();

    }
    else
    {

        m_message_box.SetShowResult("Error","Q_ATE_CONTROLLER_GetCommonSetting is NULL");
        m_message_box.exec();
    }


    //重新读取值
    if(m_currentTreeIndex==0)
    {
        wchar_t wcHar[256];
        memset(wcHar,0x00,sizeof(wcHar));
        pCommonScript->GetCommonPara(L"ActivateChannel", wcHar, 256);

        QString str;
        str = m_cla_ate_gfc.Wchar2QString(wcHar);
        qDebug()<<"ActivateChannel num="<<str;

        bool ok;
        g_iActiveChannelNums = str.toInt(&ok,10);
        qDebug()<<"g_iActiveChannelNums ="<<g_iActiveChannelNums;

        if(g_iActiveChannelNums<1 ||g_iActiveChannelNums>32)
        {
            g_iActiveChannelNums=1;
        }

        SetCommsetTreeItem();
    }


    wstrname = nullptr;
    wstValue = nullptr;
    lineedit = nullptr;
    tmpwgt   = nullptr;
}

void ATEUIMainWindow::CommsetBackBtnClicked()
{
    m_pcfgRFStackedWidget->setCurrentIndex(0);  //RF界面

    //配置界面需要从新Init
    ATE_Ctrol_Lib_Init();
    UIDataInit();
}

void ATEUIMainWindow::updatechannelnumData(QString data)
{
    //
    qInfo()<<"updatechannelnumData="<<data;

    //转换为int类型，跟之前的比较
    int inum = data.toInt();

    //如果和之前的一样，什么也不做
    if(inum==g_channelnum)
    {
        return;
    }

    g_channelnum = inum;

    //如果不一样，从新调整channel的数量
    //如果不一样，从新调整channel的数量
    //ChooseDLWgtUI(g_channelnum);

    //大于4的都隐藏
    for(int i=inum;i<32;i++)
    {
        m_DLThreadGrouplist.at(i)->setHidden(true);
        m_DL_StartBtnlist.at(i)->setHidden(true);
    }

    //显示出来
    for(int i=0;i<inum;i++)
    {
        m_DLThreadGrouplist.at(i)->setHidden(false);
        m_DL_StartBtnlist.at(i)->setHidden(false);
    }


    //配置界面需要从新Init
    ATE_Ctrol_Lib_Init();
    UIDataInit();

    //触发
    DLscrollAreawgt->setMinimumHeight(665+168*(g_channelnum-4));
}

void ATEUIMainWindow::ShowEidtPathLoss()
{
    //使用指针，打开和关闭会有问题
//    MyPathLossDlg *pathlossdlg = new MyPathLossDlg();
////    connect(setchannlenum,SIGNAL(sendData(QString)),this,SLOT(updatechannelnumData(QString)));
//    if(pathlossdlg->exec()==QDialog::Accepted)
//    {
//    }
//    delete pathlossdlg;

    MyPathLossDlg pathlossdlg(this,m_currentTreeIndex);
    pathlossdlg.exec();

}

void ATEUIMainWindow::ShowFilePath()
{
    //弹框选择文件函数
    // 创建一个QFileDialog实例
    QFileDialog dialog(this);
    // 设置对话框的标题
    dialog.setWindowTitle("选择文件");

    // 设置初始目录，可以是任何合法路径
    dialog.setDirectory(QDir::homePath());

    // 打开对话框，并获取用户选择的文件路径
    QString filePath = dialog.getOpenFileName();

    // 检查用户是否取消了操作
    if (!filePath.isEmpty())
    {
        // 用户选择了文件，处理文件路径
        // ...
        if(nullptr != m_filepathbtn)
        {
            QString replacedPath = filePath.replace('/', '\\');

            m_filepathbtn->setText(replacedPath);
        }
    }
    else
    {
        // 用户取消了操作
        // ...
    }


}

void ATEUIMainWindow::thirdpartybtnClicked()
{
    //
    qInfo()<<"thirdpartybtnClicked() Start";
    QString FullPath;

    //查找哪个通道
    QObject* button = QObject::sender();

    int i=0;
    for(i=0;i<MAX_THIRDPARTY_NUM;i++)
    {
        if(button == thirdpartybtn[i])
        {
            qInfo() << "thirdpartybtn= " << i;
            break;
        }
    }

    if(MAX_THIRDPARTY_NUM==i)
    {
         qDebug() << "not thirdpartybtn  ";
        return;
    }

    //根据通道值算出item。
    m_thirdpartyIndexNum = i;


    m_thirdpartyItem[i].replace("\\","/");

    FullPath =  QDir::currentPath() + "/"+m_thirdpartyItem[i];

    FullPath.replace("//","/");

    qInfo()<<"FullPath="<<FullPath;

    //myprocess->start("ping localhost");
    myprocess->start(FullPath);

   // process->kill();
    myprocess->waitForFinished();

    qInfo()<<"thirdpartybtnClicked() End";

}

void ATEUIMainWindow::ProcessreadAllStandardOutput()
{
   // qDebug() << QString::fromLocal8Bit(myprocess->readAllStandardOutput());
    QString readstr;
    readstr=QString::fromLocal8Bit(myprocess->readAllStandardOutput());
    readstr = readstr.trimmed();
    qDebug()<<readstr;
    //显示在界面
    thirdpartybtn[m_thirdpartyIndexNum]->setText(readstr);
    //保存到value中
    pCommonScript->SetTchPara(m_thirdpartyTsNum,m_cla_ate_gfc.QString2Wchar(m_thirdpartyName[m_thirdpartyIndexNum]),m_cla_ate_gfc.QString2Wchar(readstr));

}

void ATEUIMainWindow::DL_Btn_Start()
{
    //判断是哪个button

    QObject* button = QObject::sender();

    int i=0;
    for(i=0;i<MAX_CHANNEL_NUM;i++)
    {
        if(button == m_DL_StartBtnlist.at(i))
        {
            qDebug() << "m_DL_StartBtnlist " << i;
            break;
        }
    }

    if(i==MAX_CHANNEL_NUM)
    {
         qDebug() << "not DL_StartBtn  ";
        return;
    }

    m_DL_Current_Channel = i;


    //防止意外激发测试
    if( (this->m_irTS_DL_OBJ_State[i] == TS_OBJ_RUN) || (this->m_irTS_DL_OBJ_State[i] <= TS_OBJ_NO_USE) )
    {
        return;
    }

    //清空
    m_DLTextLogEditlist.at(i)->clear();
    memset(m_cla_DL_Run_List[i].m_QRWchart,0x00,256*2);

    qDebug() << "StartBtn Thread ";

    //取得二维码的值，传递给线程.
    int length=0;
    length = m_DLQRLineeditlist.at(i)->text().toWCharArray(m_cla_DL_Run_List[i].m_QRWchart);
    Q_UNUSED(length);
    
    tmpthreadInfoList[i].CAteMainWgt = this;
    tmpthreadInfoList[i].channelnum  = i;

    //启动之前创建时间线程来用显示测试时间
    this->m_hThread_Timer_Handle_List[i] = CreateThread( NULL,
        0,
        (LPTHREAD_START_ROUTINE)OP_EXECUTE_TIMER,
        (void *)&tmpthreadInfoList[i],
        0,
        &this->m_dwThread_Timer_ID_List[i] );
    
    //创建线程
    this->m_hThread_DL_Handle_List[i] = CreateThread( NULL,
        0,
        (LPTHREAD_START_ROUTINE)OP_EXECUTE,
        (void *)&tmpthreadInfoList[i],
        0,
        &this->m_dwThread_DL_ID_List[i] );

}

void ATEUIMainWindow::Test_Btn_Start()
{
    qInfo()<<"Test_Btn_Start() Start";
    //判断是哪个button
    int i=0;

    QObject* button = QObject::sender();
    for(i=0;i<MAX_CHANNEL_NUM;i++)
    {
        if(button == m_Test_StartBtnlist.at(i))
        {
            qDebug() << "m_Test_StartBtnlist " << i;
            break;
        }
    }

    if(i==MAX_CHANNEL_NUM)
    {
         qDebug() << "not DL_StartBtn  ";
        return;
    }

    m_DL_Current_Channel = i;


    qInfo()<<"this->m_irTS_DL_OBJ_State[i]"<<this->m_irTS_DL_OBJ_State[i];
    //防止意外激发测试
    if( (this->m_irTS_DL_OBJ_State[i] == TS_OBJ_RUN) || (this->m_irTS_DL_OBJ_State[i] <= TS_OBJ_NO_USE) )
    {
        return;
    }

    //清空
    m_TestLogTableWgtlist.at(i)->clearContents();

    while (m_TestLogTableWgtlist.at(i)->rowCount() > 0)
    {
        m_TestLogTableWgtlist.at(i)->removeRow(0);
    }
    m_TestTextLogEditlist.at(i)->clear();
    m_TestShowErrorcodelist.at(i)->clear();

    m_Test_Result_Count[i]=0;
    qDebug() << "StartBtn Thread ";

    //取得二维码的值，传递给线程.
    memset(m_cla_DL_Run_List[i].m_QRWchart,0x00,256*2);
    int length;
    //字符串统一转换成大写
    length = m_TestQRLineeditlist.at(i)->text().toWCharArray(m_cla_DL_Run_List[i].m_QRWchart);
    Q_UNUSED(length);

    tmpthreadInfoList[i].CAteMainWgt = this;
    tmpthreadInfoList[i].channelnum  = i;


    // 所有任务完成后的操作
    qInfo()<<"run synchronizer Start";
    QFutureSynchronizer<void> synchronizer;
    qInfo()<<"run  addFuture future1 Start";
    synchronizer.addFuture(future1[i]);
    qInfo()<<"run  addFuture future2 Start";
    synchronizer.addFuture(future2[i]);
     qInfo()<<"run  waitForFinished Start";
    synchronizer.waitForFinished();

    qInfo()<<"run OP_EXECUTE_TIMER Start";
     //future1 = QtConcurrent::run(OP_EXECUTE_TIMER, (void *)&tmpthreadInfoList[i]);
    future1[i] = QtConcurrent::run([this,i](){DO_DL_OP_TIMER(i);});
    qInfo()<<"run OP_EXECUTE Start";
    //future2 = QtConcurrent::run(OP_EXECUTE, (void *)&tmpthreadInfoList[i]); DO_DL_OP_TIMER
    future2[i] = QtConcurrent::run([this,i](){DO_DL_OP(i);});
    qInfo()<<"run OP_EXECUTE End====";




    // 设置监视器的未来对象
//    watcher1.setFuture(future1);
//    watcher2.setFuture(future2);

    //创建一个线程函数，用来计时，显示测试时间,如果fail或者Pass就结束计时
//    qInfo()<<"Before this->m_hThread_Timer_Handle_List[i] = "<<this->m_hThread_Timer_Handle_List[i];
//    if (this->m_hThread_Timer_Handle_List[i] != NULL)
//    {
//        qInfo()<<"Before WaitForSingleObject";

//        DWORD dwRet = 0u;
//        dwRet = ::WaitForSingleObject(this->m_hThread_Timer_Handle_List[i], 5000);
//        qInfo()<<"Behind WaitForSingleObject";

//        if (dwRet == WAIT_TIMEOUT)
//        {
//            qInfo()<<"Before TerminateThread";
//            ::TerminateThread(this->m_hThread_Timer_Handle_List[i], 1);
//            qInfo()<<"Behind TerminateThread";
//        }

//        ::CloseHandle(this->m_hThread_Timer_Handle_List[i]);

//         qInfo()<<"Behind CloseHandle";

//        this->m_hThread_Timer_Handle_List[i] = NULL;
//    }

//    qInfo()<<"m_hThread_Timer_Handle Start";
//    this->m_hThread_Timer_Handle_List[i] = CreateThread( NULL,
//        0,
//        (LPTHREAD_START_ROUTINE)OP_EXECUTE_TIMER,
//        (void *)&tmpthreadInfoList[i],
//        0,
//        &this->m_dwThread_Timer_ID_List[i] );

    //判断是否为空
//    if(this->m_hThread_Timer_Handle_List[i]==NULL)
//    {
//        // 获取错误代码
//        DWORD dwError = GetLastError();
//        // 获取错误描述
//        LPSTR lpMsgBuf = NULL;
//        DWORD bufLen = FormatMessageA(
//            FORMAT_MESSAGE_ALLOCATE_BUFFER |
//            FORMAT_MESSAGE_FROM_SYSTEM |
//            FORMAT_MESSAGE_IGNORE_INSERTS,
//            NULL,
//            dwError,
//            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//            (LPSTR)&lpMsgBuf,
//            0,
//            NULL);
//        if (bufLen > 0)
//        {
//            qInfo()<<"CreateThread OP_EXECUTE_TIMER failed with error"<<dwError<<lpMsgBuf;
//           // printf("CreateThread failed with error %d: %s", dwError, lpMsgBuf);
//        }
//        else
//        {
//            qInfo()<<"CreateThread OP_EXECUTE_TIMER failed with error (could not get description)"<<dwError;
//        }
//    }

//    qInfo()<<"Behind this->m_hThread_Timer_Handle_List[i] = "<<this->m_hThread_Timer_Handle_List[i];

    //创建线程
//    if (this->m_hThread_DL_Handle_List[i] != NULL)
//    {
//        DWORD dwRet = 0u;
//        dwRet = ::WaitForSingleObject(this->m_hThread_DL_Handle_List[i], 5000);

//        if (dwRet == WAIT_TIMEOUT)
//            ::TerminateThread(this->m_hThread_DL_Handle_List[i], 1);
//        ::CloseHandle(this->m_hThread_DL_Handle_List[i]);

//        this->m_hThread_DL_Handle_List[i] = NULL;
//    }

 //   qInfo()<<"m_hThread_DL_Handle_List Start";

//    this->m_hThread_DL_Handle_List[i] = CreateThread( NULL,
//        0,
//        (LPTHREAD_START_ROUTINE)OP_EXECUTE,
//        (void *)&tmpthreadInfoList[i],
//        0,
//        &this->m_dwThread_DL_ID_List[i] );

    //判断是否为空
//    if(this->m_hThread_DL_Handle_List[i]==NULL)
//    {
//        // 获取错误代码
//        DWORD dwError = GetLastError();
//        // 获取错误描述
//        LPSTR lpMsgBuf = NULL;
//        DWORD bufLen = FormatMessageA(
//            FORMAT_MESSAGE_ALLOCATE_BUFFER |
//            FORMAT_MESSAGE_FROM_SYSTEM |
//            FORMAT_MESSAGE_IGNORE_INSERTS,
//            NULL,
//            dwError,
//            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//            (LPSTR)&lpMsgBuf,
//            0,
//            NULL);
//        if (bufLen > 0)
//        {
//            qInfo()<<"CreateThread OP_EXECUTE failed with error"<<dwError<<lpMsgBuf;
//           // printf("CreateThread failed with error %d: %s", dwError, lpMsgBuf);
//        }
//        else
//        {
//            qInfo()<<"CreateThread OP_EXECUTE  failed with error (could not get description)"<<dwError;
//        }
//    }

//    qInfo()<<"this->m_hThread_DL_Handle_List[i] = "<<this->m_hThread_DL_Handle_List[i];

    qInfo()<<"Test_Btn_Start() End";

}


void OP_EXECUTE(LPVOID x)
{

    qInfo()<<"OP_EXECUTE() Start";
    threadInfo * pSv = (threadInfo *)x;
    
    pSv->CAteMainWgt->DO_DL_OP(pSv->channelnum);

    qInfo()<<"OP_EXECUTE() End=====";

    pSv =nullptr;
       
}

void OP_EXECUTE_TIMER(LPVOID x)
{
    threadInfo * pSv = (threadInfo *)x;

    pSv->CAteMainWgt->DO_DL_OP_TIMER(pSv->channelnum);
}

void OP_START_TS_SERVER( LPVOID x )
{
    ATEUIMainWindow  *CAteMainWgt = (ATEUIMainWindow*)(x);  //窗口句柄
    QString qdata;
    //启动TS测试台“服务器”
    if( ::TS_LAN_START_TCP_SERVER() == true )
    {
        qdata="TS测试台服务器成功启动.";
    }
    else
    {
        qdata =  "FAIL, 启动TS服务器失败!";
    }

    qInfo() <<"OP_START_TS_SERVER:"<<qdata;


    for(int i=0;i<g_iActiveChannelNums;i++)
    {
        CAteMainWgt->OnGetMsg_DL_RunLog_Msg_TS(qdata,i);
    }

    Q_UNUSED(x);

}

void OP_DL_INIT_EXECUTE(LPVOID x)
{
    threadInfo * pSv = (threadInfo *)x;

    pSv->CAteMainWgt->m_cla_DL_Run_List[pSv->channelnum].Init();

}

void OP_EXECUTE_SEND_RESULT_LAN_CMD_TO_MC_PC(LPVOID x)
{
    threadMCBCInfo * pSv = (threadMCBCInfo *)x;

    pSv->CAteMainWgt->DO_SEND_MC_PC_OP(pSv->channelnum);
}

void ATEUIMainWindow::DO_DL_OP(int i_TS_CH)
{

    qInfo()<<"DO_DL_OP() Start ";
    int iRtn = RTN_ERROR;

	//重要，第一时间标记“测试进行中”状态
	this->m_irTS_DL_OBJ_State[ i_TS_CH] = TS_OBJ_RUN;     

    //only for debug Test sleep 2
    //Sleep(1000);
	//------------------------------------- 运行具体执行（FT测试）函数 -------------------------------------
	
    iRtn=m_cla_DL_Run_List[i_TS_CH].Do_Execute();

    qInfo()<<"Do_Execute result"<<iRtn;
	//-------------------------------------

    //处理返回结果
    if( iRtn == RTN_SUCCESS )
    {
        this->m_irTS_DL_OBJ_State[i_TS_CH] = TS_OBJ_PASS;//运行结束，结果pass

    }
    else
    {
        this->m_irTS_DL_OBJ_State[i_TS_CH ] = TS_OBJ_FAIL;//运行结束，结果fail
    }


	//-------------------------------------
    Save_Result_Statistics_Data(i_TS_CH,iRtn);
    //读出统计数据在界面更新
    this->Update_Statistics_Data(i_TS_CH);

    //根据结果光标定位到相同的输入框
    AfterTestingLocateCursor();

    //是否有研发的压力测试
    RDStressTest(i_TS_CH);

     qInfo()<<"DO_DL_OP() End==== ";

}

void ATEUIMainWindow::DO_SEND_MC_PC_OP(int i_TS_CH)
{
    //
    qDebug()<<"DO_SEND_MC_PC_OP Start"<<i_TS_CH;
    int b_ret=false;

    wchar_t wcHar[256];
    int  Tsnum;
    QString qstmpchannel;
    bool ok;

    //根据channel 拿到夹具的编号值
    memset(wcHar,0x00,256);
    pCommonScript->GetTchPara(i_TS_CH+1,L"TSNum",wcHar,256);
    qstmpchannel = m_cla_ate_gfc.Wchar2QString(wcHar);
    Tsnum = qstmpchannel.toInt(&ok);
    qInfo()<<"Tsnum="<<Tsnum;


    //判断 是否是Init,Ate3.0不需要传递INIT消息
    if(tmpthreadMCBCInfo[i_TS_CH].state=="INIT")
    {
//        b_ret=TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC( m_cla_ate_gfc.QString2Wchar(m_StationName), L"INIT",i_TS_CH,true );
//        if(b_ret==false)
//        {
//            qDebug()<<m_StationName<<i_TS_CH<<tmpthreadMCBCInfo[i_TS_CH].state<<"TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC false";
//        }
//        else
//        {
//            qDebug()<<m_StationName<<i_TS_CH<<tmpthreadMCBCInfo[i_TS_CH].state<<"TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC true";
//        }
    }
    else if(tmpthreadMCBCInfo[i_TS_CH].state=="PASS")
    {
        //是否是线损校准的结果
        if(tmpthreadMCBCInfo[i_TS_CH].robot_LOSS_Status == true)
        {
            m_robot_LOSS_Status[i_TS_CH]=false;
            TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC( m_cla_ate_gfc.QString2Wchar(m_RoboticAuto_StationName), L"CLOSS", Tsnum, true );
        }
        else
        {
            //
            QString csData;
            Setup_TRD_String(i_TS_CH,&m_csIMEI_1_Key_In_Barcode[i_TS_CH], true, &csData);
            TS_LAN_SEND_TRD_LAN_CMD_TO_MC_PC( Tsnum, m_cla_ate_gfc.QString2Wchar(csData) );
            TS_LAN_SEND_PFQUAN_LAN_CMD_TO_MC_PC( m_cla_ate_gfc.QString2Wchar(m_RoboticAuto_StationName), Tsnum, m_channel_iTS_Test_Pass_Quan[i_TS_CH], m_channel_iTS_Test_Fail_Quan[i_TS_CH] );
        }
    }
    else if(tmpthreadMCBCInfo[i_TS_CH].state=="FAIL")
    {
        //
        if(tmpthreadMCBCInfo[i_TS_CH].robot_LOSS_Status  == true)
        {
            m_robot_LOSS_Status[i_TS_CH]=false;
            TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC( m_cla_ate_gfc.QString2Wchar(m_StationName), L"CLOSS", Tsnum, false );
        }
        else
        {
            QString csData;
            Setup_TRD_String(i_TS_CH,&m_csIMEI_1_Key_In_Barcode[i_TS_CH], false, &csData);
            TS_LAN_SEND_TRD_LAN_CMD_TO_MC_PC( Tsnum, m_cla_ate_gfc.QString2Wchar(csData) );
            TS_LAN_SEND_PFQUAN_LAN_CMD_TO_MC_PC( m_cla_ate_gfc.QString2Wchar(m_StationName), Tsnum, m_channel_iTS_Test_Pass_Quan[i_TS_CH], m_channel_iTS_Test_Fail_Quan[i_TS_CH] );

        }
    }

    qDebug()<<"DO_SEND_MC_PC_OP End"<<i_TS_CH;
}

void ATEUIMainWindow::Save_Result_Statistics_Data(int i_TS_CH,int b_Result)
{

    QString tsname;
    QString tmppass;
    QString tmpfail;
    tsname = QString("TS_%1").arg(i_TS_CH);

    tmppass = "/"+tsname+"/PASS";
    tmpfail = "/"+tsname+"/FAIL";
    qInfo()<<tmppass;
    qInfo()<<tmpfail;

    //保存各测试台的统计
    if( RTN_SUCCESS == b_Result )
    {
        int ipass  = m_statistics_ini->value(tmppass).toInt();
        ipass++;
        m_statistics_ini->setValue(tmppass, ipass);
    }
    else
    {
        int ifail  = m_statistics_ini->value(tmpfail).toInt();
        ifail++;
        m_statistics_ini->setValue(tmpfail, ifail);
    }
}

void ATEUIMainWindow::Update_Statistics_Data(int i_TS_CH)
{
    QString tsname;
    QString tmppass;
    QString tmpfail;
    QString passrate;
    int iTotal;
    tsname = QString("TS_%1").arg(i_TS_CH);

    tmppass = "/"+tsname+"/PASS";
    tmpfail = "/"+tsname+"/FAIL";
    passrate = "/"+tsname+"/Pass_Ratio";

    //先读取统计数据
    this->m_iTS_Test_Pass_Quan = this->m_statistics_ini->value(tmppass).toInt();
    this->m_iTS_Test_Fail_Quan = this->m_statistics_ini->value(tmpfail).toInt();

//    qInfo()<<"m_iTS_Test_Pass_Quan="<<m_iTS_Test_Pass_Quan;
//    qInfo()<<"m_iTS_Test_Fail_Quan="<<m_iTS_Test_Fail_Quan;

    m_channel_iTS_Test_Pass_Quan[i_TS_CH]=m_iTS_Test_Pass_Quan;
    m_channel_iTS_Test_Fail_Quan[i_TS_CH]=m_iTS_Test_Fail_Quan;

    //总数
    iTotal = this->m_iTS_Test_Pass_Quan + this->m_iTS_Test_Fail_Quan;
//    qInfo()<<"iTotal="<<iTotal;

    //计算pass率
    if( (this->m_iTS_Test_Pass_Quan <= 0) || (iTotal <= 0) )
    {
        this->m_dTS_Test_FPY_Percent = 0.0;
    }
    else
    {
        this->m_dTS_Test_FPY_Percent = 100.0 * this->m_iTS_Test_Pass_Quan / iTotal;

    }

    //保存 FPY 值
    QString str = QString("%1").arg(this->m_dTS_Test_FPY_Percent);
    m_statistics_ini->setValue(passrate, str);

    str = QString::number(m_dTS_Test_FPY_Percent, 'f', 2);

    //显示在界面
    if(m_current_station==TEST_STATION_DL)
    {
        m_DLShowpassnumlablist.at(i_TS_CH)->setText(QString::number(m_iTS_Test_Pass_Quan));
        m_DLShowfailnumlablist.at(i_TS_CH)->setText(QString::number(m_iTS_Test_Fail_Quan));
        m_DLShowRatelablist.at(i_TS_CH)->setText(str+"%");
    }
    else
    {
        m_TestShowpassnumlablist.at(i_TS_CH)->setText(QString::number(m_iTS_Test_Pass_Quan));
        m_TestShowfailnumlablist.at(i_TS_CH)->setText(QString::number(m_iTS_Test_Fail_Quan));
        m_TestShowRatelablist.at(i_TS_CH)->setText(str+"%");
    }

}

void ATEUIMainWindow::Clear_Statistics_Data(int i_TS_CH)
{
    //
    QString tsname;
    QString tmppass;
    QString tmpfail;
    QString passrate;
    int iTotal;
    tsname = QString("TS_%1").arg(i_TS_CH);

    tmppass = "/"+tsname+"/PASS";
    tmpfail = "/"+tsname+"/FAIL";
    passrate = "/"+tsname+"/Pass_Ratio";

    m_statistics_ini->setValue(tmppass, 0);
    m_statistics_ini->setValue(tmpfail, 0);
    m_statistics_ini->setValue(passrate, 0);

    Update_Statistics_Data(i_TS_CH);


}

void ATEUIMainWindow::DO_DL_OP_TIMER(int i_TS_CH)
{
    //
    qInfo()<<"DO_DL_OP_TIMER() Start";
    int cont=0;
    int tmpResultcount=0;
    QDateTime tmpTestResultTime = QDateTime::currentDateTime();
    QDateTime startTime = QDateTime::currentDateTime();

    Sleep(500);
    //

    while(1)
    {
        //
        if((this->m_irTS_DL_OBJ_State[i_TS_CH]== TS_OBJ_FAIL || this->m_irTS_DL_OBJ_State[i_TS_CH] == TS_OBJ_PASS ))
        {
            break;
        }
        //
        QDateTime endTime = QDateTime::currentDateTime();

        qint64  intervalTime2 = endTime.toTime_t() - startTime.toTime_t();
        QString interval2 = tr("%1").arg(intervalTime2);
        bool ok;
        m_iTest_Time[i_TS_CH]=interval2.toInt(&ok);

        Sleep(1000);

        if(m_current_station==TEST_STATION_DL)
        {
            m_DLShowtesttimelablist.at(i_TS_CH)->setText(interval2);
        }
        else
        {
             m_TestShowtesttimelablist.at(i_TS_CH)->setText(interval2);

             //判断
             if(m_Test_Result_Count[i_TS_CH]>tmpResultcount)
             {
                 tmpResultcount = m_Test_Result_Count[i_TS_CH];

                 //加上时间间隔判断
                 if((m_Test_Result_Time.toTime_t() - tmpTestResultTime.toTime_t())>2)
                 {
                      m_TestLogTableWgtlist.at(i_TS_CH)->scrollToBottom();  //显示到底部
                 }

                 tmpTestResultTime = m_Test_Result_Time;
             }
        }

        cont++;
    }

    qInfo()<<"DO_DL_OP_TIMER() End";

    return;
}

void ATEUIMainWindow::Delay_MSec(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec,&loop,SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

// 定时器事件
void ATEUIMainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == this->m_Id1)
    {

        killTimer(this->m_Id1);
    }

    //判断是否是DL
    if(m_current_station==TEST_STATION_DL)
    {
        //判断是否是在线模式
        if(m_Offline_Status=="false")
        {

            //读取MO里面的软件值
            //查找MO工单是否存在
            wchar_t wcHar[1024] = { 0 };
            m_ateMoCfg = (Q_ATE_Controller_MOInfo_Interface*)Q_ATE_CONTROLLER_GetMOInfo();
            if(m_ateMoCfg==nullptr)
            {
                qDebug()<<"Q_ATE_Controller_MOInfo_Interface NULLPTR";
                m_message_box.SetShowResult("Error","Q_ATE_CONTROLLER_GetMOInfo() is NULL");
                m_message_box.exec();
                return;
            }
            else
            {
                memset(wcHar,0x00,sizeof(wcHar));
                m_ateDownloadFW=m_cla_ate_gfc.Wchar2QString(m_ateMoCfg->GetParaValueByName(L"DownloadFW"));
                qInfo()<<"m_DownloadFW="<<m_ateDownloadFW;
                QGMRSRCEdit->setText(m_ateDownloadFW);
            }
        }
    }


    try
    {

        //不管什么都是初始化
        {
            //初始化状态中

            for(int i=0;i<g_iActiveChannelNums;i++)
            {
                //启动线程，进行初始化.
                if(m_current_station==TEST_STATION_DL)
                {
                    DL_Show_Program_State_TS(i,"INIT_ING");
                }
                else
                {
                    Test_Show_Program_State_TS(i,"INIT_ING");
                }

                m_cla_DL_Run_List[i].m_iTSCH_Numb=i;
                m_cla_DL_Run_List[i].m_hWnd_View=this->winId();
                m_cla_DL_Run_List[i].m_run_Station = m_current_station;
                m_cla_DL_Run_List[i].m_iOP_Multi_Option = m_iOP_Multi_Option;

                m_cla_DL_Run_List[i].ChannelPowerFlag =ChannelPowerFlag[i];
                //m_cla_DL_Run_List[i].Init();

                m_irTS_DL_OBJ_State[i]=TS_OBJ_STANDBY;
            }

            //启动线程进行Init初始化
            for(int i=0;i<g_iActiveChannelNums;i++)
            {
                tmpthreadInfoList[i].CAteMainWgt = this;
                tmpthreadInfoList[i].channelnum  = i;

                //创建线程
                this->m_hThread_DL_INIT_Handle_List[i] = CreateThread( NULL,
                    0,
                    (LPTHREAD_START_ROUTINE)OP_DL_INIT_EXECUTE,
                    (void *)&tmpthreadInfoList[i],
                    0,
                    &this->m_dwThread_DL_INIT_ID_List[i] );
            }

            //ATEProcessEvents();
        }
    }
    catch (...)
    {
        //Init失败
        if(m_current_station==TEST_STATION_DL)
        {
            DL_Show_Program_State_TS(0,"INIT");
        }
        else
        {
            Test_Show_Program_State_TS(0,"INIT");
        }

    }

    qDebug() <<"timerEvent end:";
}

void ATEUIMainWindow::OnGetMsg_DL_RunLog_Msg_TS(QString RunLogMsg, int TSCH_Numb)
{
   // qDebug() <<"Runnlog:"<<RunLogMsg;

    //界面显示log
    if(m_current_station==TEST_STATION_DL)
    {
        m_DLTextLogEditlist.at(TSCH_Numb)->append(RunLogMsg);
        //m_DLTextLogEditlist.at(TSCH_Numb)->moveCursor(QTextCursor::End);
    }
    else
    {
        m_TestTextLogEditlist.at(TSCH_Numb)->append(RunLogMsg);
        //m_TestTextLogEditlist.at(TSCH_Numb)->moveCursor(QTextCursor::End);
    }

}

void ATEUIMainWindow::OnGetMsg_DL_Show_Program_State_TS( WPARAM wParam, LPARAM lParam )
{
    //
    QString tmpState = m_cla_ate_gfc.Wchar2QString((wchar_t *)wParam);

    int tpTSCH_Numb = (int)(lParam);

    if(m_current_station==TEST_STATION_DL)
    {
         DL_Show_Program_State_TS(tpTSCH_Numb,tmpState);
    }
    else
    {
        Test_Show_Program_State_TS(tpTSCH_Numb,tmpState);
    }

    //更新状态
    UpdateProgramStatus(tpTSCH_Numb,tmpState);

//    qInfo()<<"m_current_station="<<m_current_station<<"tpTSCH_Numb=="<<tpTSCH_Numb;

//    qInfo()<<"tmpState"<<tmpState;

    //全自动机器人模式时，向总控PC回传初始化结果
    if(m_iOP_Multi_Option==LOAD_AUTO_ROBOT)
    {

        tmpthreadMCBCInfo[tpTSCH_Numb].CAteMainWgt=this;
        tmpthreadMCBCInfo[tpTSCH_Numb].channelnum = tpTSCH_Numb;
        tmpthreadMCBCInfo[tpTSCH_Numb].robot_LOSS_Status= m_robot_LOSS_Status[tpTSCH_Numb];
        tmpthreadMCBCInfo[tpTSCH_Numb].state=tmpState;

        //启动线程函数，传回结果
        //创建线程
        this->m_hThread_MC_PC_Handle = CreateThread( NULL,
            0,
            (LPTHREAD_START_ROUTINE)OP_EXECUTE_SEND_RESULT_LAN_CMD_TO_MC_PC,
            (void *)&tmpthreadMCBCInfo[tpTSCH_Numb],
            0,
            &this->m_dwThread_MC_PC_ID );
    }
}

void ATEUIMainWindow::OnGetMsg_DL_Result_Process_State_TS( WPARAM wParam, LPARAM lParam )
{

     m_barvalue = (double*)wParam;
     QString tmpState= QString::number(*m_barvalue, 'f', 2);

    int tpTSCH_Numb = (int)(lParam);

   // qDebug()<<tpTSCH_Numb<<tmpState;


    //判断是否running状态
//    if( (this->m_irTS_DL_OBJ_State[tpTSCH_Numb] < TS_OBJ_RUN) )
//    {
//        return;
//    }

    int DLProBar = (int )(*m_barvalue);
    //qDebug()<<"DLProBar=="<<DLProBar;
    m_DLProgBarlist.at(tpTSCH_Numb)->setValue(DLProBar);

    int tmpvalue=m_DLProgBarlist.at(tpTSCH_Numb)->value();

    //qDebug()<<"tmpvalue"<<tmpvalue;

    //tmpvalue = tmpvalue+(*value);
    if(tmpvalue>=100)
    {
        tmpvalue=99;
    }

   m_DLProgBarlist.at(tpTSCH_Numb)->setValue(DLProBar);

   m_DLPercentlablist.at(tpTSCH_Numb)->setText(QString::number(tmpvalue)+"%");


   m_DLProgBarlist.at(tpTSCH_Numb)->setStyleSheet("QProgressBar{font-weight: 700;width: 435px;height: 32px;border-radius: 4px 4px 4px 4px;background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);text-align: center;color: #0E1016;font: 14px \"Microsoft YaHei UI\";}"
                                        "QProgressBar::chunk{background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #20B2AA, stop:1 #20B2AA);width: 10px;margin: 0.5px;text-align: center;border-radius:5px;}"
                                        );


}

void ATEUIMainWindow::OnGetMsg_Load_Result_State_TS( WPARAM wParam, LPARAM lParam)
{
   TESTER_RESULT_S  *tmpResult = (TESTER_RESULT_S*)wParam;
   int tpTSCH_Numb = (int)(lParam);

   //界面累加
   QString qstmpItemName  = m_cla_ate_gfc.Wchar2QString(tmpResult->pcwcItemName);
   qstmpItemName = qstmpItemName.replace("< ","<");
   QString qstmpvalue     = m_cla_ate_gfc.Wchar2QString(tmpResult->pcwcValue);
   QString qstmpLowlimit  = m_cla_ate_gfc.Wchar2QString(tmpResult->pcwcLowlimit);
   QString qstmpHighLimit = m_cla_ate_gfc.Wchar2QString(tmpResult->pcwcHighlimit);

   //打印大概多了16s，一万行
   //qInfo()<<"Result_State"<<qstmpItemName<<qstmpvalue<<qstmpLowlimit<<qstmpHighLimit;

   int itmpResult = tmpResult->bResut;
   QString qstmpresult ;
   if(itmpResult==true)
   {
       qstmpresult ="PASS";
   }
   else
   {
       qstmpresult = "FAIL";
   }

   //显示在界面
    int row =m_TestLogTableWgtlist.at(tpTSCH_Numb)->rowCount();

    m_TestLogTableWgtlist.at(tpTSCH_Numb)->insertRow(row);
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,0,new QTableWidgetItem(QString::number(row+1)));
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,1,new QTableWidgetItem(qstmpItemName));
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,2,new QTableWidgetItem(qstmpLowlimit));
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,3,new QTableWidgetItem(qstmpHighLimit));
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,4,new QTableWidgetItem(qstmpvalue));
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,5,new QTableWidgetItem(qstmpresult));
    m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    if(itmpResult!=true)
    {
        m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,5)->setTextColor(QColor(Qt::red));
    }
    else
    {
        m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,5)->setTextColor(QColor(Qt::green));
    }


    m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,6,new QTableWidgetItem(""));
    //m_TestLogTableWgtlist.at(tpTSCH_Numb)->scrollToBottom();  //显示到底部

    m_Test_Result_Count[tpTSCH_Numb]++;
    m_Test_Result_Time = QDateTime::currentDateTime();


}

void ATEUIMainWindow::OnGetMsg_Load_Title_State_TS( WPARAM wParam, LPARAM lParam)
{
    int tpTSCH_Numb = (int)(lParam);

    QString *text = (QString*)wParam;

    QString qstmpItemName  = *text;
    qstmpItemName = qstmpItemName.replace("< ","<");
    QString qstmpvalue     = "***";
    QString qstmpLowlimit  = "***";
    QString qstmpHighLimit = "***";
    QString qstmpresult    = "***";

    //qInfo()<<"Result_State"<<qstmpItemName<<qstmpvalue<<qstmpLowlimit<<qstmpHighLimit;

    //显示在界面
     int row =m_TestLogTableWgtlist.at(tpTSCH_Numb)->rowCount();

     m_TestLogTableWgtlist.at(tpTSCH_Numb)->insertRow(row);
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,0,new QTableWidgetItem(QString::number(row+1)));
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,1,new QTableWidgetItem(qstmpItemName));
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,2,new QTableWidgetItem(qstmpLowlimit));
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,3,new QTableWidgetItem(qstmpHighLimit));
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,4,new QTableWidgetItem(qstmpvalue));
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,5,new QTableWidgetItem(qstmpresult));
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->item(row,5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

     m_TestLogTableWgtlist.at(tpTSCH_Numb)->setItem(row,6,new QTableWidgetItem(""));
     m_TestLogTableWgtlist.at(tpTSCH_Numb)->scrollToBottom();  //显示到底部

}


void ATEUIMainWindow::OnGetMsg_Load_ErrorCodeInfo_State_TS(WPARAM wParam, LPARAM lParam)
{
    TESTER_ErrorCodeInfo_S  *tmpErrorInfo = (TESTER_ErrorCodeInfo_S*)wParam;
    int tpTSCH_Numb = (int)(lParam);

    QString tmpError;

    tmpError = "ErrorInfo: "+m_cla_ate_gfc.Wchar2QString(tmpErrorInfo->pcwcErrorInfo)+" "+"("+QString::number((uint)tmpErrorInfo->iErrorCode,16)+")";

    m_TestShowErrorcodelist.at(tpTSCH_Numb)->setText(tmpError);
}

void ATEUIMainWindow::OnGetMsg_Load_MessageBox_State_TS(WPARAM wParam, LPARAM lParam)
{

    m_query_current_Value =false;
    m_qsMessBox_ret="";
    m_inputmess_box.m_inputLine_edit->setText("");

    TESTER_MESSBOX_S  *tmpMessbox = (TESTER_MESSBOX_S*)wParam;
    int tpTSCH_Numb = (int)(lParam);

    m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result ="";
    //界面累加
    QString qstmpAgrs  = m_cla_ate_gfc.Wchar2QString(tmpMessbox->pcwcAgrs);

     //判断类型
    QString tmpAPIName = m_cla_ate_gfc.Wchar2QString(tmpMessbox->pcwcAPIName);

    qInfo()<<"tmpAPIName=="<<tmpAPIName;

    //标志和内容
    QString tmpAgrs  = m_cla_ate_gfc.Wchar2QString(tmpMessbox->pcwcAgrs);

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
        qInfo()<<"tmpAPIName==MessageBox"<<tmpAPIName;

        if(!qsTitle.isEmpty())
        {
            m_message_box.SetShowResult(qsTitle,qsContent);
            m_message_box.exec();
        }

    }

    if("QueryMessageBox"==tmpAPIName)
    {
        //
        qInfo()<<"tmpAPIName==QueryMessageBox"<<tmpAPIName;

        if(!qsTitle.isEmpty())
        {
            m_querymess_box.SetShowQueryResult(qsTitle,qsContent);

            m_querymess_box.exec();

            if(true==m_query_current_Value)
            {
                //
                m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result = "--Result \"OK\"";
            }
            else
            {
                //
                m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result= "--Result \"Cancel\"";
            }
        }
    }
    else if("QueryOSBarcode"==tmpAPIName)
    {
        //判断是否是自动线
        if(LOAD_AUTO_ROBOT==m_iOP_Multi_Option)
        {

            if("OPEN"==qsTitle)
            {
                //自动线则
                m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "--Result \"OK\" --Barcode ";

                m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "\"";
                m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += m_Robotic_OpenIMEI[tpTSCH_Numb];
                m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "\"";

                m_bLoad_OS_Open_Ref_Finish_Flag[tpTSCH_Numb]=true;
            }
            else if("SHORT"==qsTitle)
            {
                //open板已经做完，告诉主控,同时等待
                //自动化线，默认第一个放置的金板是open金板，而第二个short金板的切换时需要做LAN指令与总控PC通信
                //open金板使用结束，LAN指令通知总控PC取走open金板。并等待新放置short金板。
                //LAN指令
                //STA:CLOSS:OPEN:TS*:PASS/FAIL
                //STA:CLOSS:SHORT:TS*:PASS/FAIL
                if (::TS_LAN_SEND_OS_CLOSS_RESULT_LAN_CMD_TO_MC_PC(L"OPEN", tpTSCH_Numb, true) == true)
                {
                    //显示“open金板使用结束”LAN指令发送成功
                    qInfo()<<"OS_Open_Finish";

                    //循环检查是否有short板子的消息.
                    QTime dieTime = QTime::currentTime().addSecs(120);
                    while( QTime::currentTime() < dieTime )
                    {
                        qInfo()<<"wait for Short...";
                        Delay_MSec(2000);
                        qInfo()<<"m_bLoad_OS_Open_Ref_Finish_Flag[tpTSCH_Numb]= "<<m_bLoad_OS_Open_Ref_Finish_Flag[tpTSCH_Numb]<<"tpTSCH_Numb="<<tpTSCH_Numb;
                        if(false==m_bLoad_OS_Open_Ref_Finish_Flag[tpTSCH_Numb])
                        {
                            break;
                        }

                    }

                    if(false==m_bLoad_OS_Open_Ref_Finish_Flag[tpTSCH_Numb])
                    {
                        //
                        m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "--Result \"OK\" --Barcode ";

                        m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "\"";
                        m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += m_Robotic_OpenIMEI[tpTSCH_Numb];
                        m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "\"";
                    }


                }
//                else
//                {
//                    //显示“open金板使用结束”LAN指令发送失败

//                    qInfo()<<"OS_Open_Send_Cmd Fial";
//                    m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result +="--Result \"Cancel\" --Barcode ";
//                    m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "\"";
//                    m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += m_Robotic_OpenIMEI[tpTSCH_Numb];
//                    m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "\"";
//                    //fail
//                }
            }
        }
        else
        {
        //
            m_inputmess_box.showInputMessBox(qsTitle,qsBoardType);
            m_inputmess_box.exec();

            QString tmpstr;

            tmpstr = m_inputmess_box.m_inputLine_edit->text().trimmed();
        //
            if(true==m_query_current_Value)
            {
                //
                m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "--Result \"OK\" --Barcode ";
            }
            else
            {
                //
                m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result +="--Result \"Cancel\" --Barcode ";
                        //= "--Result \"Cancel\"" +"--Barcode"+"\""+ m_inputmess_box.m_inputLine_edit->text().trimmed()+"\"";
            }

            m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "\"";
            m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += tmpstr;
            m_cla_DL_Run_List[tpTSCH_Numb].m_Query_Messbox_Result += "\"";
        }

    }
    else if("MainControl"==tmpAPIName)
    {
        //
    }

}

void ATEUIMainWindow::OnGetMsg_IEMI_State_TS( WPARAM wParam, LPARAM lParam)
{
    //
    //QString tmpIMEI = m_cla_ate_gfc.Wchar2QString((wchar_t *)wParam);
    int tpTSCH_Numb = (int)(lParam);

    QString *text = (QString*)wParam;
    QString tmpIMEI = *text;
    //
    qInfo()<<"tmpIMEI="<<tmpIMEI;
    qInfo()<<"tpTSCH_Numb"<<tpTSCH_Numb;

    if(m_current_station==TEST_STATION_DL)
    {
        m_DLShowIMEIlist.at(tpTSCH_Numb)->setText(tmpIMEI);
    }
    else
    {
        m_TestShowIMEIlist.at(tpTSCH_Numb)->setText(tmpIMEI);
    }

    m_csIMEI_1_Key_In_Barcode[tpTSCH_Numb] = tmpIMEI;
}

LRESULT ATEUIMainWindow::OnGetMsg_Process_TCP_Accept( WPARAM wParam, LPARAM lParam )
{
    qInfo()<<"OnGetMsg_Process_TCP_Accept() Start";
    ::TS_LAN_PROCESS_TCP_ACCEPT();

    qInfo()<<"OnGetMsg_Process_TCP_Accept() End";
    //
    return LRESULT();
}

LRESULT ATEUIMainWindow::OnGetMsg_Process_TCP_OP( WPARAM wParam, LPARAM lParam )
{
    qInfo()<<"OnGetMsg_Process_TCP_OP() Start";
    ::TS_LAN_PROCESS_TCP_OP( wParam, lParam );
    //
    qInfo()<<"OnGetMsg_Process_TCP_OP() End";
    return LRESULT();
}

LRESULT ATEUIMainWindow::OnGetMsg_Get_LAN_MO_Name( WPARAM wParam, LPARAM lParam )
{
    //某个 TS_CH 测试通道在“测试运行中”，此为异常，可以直接报“初始化失败”
    qInfo()<<"OnGetMsg_Get_LAN_MO_Name() Start";

    qInfo()<<"lParam="<< (int )lParam;
    int iTS_Number = (int)lParam;//测试台号

    bool mo_run_flag=false;
    for(int i=0;i<g_iActiveChannelNums;i++)
    {
        if(this->m_irTS_DL_OBJ_State[i]==TS_OBJ_RUN)
        {
            TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC(m_cla_ate_gfc.QString2Wchar(m_StationName), L"INIT",iTS_Number, false);
            mo_run_flag=true;
        }
    }

    if(mo_run_flag==true)
    {
        //返回
        qDebug()<<"Channel Running Sation Error return back";
        return LRESULT();
    }

    //
    //qInfo()<<"wParam="<< (int )wParam;
    //跟原来的工单进对比
    QString tpqstrMO="";
    tpqstrMO= m_cla_ate_gfc.Wchar2QString((wchar_t *)wParam);
    qInfo()<<"tpqstrMO="<<tpqstrMO;
    if((tpqstrMO !=m_MoName) && (!tpqstrMO.isEmpty()))
    {
        //如果不相同，直接给主控报错
        TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC(m_cla_ate_gfc.QString2Wchar(m_StationName), L"INIT",iTS_Number, false);

        //需要弹框提示
        m_message_box.SetShowResult("Error","The Main Control MO And The tool MO Are Different");
        m_message_box.exec();

        //UIDataInit(); //重新初始化
        //ActiveStaticReset();
    }
    else
    {
        //如果MO相同，则判断工具现在的状态，如果不是INIT_Fail 状态，则给主控返回初始化成功消息
        mo_run_flag =false;
        for(int i=0;i<g_iActiveChannelNums;i++)
        {
            if(this->m_irTS_Channel_State[i]==PROGRAM_INIT_FAIL)
            {
                //TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC(m_cla_ate_gfc.QString2Wchar(m_StationName), L"INIT",iTS_Number, false);
                mo_run_flag=true;
            }
        }

        if(true == mo_run_flag)
        {
            TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC(m_cla_ate_gfc.QString2Wchar(m_StationName), L"INIT",iTS_Number, false);
        }
        else
        {
            TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC(m_cla_ate_gfc.QString2Wchar(m_StationName), L"INIT",iTS_Number, true);
        }


    }

    qInfo()<<"OnGetMsg_Get_LAN_MO_Name() End";
    //返回
    return LRESULT();
}

LRESULT ATEUIMainWindow::OnGetMsg_Start_TS_Cal_RF_Loss( WPARAM wParam, LPARAM lParam )
{
    //
    qInfo()<<"OnGetMsg_Start_TS_Cal_RF_Loss() start";
    int iTS_Number = (int)lParam-1;//测试台号
    if(this->m_irTS_DL_OBJ_State[iTS_Number]==TS_OBJ_RUN)
    {
        //
        //测试通道“正在测试中”，此为异常，直接返回“校准线损失败”
        TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC(m_cla_ate_gfc.QString2Wchar(m_RoboticAuto_StationName), L"CLOSS", iTS_Number+1, false);//测试通道“正在测试中”，此为异常，直接返回“校准线损失败”
        return LRESULT();
    }

    //
     if(m_current_station==TEST_STATION_DL)
     {
         qDebug()<<"TEST_STATION_DL not closs";
         return LRESULT();
     }
     //界面设置传统金板
     pCommonScript->SetTchPara(iTS_Number+1,L"LossCalType",L"Module_Golden");
     pCommonScript->Save();

     QString tradtionboarde = m_cla_ate_gfc.Wchar2QString((wchar_t *)wParam);
     qInfo()<<"tradtionboarde="<<tradtionboarde;
     //
     m_TestQRLineeditlist.at(iTS_Number)->setText("GOLDENSAMPLE:"+tradtionboarde);
     m_Test_StartBtnlist.at(iTS_Number)->click();

     m_robot_LOSS_Status[iTS_Number] = true;

    //编辑命令
    qInfo()<<"OnGetMsg_Start_TS_Cal_RF_Loss() end";

    return LRESULT();

}

LRESULT ATEUIMainWindow::OnGetMsg_Start_TS_OS_Cal_RF_Loss( WPARAM wParam, LPARAM lParam )
{
    //
    qInfo()<<"OnGetMsg_Start_TS_OS_Cal_RF_Loss() start";
    int iTS_Number = (int)lParam;//测试台号

    //根据测试台号，找出哪个通道进行测试
    int tmpchannel=0;
    wchar_t wcHar[256] = { 0 };
    QString qstmpchannel;
    bool ok; // 用于接收转换是否成功的结果
    for(tmpchannel=0;tmpchannel<g_iActiveChannelNums;tmpchannel++)
    {
        //跟测试台号进行对比
        memset(wcHar,0x00,256);
        pCommonScript->GetTchPara(tmpchannel+1,L"TSNum",wcHar,256);

       qstmpchannel = m_cla_ate_gfc.Wchar2QString(wcHar);

       qInfo()<<"qstmpchannel="<<qstmpchannel;

       //判断
       if(iTS_Number == qstmpchannel.toInt(&ok))
       {
           break;
       }

    }

    //如果相同，说明没有找到对应的channel,
    if(tmpchannel==g_iActiveChannelNums)
    {
        qDebug()<<"iTS_Number Error";
        return LRESULT();
    }


    if(false==m_bLoad_OS_Open_Ref_Finish_Flag[tmpchannel])
    {
        if(this->m_irTS_DL_OBJ_State[tmpchannel]==TS_OBJ_RUN)
        {
            //
            //测试通道“正在测试中”，此为异常，直接返回“校准线损失败”
            TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC(m_cla_ate_gfc.QString2Wchar(m_StationName), L"CLOSS", iTS_Number, false);//测试通道“正在测试中”，此为异常，直接返回“校准线损失败”
            return LRESULT();
        }

        //设置为界面配置为OS模式
        pCommonScript->SetTchPara(tmpchannel+1,L"LossCalType",L"OS_Golden");
        pCommonScript->Save();
    }

    m_Robotic_OpenIMEI[tmpchannel] = m_cla_ate_gfc.Wchar2QString((wchar_t *)wParam);
    qInfo()<<"m_Robotic_OpenIMEI="<<m_Robotic_OpenIMEI[tmpchannel]<<" iTS_Number="<<iTS_Number;
    qInfo()<<"m_bLoad_OS_Open_Ref_Finish_Flag="<<m_bLoad_OS_Open_Ref_Finish_Flag[tmpchannel];

    //
    if(true==m_bLoad_OS_Open_Ref_Finish_Flag[tmpchannel])
    {
        m_bLoad_OS_Open_Ref_Finish_Flag[tmpchannel]=false;
    }
    else
    {
        m_TestQRLineeditlist.at(tmpchannel)->setText("GOLDENSAMPLE:");
        m_Test_StartBtnlist.at(tmpchannel)->click();
        //标志位
        m_robot_LOSS_Status[tmpchannel] = true;
    }
    qInfo()<<"m_bLoad_OS_Open_Ref_Finish_Flag="<<m_bLoad_OS_Open_Ref_Finish_Flag[tmpchannel];
    //
   //编辑命令
   qInfo()<<"OnGetMsg_Start_TS_OS_Cal_RF_Loss() end";

   return LRESULT();

}

LRESULT ATEUIMainWindow::OnGetMsg_Start_TS_Test_Cal_RF( WPARAM wParam, LPARAM lParam )
{
    qInfo()<<"OnGetMsg_Start_TS_Test_Cal_RF() start";
    int iTS_Number = (int)lParam;//测试台号

    qInfo()<<"iTS_Number ="<<iTS_Number;

    //根据测试台号，找出哪个通道进行测试
    int tmpchannel=0;
    wchar_t wcHar[256] = { 0 };
    QString qstmpchannel;
    bool ok; // 用于接收转换是否成功的结果
    for(tmpchannel=0;tmpchannel<g_iActiveChannelNums;tmpchannel++)
    {
        //跟测试台号进行对比
        memset(wcHar,0x00,256);
        pCommonScript->GetTchPara(tmpchannel+1,L"TSNum",wcHar,256);

       qstmpchannel = m_cla_ate_gfc.Wchar2QString(wcHar);

       qInfo()<<"qstmpchannel="<<qstmpchannel;

       //判断
       if(iTS_Number == qstmpchannel.toInt(&ok))
       {
           break;
       }

    }

    //如果相同，说明没有找到对应的channel,
    if(tmpchannel==g_iActiveChannelNums)
    {
        qDebug()<<"iTS_Number Error";
        return LRESULT();
    }

    if(this->m_irTS_DL_OBJ_State[tmpchannel]==TS_OBJ_RUN)
    {
        //
        //测试通道“正在测试中”，此为异常，直接返回“校准线损失败”
        TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC(m_cla_ate_gfc.QString2Wchar(m_RoboticAuto_StationName), L"CLOSS", iTS_Number, false);//测试通道“正在测试中”，此为异常，直接返回“校准线损失败”
        return LRESULT();
    }

    QString Barcode = m_cla_ate_gfc.Wchar2QString((wchar_t *)wParam);
    qInfo()<<"Barcode="<<Barcode;

    if(m_current_station==TEST_STATION_DL)
    {
        //把二维码写到
        m_DLQRLineeditlist.at(tmpchannel)->setText(Barcode);
        m_DL_StartBtnlist.at(tmpchannel)->click();
    }
    else
    {
        //
        m_TestQRLineeditlist.at(tmpchannel)->setText(Barcode);
        m_Test_StartBtnlist.at(tmpchannel)->click();
    }

   //编辑命令
   qInfo()<<"OnGetMsg_Start_TS_Test_Cal_RF() end";

}

LRESULT ATEUIMainWindow::OnGetMsg_Modify_Fixture_Code(WPARAM wParam, LPARAM lParam )
{
    qInfo()<<"OnGetMsg_Modify_Fixture_Code() start";
    int iTS_Number = (int)lParam-1;//测试台号

    QString Fixtrue = m_cla_ate_gfc.Wchar2QString((wchar_t *)wParam);
    qInfo()<<"Fixtrue="<<Fixtrue<<" iTS_Number="<<iTS_Number;

    //修改夹具编号
    pCommonScript->SetTchPara(iTS_Number+1,L"FixtureCode",(wchar_t *)wParam);
    pCommonScript->Save();

    qInfo()<<"OnGetMsg_Modify_Fixture_Code() End";
    return LRESULT();
}


LRESULT ATEUIMainWindow::OnGetMsg_Get_LAN_Barcode( WPARAM wParam, LPARAM lParam )
{
    qInfo()<<"OnGetMsg_Get_LAN_Barcode() start";

    int iTS_Number = (int)lParam;//测试台号
    QString csData,csIMEI_Temp;

    //跟原来的工单进对比
    QString tpqstrBarcode = m_cla_ate_gfc.Wchar2QString((wchar_t *)wParam);
    tpqstrBarcode.toUpper();

    if (this->m_irTS_DL_OBJ_State[iTS_Number] <= TS_OBJ_NO_USE)//测试通道“未启用”，不能激发测试
    {
        csData.sprintf("ALARM, OnGetMsg_Get_LAN_Barcode( TS_NO_USE )TS_CH_%d, Barcode:%s",iTS_Number,tpqstrBarcode);
        qDebug()<<csData;
        return LRESULT();
    }
    else if(TS_OBJ_RUN == this->m_irTS_DL_OBJ_State[iTS_Number])//测试通道“正在测试中”，需要进一步判定处理
    {
        if(m_TestQRLineeditlist.at(iTS_Number)->text().trimmed()==tpqstrBarcode)
        {
            //两个条码数据相同，此条码正在测试中，无需特别处理，只要直接返回
            //只显示报警
            csData.sprintf("ALARM, OnGetMsg_Get_LAN_Barcode( Duplicate Barcode  )TS_CH_%d, Barcode:%s",iTS_Number,tpqstrBarcode);
            qDebug()<<csData;
        }
        else
        {
            //两个条码不同，对“新输入的条码”直接返回测试失败
            //提取IMEI
            WCSTOK_STRING_S(tpqstrBarcode,&csIMEI_Temp);
            //填写错误信息
            m_csError_Info_In_Run.sprintf("TS_Is_Running_New_Barcode_Coming");
            //组建 ts_lan 指令
            Setup_TRD_String(iTS_Number,&csIMEI_Temp,false,&csData);
            //发送、回复 ts_lan 指令
            TS_LAN_SEND_TRD_LAN_CMD_TO_MC_PC(iTS_Number, m_cla_ate_gfc.QString2Wchar(csData));

            //返回
            return LRESULT();
        }
    }

    //依据测试台号提取到了正确的“本机通道号”
    //触发start 根据DL还是Test选择测试
    if(m_current_station==TEST_STATION_DL)
    {
        //把二维码写到
        m_DLQRLineeditlist.at(iTS_Number)->setText(tpqstrBarcode);
        m_DL_StartBtnlist.at(iTS_Number)->click();
    }
    else
    {
        //
        m_TestQRLineeditlist.at(iTS_Number)->setText(tpqstrBarcode);
        m_Test_StartBtnlist.at(iTS_Number)->click();
    }

    qInfo()<<"OnGetMsg_Get_LAN_Barcode() end";
    return LRESULT();
}

bool ATEUIMainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG") //windows平台
    {
        MSG* msg = reinterpret_cast<MSG*>(message); //

        if(msg->message == WM_REDIT_MSG_TS_MSG)//消息类型 runlog
        {
            QString tpqstr = m_cla_ate_gfc.Wchar2QString((wchar_t *)msg->wParam);

            int tpTSCH_Numb = (int)(msg->lParam);

            //判断是否是DL，如果是DL则调用
            {
                OnGetMsg_DL_RunLog_Msg_TS(tpqstr,tpTSCH_Numb);
            }


        }
        else if(msg->message == WM_STATE_INFO_TS_MSG) ////测试状态
        {

            {
                OnGetMsg_DL_Show_Program_State_TS(msg->wParam,msg->lParam);
            }

        }

        else if(msg->message == WM_PROGRAM_STATE_TS_MSG)
        {

        }

        else if(msg->message == WM_SHOW_PROCESS_MSG_TS_MSG) //下载进度
        {
            //
            {
                //
                OnGetMsg_DL_Result_Process_State_TS(msg->wParam,msg->lParam);
            }
        }

        else if(msg->message == WM_SHOW_LOADRESULT_MSG_TS_MSG) //Test 结果界面显示
        {
            //测试类的项目
            if(m_current_station!=TEST_STATION_DL)
            {
                OnGetMsg_Load_Result_State_TS(msg->wParam,msg->lParam);
            }
        }
        else if(WM_SHOW_LOADRESULT_TITLE_TS_MSG  == msg->message) //标题
        {
            //
            OnGetMsg_Load_Title_State_TS(msg->wParam,msg->lParam);
        }

        else if(msg->message == WM_SHOW_IMEI_TS_MSG)//是否是IMEI信息
        {
            //
            {
                OnGetMsg_IEMI_State_TS(msg->wParam,msg->lParam);
            }
        }
        else if(msg->message == WM_WS_TCP_ACCEPT_MSG)//客户端TCP接入消息
        {
            OnGetMsg_Process_TCP_Accept(msg->wParam,msg->lParam);
        }
        else if(msg->message == WM_WS_TCP_OP_MSG)//客户端TCP读、写、关操作消息
        {
            OnGetMsg_Process_TCP_OP(msg->wParam,msg->lParam);
        }
        else if(msg->message == WM_GET_LAN_MO_NAME_MSG)//收到视觉网络工单号数据，并触发TS测试台的重新初始化4028
        {
            OnGetMsg_Get_LAN_MO_Name(msg->wParam,msg->lParam);
        }
        else if(msg->message == WM_START_TS_CLOSS_MSG)//TS测试的线损校准被激发
        {
            OnGetMsg_Start_TS_Cal_RF_Loss(msg->wParam,msg->lParam);
        }
        else if(WM_SHOW_ERRORCODEINFO_TS_MSG == msg->message)  //errorinfo errorcode
        {
            //测试类的项目
            if(m_current_station!=TEST_STATION_DL)
            {
                OnGetMsg_Load_ErrorCodeInfo_State_TS(msg->wParam,msg->lParam);
            }
        }
        else if(WM_QUERY_MESSAGR_BOX_TS_MSG == msg->message)  //
        {
            //
            OnGetMsg_Load_MessageBox_State_TS(msg->wParam,msg->lParam);
        }
        else if(WM_START_TS_OS_CLOSS_MSG == msg->message) //TS通用OS金板校准线损操作被激发
        {
            //
            OnGetMsg_Start_TS_OS_Cal_RF_Loss(msg->wParam,msg->lParam);
        }
        else if(WM_GET_LAN_BARCODE_MSG == msg->message)  //收到视觉网络条码数据，并触发TS测试台开始测试
        {
            OnGetMsg_Start_TS_Test_Cal_RF(msg->wParam,msg->lParam);
        }
        else if(WM_GET_LAN_FIXTURE_CODE_MSG == msg->message) ////收到总控程序发来的夹具二维码数据，把夹具二维码发给TS测试程序
        {
            //
            OnGetMsg_Modify_Fixture_Code(msg->wParam,msg->lParam);
        }


    }

    return QWidget::nativeEvent(eventType, message, result);//交给Qt处理
}

//
void ATEUIMainWindow::DL_Show_Program_State_TS(int TSCH_Numb,QString State)
{

    qDebug()<<"DL_Show_Program_State_TS TSCH_Numb"<<TSCH_Numb<< " State" <<State;

    if(State=="RUN")
    {

        m_DLTextLogEditlist.at(TSCH_Numb)->clear();

        //staue
        m_DLStatuslablist.at(TSCH_Numb)->setText("Working");
        m_DLStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_DLStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#909090;text-align: center;}");

        //start btn
        m_DL_StartBtnlist.at(TSCH_Numb)->setEnabled(false);
        m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #909090;width: 80px;height: 30px;font-weight: 400;background: #35414f;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}");

        // 输入框
        m_DLQRLineeditlist.at(TSCH_Numb)->setEnabled(false);

        //
        //m_DLTrianglelablist.at(TSCH_Numb)->setStyleSheet("{background:#FFFFFF;}");
        m_DLTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#909090");
        m_DLChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #909090;}");


        //
        m_DLProgBarlist.at(TSCH_Numb)->setStyleSheet("QProgressBar{font-weight: 700;width: 435px;height: 32px;border-radius: 4px 4px 4px 4px;background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);text-align: center;color: #0E1016;font: 14px \"Microsoft YaHei UI\";}"
                                             "QProgressBar::chunk{background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #c8c8c8, stop:1 #acacac);width: 10px;margin: 0.5px;text-align: center;border-radius:5px;}"
                                             );

        m_DLProgBarlist.at(TSCH_Numb)->setValue(0);
        m_DLPercentlablist.at(TSCH_Numb)->setText(QString::number(0)+"%");

        menuBar->setEnabled(false);

    }
    else if(State=="PASS")
    {
        //staue
        m_DLStatuslablist.at(TSCH_Numb)->setText("PASS");
        m_DLStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_DLStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#00FEA6;text-align: center;}");

        //start btn
        m_DL_StartBtnlist.at(TSCH_Numb)->setEnabled(true);
        //m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #909090;width: 80px;height: 30px;font-weight: 400;background: #35414f;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}");
        m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(0,254,166,1.0);border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                       "QPushButton:hover{background: rgba(0,254,166,0.5);}"
                                                       "QPushButton:pressed{background: rgba(0,254,166,0.3);}"
                                                       );

        // 输入框
        m_DLQRLineeditlist.at(TSCH_Numb)->setEnabled(true);
        m_DLQRLineeditlist.at(TSCH_Numb)->clear();

        //
        m_DLTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#00FEA6");
        m_DLChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #00FEA6;}");


        //
        m_DLProgBarlist.at(TSCH_Numb)->setStyleSheet("QProgressBar{font-weight: 700;width: 435px;height: 32px;border-radius: 4px 4px 4px 4px;background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);text-align: center;color: #0E1016;font: 14px \"Microsoft YaHei UI\";}"
                                             "QProgressBar::chunk{background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #00fda5, stop:1 #00d98e);width: 10px;margin: 0.5px;text-align: center;border-radius:5px;}"
                                             );

        //pass 进度条变成100

        m_DLProgBarlist.at(TSCH_Numb)->setValue(100);
        m_DLPercentlablist.at(TSCH_Numb)->setText(QString::number(100)+"%");

        menuBar->setEnabled(true);

    }
    else if(State=="FAIL")
    {
        //staue
        m_DLStatuslablist.at(TSCH_Numb)->setText("FAIL");
        m_DLStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_DLStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#E91737;text-align: center;}");

        //start btn
        m_DL_StartBtnlist.at(TSCH_Numb)->setEnabled(true);
        //m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #909090;width: 80px;height: 30px;font-weight: 400;background: #35414f;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}");
        m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(233,23,55,1.0);border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                       "QPushButton:hover{background: rgba(233,23,55,0.5);}"
                                                       "QPushButton:pressed{background: rgba(233,23,55,0.3);}"
                                                       );

        // 输入框
        m_DLQRLineeditlist.at(TSCH_Numb)->setEnabled(true);
        m_DLQRLineeditlist.at(TSCH_Numb)->clear();

        //
        m_DLTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#E91737");
        m_DLChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #E91737;}");


        //
        m_DLProgBarlist.at(TSCH_Numb)->setStyleSheet("QProgressBar{font-weight: 700;width: 435px;height: 32px;border-radius: 4px 4px 4px 4px;background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);text-align: center;color: #0E1016;font: 14px \"Microsoft YaHei UI\";}"
                                             "QProgressBar::chunk{background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #E91737, stop:1 #E91737);width: 10px;margin: 0.5px;text-align: center;border-radius:5px;}"
                                             );
        menuBar->setEnabled(true);
    }
    else if(State=="INIT")
    {
        m_DLStatuslablist.at(TSCH_Numb)->setText("IDLE");
        m_DLStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_DLStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#1EC085;text-align: center;}");

        m_DLProgBarlist.at(TSCH_Numb)->setValue(0);
        m_DLPercentlablist.at(TSCH_Numb)->setText(QString::number(0)+"%");

        //m_DLTextLogEditlist.at(TSCH_Numb)->clear();

        // 输入框
        m_DLQRLineeditlist.at(TSCH_Numb)->setEnabled(true);

        //start btn
        m_DL_StartBtnlist.at(TSCH_Numb)->setEnabled(true);
        m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: #1EC085;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                       "QPushButton:hover{background: rgba(144,144,144,0.5);}"
                                                       "QPushButton:pressed{background: rgba(144,144,144,0.3);}"
                                                       );

        m_DLTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#1EC085");
        m_DLChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #1EC085;}");

        //
        menuBar->setEnabled(true);


        //光标定位
        AfterTestingLocateCursor();

    }

    else if(State=="INIT_ING")
    {
        m_DLStatuslablist.at(TSCH_Numb)->setText("INIT_ING");
        m_DLStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_DLStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#909090;text-align: center;}");

        m_DLProgBarlist.at(TSCH_Numb)->setValue(0);
        m_DLPercentlablist.at(TSCH_Numb)->setText(QString::number(0)+"%");

        //start btn
        m_DL_StartBtnlist.at(TSCH_Numb)->setEnabled(false);
        m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #909090;width: 80px;height: 30px;font-weight: 400;background: #35414f;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}");

        // 输入框
        m_DLQRLineeditlist.at(TSCH_Numb)->setEnabled(false);

        m_DLTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#909090");
        m_DLChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #909090;}");

        menuBar->setEnabled(false);

    }
    else if("INIT_FAIL"==State)
    {
        //staue
        m_DLStatuslablist.at(TSCH_Numb)->setText("FAIL");
        m_DLStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_DLStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#E91737;text-align: center;}");

        //start btn
        m_DL_StartBtnlist.at(TSCH_Numb)->setEnabled(false);
        //m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #909090;width: 80px;height: 30px;font-weight: 400;background: #35414f;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}");
        m_DL_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(233,23,55,1.0);border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                       "QPushButton:hover{background: rgba(233,23,55,0.5);}"
                                                       "QPushButton:pressed{background: rgba(233,23,55,0.3);}"
                                                       );

        // 输入框
        m_DLQRLineeditlist.at(TSCH_Numb)->setEnabled(true);
        m_DLQRLineeditlist.at(TSCH_Numb)->clear();

        //
        m_DLTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#E91737");
        m_DLChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #E91737;}");


        //
        m_DLProgBarlist.at(TSCH_Numb)->setStyleSheet("QProgressBar{font-weight: 700;width: 435px;height: 32px;border-radius: 4px 4px 4px 4px;background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);text-align: center;color: #0E1016;font: 14px \"Microsoft YaHei UI\";}"
                                             "QProgressBar::chunk{background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #E91737, stop:1 #E91737);width: 10px;margin: 0.5px;text-align: center;border-radius:5px;}"
                                             );

        menuBar->setEnabled(true);


        //光标定位
        AfterTestingLocateCursor();
    }
}

void ATEUIMainWindow::Test_Show_Program_State_TS(int TSCH_Numb,QString State)
{
    qDebug()<<"Test_Show_Program_State_TS TSCH_Numb"<<TSCH_Numb<< " State" <<State;
    //
    if(State=="RUN")
    {
        //staue
        m_TestStatuslablist.at(TSCH_Numb)->setText("Working");
        m_TestStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_TestStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#909090;text-align: center;}");

        //start btn
        m_Test_StartBtnlist.at(TSCH_Numb)->setEnabled(false);
        m_Test_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #909090;width: 80px;height: 30px;font-weight: 400;background: #35414f;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}");

        // 输入框
        m_TestQRLineeditlist.at(TSCH_Numb)->setEnabled(false);


        //

        m_TestTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#909090");
        m_TestChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #909090;}");

        menuBar->setEnabled(false);
    }
    else if(State=="PASS")
    {
        //staue
        m_TestStatuslablist.at(TSCH_Numb)->setText("PASS");
        m_TestStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_TestStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#00FEA6;text-align: center;}");

        //start btn
        m_Test_StartBtnlist.at(TSCH_Numb)->setEnabled(true);
        m_Test_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(0,254,166,1.0);;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                         "QPushButton:hover{background: rgba(0,254,166,0.5);}"
                                                         "QPushButton:pressed{background: rgba(0,254,166,0.3);}"
                                                         );

        // 输入框
        m_TestQRLineeditlist.at(TSCH_Numb)->setEnabled(true);

        if(m_rdretrynum<=0)
        {
            m_TestQRLineeditlist.at(TSCH_Numb)->clear();
        }

        //

        m_TestTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#00FEA6");
        m_TestChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #00FEA6;}");


        //
        menuBar->setEnabled(true);

    }
    else if(State=="FAIL")
    {
        //staue
        m_TestStatuslablist.at(TSCH_Numb)->setText("FAIL");
        m_TestStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_TestStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#E91737;text-align: center;}");

        //start btn
        m_Test_StartBtnlist.at(TSCH_Numb)->setEnabled(true);
        m_Test_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(233,23,55,1.0);border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                         "QPushButton:hover{background: rgba(233,23,55,0.5);}"
                                                         "QPushButton:pressed{background: rgba(233,23,55,0.3);}"
                                                         );

        // 输入框
        m_TestQRLineeditlist.at(TSCH_Numb)->setEnabled(true);
        if(m_rdretrynum<=0)
        {
            m_TestQRLineeditlist.at(TSCH_Numb)->clear();
        }

        //
        m_TestTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#E91737");
        m_TestChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #E91737;}");


        //
        menuBar->setEnabled(true);

    }
    else if(State=="INIT")
    {
        m_TestStatuslablist.at(TSCH_Numb)->setText("IDLE");
        m_TestStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_TestStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#1EC085;text-align: center;}");

        //start btn
        m_Test_StartBtnlist.at(TSCH_Numb)->setEnabled(true);
        m_Test_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: #1EC085;border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                       "QPushButton:hover{background: rgba(144,144,144,0.5);}"
                                                       "QPushButton:pressed{background: rgba(144,144,144,0.3);}"
                                                       );

        m_TestTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#1EC085");
        m_TestChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #1EC085;}");

        //m_TestTextLogEditlist.at(TSCH_Numb)->clear();
        menuBar->setEnabled(true);

        //光标定位
        AfterTestingLocateCursor();
    }
    else if(State=="INIT_ING")
    {
        m_TestStatuslablist.at(TSCH_Numb)->setText("INIT_ING");
        m_TestStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_TestStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#909090;text-align: center;}");


        //m_TestTextLogEditlist.at(TSCH_Numb)->clear();
        menuBar->setEnabled(false);
    }
    else if("INIT_FAIL"==State)
    {
        //staue
        m_TestStatuslablist.at(TSCH_Numb)->setText("INIT_FAIL");
        m_TestStatuslablist.at(TSCH_Numb)->setAlignment(Qt::AlignCenter);
        m_TestStatuslablist.at(TSCH_Numb)->setStyleSheet("QLabel{font: 14px \"Microsoft YaHei UI\";color:#0E1016;background-color:#E91737;text-align: center;}");

        //start btn
        m_Test_StartBtnlist.at(TSCH_Numb)->setEnabled(false);
        m_Test_StartBtnlist.at(TSCH_Numb)->setStyleSheet("QPushButton{color: #0E1016;width: 80px;height: 30px;font-weight: 400;background: rgba(233,23,55,1.0);border-radius: 4px 4px 4px 4px;font: 16px \"Microsoft YaHei UI\";}"
                                                         "QPushButton:hover{background: rgba(233,23,55,0.5);}"
                                                         "QPushButton:pressed{background: rgba(233,23,55,0.3);}"
                                                         );

        // 输入框
        m_TestQRLineeditlist.at(TSCH_Numb)->setEnabled(true);
        if(m_rdretrynum<=0)
        {
            m_TestQRLineeditlist.at(TSCH_Numb)->clear();
        }

        //
        m_TestTrianglelablist.at(TSCH_Numb)->ChangeBackGround("#E91737");
        m_TestChanNumlist.at(TSCH_Numb)->setStyleSheet("QLabel{font-weight: 700;font: 16px \"Microsoft YaHei UI\";color: #0E1016;background: #E91737;}");

        menuBar->setEnabled(true);

        //光标定位
        AfterTestingLocateCursor();
    }

    //无论怎么样，都进行刷新
    m_TestLogTableWgtlist.at(TSCH_Numb)->scrollToBottom();  //显示到底部
}


//从字符串列表中分割出第一个不是空的字符串
void ATEUIMainWindow::WCSTOK_STRING_S(QString wcp_StrToken,QString  *csp_Get_CString)
{
     QStringList list;
     list=wcp_StrToken.split(QRegExp("[,;|]")); //正则表达式

     for(int i=0;i<list.count();i++)
     {
        if(list.at(i)!=nullptr)
        {
            *csp_Get_CString = list.at(i);

            break;
        }
     }
}

void ATEUIMainWindow::Setup_TRD_String(int i_TS_CH,const QString * wcp_Key_IMEI, bool b_Test_Result, QString * csp_TRD_String)
{
    //组建TRD字符串
    //IMEI;工位名;PASS/FAIL测试结果;工具软件名;ERRCODE;passfail描述信息;工单号;测试PC名;日期时间

    QString csTime;
    QDateTime currentDate = QDateTime::currentDateTime();

    int year = currentDate.date().year();
    int month = currentDate.date().month();
    int day = currentDate.date().day();
    int hour = currentDate.time().hour();
    int minute = currentDate.time().minute();
    int second = currentDate.time().second();

    //时间数据
    csTime.sprintf("%04d_%02d_%02d--%02d:%02d:%02d", year, month, day, hour, minute, second);

    //组建指令
    if (true == b_Test_Result)
    {
        //pass
//        csp_TRD_String->sprintf(("%s;%s;%s;%s;%08X;%d;%s;%s;%s"),
//            wcp_Key_IMEI, m_RoboticAuto_StationName, L"PASS", "QATE",
//            0x05000000, m_iTest_Time[i_TS_CH], this->m_MoName, this->m_csLocal_PC_Name, csTime);
        *csp_TRD_String = QString("%1;%2;%3;%4;%5;%6;%7;%8;%9").arg(*wcp_Key_IMEI).arg(m_RoboticAuto_StationName).arg("PASS").arg("QATE").arg("05000000").arg(m_iTest_Time[i_TS_CH])
                           .arg(this->m_MoName).arg(this->m_csLocal_PC_Name).arg(csTime);

        qInfo()<<"csTime="<<csTime;
        qInfo()<<"csp_TRD_String="<<*csp_TRD_String;
    }
    else
    {
        //fail
        if ((this->m_iError_Code_In_Test != 0x0) && (this->m_csError_Info_In_Test.length() > 0))
        {
//            csp_TRD_String->sprintf(("%s;%s;%s;%s;%08X;%s;%s;%s;%s"),
//                wcp_Key_IMEI, m_RoboticAuto_StationName,("FAIL"), "QATE",
//                this->m_iError_Code_In_Test, this->m_csError_Info_In_Test, this->m_MoName, this->m_csLocal_PC_Name, csTime);

            *csp_TRD_String = QString("%1;%2;%3;%4;%5;%6;%7;%8;%9").arg(*wcp_Key_IMEI).arg(m_RoboticAuto_StationName).arg("FAIL").arg("QATE").arg(this->m_iError_Code_In_Test,8,16,QChar('0')).arg(m_iTest_Time[i_TS_CH])
                               .arg(this->m_MoName).arg(this->m_csLocal_PC_Name).arg(csTime);
        }
        else
        {
//            csp_TRD_String->sprintf(("%s;%s;%s;%s;%08X;%s;%s;%s;%s"),
//                wcp_Key_IMEI, m_RoboticAuto_StationName, ("FAIL"), "QATE",
//                0x05000001, this->m_csError_Info_In_Run, this->m_MoName, this->m_csLocal_PC_Name, csTime);

            *csp_TRD_String = QString("%1;%2;%3;%4;%5;%6;%7;%8;%9").arg(*wcp_Key_IMEI).arg(m_RoboticAuto_StationName).arg("FAIL").arg("QATE").arg("05000001").arg(m_iTest_Time[i_TS_CH])
                               .arg(this->m_MoName).arg(this->m_csLocal_PC_Name).arg(csTime);
        }
    }
}

void ATEUIMainWindow::ATEProcessEvents()
{

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
    dialog.setMaximum(100);

    dialog.show();
    dialog.setValue(1);

    int bar_current_value=0;

    while(1)
    {
        _sleep(100);
        QCoreApplication::processEvents();      // 避免界面冻结
       // qInfo()<<"bar_current_value="<<bar_current_value;

        if(bar_current_value==101)
        {
            break;
        }

        dialog.setValue(bar_current_value);

        bar_current_value++;
    }

//    if(m_Init_WorkResource_Flag==true)
//    {
//       close();
//    }
//    else
//    {
//        //弹框提示失败
//        MyMessageBox message_box(this,"Error","Q_ATE_CONTROLLER_Init_WorkResource() Fail");
//        message_box.exec();

//    }
}

void ATEUIMainWindow::resizeEvent(QResizeEvent *event)
{

//    qDebug()<<"event->size()="<<event->size();
//    qDebug()<<"event->oldSize()="<<event->oldSize();

    //根据进行拉升和原始的对比长度(1024,768)
    int x= event->size().width()-1024;
    int y= event->size().height()-768;

    //
     m_closeBtn->setGeometry(976+x,8,24,24);
     m_maxBtn->setGeometry(932+x,8,24,24);
     m_minBtn->setGeometry(888+x,8,24,24);

     //(0,41)
//     m_pcfgRFStackedWidget->setMinimumWidth(1024);
//     m_pcfgRFStackedWidget->setMinimumHeight(728);
     m_pcfgRFStackedWidget->setGeometry(0,41,1024+x-10,728+y-10);
    // m_TestRfTabWgt->setGeometry(8,8,756+x-20,664+48+y-20);

//     m_pStackedWidget->move(245,0);
//     m_pStackedWidget->setMinimumWidth(764+8+8);
//     m_pStackedWidget->setMinimumHeight(664+48+8);
     m_pStackedWidget->setGeometry(245,0,764+8+8+x-10,664+48+8+y-10);

     m_TestRfTabWgt->setGeometry(8,8,756+x-10,664+48+y-10);
     m_TestRfTabWgt->setGeometry(8,8,756+x-10,664+48+y-10);

     //每个控件都要移动
     //
     for(int i=0;i<4;i++)
     {
         //groupbox的大小和样式
          m_TestThreadGrouplist.at(i)->setGeometry(16,i*155+i*8+16+i*y/4,725+x,155+y/4);

         //
         //button在哪里
         m_Test_StartBtnlist.at(i)->move(555+x,15);


         //lineedit 510 30
         m_TestQRLineeditlist.at(i)->move(40,15);
         m_TestQRLineeditlist.at(i)->setGeometry(40,15,510+x,30);
         //状态
         m_TestStatuslablist.at(i)->move(651+x,1);

         //test time
         m_TestTiptesttimelablist.at(i)->move(405+x,57);

         //show  test time
         //m_TestShowtesttimelablist.at(i)->setText("00:48:32");
         m_TestShowtesttimelablist.at(i)->move(485+x,57);

         // test No.
         m_TestTiptestNolablist.at(i)->move(405+x,83);

         //show test No.
         m_TestShowtestNolablist.at(i)->move(485+x,83);

         //rate
         m_TestTipRatelablist.at(i)->move(403+x,110);

         // show Rate
         m_TestShowRatelablist.at(i)->move(485+x,110);
         //errorcode
         //m_TestShowErrorcodelist.at(i)->setText("ErrorCode1234567891234567891234500");
        // m_TestShowErrorcodelist.at(i)->move(42+x,135);

         //
         m_TestLogTableWgtlist.at(i)->setGeometry(16,20,720+x-10,392+y/2);

         m_TestLogTableWgtlist.at(i)->setColumnWidth(1, 320+x/3);

         //颜色交叉进行
         //m_TestLogTableWgtlist.at(i) ->setAlternatingRowColors(true);


         //logshow QTextEdit
         m_TestTextLogEditlist.at(i)->setGeometry(16, 430+y/2, 720+x-10, (224+y/2-10));

     }

     //DL 界面
//     m_dlcommlab->move(8,0);
//     m_dlcommlab->setMinimumWidth(764);
//     m_dlcommlab->setMinimumHeight(48);

     m_dlcommlab->setGeometry(8,0,764+x-10,48);

     DLscrollAreawgt->setMinimumHeight(665+168*(g_channelnum-4));
     //m_dlscrollArea->setMaxmumHeight(665+168*(6-4)+y-30);

//     scrollArea->setMinimumWidth(765);
//     scrollArea->setMinimumHeight(664);
//     scrollArea->setWidget(DLscrollAreawgt);
//     scrollArea->move(8,56);

     m_dlscrollArea->setGeometry(8,56,765+x,665+y-10);

     for(int i=0;i<32;i++)
     {
         //m_DLThreadGrouplist.at(i)->setMinimumWidth(754);
         //m_DLThreadGrouplist.at(i)->setMaximumWidth(754);
         //m_DLThreadGrouplist.at(i)->setMinimumHeight(160);
         //m_DLThreadGrouplist.at(i)->setMaximumHeight(160);

         //m_DLThreadGrouplist.at(i)->move(0,i*160+i*8);

         m_DLThreadGrouplist.at(i)->setGeometry(0,i*160+i*8,754+x,160);

         m_DLStatuslablist.at(i)->move(680+x,1);

         m_DLTextLogEditlist.at(i)->setGeometry(505+x, 45, 240, 100);

         m_DL_StartBtnlist.at(i)->move(412+x,10);

//         m_DLQRLineeditlist.at(i)->move(60+x,10);
//         m_DLQRLineeditlist.at(i)->setStyleSheet("QLineEdit{width: 345px;height: 30px;border-radius: 4px 4px 4px 4px;background: #252e39;font-weight: 400;font: 14px \"Microsoft YaHei UI\";text-align: left;color: #FFFFFF;}");
         m_DLQRLineeditlist.at(i)->setGeometry(60, 10, 345+x, 30);

//         m_DLProgBarlist.at(i)->move(60,50);
//         m_DLProgBarlist.at(i)->setStyleSheet("QProgressBar{font-weight: 700;width: 435px;height: 32px;border-radius: 4px 4px 4px 4px;background: qlineargradient(x1:0, y1:0, x2:0, y2:1 ,stop:0 #393939, stop:1 #464646);text-align: center;color: #0E1016;font: 14px \"Microsoft YaHei UI\";}"
//                                              "QProgressBar::chunk{background-color: #E91737;width: 10px;margin: 0.5px;text-align: center;border-radius:5px;}"
//                                              );

         m_DLProgBarlist.at(i)->setGeometry(60, 50, 435+x, 32);

         m_DLTiptesttimelablist.at(i)->move(330+x,87);
         m_DLShowtesttimelablist.at(i)->move(410+x,87);

         m_DLTiptestNolablist.at(i)->move(330+x,110);
         m_DLShowtestNolablist.at(i)->move(410+x,110);

         m_DLTipRatelablist.at(i)->move(330+x,132);
         m_DLShowRatelablist.at(i)->move(410+x,132);

     }

//     m_cfgwholelabel->move(12,0);
//     m_cfgwholelabel->setMinimumWidth(993);
// //    wholelabel->setMaximumWidth(993);
//     m_cfgwholelabel->setMinimumHeight(708);
// //    wholelabel->setMaximumHeight(708);
     m_cfgwholelabel->setGeometry(12, 0, 993+x, 708+y);
     m_cfgcomsetlabel->move(834+x,20);
     m_cfgStationLine->setGeometry(0, 56, 993+x, 2);
     m_CommsetTableWgt->setGeometry(273,84,710+x,530+y);
     m_CommsetTableWgt->setColumnWidth(0, 230+x/3);
     m_CommsetTableWgt->setColumnWidth(1, 230+x/3);

     m_CommsetSaveBtn->setGeometry(480+x/2,645+y,136,40);
     m_CommsetCancelBtn->setGeometry(480+136+20+x/2,645+y,136,40);
}

//接下来就是对三个鼠标事件的重写
void ATEUIMainWindow::mousePressEvent(QMouseEvent *event)
{
    if( (event->button() == Qt::LeftButton) )
    {
        mouse_press = true;
        mousePoint = event->globalPos() - this->pos();

    // event->accept();
    }

}
void ATEUIMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // if(event->buttons() == Qt::LeftButton){ //如果这里写这行代码，拖动会有点问题
    if(mouse_press && (mousePoint!=QPoint()))
    {
    move(event->globalPos() - mousePoint);
    // event->accept();
    }
}
void ATEUIMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    mousePoint = QPoint();

    (void) *event;
}

void ATEUIMainWindow::myQueryMessBoxSlot(bool value)
{
    //
    m_query_current_Value = value;
}

void ATEUIMainWindow::RDStressTest(int TSCH_Numb)
{
    //判断是否大于1
    if(m_rdretrynum>0)
    {
        //从新跑一次
        qInfo()<<"m_rdretrynum=="<<m_rdretrynum;
        m_Test_StartBtnlist.at(TSCH_Numb)->click();

        m_rdretrynum--;
    }
}

void ATEUIMainWindow::StationNameToRoboticStationName()
{
    if("RF_CAL"==m_StationName)
    {
        m_RoboticAuto_StationName = "CAL";
    }
    else if("RF_FT"==m_StationName)
    {
        m_RoboticAuto_StationName = "FT";
    }
    else
    {
        m_RoboticAuto_StationName = m_StationName;
    }

    qInfo()<<"RoboticAuto_StationName="<<m_RoboticAuto_StationName;
}


void ATEUIMainWindow::AfterTestingLocateCursor()
{
    //自动线不需要光标定位
    if(m_iOP_Multi_Option != LOAD_AUTO_ROBOT)
    {
        //判断站位
        if(TEST_STATION_DL == m_current_station)
        {
            for(int i=0;i<g_iActiveChannelNums;i++)
            {
                if(this->m_irTS_DL_OBJ_State[i]!=TS_OBJ_RUN)
                {
                    m_DLQRLineeditlist.at(i)->setFocus();
                    break;
                }
            }
        }
        else
        {
            //
            for(int i=0;i<g_iActiveChannelNums;i++)
            {
                if(this->m_irTS_DL_OBJ_State[i]!=TS_OBJ_RUN)
                {
                    m_TestQRLineeditlist.at(i)->setFocus();
                    break;
                }
            }
        }
    }

}

//根据结果，
void ATEUIMainWindow::UpdateProgramStatus(int TSCH_Numb,QString State)
{
       //
    if(State=="RUN")
    {
        //staue
        m_irTS_Channel_State[TSCH_Numb]=PROGRAM_RUN;
    }
    else if(State=="PASS")
    {
        m_irTS_Channel_State[TSCH_Numb]=PROGRAM_PASS;
    }
    else if(State=="FAIL")
    {
        m_irTS_Channel_State[TSCH_Numb]=PROGRAM_FAIL;
    }
    else if(State=="INIT")
    {
        m_irTS_Channel_State[TSCH_Numb]=PROGRAM_IDLE;
    }
    else if(State=="INIT_ING")
    {
        m_irTS_Channel_State[TSCH_Numb]= PROGRAM_INIT_ING;
    }
    else if("INIT_FAIL"==State)
    {
        m_irTS_Channel_State[TSCH_Numb]= PROGRAM_INIT_FAIL;
    }

}
