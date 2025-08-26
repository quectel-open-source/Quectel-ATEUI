#include "mypathlossdlg.h"

MyPathLossDlg::MyPathLossDlg(QWidget *parent,int channel) : QDialog(parent)
{
    //
    m_channel = channel;

    m_New_ant_display_mode = false;
    //初始化
    UIinit();

    InitData();

}

void MyPathLossDlg::UIinit()
{
    //
    this->setWindowFlag(Qt::FramelessWindowHint);

    //设置大小
    this->setFixedSize(800,600);

    this->setStyleSheet("MyPathLossDlg{ border: 1px solid #808080;background-color: #2A2A2A;border-radius: 4px;}");

    //QLabel
    QLabel *titlabel = new QLabel(this);
    //titlabel->setFixedSize(800,30);
    titlabel->move(20,16);
    titlabel->setText("RF Loss");
    titlabel->setStyleSheet("QLabel{color:#FFFFFF;line-height: 24px; font-size: 18px;text-align: left;background-color:#2A2A2A;}");

    //生成
    tptablewgt = new QTableWidget(this);

    tptablewgt->setGeometry(16,50,768,495);
    //设置表格中每一行的表头
    tptablewgt->setColumnCount(5);
    tptablewgt->setHorizontalHeaderLabels(QStringList() << "ANT" << "Type" << "Band"<<"Tx Input"<<"Rx Output");

    tptablewgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    tptablewgt->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
//    tptablewgt->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);

    //去掉行号
    tptablewgt->verticalHeader()->setHidden(true);

    //去掉网格线
    tptablewgt->setShowGrid(false);

    //颜色交叉进行  QHeaderView::section {background-color: %1;}
    tptablewgt ->setAlternatingRowColors(true);

    //表头不需要高亮
    tptablewgt->horizontalHeader()->setHighlightSections(false);

    tptablewgt->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color: #646464;color: #FFFFFF;font-weight: 700;font: 16px \"Microsoft YaHei UI\";}");
    //tptablewgt->horizontalHeader()->setStyleSheet("QHeaderView::section,QTableCornerButton::section{background:#646464;color: #FFFFFF;font-weight: 700;font: 16px \"Microsoft YaHei UI\";}");
    tptablewgt->setStyleSheet("QTableWidget{background: #262626;border: 1px solid #8a8a8a;color: #FFFFFF;font: 14px \"Microsoft YaHei UI\";}"
                              "QTableView::item{border: 1px solid #8a8a8a;background: #252e39;}"
                              "QTableWidget::item:selected { background-color: #4D6380; color: #FFFFFF; }"
                              );

    //取出滚动条，然后再设置
    tptablewgt->verticalScrollBar()->setStyleSheet( "QScrollBar:vertical{background:#2B2B2B;width:6px;border-radius: 4px;padding-top:0px;padding-bottom:0px;padding-left:0px;padding-right:0px;}"    //左/右预留位置
                                                                     "QScrollBar::handle:vertical{background:#383838;border-radius:4px;min-height:96px;}"    //滑块最小高度
                                                                     "QScrollBar::handle:vertical:hover{background:#383838;border-radius: 4px;}"//鼠标触及滑块样式/滑块颜色
                                                                     "QScrollBar::add-line:vertical{border: 0px; height: 0px; }"//向下箭头样式
                                                                     "QScrollBar::sub-line:vertical{border: 0px; height: 0px; }"//向上箭头样式
                                                                     "QScrollBar::add-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在垂直区域
                                                                     "QScrollBar::sub-page:vertical{background: #090c13;border-radius:2px;}"//滑块所在区域
                                                                      );

    //去掉行号
    tptablewgt->verticalHeader()->setHidden(true);
    //tptablewgt->verticalHeader()->setFixedHeight(30);

    //选中整行
    tptablewgt->setSelectionBehavior(QAbstractItemView::SelectRows);

    //OK btn
    OkBtn = new QPushButton(this);
    OkBtn->setText("Save");

    OkBtn->move(250,560);
    OkBtn->setStyleSheet("QPushButton{color: #27E29E;width: 96px;height: 28px;border: 1px solid #27E29E;font-size: 14px;font-weight: 400;background: rgba(15,80,64,1.0);border-radius: 4px 4px 4px 4px;font: 18px \"Microsoft YaHei UI\";}"
                                    "QPushButton:hover{background: rgba(15,80,64,0.6);}"
                                    "QPushButton:pressed{background: rgba(15,80,64,0.1);}"
                                    );

    OkBtn->setFocusPolicy(Qt::NoFocus);

    connect(OkBtn, SIGNAL(clicked()), this, SLOT(Okclicked()));

    CancelBtn = new QPushButton(this);
    CancelBtn->setText("Quit");
    CancelBtn->setStyleSheet("QPushButton{color: #27e29e;width: 96px;height: 28px;border: 1px solid #98989a;font-size: 14px;font-weight: 400;background: transparent;border-radius: 4px 4px 4px 4px;font: 18px \"Microsoft YaHei UI\";}"
                                    "QPushButton:hover{background: rgba(0,0,0,0.4);}"
                                    "QPushButton:pressed{background: rgba(0,0,0,0.0);}"
                             );


    CancelBtn->move(450,560);
    connect(CancelBtn, SIGNAL(clicked()), this, SLOT(Cancelclicked()));

    CancelBtn->setFocusPolicy(Qt::NoFocus);

}

