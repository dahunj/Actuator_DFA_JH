#pragma once
#include "DispatchV1.h"
#include "RosProtocol.h"


class CROSEmulatorDlg;
class CDispatcher : public CWnd
{
	DECLARE_DYNAMIC(CDispatcher)
public:

	CDispatcher();
	virtual ~ CDispatcher();
	void Initialize(CROSEmulatorDlg* pParentDlg);
	void Connect();
	void Terminate();
	void Receive_Command(CString sCommand, CString sIp);
	void OnUdpClientReceive();
	void Set_ConnectRequest();
	BOOL Is_Connected() const
	{
		return m_bConnected;
	}

	void Set_ParentDlg(CROSEmulatorDlg* pDlg)
	{
		m_pParentDlg = pDlg;
	}

protected:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

private:

	struct JUDGE_PARAM
	{
		DWORD dwStart, dwDelay;
		CString sIp;
		std::vector<std::string> strArg;
		char chJudge;

	};
	CDispatchV1 m_Dispatch1;
	CROSEmulatorDlg* m_pParentDlg;
	BOOL m_bRunning, m_bConnected;
	CString m_sRosIp, m_sHandlerIp;

	unsigned m_nRecvCmdCount, m_nSendCmdCount, m_nSendFailCount;
	unsigned m_nJudgeGoodCount, m_nJudgeNGCount, m_nJudgeRepairCount;
	std::vector<JUDGE_PARAM> m_vecJudgeReply;

	void Get_ConnectRequest();
	void Get_ConnectReply();
	void Get_ConnectEnd();
	void Get_StatusRequest();
	void Get_JudgeRequest(const std::vector<std::string>& strArg);
	void Set_ConnectReply();
	void Set_ConnectEnd();
	void Set_StatusReply();
	BOOL Set_JudgeReply(const JUDGE_PARAM& stJudge);
	BOOL Send_Command(CString sIp, CString strSendSocket);
	int Get_Random(int nStart, int nEnd);

	bool Get_Settings(int& nDelayMin, int& nDelayMax, int& nGoodPercent, int& nNGPercent);
	void Save_DispatcherLog(CString strLog);
	void Update_Status();
};


extern CDispatcher g_objDispatcher;
