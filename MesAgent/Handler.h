// Handler.h : 헤더 파일
//
#pragma once

#include <vector>
#include <map>
using namespace std;

#define HANDLER_PORT	10000	// Local Port for Handler
// #define HANDLER_PORT	10001	// Local Port for Handler

class CHandler : public CWnd
{
	DECLARE_DYNAMIC(CHandler)

public:
	CHandler();
	virtual ~CHandler();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnServerAccept(WPARAM wClientIdx, LPARAM lServerPort);
	afx_msg LRESULT OnServerRemove(WPARAM wClientIdx, LPARAM lServerPort);
	afx_msg LRESULT OnServerReceive(WPARAM wClientIdx, LPARAM lServerPort);

private:
	CServerSocketCS m_Server;
	int		m_nClientIdx;
	BOOL	m_bConnected;
	CString m_strRecvCmd;

private:
	void Get_OperUpdate(CString sOperId);
	void Get_EquipState(CString sState, CString sOperId);	// 1:Idle, 2:Run, 3:Down
	void Get_ErrorUpdate(CString sFlag, CString sErrNo, CString sErrCat);
	void Get_ControlState(CString sFlag, CString sOperId);	// 1:Online, 2:Offline
	void Get_LotStart(CString sLotId, CString sMGZId, CString sSlot, CString sTrayID, CString sRecipe);
	void Get_LotEnd(CString sLotId, CString sMZId, CString sTrayId, CString sRecipe, CString sHCount, CString sOk, CString sNg);
	void Get_LotAbort(CString sLotId);
	void Get_CmRequest(CString sLotId, CString sCmId);
	void Get_CmEnd(CString sLotId, CString sCmId, CString sResult, CString sNgCode, CString sFmMZID, CString sFmCarID, CString sFmPocket, CString sToCarID, CString sToPocket, CString sROSResult);
	void Get_IdleReport(CString sOperId, CString sSTime, CString sETime, CString sCode, CString sText, CString sType);
	void Get_AccessChanged(CString sAccessMode);
	void Get_UnitState(CString sState);
	void Get_UnitMaterialCount(CString sMDCount, CString sPortNo, CString sInputCnt, CString sOK, CString sNG);
	void Get_UnitProcessingTimeReport(CString sLotID, CString sProcessID, CString sModelID, CString sRecipe, CString sTactTime, CString sCycleTime);

	void Get_DownAction(CString sActionCode, CString sActionDetail, CString sStartTime, CString sEndTime, CString sErrNo, CString sErrCat, CString sErrMsg);

	void Get_RecipeList(CString sRecipeData);
	void Get_TerminalOK();

	void Get_MGZIdReport(CString sType, CString sMGZId);
	void Get_MGZIdRemove(CString sType, CString sMGZId, CString sRecipe);
	void Get_CarrierIdReport(CString sType, CString sMGZID, CString sCarrierID);
	void Get_CarrierOutReport(CString sMGZID, CString sCarrierID, CString sSlotNo);
	void Get_CarrierInReport(CString sType, CString sLotId, CString sMGZID, CString sCarrierID, CString sSlotNo);

public:
	void Send_Command(CString sSend);
	void Initialize();
	void Terminate();

	BOOL Is_Connected() { return m_bConnected; }

	void Set_ControlState(int nFlag);	// 1:Online, 2:Offline
	void Set_LotStart();
	void Set_LotCancel();
	void Set_CmResult();
	void Set_TimeSync();
	void Set_ModuleFail();
	void Set_MGZConfirm();
	void Set_MGZCancel();
	void Set_CarrierConfirm();
	void Set_CarrierCancel();

	void Set_IdleReasonCode(map<CString, CString>& data);
	void Set_DownActionCode(map<CString, CString>& data);

	void Set_RecipeListRequest(BOOL bList);
	void Set_HostMsg(CString sMsg);
};

extern CHandler g_objHandler;

///////////////////////////////////////////////////////////////////////////////