void MyPathLossDlg::InitData()
{
    wchar_t antwcHar[256] = { 0 };
    wchar_t techwcHar[256] = { 0 };
    //wchar_t bandwcHar[256] = { 0 };
    wchar_t txwcHar[256] = { 0 };
    wchar_t rxwcHar[256] = { 0 };

    pathlossProductInfoScript = (Q_ATE_Controller_ProductInfo_Interface*)Q_ATE_CONTROLLER_GetProductInfo();
    if(pathlossProductInfoScript==nullptr)
    {
        qDebug()<<"Q_ATE_Controller_ProductInfo_Interface is NULL";
        return;
    }

    pPathLossCommonScript = (Q_ATE_Controller_CommonScript_Interface*)Q_ATE_CONTROLLER_GetCommonSetting();
    if (pPathLossCommonScript == NULL)
    {
        qDebug()<<"Q_ATE_CONTROLLER_GetCommonSetting is NULL";
        return;
    }

    pPathlossCfg = (Q_ATE_Controller_TCH_LOSSTABLE_Interface*)(pPathLossCommonScript->GetTchLossTable(m_channel));
    if (pPathlossCfg == NULL)
    {
        qDebug()<<"GetTchLossTable is NULL"<<m_channel;
        return;
    }

    //取得天线，制式，以及band.
    QString tmpant;
    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"ANT_Prop_HW_Type_Names"));
    m_pathlossAntlist = tmpant.split(",");   
    qInfo()<<"m_pathlossAntlist="<<tmpant; //打印到debug_log中，方便debug

    //取得天线的属性，ANT_Prop_HW_DES，GPS,WLAN,Bluetooth,LORA,SPARKLINK
    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"ANT_Prop_SW_Numbs"));
    m_ANT_Prop_HW_DESList = tmpant.split(","); //取得所有天线蜂窝部分的标志位
    qInfo()<<"m_ANT_Prop_HW_DESList="<<tmpant; //打印到debug_log中，方便debug


    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"ANT_Prop_GPS_SW_Numbs"));
    m_ANT_Prop_GPSList = tmpant.split(","); //取得所有天线GPS部分的标志位
    qInfo()<<"m_ANT_Prop_GPSList="<<tmpant; //打印到debug_log中，方便debug

    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"ANT_Prop_WLAN_SW_Numbs"));
    m_ANT_Prop_WLANList = tmpant.split(","); //取得所有天线WLAN部分的标志位
    qInfo()<<"m_ANT_Prop_WLANList="<<tmpant; //打印到debug_log中，方便debug

    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"ANT_Prop_BlueTooth_SW_Numbs"));
    m_ANT_Prop_BlueToothList = tmpant.split(","); //取得所有天线蓝牙部分的标志位
    qInfo()<<"m_ANT_Prop_BlueToothList="<<tmpant; //打印到debug_log中，方便debug


    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"ANT_Prop_LORA_SW_Numbs"));
    m_ANT_Prop_LORAList = tmpant.split(","); //取得所有天线lora部分的标志位
    qInfo()<<"m_ANT_Prop_LORAList="<<tmpant; //打印到debug_log中，方便debug

    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"ANT_Prop_SPARKLINK_SW_Numbs"));
    m_ANT_Prop_SPARKLINKList = tmpant.split(","); //取得所有天线星闪部分的标志位
    qInfo()<<"m_ANT_Prop_SPARKLINKList="<<tmpant; //打印到debug_log中，方便debug

    //判断天线的数量和属性的数量是否一致，如果是一致的，则说明用新标准，如果不一致不能使用
    if( (m_pathlossAntlist.count()==m_ANT_Prop_HW_DESList.count())&&
        (m_pathlossAntlist.count()==m_ANT_Prop_GPSList.count())&&
        (m_pathlossAntlist.count()==m_ANT_Prop_WLANList.count())&&
        (m_pathlossAntlist.count()==m_ANT_Prop_BlueToothList.count())&&
        (m_pathlossAntlist.count()==m_ANT_Prop_LORAList.count())&&
        (m_pathlossAntlist.count()==m_ANT_Prop_SPARKLINKList.count())
      )
    {
        //
        m_New_ant_display_mode = true; //把新天线显示方式赋值为true，用新天线方式来显示
    }

    qInfo()<<"m_New_ant_display_mode="<<m_New_ant_display_mode;


    //取得2G的band
    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"Bands_2G"));
    qInfo()<<"2G="<<tmpant;
    if(!tmpant.isEmpty())
    {
        m_pathlossBands_2Glist = tmpant.split(",");

        for(int i=0;i<m_pathlossBands_2Glist.count();i++)
        {
            if("0"==m_pathlossBands_2Glist.at(i))
            {
                //GSM
                m_pathlossBands_2GGSMlist.append("900");

            }
            else if("1"==m_pathlossBands_2Glist.at(i))
            {
                m_pathlossBands_2GGSMlist.append("1800");

            }
            else if("2"==m_pathlossBands_2Glist.at(i))
            {
                m_pathlossBands_2GGSMlist.append("1900");

            }
            else if("3"==m_pathlossBands_2Glist.at(i))
            {
                m_pathlossBands_2GGSMlist.append("850");
            }
            else
            {
                qDebug()<<"Incorrect GSM band is NULL";
                return;
            }
        }
    }
    //取得wcmda的band
    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"WCDMA_Bands"));
    qInfo()<<"3G="<<tmpant;
    if(!tmpant.isEmpty())
    {
        m_pathlossWCDMAlist = tmpant.split(",");
    }
    //取得LTE的band
    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"LTE_Bands"));    
    qInfo()<<"4G="<<tmpant;
    if(!tmpant.isEmpty())
    {
        m_pathlossLTElist = tmpant.split(",");
    }

    //取得NR的band
   if(nullptr== pathlossProductInfoScript->GetParaValueByName(L"NR_5G_SUB6_Bands"))
   {
        qInfo()<<"NR_5G_SUB6_Bands is NULL";
   }
   else
   {
       qInfo()<<pathlossProductInfoScript->GetParaValueByName(L"NR_5G_SUB6_Bands")[0];
   }

    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"NR_5G_SUB6_Bands"));
    qInfo()<<"5G="<<tmpant;
    if(!tmpant.isEmpty())
    {
        m_pathlossNR_5G_SUB6list = tmpant.split(",");
    }

    //取得NBIoT_Bands的band
    if(nullptr== pathlossProductInfoScript->GetParaValueByName(L"NBIoT_Bands"))
    {
         qInfo()<<"NBIoT_Bands is NULL";
    }
    else
    {
        qInfo()<<pathlossProductInfoScript->GetParaValueByName(L"NBIoT_Bands")[0];
    }

    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"NBIoT_Bands"));
    qInfo()<<"NBIoT_Bands="<<tmpant;
    if(!tmpant.isEmpty())
    {
        m_pathlossNBIoT_Bandslist = tmpant.split(",");
    }

    //取得C2K_Bands
    if(nullptr== pathlossProductInfoScript->GetParaValueByName(L"C2K_Bands"))
    {
         qInfo()<<"C2K_Bands is NULL";
    }
    else
    {
        qInfo()<<pathlossProductInfoScript->GetParaValueByName(L"C2K_Bands")[0];
    }
    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"C2K_Bands"));
    qInfo()<<"C2K_Bands="<<tmpant;
    if(!tmpant.isEmpty())
    {
        m_pathlossC2K_Bandslist = tmpant.split(",");
    }

    //取得TD_Bands
    if(nullptr== pathlossProductInfoScript->GetParaValueByName(L"TD_Bands"))
    {
         qInfo()<<"TD_Bands is NULL";
    }
    else
    {
        qInfo()<<pathlossProductInfoScript->GetParaValueByName(L"TD_Bands")[0];
    }
    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"TD_Bands"));
    qInfo()<<"TD_Bands="<<tmpant;
    if(!tmpant.isEmpty())
    {
        m_pathlossTD_Bandslist = tmpant.split(",");
    }

    //取得CV2X_Bands
    if(nullptr== pathlossProductInfoScript->GetParaValueByName(L"CV2X_Bands"))
    {
         qInfo()<<"CV2X_Bands is NULL";
    }
    else
    {
        qInfo()<<pathlossProductInfoScript->GetParaValueByName(L"CV2X_Bands")[0];
    }
    tmpant = m_pathloss_logon_gfc.Wchar2QString(pathlossProductInfoScript->GetParaValueByName(L"CV2X_Bands"));
    qInfo()<<"CV2X_Bands="<<tmpant;
    if(!tmpant.isEmpty())
    {
        m_pathlossCV2X_Bandslist = tmpant.split(",");
    }


    //wlanblueteeth
    m_pathlossWlanBTlist.append("WLAN_2.4G");
    m_pathlossWlanBTlist.append("WLAN_5G_A");
    m_pathlossWlanBTlist.append("WLAN_5G_B");
    m_pathlossWlanBTlist.append("WLAN_5G_C");
    m_pathlossWlanBTlist.append("WLAN_6G_A");
    m_pathlossWlanBTlist.append("WLAN_6G_B");
    m_pathlossWlanBTlist.append("WLAN_6G_C");
    m_pathlossWlanBTlist.append("BLUETOOTH");


    //WLAN
    m_pathlossWLANlist.append("WLAN_2.4G");
    m_pathlossWLANlist.append("WLAN_5G_A");
    m_pathlossWLANlist.append("WLAN_5G_B");
    m_pathlossWLANlist.append("WLAN_5G_C");
    m_pathlossWLANlist.append("WLAN_6G_A");
    m_pathlossWLANlist.append("WLAN_6G_B");
    m_pathlossWLANlist.append("WLAN_6G_C");

    //store Wlan list 存到xml线损时候使用

    //blueteeth
    m_pathlossBlueToothlist.append("BLUETOOTH");

    //store blueteeth 存到xml 线损使用

    //GPS
    m_pathlossGPSlist.append("1");
    m_pathlossGPSlist.append("2");
    m_pathlossGPSlist.append("5");

    //LORA
    m_pathlossLORAlist.append("1");
    m_pathlossLORAlist.append("2");

    //星闪
    m_pathlossSPARKLINKlist.append("1");
    m_pathlossSPARKLINKlist.append("2");
    m_pathlossSPARKLINKlist.append("3");

    //2G
    //ShowTxRxPathloss(L"GSM",m_pathlossBands_2Glist);
    //WCDMA
    //ShowTxRxPathloss(L"WCDMA",m_pathlossLTElist);
    //LTE

    //判断是否是新的天线显示方式
    if(true == m_New_ant_display_mode)
    {
        //调用新的函数来显示新的方法线损值
        Show_New_Mode_Ant_Pathloss();
    }
    else //如果是老的，还是用老的方法来显示天线线损
    {
        for(int i=0;i<m_pathlossAntlist.count();i++)
        {
            QString qsant = m_pathlossAntlist.at(i);

            int iant = qsant.toInt();
            if(ANT_TYPE_IDX_WIFI_BLU==iant ||ANT_TYPE_IDX_WIFI_MIMO==iant||ANT_TYPE_IDX_WIFI_BLU_GPS==iant
                    ||ANT_TYPE_IDX_BLU_ONLY==iant||ANT_TYPE_IDX_WIFI_0==iant
                    ||ANT_TYPE_IDX_R2_WIFI_BLU==iant)
            {
                ShowTxRxPathloss(i,L"WLAN",m_pathlossWlanBTlist);
            }
            else if(ANT_TYPE_IDX_GPS==iant )
            {
                ShowTxRxPathloss(i,L"GPS",m_pathlossGPSlist);
            }
            else if(ANT_TYPE_IDX_DIV_GPS==iant)
            {
                ShowTxRxPathloss(i,L"GPS",m_pathlossGPSlist);
                ShowTxRxPathloss(i,L"GSM"     ,m_pathlossBands_2GGSMlist);
                ShowTxRxPathloss(i,L"WCDMA"   ,m_pathlossWCDMAlist);
                ShowTxRxPathloss(i,L"LTE"     ,m_pathlossLTElist);
                ShowTxRxPathloss(i,L"NR5GSUB6",m_pathlossNR_5G_SUB6list);
                ShowTxRxPathloss(i,L"NB-IoT",m_pathlossNBIoT_Bandslist);
                ShowTxRxPathloss(i,L"C2K",m_pathlossC2K_Bandslist);
                ShowTxRxPathloss(i,L"TDSCDMA",m_pathlossTD_Bandslist);
                ShowTxRxPathloss(i,L"V2X",m_pathlossCV2X_Bandslist);
            }
            else
            {
                ShowTxRxPathloss(i,L"GSM"     ,m_pathlossBands_2GGSMlist);
                ShowTxRxPathloss(i,L"WCDMA"   ,m_pathlossWCDMAlist);
                ShowTxRxPathloss(i,L"LTE"     ,m_pathlossLTElist);
                ShowTxRxPathloss(i,L"NR5GSUB6",m_pathlossNR_5G_SUB6list);
                ShowTxRxPathloss(i,L"NB-IoT",m_pathlossNBIoT_Bandslist);
                ShowTxRxPathloss(i,L"C2K",m_pathlossC2K_Bandslist);
                ShowTxRxPathloss(i,L"TDSCDMA",m_pathlossTD_Bandslist);
                ShowTxRxPathloss(i,L"V2X",m_pathlossCV2X_Bandslist);
            }
        }
    }

    // 连接信号和槽
    connect(tptablewgt,SIGNAL(itemChanged(QTableWidgetItem *)),this, SLOT(MyitemChanged(QTableWidgetItem *)));

    //--------------------------------------------------------------------
    wchar_t wcHar[256] = { 0 };

    m_pathlossStationcfg = (Q_ATE_Controller_STATIONCONFIG_Interface*)Q_ATE_CONTROLLER_GetStationInfo();
    if(nullptr==m_pathlossStationcfg)
    {
        qDebug()<<"Q_ATE_CONTROLLER_GetStationInfo() is NULL";
        return;
    }
    else
    {
        //
        //读取
        memset(wcHar,0x00,sizeof(wcHar));
        m_pathlossStationcfg->GetParaValue(L"OFFLINE_MODE", wcHar, 256);
        m_PathlossOffline_Status = m_pathloss_logon_gfc.Wchar2QString(wcHar);
        qDebug()<<" InitData() ENABLE_FDB "<<m_PathlossOffline_Status;

        //读取是否是传统金板
        memset(wcHar,0x00,sizeof(wcHar));
        m_pathlossStationcfg->GetParaValue(L"STATION_WORKMODE", wcHar, 256);
        m_PathlossTradGold_Status = m_pathloss_logon_gfc.Wchar2QString(wcHar);
        qDebug()<<" InitData() m_PathlossTradGold_Status "<<m_PathlossTradGold_Status;
    }

    //
    if("false"==m_PathlossOffline_Status)
    {
        //判断是否是传统金板,不是传统金板，则不能修改线损
        if("1"!=m_PathlossTradGold_Status)
        {
            OkBtn->setEnabled(false);
        }

        int row =tptablewgt->rowCount();

//        for(int j=0;j<row;j++)
//        {
//            for(int k=3;k<5;k++)
//            {
//                tptablewgt->item(j, k)->setFlags(tptablewgt->item(j,k)->flags() & (~Qt::ItemIsEditable));
//            }
//        }
    }
}

