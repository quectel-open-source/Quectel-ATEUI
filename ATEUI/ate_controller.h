#pragma once
#include "ate_controller_defines.h"

#ifdef _ATE_CONTROLLER_EXPORTS
#define Q_ATE_CONTROLLER_API extern "C" _declspec(dllexport)
#else
#define Q_ATE_CONTROLLER_API extern "C" //_declspec(dllimport)
#endif

Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_LoadRunningLogCallBack(int iTchNo, Q_ATE_CONTROLLER_RuningLogShowCalllBack qAteRunningLogCallback,void* pCallBackObj);
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_LoadSyslogCallBack(Q_ATE_CONTROLLER_SysLogShowCallBack qAtesysLog, void*);
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_LoadResultCallBack(int iTchNo, Q_ATE_CONTROLLER_ResultShowCallBack qAteResultCallBack, void* pCallBackObj);
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_LoadResultProcessCallBack(int iTchNo, Q_ATE_CONTROLLER_ResultProcessShowCallBack qAteResultProcessCallBack, void* pCallBackObj);
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_LoadBarCodeShowCallBack(int iTchNo, Q_ATE_CONTROLLER_BarcodeShowCallBack qAteBarcodetCallBack, void* pCallBackObj);
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_LoadErrorInfoShowCallBack(int iTchNo, Q_ATE_CONTROLLER_ShowErrorInfoCallBack qAteBarcodetCallBack, void* pCallBackObj);
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_LoadResultTitleCallBack(int iTchNo, Q_ATE_CONTROLLER_ResultTitleShowCallBack qAteResultTitleCallBack, void* pCallBackObj);
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_AddSysLog(const wchar_t* pwcmsg);
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_LoadCallExternalAPICallBack(int iTchNo,Q_ATE_CONTROLLER_CallExternalAPI qATeCallExternalAPI,void *pCallBackObj);
Q_ATE_CONTROLLER_API bool Q_ATE_CONTROLLER_InitFDB();
Q_ATE_CONTROLLER_API void* Q_ATE_CONTROLLER_GetMOInfo();
Q_ATE_CONTROLLER_API void* Q_ATE_CONTROLLER_GetProductInfo();
Q_ATE_CONTROLLER_API void* Q_ATE_CONTROLLER_GetCommonSetting();
Q_ATE_CONTROLLER_API void* Q_ATE_CONTROLLER_GetTestScript();
Q_ATE_CONTROLLER_API void* Q_ATE_CONTROLLER_GetStationInfo();
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_GetTestFlowStationInfo(wchar_t* wcStationList, int iBufferSize);
Q_ATE_CONTROLLER_API bool Q_ATE_CONTROLLER_Init_TestChannel(int iTchNo);
Q_ATE_CONTROLLER_API bool Q_ATE_CONTROLLER_StartTest(int iTchNo,const wchar_t* pwcBarcode);
Q_ATE_CONTROLLER_API bool Q_ATE_CONTROLLER_InitFirst();
Q_ATE_CONTROLLER_API void Q_ATE_CONTROLLER_ReleaseAll();
Q_ATE_CONTROLLER_API bool Q_ATE_CONTROLLER_Init_WorkResource();
