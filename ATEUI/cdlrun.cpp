#include "CDLRun.h"



TESTER_RESULT_S         tmploadresult[32];
TESTER_ErrorCodeInfo_S  tmpErrorInfoCode[32];
TESTER_MESSBOX_S        tmpMessbox;


void __stdcall GetRunningLog(const wchar_t* msg, void* pobj)
{

    CDLRun *tmpDLRun = (CDLRun *)pobj;

    tmpDLRun->Show_Running_Log_Msg(msg,true);
}

void __stdcall GetIMEI(const wchar_t* msg, void* pobj)
{
    CDLRun *tmpRun = (CDLRun *)pobj;

    if(tmpRun!=nullptr)
    {
        tmpRun->Show_Running_IMEI_Msg(msg,true);
    }

}

void __stdcall GetErrorCodeErrorInfo(int errorcode, const wchar_t* msg, void* pobj)
{
    CDLRun *tmpRun = (CDLRun *)pobj;

    qInfo()<<"GetErrorCodeErrorInfo() Start";

    qInfo()<<"errorcode="<<QString::number((uint)errorcode, 16);
    tmpErrorInfoCode[tmpRun->m_iTSCH_Numb].pcwcErrorInfo = msg;
    tmpErrorInfoCode[tmpRun->m_iTSCH_Numb].iErrorCode = errorcode;

    if(tmpRun!=nullptr)
    {
        tmpRun->Show_Running_Error_Info_Code(&tmpErrorInfoCode[tmpRun->m_iTSCH_Numb]);
    }

    qInfo()<<"GetErrorCodeErrorInfo() End";
}

void __stdcall GetDLResultProcess(double msg, void* pobj)
{

    CDLRun *tmpDLRun = (CDLRun *)pobj;

    tmpDLRun->Show_Result_Process_Msg(&msg,true);
}

void __stdcall GetLoadResult(const wchar_t* pcwcItemName, const wchar_t* pcwcValue, const wchar_t* pcwcLowlimit, const wchar_t* pcwcHighlimit, bool bResut, const wchar_t*, void* pobj)
{

    CDLRun *tmpDLRun = (CDLRun *)pobj;

    tmploadresult[tmpDLRun->m_iTSCH_Numb].pcwcItemName =pcwcItemName;
    tmploadresult[tmpDLRun->m_iTSCH_Numb].pcwcValue = pcwcValue;
    tmploadresult[tmpDLRun->m_iTSCH_Numb].pcwcLowlimit = pcwcLowlimit;
    tmploadresult[tmpDLRun->m_iTSCH_Numb].pcwcHighlimit = pcwcHighlimit;
    tmploadresult[tmpDLRun->m_iTSCH_Numb].bResut = bResut;


    tmpDLRun->Show_Load_Result_Msg(&tmploadresult[tmpDLRun->m_iTSCH_Numb],true);
}

void __stdcall GetLoadResultTitle(const wchar_t* title, void* pobj)
{

    CDLRun *tmpDLRun = (CDLRun *)pobj;

    if(nullptr==title)
    {
        qDebug()<<"title is NULL";
        return;
    }

    tmpDLRun->Show_Load_Result_Title(title);
}

//显示弹框
bool __stdcall GetCallExternalAPI(const wchar_t* pcwcAPIName, const wchar_t* pcwcAgrs, wchar_t* pwcResValueList, int iValueBufferSize, void* pobj)
{
    //这里可以直接显示弹框效果
    CDLRun *tmpDLRun = (CDLRun *)pobj;

    if(nullptr==pcwcAPIName || nullptr==pcwcAgrs || nullptr ==pwcResValueList)
    {
        qDebug()<<"pcwcAPIName is NULL";
        return false;
    }



    tmpMessbox.pcwcAPIName=pcwcAPIName;
    tmpMessbox.pcwcAgrs = pcwcAgrs;
    tmpMessbox.iValueBufferSize = iValueBufferSize;

//    if(tmpDLRun->m_iOP_Multi_Option != LOAD_AUTO_ROBOT)
        {
             tmpDLRun->Show_Load_MessBox(&tmpMessbox);
        }


    //tmpDLRun->m_cla_gfc.QString2Wchar(tmpDLRun->m_Query_Messbox_Result);
    if(iValueBufferSize<tmpDLRun->m_Query_Messbox_Result.length()+1)
    {
        qDebug()<<"GetCallExternalAPI() iValueBufferSize small";
       return false;
    }
//    memcpy_s(pwcResValueList,iValueBufferSize*2,tmpDLRun->m_cla_gfc.QString2Wchar(tmpDLRun->m_Query_Messbox_Result),tmpDLRun->m_Query_Messbox_Result.length()*2);
    wcscpy_s(pwcResValueList,iValueBufferSize,tmpDLRun->m_cla_gfc.QString2Wchar(tmpDLRun->m_Query_Messbox_Result));
}