void MyPathLossDlg::Show_New_Mode_Ant_Pathloss()
{
    qInfo()<<"----Show_New_Mode_Ant_Pathloss() Start-----";
    //进行天线大循环
    for(int i=0;i<m_pathlossAntlist.count();i++)
    {
        //根据天线的具体属性，来决定是否显示
        //先判断是否有GPS天线，只要值大于等于0，就表示有GPS
       qInfo()<<"i=="<<i<<"m_ANT_Prop_GPS="<<m_ANT_Prop_GPSList.at(i).toInt();
       if(m_ANT_Prop_GPSList.at(i).toInt()>=0)
       {
           //
           ShowCellularPathloss(i,L"GPS",m_pathlossGPSlist);
       }
       //判断是否是蜂窝的天线,只要值大于等于0，就表示有蜂窝
       qInfo()<<"i=="<<i<<"m_ANT_Prop_HW_DESList="<<m_ANT_Prop_HW_DESList.at(i).toInt();
       if(m_ANT_Prop_HW_DESList.at(i).toInt()>=0)
       {
           //调用蜂窝函数显示线损
           ShowCellularPathloss(i,L"GSM"     ,m_pathlossBands_2GGSMlist);
           ShowCellularPathloss(i,L"WCDMA"   ,m_pathlossWCDMAlist);
           ShowCellularPathloss(i,L"LTE"     ,m_pathlossLTElist);
           ShowCellularPathloss(i,L"NR5GSUB6",m_pathlossNR_5G_SUB6list);
           ShowCellularPathloss(i,L"NB-IoT",m_pathlossNBIoT_Bandslist);
           ShowCellularPathloss(i,L"C2K",m_pathlossC2K_Bandslist);
           ShowCellularPathloss(i,L"TDSCDMA",m_pathlossTD_Bandslist);
           ShowCellularPathloss(i,L"V2X",m_pathlossCV2X_Bandslist);
       }
       //判断是否是WLAN，只要值大于等于0，就表示有WLAN
        qInfo()<<"i=="<<i<<"m_ANT_Prop_WLANList="<<m_ANT_Prop_WLANList.at(i).toInt();
       if(m_ANT_Prop_WLANList.at(i).toInt()>=0)
       {
           ShowCellularPathloss(i,L"WLAN",m_pathlossWLANlist);
       }
       //判断是否是蓝牙产品，只要值大于等于0，就表示有蓝牙
        qInfo()<<"i=="<<i<<"m_ANT_Prop_BlueToothList="<<m_ANT_Prop_BlueToothList.at(i).toInt();
       if(m_ANT_Prop_BlueToothList.at(i).toInt()>=0)
       {
           //
           ShowCellularPathloss(i,L"BLUETOOTH",m_pathlossBlueToothlist); //
       }
       //判断是否是LORA产品，只要值大于等于0，就表示有LORA天线
       if(m_ANT_Prop_LORAList.at(i).toInt()>=0)
       {
           ShowCellularPathloss(i,L"LORA",m_ANT_Prop_LORAList);
       }

       //判断是否是星闪产品，只要值大于等于0，就表示有星闪天线
       qInfo()<<"i=="<<i<<"m_ANT_Prop_SPARKLINKList="<<m_ANT_Prop_SPARKLINKList.at(i).toInt();
       if(m_ANT_Prop_SPARKLINKList.at(i).toInt()>=0)
       {
           //调用通用函数显示星闪
           ShowCellularPathloss(i,L"SPARKLINK",m_pathlossSPARKLINKlist);
       }
    }
}

