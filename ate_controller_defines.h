#pragma once

typedef  void(__stdcall* Q_ATE_CONTROLLER_RuningLogShowCalllBack)(const wchar_t*, void*);
typedef  void(__stdcall* Q_ATE_CONTROLLER_ResultShowCallBack)(const wchar_t* pcwcItemName, const wchar_t* pcwcValue, const wchar_t* pcwcLowlimit, const wchar_t* pcwcHighlimit, bool bResut, const wchar_t*, void*);
typedef  void(__stdcall* Q_ATE_CONTROLLER_ResultTitleShowCallBack)(const wchar_t* pcwcItemName,void*);
typedef  void(__stdcall* Q_ATE_CONTROLLER_SysLogShowCallBack)(const wchar_t*, void*);
typedef  void(__stdcall* Q_ATE_CONTROLLER_ResultProcessShowCallBack)(double, void*);
//typedef  void(__stdcall* Q_ATE_CONTROLLER_TrigerTestCallBack)(int, const wchar_t*, void*);
typedef  void(__stdcall* Q_ATE_CONTROLLER_BarcodeShowCallBack)(const wchar_t*, void*);
typedef  void(__stdcall* Q_ATE_CONTROLLER_ShowErrorInfoCallBack)(int,const wchar_t*, void*);
typedef  bool(__stdcall* Q_ATE_CONTROLLER_CallExternalAPI)(const wchar_t* pcwcAPIName, const wchar_t* pcwcAgrs, wchar_t* pwcResValueList, int iValueBufferSize, void*);