CDLRun::CDLRun()
{

}

CDLRun::~CDLRun()
{

}

int CDLRun::Do_Execute( void )
{
    bool bResult;
    int iRtn;

	//总执行函数，按选项和配置要求分别再执行其他函数
    qInfo()<<"Do_Execute() Start";
	//
	try
	{
        //设定控件状态
        this->Show_Program_State( PROGRAM_RUN );

        //判断是否是自动线
        if( LOAD_AUTO_ROBOT != m_iOP_Multi_Option)
        {
            //不是自动线，判断是否控制电源
            qInfo()<<"ChannelPowerFlag ="<<ChannelPowerFlag;
            if(true==ChannelPowerFlag)
            {
                //延时三秒
                QThread::msleep(3000); // 后台线程可以安全休眠
            }
        }

        qInfo()<<"Q_ATE_CONTROLLER_StartTest Start";
         bResult = Q_ATE_CONTROLLER_StartTest(m_iTSCH_Numb+1,m_QRWchart);
        qInfo()<<"Q_ATE_CONTROLLER_StartTest End"<<bResult;

         //------------------------------------- 显示PASS与FAIL -------------------------------------
         if( true==bResult)
         {
             //bret=::Q_ATE_CONTROLLER_Init_TestChannel(m_iTSCH_Numb+1);
            QString tmpgoldsample;
            bool bret=false;
            tmpgoldsample = m_cla_gfc.Wchar2QString(m_QRWchart);
            qInfo()<<"tmpgoldsample="<<tmpgoldsample;
            if(tmpgoldsample.contains("GOLDENSAMPLE"))
            {
                bret=::Q_ATE_CONTROLLER_Init_TestChannel(m_iTSCH_Numb+1);
                if(false==bret)
                {
                    //fail图标
                    switch( this->m_iOP_Multi_Option )
                    {
                    case LOAD_MANUAL:
                        this->Show_Program_State( PROGRAM_FAIL );
                        break;
                    case LOAD_SEMI_FIXTURE:
                    case LOAD_AUTO_ROBOT:
                        this->Show_Program_State( PROGRAM_AUTO_FAIL );
                        break;
                    default:
                        break;
                    }


                    //fail返回
                    iRtn = RTN_FAIL;

                    throw(0);
                }
            }

             //pass图标
             switch( this->m_iOP_Multi_Option )
             {
             case LOAD_MANUAL:
                 this->Show_Program_State( PROGRAM_PASS );
                 break;
             case LOAD_SEMI_FIXTURE:
             case LOAD_AUTO_ROBOT:
                 this->Show_Program_State( PROGRAM_AUTO_PASS );
                 break;
             default:
                 break;
             }


             //pass返回
             iRtn = RTN_SUCCESS;
         }
         else
         {
             //fail图标
             switch( this->m_iOP_Multi_Option )
             {
             case LOAD_MANUAL:
                 this->Show_Program_State( PROGRAM_FAIL );
                 break;
             case LOAD_SEMI_FIXTURE:
             case LOAD_AUTO_ROBOT:
                 this->Show_Program_State( PROGRAM_AUTO_FAIL );
                 break;
             default:
                 break;
             }


             //fail返回
             iRtn = RTN_FAIL;
         }
         //-------------------------------------

    }
    catch(...)
    {
        //fail返回
        iRtn = RTN_FAIL;
    }
    
    return iRtn;
}

