#pragma once


//
#ifdef DLL_IMPLEMENT
#define DLL_IMP extern "C" _declspec(dllexport)
#else
#define DLL_IMP extern "C" //_declspec(dllimport)
#endif


//prototype of callback function
typedef void( __stdcall * CALLBACK_SHOW_REDIT_INFO )(const wchar_t * wcp_Msg, void * p_usr_Arg);
typedef void( __stdcall * CALLBACK_SHOW_STATE )(const wchar_t * wcp_State, void * p_usr_Arg);
typedef bool( __stdcall * CALLBACK_SHOW_TEST_RESULT )(const wchar_t * wcp_Name, double d_Low, double d_High, double d_Data, void * p_usr_Arg);
typedef void( __stdcall * CALLBACK_SAVE_TRACE_LOG )(const wchar_t * wcp_Trace_Log, void * p_usr_Arg);
typedef void( __stdcall * CALLBACK_SHOW_TITLE )(const wchar_t * wcp_Title, void * p_usr_Arg);

//主界面显示 callback 结构体
typedef struct tagQTI_PROG_ARG
{
    //qti progress callback
    void *							p_usr_Arguments;
    CALLBACK_SHOW_REDIT_INFO		cb_Show_REdit_Info;
    CALLBACK_SHOW_STATE				cb_Show_State;
    CALLBACK_SHOW_TEST_RESULT		cb_Show_Test_Result;
    CALLBACK_SAVE_TRACE_LOG			cb_Save_Trace_Log;
    CALLBACK_SHOW_TITLE				cb_Show_Title;
}QTI_PROG_ARG;



//输出函数接口
//输出函数接口
DLL_IMP void TS_LAN_INIT_FIRST( HWND h_Wnd );
DLL_IMP void TS_LAN_LOAD_QTI_ARG( QTI_PROG_ARG * strp_Qti_Arg );
DLL_IMP void TS_LAN_LOAD_TS_PC_LAN_IP_AND_PORT( const wchar_t * wcp_IP, int i_Port );
DLL_IMP void TS_LAN_LOAD_MAIN_CTRL_SERVER_LAN_IP_AND_PORT( const wchar_t * wcp_IP, int i_Port );
DLL_IMP bool TS_LAN_START_TCP_SERVER( void );
DLL_IMP void TS_LAN_STOP_TCP_SERVER( void );
DLL_IMP bool TS_LAN_PROCESS_TCP_ACCEPT( void );
DLL_IMP bool TS_LAN_PROCESS_TCP_OP( WPARAM w_Param, LPARAM l_Param );
DLL_IMP bool TS_LAN_SEND_RESULT_LAN_CMD_TO_MC_PC( const wchar_t * wcp_TS_Name, const wchar_t * wcp_Cmd_Type, int i_TS_Number, bool b_Result );
DLL_IMP bool TS_LAN_SEND_DC_LAN_CMD_TO_MC_PC( int i_TS_Number, const wchar_t * wcp_Cmd_Type );
DLL_IMP bool TS_LAN_SEND_PFQUAN_LAN_CMD_TO_MC_PC( const wchar_t * wcp_TS_Name, int i_TS_Number, int i_Quan_Pass, int i_Quan_Fail );
DLL_IMP bool TS_LAN_SEND_TRD_LAN_CMD_TO_MC_PC( int i_TS_Number, const wchar_t * wcp_TRD );
DLL_IMP bool TS_LAN_SEND_TRD_PANEL_LAN_CMD_TO_MC_PC( int i_TS_Number, const wchar_t * wcp_TRD_PANEL );
DLL_IMP bool TS_LAN_SEND_RELOAD_FIXTURE_LAN_CMD_TO_MC_PC( int i_TS_Number );
//DLL_IMP void TS_LAN_GET_MO_AND_TEST_INFO_V1( CString * csp_MO_Name, CString * csp_Test_Line_Numb, CString * csp_Operator_ID );
DLL_IMP void TS_LAN_GET_MO_AND_TEST_INFO_V2( wchar_t * wcp_MO_Name, wchar_t * wcp_Test_Line_Numb, wchar_t * wcp_Operator_ID );
DLL_IMP bool TS_LAN_SEND_QUERY_MO_NAME_LAN_CMD_TO_MC_PC( int i_TS_Number );
DLL_IMP bool TS_LAN_SEND_QUERY_FIXTURE_CODE_LAN_CMD_TO_MC_PC( int i_TS_Number );
//DLL_IMP bool TS_LAN_GET_BARCODE_PANELCODE_AFTER_MSG( int i_TS_Number, CString & oj_cs_Code_Data );
DLL_IMP bool TS_LAN_SEND_OS_CLOSS_RESULT_LAN_CMD_TO_MC_PC( const wchar_t * wcp_OS_Type, int i_TS_Number, bool b_Result );
DLL_IMP bool TS_LAN_SEND_QUERY_FIXTURE_AND_LOAD_DUT_STATUS_LAN_CMD_TO_MC_PC_V1( int i_TS_Number );
DLL_IMP bool TS_LAN_SEND_QUERY_FIXTURE_AND_LOAD_DUT_STATUS_LAN_CMD_TO_MC_PC_V2( int i_TS_Number, int i_Query_Timeout_S, bool & oj_b_Fixture_Is_Opened, bool & oj_b_Has_Loaded_DUT );
//DLL_IMP void TS_LAN_GET_ERROR_INFO_ALL( CString * csp_Error_Info );
DLL_IMP bool TS_LAN_SEND_APLC20_CLOSS_RESULT_LAN_CMD_TO_MC_PC( int i_TS_Number, bool b_Result );
