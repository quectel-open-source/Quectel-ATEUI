#pragma once
struct Q_ATE_Controller_TestScript_Interface
{
	virtual int GetDLlDependsCount() = 0;
	virtual void* GetDependNode(int iIndex) = 0;
	virtual const wchar_t* GetAttribute(void* pNode, const wchar_t* pcwcName) = 0;
	virtual const wchar_t* GetNodeValue(void* pNode) = 0;
	virtual const wchar_t* GetNodeText(void* pNode) = 0;
	virtual const wchar_t* GetChildNodeText(void* pNode, const wchar_t* pcwcName) = 0;
	virtual int GetInitNodeChildCount(void* pParent) = 0;
	virtual void* GetInitNodeChild(void* pParent, int iIndex) = 0;
	virtual int GetTestNodeChildCount(void* pParent) = 0;
	virtual void* GetTestNodeChild(void* pParent, int iIndex) = 0;
	virtual int GetNodeInputParasCount(void* pNode) = 0;
	virtual int GetNodeOutParasCount(void* pNode) = 0;
	virtual void* GetNodeInputPara(void* pNode, int iIndex) = 0;
	virtual void* GetNodeOutputPara(void* pNode, int iIndex) = 0;
	virtual void* GenRunInstance(int iTchNo) = 0;
	virtual bool Save()=0;
};

struct Q_ATE_Controller_STATIONCONFIG_Interface
{
	virtual bool GetParaValue(const wchar_t* wcName, wchar_t* wcValue, int iBufferSize) = 0;
	virtual void SetParaValue(const wchar_t* pcwcName, const wchar_t* wcValue) = 0;
	virtual bool Save() = 0;
};

struct  Q_ATE_Controller_MOInfo_Interface
{
	virtual const wchar_t* GetParaValueByName(const wchar_t* pcwcName) = 0;
	virtual int GetParaCout() = 0;
	virtual const wchar_t* GetParaName(int iIndex) = 0;
	virtual const wchar_t* GetParaValue(int iIndex) = 0;
};

struct  Q_ATE_Controller_ProductInfo_Interface
{
	virtual const wchar_t* GetParaValueByName(const wchar_t* pcwcName) = 0;
	virtual int GetParaCout() = 0;
	virtual const wchar_t* GetParaName(int iIndex) = 0;
	virtual const wchar_t* GetParaValue(int iIndex) = 0;
};

struct Q_ATE_Controller_TCH_LOSSTABLE_Interface
{
	virtual bool GetTxCableLoss(wchar_t* pwcTxLoss, int iBufferSize, int iBandName, const wchar_t* pwcTechName, int iAntennaIndex) =0;
	virtual bool GetRxCableLoss(wchar_t* pwcRxLoss, int iBufferSize, int iBandName, const wchar_t* pwcTechName, int iAntennaIndex) = 0;
	virtual bool SetCableLoss(int iAntennaIndex, const wchar_t* pwcTechName, int iBandName, const wchar_t* pwcTxLoss, const wchar_t* pwcRxLoss) = 0;
	virtual bool Save() = 0;
	virtual void Reload() = 0;
};
struct  Q_ATE_Controller_CommonScript_Interface
{
	virtual bool GetCommonPara(const wchar_t* pcwcName, wchar_t* wcValue, int iBufferSize) = 0;
	virtual bool SetCommonPara(const wchar_t* pcwcName, const wchar_t* wcValue) = 0;
	virtual bool GetTchPara(int iTestChannelNo, const wchar_t* pcwcName, wchar_t* wcValue, int iBufferSize) = 0;
	virtual bool SetTchPara(int iTestChannelNo, const wchar_t* pcwcName, const wchar_t* wcValue) = 0;

	virtual int GetCommonParaCount()=0;
	virtual const wchar_t* GetCommonParaName(int iIndex) = 0;
	virtual const wchar_t* GetCommonParaValue(int iIndex) = 0;
	virtual const wchar_t* GetCommonParaDes(int iIndex) = 0;
	virtual const wchar_t* GetCommonParaItem(int iIndex) = 0;
	virtual int GetCommonParaType(int iIndex) = 0;

	virtual int GetTchParaCount(int iTestChannelNo) = 0;
	virtual const wchar_t* GetTchParaName(int iTestChannelNo,int iIndex) = 0;
	virtual const wchar_t* GetTchParaValue(int iTestChannelNo, int iIndex) = 0;
	virtual const wchar_t* GetTchParaDes(int iTestChannelNo, int iIndex) = 0;
	virtual const wchar_t* GetTchParaItem(int iTestChannelNo, int iIndex) = 0;
	virtual int GetTchParaType(int iTestChannelNo, int iIndex) = 0;
	virtual Q_ATE_Controller_TCH_LOSSTABLE_Interface* GetTchLossTable(int iTestChannelNo)=0;
	virtual bool Save() = 0;
};

