#ifndef CDLRun_H
#define CDLRun_H


#include <QDebug>
#include <windows.h>
#include <qsettings.h>
#include <QDir>
#include <qtextcodec.h>
#include <QMainWindow>
#include <qthread.h>

#include "logondialog.h"
#include "TriangleWidget.h"
#include "ate_controller.h"
#include "Global_Header.h"
#include "cgfc.h"

#include "mymessagebox.h"
#include "myquerymessagebox.h"

class CDLRun
{

public:
    CDLRun();
    ~CDLRun();
    
    int Init();
    int Do_Execute( void );

    QSettings *m_cla_IniStatistics;
    void Save_Result_Statistics_Data( bool b_Result );

    void Show_Running_Log_Msg( const wchar_t * wcp_Msg, bool b_RN );
    void Show_Running_IMEI_Msg( const wchar_t * wcp_Msg, bool b_RN );
    void Show_Result_Process_Msg(double* wcp_Msg, bool b_RN );
    void Show_Load_Result_Msg( TESTER_RESULT_S *loadresult, bool b_RN );
    void Show_Load_Result_Title( const wchar_t * wcp_Title);
    void Show_Running_Error_Info_Code(TESTER_ErrorCodeInfo_S *ErrorCodeInfo);
    void Show_Load_MessBox( TESTER_MESSBOX_S *loadMessBox);



    int m_iTSCH_Numb;	//测试通道号（1~4）
    int m_iTS_Numb;		//测试台号（1~16）
    int m_run_Station;
    WId m_hWnd_View;

    int m_iOP_Multi_Option;
    bool ChannelPowerFlag;


    wchar_t  m_QRWchart[256];

    CGFC m_cla_gfc;

    void Show_Program_State( int i_Program_State );
    void Show_State( const wchar_t * wcp_State );
    void Ctrl_Colour_State( int i_Colour_Code );

    QString s_m_runIMEI;

    MyMessageBox m_runmessage_box; //弹框信息
     bool  m_bquery_messbox_flag;
     QString m_Query_Messbox_Result;
     wchar_t m_wcQuery_Messbox_Result[200];


private slots:


};

#endif // CDLRun_H