void MyPathLossDlg::ShowTxRxPathloss(int Ant,wchar_t* Tech,QStringList PathlossTechList)
{
    wchar_t txwcHar[256] = { 0 };
    wchar_t rxwcHar[256] = { 0 };

    QTableWidgetItem *tmpItem=nullptr;


    QString qsant = m_pathlossAntlist.at(Ant);

    int iant = qsant.toInt();

    //判断天线是否是WiFi，蓝牙，GPS的天线
    if(ANT_TYPE_IDX_GPS==iant)
    {
        ShowGPSpathloss(Ant,PathlossTechList);
    }
    else if(ANT_TYPE_IDX_DIV_GPS==iant)
    {
        if(L"GPS"==Tech)
        {
            ShowGPSpathloss(Ant,PathlossTechList);
        }
        else
        {
            ShowCellularPathloss(Ant,Tech,PathlossTechList);
        }

    }
    else if(ANT_TYPE_IDX_WIFI_BLU==iant||ANT_TYPE_IDX_R2_WIFI_BLU==iant)
    {

        ShowWifiPathloss(Ant,PathlossTechList);

        ShowBLUETOOTHPathloss(Ant,PathlossTechList);
    }
    else if(ANT_TYPE_IDX_WIFI_MIMO==iant||ANT_TYPE_IDX_WIFI_0==iant)
    {
        ShowWifiPathloss(Ant,PathlossTechList);
    }
    else if(ANT_TYPE_IDX_BLU_ONLY==iant)
    {
        ShowBLUETOOTHPathloss(Ant,PathlossTechList);
    }
    else if(ANT_TYPE_IDX_WIFI_BLU_GPS==iant)
    {
        ShowWifiPathloss(Ant,PathlossTechList);

        ShowBLUETOOTHPathloss(Ant,PathlossTechList);

        ShowGPSpathloss(Ant,PathlossTechList);

    }
    else //正常的天线
    {
        for(int j=0;j<PathlossTechList.count();j++)
        {

            QString sband = PathlossTechList.at(j);
            //天线-----------------------------------------------------------------
            int row =tptablewgt->rowCount();
            tptablewgt->insertRow(row);
            tptablewgt->setItem(row,0,new QTableWidgetItem(QString::number(Ant)));

            //制式-----------------------------------------------------------------
            tptablewgt->setItem(row,1,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(Tech)));

            //band
            tptablewgt->setItem(row,2,new QTableWidgetItem(sband));


            //Tx------------------------------------------------------------------
            memset(txwcHar,0x00,sizeof(txwcHar));
            pPathlossCfg->GetTxCableLoss(txwcHar,256,sband.toInt(),Tech,Ant);
            tptablewgt->setItem(row,3,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(txwcHar)));
            tmpItem = tptablewgt->item(row,3);
            tmpItem->setTextAlignment(Qt::AlignCenter);

            //Rx------------------------------------------------------------------
            memset(rxwcHar,0x00,sizeof(rxwcHar));
            pPathlossCfg->GetRxCableLoss(rxwcHar,256,sband.toInt(),Tech,Ant);
            tptablewgt->setItem(row,4,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(rxwcHar)));
            tmpItem = tptablewgt->item(row,4);
            tmpItem->setTextAlignment(Qt::AlignCenter);

            //属性
            for(int j=0;j<3;j++)
            {
                tptablewgt->item(row, j)->setTextAlignment(Qt::AlignCenter);
                tptablewgt->item(row, j)->setFlags(tptablewgt->item(row,j)->flags() & (~Qt::ItemIsEditable));
            }

            tptablewgt->setRowHeight(row,10);
        }
    }

    tmpItem=nullptr;
}

