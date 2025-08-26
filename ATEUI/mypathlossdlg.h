#ifndef MYPATHLOSSDLG_H
#define MYPATHLOSSDLG_H

#include <QObject>
#include <QDialog>
#include <QMouseEvent>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QProgressBar>
#include <QScrollBar>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>

#include "ate_controller_defines_ext.h"
#include "ate_controller.h"
#include "cgfc.h"
#include "mymessagebox.h"

class MyPathLossDlg : public QDialog
{
    Q_OBJECT
public:
    explicit MyPathLossDlg(QWidget *parent = nullptr,int channel=0);

    void UIinit();
    void InitData();
    void ShowTxRxPathloss(int Ant,wchar_t* Tech,QStringList PathlossTechList);
    void ShowWifiPathloss(int Ant_index,QStringList PathlossTechList);
    void ShowBLUETOOTHPathloss(int Ant_index,QStringList PathlossTechList);
    void ShowGPSpathloss(int Ant_index,QStringList PathlossTechList);
    void ShowCellularPathloss(int Ant_index,wchar_t* Tech,QStringList PathlossTechList);

    QPoint   mousePoint;
    bool     mouse_press;
    //生成
    QTableWidget *tptablewgt;
    QPushButton  *OkBtn;
    QPushButton  *CancelBtn;

    Q_ATE_Controller_TCH_LOSSTABLE_Interface*              pPathlossCfg              = NULL;
    Q_ATE_Controller_CommonScript_Interface*               pPathLossCommonScript     = NULL;
    Q_ATE_Controller_ProductInfo_Interface*                pathlossProductInfoScript = NULL;
    Q_ATE_Controller_STATIONCONFIG_Interface*              m_pathlossStationcfg      = NULL; //读取Station的值

    int  m_channel=0;

    CGFC m_pathloss_logon_gfc;

    MyMessageBox m_message_box; //弹框信息


    QStringList m_pathlossAntlist;
    QStringList m_pathlossBands_2Glist;
    QStringList m_pathlossBands_2GGSMlist;
    QStringList m_pathlossWCDMAlist;
    QStringList m_pathlossLTElist;
    QStringList m_pathlossNR_5G_SUB6list;
    QStringList m_pathlossNBIoT_Bandslist;
    QStringList m_pathlossC2K_Bandslist;
    QStringList m_pathlossTD_Bandslist;
    QStringList m_pathlossCV2X_Bandslist;
    QStringList m_pathlossNR_5G_MMWlist;
    QStringList m_pathlossWlanBTlist;
    QStringList m_pathlossGPSlist;
    QStringList m_pathlossWLANlist;
    QStringList m_pathlossBlueToothlist;
    QStringList m_pathlossLORAlist;   //星闪天线
    QStringList m_pathlossSPARKLINKlist;   //星闪天线

    QStringList m_ANT_Prop_HW_DESList;      //蜂窝
    QStringList m_ANT_Prop_GPSList;         //GPS
    QStringList m_ANT_Prop_WLANList;        //WLAN
    QStringList m_ANT_Prop_BlueToothList;   //Bluetooth
    QStringList m_ANT_Prop_LORAList;        //LORA
    QStringList m_ANT_Prop_SPARKLINKList;   //SPARKLINK

    bool m_New_ant_display_mode;
    void Show_New_Mode_Ant_Pathloss();

    QString m_PathlossOffline_Status;  //
    QString m_PathlossTradGold_Status;  //



signals:

private slots:
        void Okclicked();
        void Cancelclicked();

        void MyitemChanged(QTableWidgetItem *item);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    //键盘事件
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // MYPATHLOSSDLG_H
