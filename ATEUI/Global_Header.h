#pragma once//防止头文件被多次include，这条指令就能够保证头文件被编译一次

//测试站位
#define TEST_STATION_DL			    0
#define TEST_STATION_BT				1
#define TEST_STATION_FT				2
#define TEST_STATION_FCT			3
#define TEST_STATION_WRITE			4
#define TEST_STATION_CHECK			5

//返回结果定义
#define RTN_SUCCESS					0
#define RTN_ERROR					-1
#define RTN_FAIL					1
#define RTN_IGNORE					11


//颜色定义
#define COL_BLACK					0
#define COL_RED						1
#define COL_BLUE					2
#define COL_GREEN					3


//登录等级
#define LOGIN_NDF					0
#define LOGIN_TOP					1
#define LOGIN_DESIGN				2
#define LOGIN_FAC					3

//不同项目所需要的端口类型不同
#define NEED_PORT_TYPE_NDF		-1
#define NEED_PORT_TYPE_1		1
#define NEED_PORT_TYPE_2		2

typedef void(__stdcall * CALLBACK_SQL_GOLDEN_INFO)(const wchar_t * wcp_SQL_Golden_Info, void * p_usr_Arg);

//数据库金板 callback 结构体
typedef struct tagQTI_PROG_ARG_SQL
{
	void *							p_usr_Arguments;
	CALLBACK_SQL_GOLDEN_INFO		cb_SQL_Golden_Info;
}QTI_PROG_ARG_SQL;

#define WM_USER                         0x0400
//消息定义
#define WM_REDIT_MSG_TS_MSG				    WM_USER + 1001
#define WM_STATE_INFO_TS_MSG				WM_USER + 1002
#define WM_TEST_RESULT_TS_MSG				WM_USER + 1003
#define WM_SAVE_TRACE_LOG_MSG				WM_USER + 1004
#define WM_CTRL_COLOUR_STATE_TS_MSG			WM_USER + 1005
#define WM_RESET_VIEW_TS_MSG				WM_USER + 1006
#define WM_PROGRAM_STATE_TS_MSG				WM_USER + 1007
#define WM_SHOW_TITLE_TS_MSG				WM_USER + 1008
#define WM_SHOW_STAT_INFO_MSG				WM_USER + 1009
#define WM_SCAN_AND_START_MSG				WM_USER + 1010
#define WM_EXT_PS_POWER_ON_MSG				WM_USER + 1011
#define WM_EXT_PS_POWER_OFF_MSG				WM_USER + 1012
#define WM_GOLDEN_INFO_TS_MSG				WM_USER + 1013
#define WM_SHOW_BOUND_MSG_TS_MSG			WM_USER + 1014
#define WM_SHOW_PROCESS_MSG_TS_MSG			WM_USER + 1015
#define WM_SHOW_LOADRESULT_MSG_TS_MSG	    WM_USER + 1016
#define WM_SHOW_IMEI_TS_MSG                 WM_USER + 1017
#define WM_SHOW_ERRORCODEINFO_TS_MSG        WM_USER + 1018
#define WM_SHOW_LOADRESULT_TITLE_TS_MSG	    WM_USER + 1019
#define WM_QUERY_MESSAGR_BOX_TS_MSG	        WM_USER + 1020

//logon
#define WM_LOGON_MESSAGR_TS_MSG	            WM_USER + 1021
#define WM_LOGON_PROCESS_MSG_TS_MSG			WM_USER + 1022

//CWinSocket专用
#define WM_WS_TCP_ACCEPT_MSG				WM_USER + 2000//客户端TCP接入消息
#define WM_WS_TCP_OP_MSG					WM_USER + 2001//客户端TCP读、写、关操作消息

//来自DLL的消息
//#define WM_START_TS_TEST_MSG				WM_USER + 3001//TS测试的正常测试被激发
#define WM_START_TS_CLOSS_MSG				WM_USER + 3002//TS测试的线损校准被激发
#define WM_GET_LAN_BARCODE_MSG				WM_USER + 3003//收到视觉网络条码数据，并触发TS测试台开始测试
#define WM_GET_LAN_MO_NAME_MSG				WM_USER + 3004//收到视觉网络工单号数据，并触发TS测试台的重新初始化
#define WM_CLEAR_TS_FPY_MSG					WM_USER + 3005//清空测试台的FPY
#define WM_GET_LAN_FIXTURE_CODE_MSG			WM_USER + 3006//收到总控程序发来的夹具二维码数据，把夹具二维码发给TS测试程序
#define WM_GET_LAN_QUERY_MO_MSG				WM_USER + 3007//收到总控程序发来的“工单号,线号,OperatorID”信息，“提醒”TS测试程序可以提取新MO数据
#define WM_GET_HEARTBEAT_MSG				WM_USER + 3009//收到测试总控的心跳包信号
#define WM_START_TS_OS_CLOSS_MSG			WM_USER + 3010//TS通用OS金板校准线损操作被激发