//show wifi pathloss
void  MyPathLossDlg::ShowWifiPathloss(int Ant_index,QStringList PathlossTechList)
{
    wchar_t txwcHar[256] = { 0 };
    wchar_t rxwcHar[256] = { 0 };
    QTableWidgetItem *tmpItem=nullptr;

    for(int j=0;j<7;j++) //wlan 2.5到6G
    {
        //天线-----------------------------------------------------------------
        int row =tptablewgt->rowCount();
        tptablewgt->insertRow(row);
        tptablewgt->setItem(row,0,new QTableWidgetItem(QString::number(Ant_index)));

        //制式-----------------------------------------------------------------
        {
            tptablewgt->setItem(row,1,new QTableWidgetItem("WLAN"));
        }

        //band
        tptablewgt->setItem(row,2,new QTableWidgetItem(PathlossTechList.at(j)));

        //Tx------------------------------------------------------------------
        memset(txwcHar,0x00,sizeof(txwcHar));
        {
             pPathlossCfg->GetTxCableLoss(txwcHar,256,j,L"WLAN",Ant_index);
        }

        tptablewgt->setItem(row,3,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(txwcHar)));
        tmpItem = tptablewgt->item(row,3);
        tmpItem->setTextAlignment(Qt::AlignCenter);

        //Rx------------------------------------------------------------------
        memset(rxwcHar,0x00,sizeof(rxwcHar));
        {
            pPathlossCfg->GetRxCableLoss(rxwcHar,256,j,L"WLAN",Ant_index);
        }

        tptablewgt->setItem(row,4,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(rxwcHar)));
        tmpItem = tptablewgt->item(row,4);
        tmpItem->setTextAlignment(Qt::AlignCenter);

        //属性
        for(int j=0;j<3;j++)
        {
            tptablewgt->item(row, j)->setTextAlignment(Qt::AlignCenter);
            tptablewgt->item(row, j)->setFlags(tptablewgt->item(row,j)->flags() & (~Qt::ItemIsEditable));
        }

        tptablewgt->setRowHeight(row,10);

    }

    tmpItem=nullptr;
}

