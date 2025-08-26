#ifndef ATEUIMAINWINDOW_H
#define ATEUIMAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QWidget>
#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLine>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QList>
#include <QVector>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QHeaderView>
#include <QProgressBar>
#include <QFontDatabase>
#include <QDebug>
#include <QPoint>
#include <QGroupBox>
#include <QScrollArea>
#include <QPalette>
#include <QProgressBar>
#include <QTextEdit>
#include <QTabWidget>
#include <QScrollBar>
#include <QLibrary>
#include <QApplication>
#include <QEventLoop>
#include <QTimer>
#include <QTimerEvent>
#include <cdlrun.h>
#include <QAction>
#include <windows.h>
#include <QTreeWidget>
#include <QDateTime>
#include <QSettings>
#include <QTextCodec>
#include <QHostInfo>
#include <QCoreApplication>
#include <QDir>
#include <QTime>
#include <QProcess>
#include <QTranslator>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QThread>

#include <QFileDialog>
#include <QString>
#include"qlogoutput.h"

#include "logondialog.h"
#include "TriangleWidget.h"
#include "ate_controller.h"
#include "Global_Header.h"
#include "ate_controller_defines_ext.h"
#include "mymessagebox.h"
#include "mychannelnumdlg.h"
#include "mypathlossdlg.h"
#include "mytoolversion.h"
#include "MyComboBox.h"
#include "myquerymessagebox.h"
#include "myinputmessbox.h"


//定义最大通道数32
#define MAX_CHANNEL_NUM        32
#define MAX_THIRDPARTY_NUM     20
#define DL_USE_CHANNEL_NUM      4
#define TEST_USE_CHANNEL_NUM    4

class ATEUIMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ATEUIMainWindow(QWidget *parent = nullptr);
    ~ATEUIMainWindow();

    void UIinit();
    void OnlyOneInit();
    void UIDataInit();
    void MenuInit();
    void ATE_Ctrol_Lib_Init();

    void SetMainWigdetUI();
    void SetPublicWgtUI();
    void SetDownLoadWgtUI();
    void SetCfgRFWgtUI();
    void SetCfgPageWidgetUI();
    void SetChannelInterfaceUI();
    void ChooseDLWgtUI(int channelnum);
    void ChooseTestWgtUI(int channelnum);

    void SetTestMainWgtUI();

    void Delay_MSec(unsigned int msec);

    //定时器
    void timerEvent(QTimerEvent *event);

    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    int m_Id1;
    int m_Id2;

    QTabWidget *tabWidget;

    QWidget *m_pwind;
    QWidget *m_commomwgt;
    QStackedWidget *m_pStackedWidget;

    QStackedWidget *m_pcfgRFStackedWidget;
    QWidget *CfgPageWidget;
    QWidget *RFPageWidget;

    QMenuBar* menuBar;
    QMenu*    filename;
    QAction* setnumchannels;
    QAction* commonsetdlg;
    QAction* testlineset;
    QMenu* StatisticalReset;
    QAction* actSResetdlg;

    QMenu* Goldplatecalibration;

    QAction* chinaesedlg;
    QAction* englishdlg;

    QPushButton *commonbackBtn;
    QLabel  *m_cfgwholelabel;
    QLabel  *m_cfgcomsetlabel;
    QLineEdit *m_cfgStationLine;

    QLabel  *m_molabel;
    QLineEdit *m_molineedit;

    QLabel  *m_oclabel;
    QLineEdit *m_oclineedit;

    QLabel  *m_linelabel;
    QLineEdit *m_linelineedit;

    QLabel  *m_operatorlabel;
    QLineEdit *m_operatorlineedit;

    QLabel  *m_logopiclabel;
    QLineEdit *m_logopiclineedit;

    //
    QWidget *TestPageWidget;
    QWidget *WriteCheckPageWidget;
    QWidget *DownLoadPageWidget;

    int RF_Thread_Count;
    int DL_Thread_Count;

    QVector<QWidget*> m_RfThreadWidgetlist;
    QVector<QWidget*> m_DLThreadWidgetlist;

    //登录框
    LogonDiaLog *m_logonDlg;
    int         m_current_station;

    void LogFileInit();
    void LogonInit();

    QPoint mousePoint;
    bool mouse_press;

    QWidget *m_menuBarwgt;
    QPushButton *m_closeBtn;
    QPushButton *m_minBtn;
    QPushButton *m_maxBtn;

    QWidget *m_publicWgt;
    QLabel *showStationlab;
    QLabel *m_showVersionlab;
    QLabel *showMOlab;
    QLabel *showOClab;
    QLabel *showLinelab;
    QLabel *showOperatorlab;
    //QLabel *showTestModelab;
    QTextEdit *showtestModeEdit;
    QLabel *m_OfflineModelabel;
    QLabel *m_showOfflineModelabel;
    QLabel *m_DLScriptlabel;
    QLabel *m_showDLScriptlabel;
    QLabel *m_DLFunctionDll;
    QLabel *m_showDLFunctionDll;

    QLabel  *m_dlcommlab;
    QLabel *QGMRSRClab;
    QLineEdit *QGMRSRCEdit;

    QScrollArea* m_dlscrollArea;

    QWidget *DLscrollAreawgt;



    QVector<QGroupBox*>              m_DLThreadGrouplist;
    QVector<QPushButton*>            m_DL_StartBtnlist;
    QVector<QLineEdit*>              m_DLQRLineeditlist;
    QVector<QProgressBar*>           m_DLProgBarlist;
    QVector<QLabel*>                 m_DLStatuslablist;
    QVector<QLabel*>                 m_DLPercentlablist;
    QVector<TriangleWidget*>         m_DLTrianglelablist;
    QVector<QLabel*>                 m_DLChanNumlist;
    QVector<QLabel*>                 m_DLTipIMEIlist;
    QVector<QLabel*>                 m_DLShowIMEIlist;
    QVector<QLabel*>                 m_DLTipFixNolist;
    QVector<QLabel*>                 m_DLShowFixNolist;
    QVector<QLabel*>                 m_DLTippassnumlablist;
    QVector<QLabel*>                 m_DLShowpassnumlablist;
    QVector<QLabel*>                 m_DLTipfailnumlablist;
    QVector<QLabel*>                 m_DLShowfailnumlablist;
    QVector<QLabel*>                 m_DLTiptesttimelablist;
    QVector<QLabel*>                 m_DLShowtesttimelablist;
    QVector<QLabel*>                 m_DLTiptestNolablist;
    QVector<QLabel*>                 m_DLShowtestNolablist;
    QVector<QLabel*>                 m_DLTipRatelablist;
    QVector<QLabel*>                 m_DLShowRatelablist;
    QVector<QTextEdit*>              m_DLTextLogEditlist;

    //Test
    QTabWidget                    *m_TestRfTabWgt;
    QWidget                       *m_Testchannelwgt;
    QVector<QGroupBox*>            m_TestThreadGrouplist;
    QVector<QWidget*>              m_TestShowLogWgtlist;
    QVector<QTableWidget*>         m_TestLogTableWgtlist;
    QVector<QTextEdit*>            m_TestTextLogEditlist;
    QVector<QPushButton*>          m_Test_StartBtnlist;
    QVector<QLineEdit*>            m_TestQRLineeditlist;
    QVector<QLabel*>               m_TestStatuslablist;
    QVector<TriangleWidget*>       m_TestTrianglelablist;
    QVector<QLabel*>               m_TestChanNumlist;
    QVector<QLabel*>               m_TestTipIMEIlist;
    QVector<QLabel*>               m_TestShowIMEIlist;
    QVector<QLabel*>               m_TestTipFixNolist;
    QVector<QLabel*>               m_TestShowFixNolist;
    QVector<QLabel*>               m_TestTippassnumlablist;
    QVector<QLabel*>               m_TestShowpassnumlablist;
    QVector<QLabel*>               m_TestTipfailnumlablist;
    QVector<QLabel*>               m_TestShowfailnumlablist;
    QVector<QLabel*>               m_TestTiptesttimelablist;
    QVector<QLabel*>               m_TestShowtesttimelablist;
    QVector<QLabel*>               m_TestTiptestNolablist;
    QVector<QLabel*>               m_TestShowtestNolablist;
    QVector<QLabel*>               m_TestTipRatelablist;
    QVector<QLabel*>               m_TestShowRatelablist;
    QVector<QLabel*>               m_TestShowErrorcodelist;


     int      m_DL_Current_Channel;
     ATEUIMainWindow *m_ATEUIMainWindow;


     QTreeWidget *m_CommsetTree;
     QTableWidget *m_CommsetTableWgt;
     QPushButton  *m_CommsetSaveBtn;
     QPushButton  *m_CommsetCancelBtn;
     int          m_currentTreeIndex;
     QTreeWidgetItem           *m_topLevelItem;

     void SetCommsetTreeItem();
     



    //--------------------DL 参数---------------------------------------
     CDLRun m_cla_DL_Run_List[MAX_CHANNEL_NUM];
     HANDLE m_hThread_DL_Handle_List[MAX_CHANNEL_NUM];
     DWORD m_dwThread_DL_ID_List[MAX_CHANNEL_NUM];
     
     HANDLE m_hThread_DL_INIT_Handle_List[MAX_CHANNEL_NUM];
     DWORD m_dwThread_DL_INIT_ID_List[MAX_CHANNEL_NUM];

     HANDLE m_hThread_Timer_Handle_List[MAX_CHANNEL_NUM];
     DWORD m_dwThread_Timer_ID_List[MAX_CHANNEL_NUM];

     int m_irTS_DL_OBJ_State[MAX_CHANNEL_NUM];//各通道的状态
     int m_irTS_Channel_State[MAX_CHANNEL_NUM];//各通道的状态
     void UpdateProgramStatus(int TSCH_Numb,QString State);
     void DO_DL_OP(int i_TS_CH);
     void DO_DL_OP_TIMER(int i_TS_CH);
     CGFC m_cla_ate_gfc;
     QSettings  *m_statistics_ini;

     QSettings  *m_yanfa_ini;
     int        m_rdretrynum;

     void OnGetMsg_DL_RunLog_Msg_TS(QString RunLogMsg, int TSCH_Numb);
     void OnGetMsg_DL_Show_Program_State_TS( WPARAM wParam, LPARAM lParam);
     void OnGetMsg_DL_Result_Process_State_TS( WPARAM wParam, LPARAM lParam);
     void OnGetMsg_Load_Result_State_TS( WPARAM wParam, LPARAM lParam);
     void OnGetMsg_Load_Title_State_TS( WPARAM wParam, LPARAM lParam);
     void OnGetMsg_IEMI_State_TS( WPARAM wParam, LPARAM lParam);
     void OnGetMsg_Load_ErrorCodeInfo_State_TS( WPARAM wParam, LPARAM lParam);

     void OnGetMsg_Load_MessageBox_State_TS( WPARAM wParam, LPARAM lParam);

     //----------------------robot------------------------------------
     LRESULT OnGetMsg_Process_TCP_Accept( WPARAM wParam, LPARAM lParam );
     LRESULT OnGetMsg_Process_TCP_OP( WPARAM wParam, LPARAM lParam );
     LRESULT OnGetMsg_Get_LAN_MO_Name( WPARAM wParam, LPARAM lParam );
     LRESULT OnGetMsg_Start_TS_Cal_RF_Loss( WPARAM wParam, LPARAM lParam );
     LRESULT OnGetMsg_Get_LAN_Barcode( WPARAM wParam, LPARAM lParam );

     LRESULT OnGetMsg_Start_TS_OS_Cal_RF_Loss( WPARAM wParam, LPARAM lParam );
     LRESULT OnGetMsg_Start_TS_Test_Cal_RF( WPARAM wParam, LPARAM lParam );
     LRESULT OnGetMsg_Modify_Fixture_Code(WPARAM wParam, LPARAM lParam );

     QString m_Robotic_OpenIMEI[TEST_USE_CHANNEL_NUM];
     bool m_bLoad_OS_Open_Ref_Finish_Flag[TEST_USE_CHANNEL_NUM];	//是否“已放置short金板，并已收到short金板条码数据”的标识

     //-------------------- 参数---------------------------------------
     void DL_Show_Program_State_TS(int TSCH_Numb,QString State);
     void Test_Show_Program_State_TS(int TSCH_Numb,QString State);

     void Save_Result_Statistics_Data(int i_TS_CH,int b_Result);
     void Update_Statistics_Data(int i_TS_CH);
     void Clear_Statistics_Data(int i_TS_CH);
     int m_iTS_Test_Pass_Quan;
     int m_iTS_Test_Fail_Quan;
     double m_dTS_Test_FPY_Percent;
     int m_channel_iTS_Test_Pass_Quan[32];
     int m_channel_iTS_Test_Fail_Quan[32];


     MyMessageBox m_message_box; //弹框信息


     //common   setting
     Q_ATE_Controller_CommonScript_Interface*        pCommonScript = NULL;
     Q_ATE_Controller_ProductInfo_Interface*         pProductInfoScript = NULL;
     Q_ATE_Controller_STATIONCONFIG_Interface *      m_ateStationcfg =NULL; //读取Station的值
     Q_ATE_Controller_MOInfo_Interface*              m_ateMoCfg=NULL;

     QString m_StationName;
     QString m_RoboticAuto_StationName;
     void StationNameToRoboticStationName();
     QString m_MoName;
     QString m_Test_Process;
     QString m_Offline_Status;
     QString m_DownLoadScript_Status;
     QString m_DLFunctionDll_Status;
     QString m_ateDownloadFW;
     QString m_testscript_version;
     QString m_function_dll_version;
     QString m_IS_ECN;

     int m_iOP_Multi_Option;  //工具类型
     HANDLE m_hThread_Handle_5;
     DWORD m_dwThread_ID_5;
     int m_Robot_Current_channel;
     QString m_csError_Info_In_Run;
     QString m_csError_Info_In_Test;	//错误信息
     int m_iError_Code_In_Test;		//测试中产生的错误代码

     void WCSTOK_STRING_S(QString wcp_StrToken,QString  *csp_Get_CString); //从字符串列表中分割出第一个不是空的字符串
     void Setup_TRD_String(int i_TS_CH,const QString * wcp_Key_IMEI, bool b_Test_Result, QString * csp_TRD_String);
     int m_iTest_Time[32];
     QString m_csLocal_PC_Name;
     bool m_robot_LOSS_Status[32];

     QString m_csIMEI_1_Key_In_Barcode[MAX_CHANNEL_NUM];

     HANDLE m_hThread_MC_PC_Handle;
     DWORD m_dwThread_MC_PC_ID;

     void DO_SEND_MC_PC_OP(int i_TS_CH);

     void ATEProcessEvents(); //防止界面卡死函数

     int         m_thirdpartyTsNum;
     int         m_thirdpartyIndexNum;
     QString     m_thirdpartyItem[MAX_THIRDPARTY_NUM];
     QString     m_thirdpartyName[MAX_THIRDPARTY_NUM];
     QPushButton *thirdpartybtn[MAX_THIRDPARTY_NUM];
     QProcess    *myprocess;

     QPushButton *m_filepathbtn;


     double *m_barvalue;

     QAction* setchannel1LineLoss;
     QAction* setchannel2LineLoss;
     QAction* setchannel3LineLoss;
     QAction* setchannel4LineLoss;
     QVector<QAction*>       m_channelLineLosslist;

     MyQueryMessageBox m_querymess_box; //弹框
     bool m_query_current_Value;
     QString m_qsMessBox_ret;

     MyInputMessBox   m_inputmess_box;

    int m_Test_Result_Count[4];

    QDateTime m_Test_Result_Time;



    QTranslator mTranslator; //中英文
    void RetranslateUI();

    void RDStressTest(int TSCH_Numb); //研发压力测试

    //电源标志位
    bool ChannelPowerFlag[MAX_CHANNEL_NUM];

    //测试完成后光标定位
    void AfterTestingLocateCursor();

    bool     m_Max_Normal_Flag;

    // 创建监视器
    QFutureWatcher<void> watcher1;
    QFutureWatcher<void> watcher2;

    QFuture<void> future1[TEST_USE_CHANNEL_NUM];
    QFuture<void> future2[TEST_USE_CHANNEL_NUM];

private slots:
    void CloseMainWidget();
    void MinMainWidget();
    void MaxMainWidget();
    void DL_Btn_Start();
    void Test_Btn_Start();

    void CommonSetting();
    void ActiveStaticReset();
    void MainSetting();
    void AboutThisToolVer();
    void LanguageForTest();
    void LanguageForChinese();
    void LanguageForEnglish();

    void CommsetTreesClicked(QTreeWidgetItem* item,int index);
    void CommsetTreesItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void CommsetSaveBtnClicked();
    void CommsetBackBtnClicked();

    void updatechannelnumData(QString data);
    void ShowEidtPathLoss();
    void thirdpartybtnClicked();
    void ProcessreadAllStandardOutput();

    void ChannelLineLoss();

    void on_QRLineedit_returnPressed(); //输入框响应函数

    void ShowFilePath();

     void myQueryMessBoxSlot(bool value);

     void changeEvent(QEvent *e);//changeEvent槽函数声明

signals:
    void sendData(QString,QString); //只需要声明，不需要定义

protected:

    //设置为随着窗口变化而变化
    virtual void resizeEvent(QResizeEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


};
#endif // ATEUIMAINWINDOW_H