void CDLRun::Save_Result_Statistics_Data(bool b_Result)
{

    QString tsname;
    QString tmppass;
    QString tmpfail;
    tsname = QString("TS_%1").arg(m_iTSCH_Numb);

    tmppass = "/"+tsname+"/PASS";
    tmpfail = "/"+tsname+"/FAIL";
    qInfo()<<tmppass;
    qInfo()<<tmpfail;

    //保存各测试台的统计
    if( true == b_Result )
    {
        int ipass  = m_cla_IniStatistics->value(tmppass).toInt();
        ipass++;
        m_cla_IniStatistics->setValue(tmppass, ipass);
    }
    else
    {
        int ifail  = m_cla_IniStatistics->value(tmpfail).toInt();
        ifail++;
        m_cla_IniStatistics->setValue(tmpfail, ifail);
    }
}

int CDLRun::Init()
{
    //初始化状态显示

    //百分比回调
    if(m_run_Station==TEST_STATION_DL)
    {
        Q_ATE_CONTROLLER_LoadResultProcessCallBack(m_iTSCH_Numb+1,GetDLResultProcess,this);
        Q_ATE_CONTROLLER_LoadResultCallBack(m_iTSCH_Numb+1,NULL,NULL);
    }
    else
    {
        Q_ATE_CONTROLLER_LoadResultProcessCallBack(m_iTSCH_Numb+1,NULL,NULL);
        Q_ATE_CONTROLLER_LoadResultCallBack(m_iTSCH_Numb+1,GetLoadResult,this);
    }


    //回调errorcode 和errorinfo
    Q_ATE_CONTROLLER_LoadErrorInfoShowCallBack(m_iTSCH_Numb+1,GetErrorCodeErrorInfo,this);

    Q_ATE_CONTROLLER_LoadRunningLogCallBack(m_iTSCH_Numb+1, GetRunningLog, this);


    //回调IMEI
    Q_ATE_CONTROLLER_LoadBarCodeShowCallBack(m_iTSCH_Numb+1,GetIMEI,this);

    //显示标题
    Q_ATE_CONTROLLER_LoadResultTitleCallBack(m_iTSCH_Numb+1,GetLoadResultTitle,this);

    //弹出框
    Q_ATE_CONTROLLER_LoadCallExternalAPICallBack(m_iTSCH_Numb+1,GetCallExternalAPI,this);


    Sleep(100);

    bool bret=false;
    bret=::Q_ATE_CONTROLLER_Init_TestChannel(m_iTSCH_Numb+1); //在最后面

    //
    //初始化状态显示
    if(bret==false)
    {
        Show_Program_State( PROGRAM_INIT_FAIL );
    }
    else
    {
        Show_Program_State( PROGRAM_IDLE );
    }

    //初始化ini文件句柄
    return bret;

}

void CDLRun::Show_Running_Log_Msg( const wchar_t * wcp_Msg, bool b_RN )
{
    //richedit
    ::SendMessage((HWND)this->m_hWnd_View, WM_REDIT_MSG_TS_MSG, (WPARAM)wcp_Msg, (LPARAM)this->m_iTSCH_Numb );

    (void) b_RN;
}

void CDLRun::Show_Running_IMEI_Msg( const wchar_t * wcp_Msg, bool b_RN )
{
    //richedit
    QString runIMEI = "";
    if(wcp_Msg!=nullptr)
    {
        runIMEI=m_cla_gfc.Wchar2QString(wcp_Msg);
        qInfo()<<"m_runIMEI="<<s_m_runIMEI;
    }
    else
    {
       qDebug()<<"Show_Running_IMEI_Msg wcp_Msg is NULL";
    }


    if(::SendMessage((HWND)this->m_hWnd_View, WM_SHOW_IMEI_TS_MSG, (WPARAM)&runIMEI, (LPARAM)this->m_iTSCH_Numb )==0)
    {
        //qDebug()<<"SendMessage successs";
    }
    else
    {
         qDebug()<<"SendMessage fail";
    }

    //::SendMessage((HWND)this->m_hWnd_View, WM_SHOW_IMEI_TS_MSG, (WPARAM)wcp_Msg, (LPARAM)this->m_iTSCH_Numb );
    (void) b_RN;
}