//show BLUETOOTH pathloss
void MyPathLossDlg::ShowBLUETOOTHPathloss(int Ant_index,QStringList PathlossTechList)
{
    wchar_t txwcHar[256] = { 0 };
    wchar_t rxwcHar[256] = { 0 };

    QTableWidgetItem *tmpItem=nullptr;

    for(int j=0;j<1;j++)
    {
        //天线-----------------------------------------------------------------
        int row =tptablewgt->rowCount();
        tptablewgt->insertRow(row);
        tptablewgt->setItem(row,0,new QTableWidgetItem(QString::number(Ant_index)));

        //制式-----------------------------------------------------------------
        {
            tptablewgt->setItem(row,1,new QTableWidgetItem("BLUETOOTH"));
        }

        //band
        tptablewgt->setItem(row,2,new QTableWidgetItem(PathlossTechList.at(j+7)));

        //Tx------------------------------------------------------------------
        memset(txwcHar,0x00,sizeof(txwcHar));
        {
             pPathlossCfg->GetTxCableLoss(txwcHar,256,j,L"BLUETOOTH",Ant_index);
        }

        tptablewgt->setItem(row,3,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(txwcHar)));
        tmpItem = tptablewgt->item(row,3);
        tmpItem->setTextAlignment(Qt::AlignCenter);

        //Rx------------------------------------------------------------------
        memset(rxwcHar,0x00,sizeof(rxwcHar));
        {
            pPathlossCfg->GetRxCableLoss(rxwcHar,256,j,L"BLUETOOTH",Ant_index);
        }

        tptablewgt->setItem(row,4,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(rxwcHar)));
        tmpItem = tptablewgt->item(row,4);
        tmpItem->setTextAlignment(Qt::AlignCenter);

        //属性
        for(int j=0;j<3;j++)
        {
            tptablewgt->item(row, j)->setTextAlignment(Qt::AlignCenter);
            tptablewgt->item(row, j)->setFlags(tptablewgt->item(row,j)->flags() & (~Qt::ItemIsEditable));
        }

        tptablewgt->setRowHeight(row,10);
    }

    tmpItem=nullptr;
}

