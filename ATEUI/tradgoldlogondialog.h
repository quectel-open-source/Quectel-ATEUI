#ifndef TRADGOLDLOGONDIALOG_H
#define TRADGOLDLOGONDIALOG_H

#include <QObject>
#include <QDialog>
#include <QPoint>
#include <QMouseEvent>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QAction>
#include<QLineEdit>
#include <QIcon>
#include <QDebug>
#include <QProgressDialog>
#include <QCoreApplication>
#include <QProgressBar>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include<QDir>
#include <QLockFile>
#include <QMessageBox>
#include "qlogoutput.h"


#include <windows.h>

#include "ate_controller.h"
#include "Global_Header.h"
#include "cgfc.h"
#include "ate_controller_defines_ext.h"
#include "mymessagebox.h"
#include "ts_lan.h"
#include "myquerymessagebox.h"
#include "TrianglePushButton.h"


class TradGoldLogonDiaLog: public QDialog
{
     Q_OBJECT

public:
    explicit TradGoldLogonDiaLog(QWidget *parent = nullptr);
    ~TradGoldLogonDiaLog();
    void Init();
    void InitExternalAPI();
    QPoint mousePoint;
    bool mouse_press;
    void ReadCfg();
    void SetCfg();
    void CheckAndSetStatus();
    void InitLogon();

    QPushButton *m_closebtn;
    QComboBox   *m_IPcombox;
    QLineEdit   *m_IPLineEdit;
    QPushButton *m_IPTestbtn;
    QLineEdit   *m_MOLineEdit;
    QPushButton *m_Searchbtn;
    QComboBox   *m_OfflineModecombox;
    QPushButton *m_Enterbtn;
    QPushButton *m_ReSelectbtn;

    HANDLE m_hThread_Logon_INIT_Handle;
    DWORD m_dwThread_Logon_INIT_ID;

    void DO_Logon_OP();
    int  m_bar_current_value;
    bool m_Init_WorkResource_Flag;

    // station cfg
    Q_ATE_Controller_STATIONCONFIG_Interface *      pStationcfg =NULL;
    Q_ATE_Controller_MOInfo_Interface*              pMoCfg=NULL;
    //Q_ATE_Controller_MOInfo_Interface*              pMoCfg=NULL;
    CGFC m_cla_logon_gfc;

    QString m_DownloadFW;         //软件版本

    void LogFileInit();

    MyQueryMessageBox m_quermessbox;
    MyMessageBox m_logon_messbox; //弹框信息

    WId m_hWnd_View;
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    void ShowMessageBox(QString messtype,QString messTitle,QString messContent);

    QString m_Query_Messbox_Result;
    wchar_t m_wcQuery_Messbox_Result[200];
    bool m_query_current_Value;

    QVector<QLabel*>              m_triangleLabellist;
    bool  m_btriangleBtn_flag;
    int m_current_triangle_Index;

    void ShowStation(const wchar_t *stationinfo);

    void OnGetMsg_Load_MessageBox_State_TS(WPARAM wParam, LPARAM lParam);
    void OnGetMsg_Process_State_TS( WPARAM wParam, LPARAM lParam);
    double *m_barvalue;

signals:
    void SendData(QString s);  //信号

private slots:
    void CloseLogon();
    void EnterLogon();
    void ReSelect();
    void SearchMO();
    void onOffLineCurrentIndexChanged(int index);
    void TestModeCurrentIndexChanged(int index);
    void IPTest();

    void IPcomboxcurrentIndexChanged(int index);

    void myQueryMessBoxSlot(bool value);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // TradGoldLogonDiaLog_H