//程序运行状态
#define PROGRAM_INIT_ING			0//初始化中
#define PROGRAM_IDLE				1//“人工”的待机
#define PROGRAM_RUN					2//运行中
#define PROGRAM_PASS				3//测试pass
#define PROGRAM_FAIL				4//测试fail
#define PROGRAM_INIT_FAIL			5//初始化失败
#define PROGRAM_CL_RUN				6//校线损运行中
#define PROGRAM_CL_PASS				7//校线损pass
#define PROGRAM_CL_FAIL				8//校线损fail
#define PROGRAM_AUTO_IDLE			9//“自动化”的待机
#define PROGRAM_AUTO_PASS			10//“自动化”的pass
#define PROGRAM_AUTO_FAIL			11//“自动化”的fail



//界面选择“测试站位、测试内容、测试操作”
#define TS_GSM					0
#define TS_WCDMA				1
#define TS_LTE					2
#define TS_2G_3G				3//2G + 3G 连测
#define TS_2G_3G_4G				4//2G + 3G + 4G 连测


//GPIB卡传输协议
#define GPIB_NI					0
#define GPIB_VISA				1
#define GPIB_VISA_TCPIP			2


//SN长度（模式）
#define PROCESS_BARCODE_17		0//有SN
#define PROCESS_BARCODE_2		1//无SN只写标志位
#define PROCESS_BARCODE_0		2


//操作类型(模式)
#define LOAD_MANUAL				0
#define LOAD_SEMI_FIXTURE		1
#define LOAD_AUTO_ROBOT			2


//FX的使用状态
#define FX_HIDE			0//未知状态
#define FX_NO_USE		1//不使用，未激活的状态，关闭使用
#define FX_PASS			2//测试pass
#define FX_FAIL			3//测试fail
#define FX_STANDBY		4//待命
#define FX_LOAD			5//已加载模块，夹具未合上
#define FX_SCAN			6//start按钮按下，做扫码操作
#define FX_WORK			7//已加载模块，夹具合上，测试中，合上夹具了正在测试
#define FX_FINISH		8//测试结束，夹具打开，模块未取走
#define FX_ALARM		9//提醒


//独立端口（对象）运行状态
#define TS_OBJ_NDF			0//未知状态
#define TS_OBJ_NO_USE		1//不使用，未激活的状态，未激活-未选中-不使用的端口
#define TS_OBJ_STANDBY		2//待命，已激活，（无模块）
#define TS_OBJ_LOAD			3//放置模块（有模块）
#define TS_OBJ_SCAN			4//扫描模块
#define TS_OBJ_RUN			5//模块测试运行中
#define TS_OBJ_PASS			6//测试pass
#define TS_OBJ_FAIL			7//测试fail

//result 结构体
typedef struct tagTESTER_RESULT_S
{
 const wchar_t* pcwcItemName;
 const wchar_t* pcwcValue;
 const wchar_t* pcwcLowlimit;
 const wchar_t* pcwcHighlimit;
 bool bResut;
}TESTER_RESULT_S;

//messbox 结构体
typedef struct tagTESTER_MESSBOX_S
{
 const wchar_t* pcwcAPIName;
 const wchar_t* pcwcAgrs;
 const wchar_t* pwcResValueList;
 int            iValueBufferSize;
}TESTER_MESSBOX_S;

//errorcodeinfo
typedef struct tagTESTER_ErrorCodeInfo_S
{
 const wchar_t* pcwcErrorInfo;
 int iErrorCode;
}TESTER_ErrorCodeInfo_S;

//WCDMA测试项目
typedef enum EU_WCDMA_FT_ITEM_IDX
{
    WFI_MAX_POWER = 0,
    WFI_MIN_POWER,
    WFI_SEM,
    WFI_EVM,
    WFI_FREQ_ERR,
    WFI_PCDE,
    WFI_OBW,
    WFI_PHASE_DISC,
    WFI_ACLR_M_10,
	WFI_ACLR_M_5,
	WFI_ACLR_P_5,
	WFI_ACLR_P_10,
	WFI_BER,
	WFI_ILPC_E_MAX,
	WFI_ILPC_F_MAX,
	WFI_ILPC_E_MIN,
	WFI_ILPC_F_MIN,
	WFI_MAX_CALL_CURR,
	WFI_SENS,
    WFI_COUNT
}EU_WCDMA_FT_ITEM_IDX;

//cdma2000测试项目
typedef enum EU_CDMA2K_FT_ITEM_IDX
{
    CFI_OLPC_PWR_25 = 0,
	CFI_OLPC_PWR_65,
	CFI_RXQ_FER,
	CFI_CONF_LEV,
	CFI_WAVE_QLTY,
	CFI_FREQ_ERR,
	CFI_TIME_ERR,
	CFI_MAX_PWR,
	CFI_MIN_PWR,
	CFI_OLTR_1UP,
	CFI_OLTR_2DOWN,
	CFI_OLTR_3DOWN,
	CFI_OLTR_4UP,
	CFI_GOUT,
	CFI_SPEC_PWR_N4,
	CFI_SPEC_PWR_N3,
	CFI_SPEC_PWR_N2,
	CFI_SPEC_PWR_N1,
	CFI_SPEC_PWR_P1,
	CFI_SPEC_PWR_P2,
	CFI_SPEC_PWR_P3,
	CFI_SPEC_PWR_P4,
    CFI_COUNT
}EU_CDMA2K_FT_ITEM_IDX;