//show GPS pathloss
void MyPathLossDlg::ShowGPSpathloss(int Ant_index,QStringList PathlossTechList)
{
    wchar_t txwcHar[256] = { 0 };
    wchar_t rxwcHar[256] = { 0 };

    QTableWidgetItem *tmpItem=nullptr;

    //
    for(int j=0;j<3;j++)
    {
        QString sband = PathlossTechList.at(j);
        //天线-----------------------------------------------------------------
        int row =tptablewgt->rowCount();
        tptablewgt->insertRow(row);
        tptablewgt->setItem(row,0,new QTableWidgetItem(QString::number(Ant_index)));
        //制式-----------------------------------------------------------------
        tptablewgt->setItem(row,1,new QTableWidgetItem("GPS"));
        //band
        tptablewgt->setItem(row,2,new QTableWidgetItem(PathlossTechList.at(j)));

        //Tx------------------------------------------------------------------
        memset(txwcHar,0x00,sizeof(txwcHar));
        pPathlossCfg->GetTxCableLoss(txwcHar,256,sband.toInt(),L"GPS",Ant_index);
        tptablewgt->setItem(row,3,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(txwcHar)));
        tmpItem = tptablewgt->item(row,3);
        tmpItem->setTextAlignment(Qt::AlignCenter);

        //Rx------------------------------------------------------------------
        memset(rxwcHar,0x00,sizeof(rxwcHar));
        pPathlossCfg->GetRxCableLoss(rxwcHar,256,sband.toInt(),L"GPS",Ant_index);
        tptablewgt->setItem(row,4,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(rxwcHar)));
        tmpItem = tptablewgt->item(row,4);
        tmpItem->setTextAlignment(Qt::AlignCenter);
        //属性
        for(int j=0;j<3;j++)
        {
            tptablewgt->item(row, j)->setTextAlignment(Qt::AlignCenter);
            tptablewgt->item(row, j)->setFlags(tptablewgt->item(row,j)->flags() & (~Qt::ItemIsEditable));
        }

        tptablewgt->setRowHeight(row,10);
    }

    tmpItem=nullptr;
}

void MyPathLossDlg::ShowCellularPathloss(int Ant_index,wchar_t* Tech,QStringList PathlossTechList)
{
    wchar_t txwcHar[256] = { 0 };
    wchar_t rxwcHar[256] = { 0 };

    QTableWidgetItem *tmpItem=nullptr;

    for(int j=0;j<PathlossTechList.count();j++)
    {

        QString sband = PathlossTechList.at(j);
        //天线-----------------------------------------------------------------
        int row =tptablewgt->rowCount();
        tptablewgt->insertRow(row);
        tptablewgt->setItem(row,0,new QTableWidgetItem(QString::number(Ant_index)));

        //制式-----------------------------------------------------------------
        tptablewgt->setItem(row,1,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(Tech)));

        //band
        tptablewgt->setItem(row,2,new QTableWidgetItem(sband));


        //Tx------------------------------------------------------------------
        memset(txwcHar,0x00,sizeof(txwcHar));
        //判断是否是WIFI(0-6)和BLUETOOTH(0)，如果是，则需要使用index做为参数传入
        if(Tech==L"WLAN" ||Tech==L"BLUETOOTH")
        {
            pPathlossCfg->GetTxCableLoss(txwcHar,256,j,Tech,Ant_index);
        }
        else
        {
            pPathlossCfg->GetTxCableLoss(txwcHar,256,sband.toInt(),Tech,Ant_index);
        }
        qInfo()<<sband.toInt()<<Tech<<Ant_index<<txwcHar;
        tptablewgt->setItem(row,3,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(txwcHar)));
        tmpItem = tptablewgt->item(row,3);
        tmpItem->setTextAlignment(Qt::AlignCenter);

        //Rx------------------------------------------------------------------
        memset(rxwcHar,0x00,sizeof(rxwcHar));
        if(Tech==L"WLAN"||Tech==L"BLUETOOTH")
        {
            pPathlossCfg->GetRxCableLoss(rxwcHar,256,j,Tech,Ant_index);
        }
        else
        {
            pPathlossCfg->GetRxCableLoss(rxwcHar,256,sband.toInt(),Tech,Ant_index);
        }
        tptablewgt->setItem(row,4,new QTableWidgetItem(m_pathloss_logon_gfc.Wchar2QString(rxwcHar)));
        tmpItem = tptablewgt->item(row,4);
        tmpItem->setTextAlignment(Qt::AlignCenter);

        //属性
        for(int j=0;j<3;j++)
        {
            tptablewgt->item(row, j)->setTextAlignment(Qt::AlignCenter);
            tptablewgt->item(row, j)->setFlags(tptablewgt->item(row,j)->flags() & (~Qt::ItemIsEditable));
        }

        tptablewgt->setRowHeight(row,10);
    }

     tmpItem=nullptr;
}