//发送errorcode和errorinfo
void CDLRun::Show_Running_Error_Info_Code(TESTER_ErrorCodeInfo_S *ErrorCodeInfo)
{
    if(::SendMessage( (HWND)this->m_hWnd_View, WM_SHOW_ERRORCODEINFO_TS_MSG, (WPARAM)ErrorCodeInfo, (LPARAM)this->m_iTSCH_Numb )==0)
    {

    }
    else
    {

    }
}

void CDLRun::Show_Result_Process_Msg( double * wcp_Msg, bool b_RN )
{
    //richedit
    if(::SendMessage( (HWND)this->m_hWnd_View, WM_SHOW_PROCESS_MSG_TS_MSG, (WPARAM)wcp_Msg, (LPARAM)this->m_iTSCH_Numb )==0)
    {
       // qDebug()<<"SendMessage successs";
    }
    else
    {
        qDebug()<<"SendMessage fail";
    }
    (void) b_RN;
}

void CDLRun::Show_Load_Result_Msg(TESTER_RESULT_S *loadresult, bool b_RN )
{
    //
    if(::SendMessage( (HWND)this->m_hWnd_View, WM_SHOW_LOADRESULT_MSG_TS_MSG, (WPARAM)loadresult, (LPARAM)this->m_iTSCH_Numb )==0)
    {

    }
    else
    {

    }
    (void) b_RN;
}

void CDLRun::Show_Load_MessBox( TESTER_MESSBOX_S *loadMessBox)
{
    ::SendMessage( (HWND)this->m_hWnd_View, WM_QUERY_MESSAGR_BOX_TS_MSG, (WPARAM)loadMessBox, (LPARAM)this->m_iTSCH_Numb );
}

void CDLRun::Show_Load_Result_Title( const wchar_t * wcp_Title)
{
    //
    QString runTitle = "";
    if(wcp_Title!=nullptr)
    {
        runTitle=m_cla_gfc.Wchar2QString(wcp_Title);
    }
    else
    {
       qDebug()<<"Show_Load_Result_Title wcp_Title is NULL";
       return;
    }

    ::SendMessage( (HWND)this->m_hWnd_View, WM_SHOW_LOADRESULT_TITLE_TS_MSG, (WPARAM)&runTitle, (LPARAM)this->m_iTSCH_Numb );
}

void CDLRun::Show_Program_State( int i_Program_State )
{

    //主界面的“各TS分页”控件显示
    switch( i_Program_State )
    {
    case PROGRAM_INIT_ING:
        this->Show_State( m_cla_gfc.QString2Wchar("INIT_ING") );
        break;
    case PROGRAM_INIT_FAIL:
        this->Show_State( m_cla_gfc.QString2Wchar("INIT_FAIL") );
        break;
    case PROGRAM_IDLE:
    case PROGRAM_AUTO_IDLE:
        this->Show_State( m_cla_gfc.QString2Wchar("INIT") );
        break;
    case PROGRAM_CL_RUN:
    case PROGRAM_RUN:
        this->Show_State( m_cla_gfc.QString2Wchar("RUN") );
        break;
    case PROGRAM_PASS:
    case PROGRAM_CL_PASS:
    case PROGRAM_AUTO_PASS:
        this->Show_State( m_cla_gfc.QString2Wchar("PASS") );
        break;
    case PROGRAM_FAIL:
    case PROGRAM_CL_FAIL:
    case PROGRAM_AUTO_FAIL:
        this->Show_State( m_cla_gfc.QString2Wchar("FAIL"));
        break;
    default:
        return;
    }


    //主界面“自身”控件显示
    ::SendMessage( (HWND)this->m_hWnd_View, WM_PROGRAM_STATE_TS_MSG, (WPARAM)i_Program_State, (LPARAM)this->m_iTSCH_Numb );
}

void CDLRun::Show_State( const wchar_t * wcp_State )
{
    ::SendMessage( (HWND)this->m_hWnd_View, WM_STATE_INFO_TS_MSG, (WPARAM)wcp_State, (LPARAM)this->m_iTSCH_Numb );
}

void CDLRun::Ctrl_Colour_State( int i_Colour_Code )
{
    ::SendMessage( (HWND)this->m_hWnd_View, WM_CTRL_COLOUR_STATE_TS_MSG, (WPARAM)i_Colour_Code, (LPARAM)this->m_iTSCH_Numb );
}