typedef enum EU_C2K_NS_FT_ITEM_IDX
{
    CNFI_MAX_PWR = 0,
	CNFI_MIN_PWR,
	CNFI_WAVE_QLTY,
	CNFI_FREQ_ERR,
	CNFI_TIME_ERR,
	CNFI_IQ_OO,
	CNFI_PE_RMS,
	CNFI_EVM_RMS,
	CNFI_ACPR_ADJ_L,
	CNFI_ACPR_ADJ_H,
	CNFI_ACPR_ALT_L,
	CNFI_ACPR_ALT_H,
    CNFI_COUNT
}EU_C2K_NS_FT_ITEM_IDX;

//LTE非信令测试项目
typedef enum EU_LTE_NS_FT_ITEM_IDX
{
	LNFI_TX_PWR_AVG = 0,
	LNFI_TX_PWR_MAX,
	LNFI_TX_PWR_MIN,
	LNFI_EVM_AVG,
	LNFI_EVM_MAX,
	LNFI_IQ_OO,
	LNFI_FREQ_ERR,
	LNFI_SPEC_FLAT,
	LNFI_MARGIN,
	LNFI_OBW,
	LNFI_SLOT_PWR,
	LNFI_SEM,
	LNFI_ACLR_N2,
	LNFI_ACLR_N1,
	LNFI_ACLR_EN1,
	LNFI_ACLR_EP1,
	LNFI_ACLR_P1,
	LNFI_ACLR_P2,
	LNFI_ACLR_EPWR,
	LNFI_COUNT
}EU_LTE_NS_FT_ITEM_IDX;

//TD非信令测试项目
typedef enum EU_TD_NS_FT_ITEM_IDX
{
	TNFI_TX_PWR_AVG = 0,
	TNFI_TX_PWR_MAX,
	TNFI_OBW,
	TNFI_ACLR_L2,
	TNFI_ACLR_L1,
	TNFI_ACLR_R1,
	TNFI_ACLR_R2,
	TNFI_COUNT
}EU_TD_NS_FT_ITEM_IDX;

//GSM非信令测试项目
typedef enum EU_GSM_NS_FT_ITEM_IDX
{
	GNFI_TX_PWR = 0,
	GNFI_PVT,
	GNFI_FREQ_ERR,
	GNFI_PE_RMS,
	GNFI_PE_PEAK,
	GNFI_SPEC_SWIT,
	GNFI_SPEC_MOD,
	GNFI_COUNT
}EU_GSM_NS_FT_ITEM_IDX;

//NB-IoT非信令测试项目
enum NB_RFItems
{
	NB_TEST_Ber = 0,
	NB_TEST_MaxPower,
	NB_TEST_ACLR,
	NB_TEST_EVM,
	NB_TEST_FreqErr,
	NB_TEST_OBW,
	NB_TEST_SEM,
	NB_TEST_MinPower,
	NB_TEST_COUNT
};


//生产性质（正常、REP等）
#define PRODU_PROP_NDF				0
#define PRODU_PROP_NORMAL			1//正常工单
#define PRODU_PROP_REP				2//REP工单, 返工, 重流, 纸质标签必换标签和IMEI号段, 镭雕则不更换


//标签性质（纸质、镭雕等）
#define LABEL_PROP_NDF				0
#define LABEL_PROP_PAPER			1//纸质
#define LABEL_PROP_LASER			2//镭雕


//网络协议
#define MODE_TCP					0
#define MODE_UDP					1


//测试应用对象（1.产线测试版; 2.客户测试版）
#define APP_OBJ_TL					1
#define APP_OBJ_CUS					2



//注册相关
#define OP_SUCCESS						0
#define LICENSE_ALARM_BEGIN				300
#define LICENSE_ALARM_OVER_NUM			301
#define LICENSE_ALARM_OVER_TIME			302
#define LICENSE_ALARM_END				399
#define REGISTER_RESTART				401



//测试工位
#define	STATION_WRITE_ONLY			0//手动线写IMEI号工位
#define	STATION_CHECK_ONLY			1//手动线check工位
#define	STATION_WRITE_IN_CAL		2//自动化线在RF_CAL工位集成写IMEI号工位
#define	STATION_CHECK_IN_FT			3//自动化线在RF_FT工位集成check工位



//
extern "C" __declspec(dllimport) int __stdcall Run_Model( bool bInit, int iIndex );



//View界面相关(05)
//网络处理(01)
#define ERR_FT_VIEW_UPLOAD_FTP						0x05050101
#define ERR_FT_VIEW_CHECK_CPK_LATEST_CPK			0x05050102

//错误代码
#define ERR_BT_FT_DETECT_AT_COMPORT						0x050000021//搜寻AT端口失败


//语言
#define LANGUAGE_CN			1
#define LANGUAGE_EN			2


extern bool m_UI_Debug_Flag;  // 声明全局变量（extern) 用来打印log