void MyPathLossDlg::Okclicked()
{
    //首先清除common table 里面所有的内容 contents可以删除
//    tptablewgt->clearContents();

//    while (tptablewgt->rowCount() > 0) {

//        tptablewgt->removeRow(0);
//    }

    //
    int      antwcHar  =0;
    wchar_t* techwcHar =nullptr;
    int      iband     = 0 ;
    wchar_t* txwcHar   = nullptr;
    wchar_t* rxwcHar   = nullptr;

    QLineEdit   *lineedit = nullptr;

    for(int i=0;i<tptablewgt->rowCount();i++)
    {
        //天线
        antwcHar =(tptablewgt->item(i,0)->text().trimmed()).toInt();

        QString qsant = m_pathlossAntlist.at(antwcHar);

        int iant = qsant.toInt();

        //Tech 制式
        techwcHar =m_pathloss_logon_gfc.QString2Wchar(tptablewgt->item(i,1)->text().trimmed());

        //band
        if("WLAN_2.4G"==tptablewgt->item(i,2)->text().trimmed())
        {
            iband=0;
        }
        else if("WLAN_5G_A"==tptablewgt->item(i,2)->text().trimmed())
        {
            iband=1;
        }
        else if("WLAN_5G_B"==tptablewgt->item(i,2)->text().trimmed())
        {
            iband=2;
        }
        else if("WLAN_5G_C"==tptablewgt->item(i,2)->text().trimmed())
        {
            iband=3;
        }
        else if("WLAN_6G_A"==tptablewgt->item(i,2)->text().trimmed())
        {
            iband=4;
        }
        else if("WLAN_6G_B"==tptablewgt->item(i,2)->text().trimmed())
        {
            iband=5;
        }
        else if("WLAN_6G_C"==tptablewgt->item(i,2)->text().trimmed())
        {
            iband=6;
        }
        else if("BLUETOOTH"==tptablewgt->item(i,2)->text().trimmed())
        {
            iband=0;
        }
        else
        {
             iband =  tptablewgt->item(i,2)->text().trimmed().toInt();
        }

        //Tx value
        txwcHar = m_pathloss_logon_gfc.QString2Wchar(tptablewgt->item(i,3)->text().trimmed());

        //Rx value
        rxwcHar = m_pathloss_logon_gfc.QString2Wchar(tptablewgt->item(i,4)->text().trimmed());

        //调用保存
        if(pPathlossCfg->SetCableLoss(antwcHar,techwcHar,iband,txwcHar,rxwcHar)==false)
        {
            m_message_box.SetShowResult("Error","SetCableLoss Fail!!!");
            m_message_box.exec();

            return;
        }

    }



    if(pPathlossCfg->Save()==true)
    {
        m_message_box.SetShowResult("Success","Save PathLoss Success!");
        m_message_box.exec();
    }
    else
    {
        m_message_box.SetShowResult("Error","Save PathLoss Fail!!!");
        m_message_box.exec();
    }


     techwcHar = nullptr;
     txwcHar   = nullptr;
     rxwcHar   = nullptr;
     lineedit  = nullptr;

}

void MyPathLossDlg::Cancelclicked()
{
    this->close();
}

void MyPathLossDlg::MyitemChanged(QTableWidgetItem *item)
{
    // 这里处理编辑后的逻辑
    qDebug() << "Item changed at (" << item->row() << ", " << item->column() << "): " << item->text();
    //判断是否是多行被选中，如果多行被选中，则断开connect，修改多行中的内容，最后在从新connect的.
    QList<QTableWidgetSelectionRange> ranges = tptablewgt->selectedRanges();

    int RowCount=0;
    int count=ranges.count();
    qDebug() <<"ranges count= "<< count;

    for(int i=0;i<count;i++)
    {
        int topRow=ranges.at(i).topRow();
        int bottomRow=ranges.at(i).bottomRow();
        for(int j=topRow;j<=bottomRow;j++)
        {
            RowCount++;
        }
    }

    //判断是否大于1，如果大于1，则修改所有行的内容
    if(RowCount>1)
    {
        // 取消信号和槽
        disconnect(tptablewgt,SIGNAL(itemChanged(QTableWidgetItem *)),this, SLOT(MyitemChanged(QTableWidgetItem *)));

        //修改Rx和Tx的值

        for(int i=0;i<count;i++)
        {
            int topRow=ranges.at(i).topRow();
            int bottomRow=ranges.at(i).bottomRow();
            for(int j=topRow;j<=bottomRow;j++)
            {
                //Tx修改的值
                tptablewgt->item(j,3)->setText(item->text().trimmed());
                //Rx的值修改
                tptablewgt->item(j,4)->setText(item->text().trimmed());
            }
        }

        //结束后再次打开信号
        connect(tptablewgt,SIGNAL(itemChanged(QTableWidgetItem *)),this, SLOT(MyitemChanged(QTableWidgetItem *)));

    }

}

//可以在构造函数中初始一下last变量用其成员函数setX,setY就是了
//接下来就是对三个鼠标事件的重写
void MyPathLossDlg::mousePressEvent(QMouseEvent *event)
{
    if( (event->button() == Qt::LeftButton) )
    {
        mouse_press = true;
        mousePoint = event->globalPos() - this->pos();

    // event->accept();
    }

}
void MyPathLossDlg::mouseMoveEvent(QMouseEvent *event)
{
    // if(event->buttons() == Qt::LeftButton){ //如果这里写这行代码，拖动会有点问题
    if(mouse_press && (mousePoint!=QPoint()))
    {
    move(event->globalPos() - mousePoint);
    // event->accept();
    }
}
void MyPathLossDlg::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    mousePoint = QPoint();
}

void MyPathLossDlg::keyPressEvent(QKeyEvent *event)
{
    // 检查按键是否为特定键，例如回车键
    if (event->key() == Qt::Key_Return) {
        // 处理回车键按下的事件
        qDebug() << "Enter key pressed!";
        //判断是否多行被选中，
    }
    else if(event->key() == Qt::Key_Enter)
    {
        // 处理回车键按下的事件
        qDebug() << "Key_Enter key pressed!";
    }
    else {
        // 调用父类的keyPressEvent来处理其他按键
        QDialog::keyPressEvent(event);
    }
}


