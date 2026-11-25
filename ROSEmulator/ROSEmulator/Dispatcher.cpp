#include "stdafx.h"
#include "Dispatcher.h"
#include "ROSEmulatorDlg.h"
#include "RosNetwork.h"

IMPLEMENT_DYNAMIC(CDispatcher, CWnd)

CDispatcher g_objDispatcher;


BEGIN_MESSAGE_MAP(CDispatcher, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CDispatcher::CDispatcher()
	: m_pParentDlg(NULL),
	  m_bRunning(FALSE),
	  m_bConnected(FALSE),
	  m_nRecvCmdCount(0),
	  m_nSendCmdCount(0),
	  m_nSendFailCount(0),
	  m_nJudgeGoodCount(0),
	  m_nJudgeNGCount(0),
	  m_nJudgeRepairCount(0)
{
}


CDispatcher::~CDispatcher()
{
}


void CDispatcher::Initialize(CROSEmulatorDlg* pParentDlg)
{
	m_pParentDlg = pParentDlg;

	if (!GetSafeHwnd())
	{
		if (!Create(NULL, NULL, WS_CHILD, CRect(0, 0, 0, 0), pParentDlg, 0))
		{
			Save_DispatcherLog("Cannot create Dispatcher message window.");
			return;
		}
	}

	srand((unsigned)time(NULL) ^ GetTickCount());
}


void CDispatcher::Save_DispatcherLog(CString strLog)
{
	if (m_pParentDlg && m_pParentDlg->GetSafeHwnd())
		m_pParentDlg->Save_DispatcherLog(strLog);
}


bool CDispatcher::Get_Settings(int& nDelayMin, int& nDelayMax, int& nGoodPercent, int& nNGPercent)
{
	BOOL a, b, c, d;

	nDelayMin = (int)m_pParentDlg->GetDlgItemInt(IDC_MIN, &a, FALSE);
	nDelayMax = (int)m_pParentDlg->GetDlgItemInt(IDC_MAX, &b, FALSE);
	nGoodPercent = (int)m_pParentDlg->GetDlgItemInt(IDC_GOOD, &c, FALSE);
	nNGPercent = (int)m_pParentDlg->GetDlgItemInt(IDC_NG, &d, FALSE);

	if (!a ||
		!b ||
		!c ||
		!d ||
		nDelayMin < 0 ||
		nDelayMax < nDelayMin ||
		nDelayMax > 600000 ||
		nGoodPercent < 0 ||
		nNGPercent < 0 ||
		nGoodPercent > 100 ||
		nNGPercent > 100 ||
		nGoodPercent + nNGPercent > 100)
	{
		Save_DispatcherLog("Invalid settings: 0 <= delay min <= max <= 600000; G + N <= 100.");

		return false;
	}

	return true;
}


void CDispatcher::Connect()
{
	if (m_bRunning)
		return;

	if (!GetSafeHwnd() || !m_pParentDlg)
		return;

	int nDelayMin, nDelayMax, nGoodPercent, nNGPercent;

	if (!Get_Settings(nDelayMin, nDelayMax, nGoodPercent, nNGPercent))
		return;

	m_pParentDlg->GetDlgItemText(IDC_LOCAL, m_sRosIp);
	m_pParentDlg->GetDlgItemText(IDC_HANDLER, m_sHandlerIp);
	m_sRosIp.Trim();
	m_sHandlerIp.Trim();

	BOOL bValidPort = FALSE;
	const UINT nLocalPort = m_pParentDlg->GetDlgItemInt(IDC_ROS_PORT, &bValidPort, FALSE);
	if (!bValidPort || nLocalPort == 0 || nLocalPort > 65535)
	{
		Save_DispatcherLog("ROS UDP port must be between 1 and 65535.");
		return;
	}

	const unsigned long nRosAddress = inet_addr(m_sRosIp);
	const unsigned long nHandlerAddress = inet_addr(m_sHandlerIp);
	if (nRosAddress == INADDR_NONE || nRosAddress == INADDR_ANY ||
		nHandlerAddress == INADDR_NONE || nHandlerAddress == INADDR_ANY ||
		(nRosAddress == nHandlerAddress && nLocalPort == HANDLER_LISTEN_PORT))
	{
		Save_DispatcherLog("Use valid IPv4 addresses and distinct ROS and Handler endpoints.");
		return;
	}

	if (!m_Dispatch1.Initialize(this, m_sRosIp, nLocalPort))
	{
		CString strLog;

		strLog.Format("Bind failed on %s:%u (WSA %d).", (LPCSTR)m_sRosIp, nLocalPort, CAsyncSocket::GetLastError());
		Save_DispatcherLog(strLog);
		return;
	}

	if (!SetTimer(1, 10, NULL))
	{
		m_Dispatch1.Terminate();
		Save_DispatcherLog("Cannot create reply timer.");
		return;
	}

	m_bRunning = true;
	m_bConnected = false;
	m_vecJudgeReply.clear();
	m_nRecvCmdCount = m_nSendCmdCount = m_nSendFailCount = m_nJudgeGoodCount = m_nJudgeNGCount = m_nJudgeRepairCount = 0;
	m_pParentDlg->GetDlgItem(IDC_LOCAL)->EnableWindow(FALSE);
	m_pParentDlg->GetDlgItem(IDC_HANDLER)->EnableWindow(FALSE);
	m_pParentDlg->GetDlgItem(IDC_ROS_PORT)->EnableWindow(FALSE);
	CString strLog;
	strLog.Format("Listening on %s:%u. Sending to %s:%d.",
		(LPCSTR)m_sRosIp, nLocalPort, (LPCSTR)m_sHandlerIp, HANDLER_LISTEN_PORT);
	Save_DispatcherLog(strLog);
	Set_ConnectRequest();
	Update_Status();
}


void CDispatcher::Set_ConnectRequest()
{
	if (!m_bRunning)
	{
		Save_DispatcherLog("Start first.");
		return;
	}

	CString strSendCmd;

	strSendCmd.Format("@CONNECT,REQUEST,%s\n", (LPCSTR)m_sRosIp);
	Send_Command(m_sHandlerIp, strSendCmd);
}


void CDispatcher::Terminate()
{
	if (!m_bRunning)
		return;

	Set_ConnectEnd();
	KillTimer(1);
	m_Dispatch1.Terminate();
	m_vecJudgeReply.clear();
	m_bRunning = false;
	m_bConnected = false;
	m_pParentDlg->GetDlgItem(IDC_LOCAL)->EnableWindow(TRUE);
	m_pParentDlg->GetDlgItem(IDC_HANDLER)->EnableWindow(TRUE);
	m_pParentDlg->GetDlgItem(IDC_ROS_PORT)->EnableWindow(TRUE);
	Save_DispatcherLog("Stopped; pending replies cancelled.");
	Update_Status();
}


void CDispatcher::Update_Status()
{
	CString strLog;

	strLog.Format("%s / %s | RX %u TX %u Fail %u Pending %u | G %u N %u R %u",
		m_bRunning ? "Listening" : "Stopped",
		m_bConnected ? "Connected" : "Disconnected",
		m_nRecvCmdCount,
		m_nSendCmdCount,
		m_nSendFailCount,
		(unsigned)m_vecJudgeReply.size(),
		m_nJudgeGoodCount,
		m_nJudgeNGCount,
		m_nJudgeRepairCount);
	m_pParentDlg->SetDlgItemText(IDC_STATE, strLog);
}


BOOL CDispatcher::Send_Command(CString sIp, CString strSendSocket)
{
	const CString& sTargetIp = sIp;
	std::string strPacket = (LPCSTR)strSendSocket;
	int n = m_Dispatch1.Send_Message(CString(strPacket.c_str()), sTargetIp);
	CString strLog;

	if (n != (int)strPacket.size())
	{
		++m_nSendFailCount;
		strLog.Format("TX failed (%d) to %s:8020", CAsyncSocket::GetLastError(), (LPCSTR)sTargetIp);
		Save_DispatcherLog(strLog);

		return false;
	}

	++m_nSendCmdCount;
	std::string strRecvCmd = strPacket;

	while (!strRecvCmd.empty() && (strRecvCmd[strRecvCmd.size() - 1] == '\n' || strRecvCmd[strRecvCmd.size() - 1] == '\r'))
		strRecvCmd.erase(strRecvCmd.size() - 1);
	strLog.Format("TX %s:8020 %s", (LPCSTR)sTargetIp, strRecvCmd.c_str());
	Save_DispatcherLog(strLog);

	return true;
}


void CDispatcher::OnUdpClientReceive()
{
	for (int nCount = 0; nCount < 256 && m_bRunning; ++nCount)
	{
		char chRecv[1025];
		CString strIp;
		UINT nPort = 0;
		int n = m_Dispatch1.ReceiveFrom(chRecv, 1024, strIp, nPort);

		if (n == SOCKET_ERROR)
		{
			int nErrorCode = CAsyncSocket::GetLastError();

			if (nErrorCode != WSAEWOULDBLOCK)
			{
				CString strLog;

				strLog.Format("RX error %d", nErrorCode);
				Save_DispatcherLog(strLog);
			}

			break;
		}

		if (n == 0)
			continue;

		if (strIp != m_sHandlerIp)
		{
			Save_DispatcherLog("Ignored packet from an unconfigured Handler IP.");
			continue;
		}

		std::string strPacket(chRecv, n);

		size_t nStart = 0;
		// UDP packets are independent. Never combine data from separate senders/datagrams.
		while (nStart < strPacket.size())
		{
			size_t nEnd = strPacket.find('\n', nStart);

			if (nEnd == std::string::npos)
			{
				Save_DispatcherLog("Ignored incomplete frame (missing LF).");
				break;
			}

			std::string strFrame = strPacket.substr(nStart, nEnd - nStart);

			nStart = nEnd + 1;

			if (!strFrame.empty() && strFrame[strFrame.size() - 1] == '\r')
				strFrame.erase(strFrame.size() - 1);

			if (strFrame.empty() || strFrame[0] != '@')
			{
				Save_DispatcherLog("Ignored malformed frame.");
				continue;
			}

			Receive_Command(CString(strFrame.substr(1).c_str()), strIp);
		}
	}

	Update_Status();
}


void CDispatcher::Receive_Command(CString sCommand, CString sIp)
{
	std::string strRecvCmd = (LPCSTR)sCommand;

	const CString& strIp = sIp;
	++m_nRecvCmdCount;
	CString strLog;

	strLog.Format("RX %s %s", (LPCSTR)strIp, strRecvCmd.c_str());
	Save_DispatcherLog(strLog);
	std::vector<std::string> strArg = RosProtocol::Split(strRecvCmd);

	if (strArg.size() < 3)
		return;

	if (strArg[0] == "CONNECT")
	{
		if (strArg[1] == "REQUEST")
		{
			Get_ConnectRequest();
		}
		else if (strArg[1] == "REPLY")
			Get_ConnectReply();
		else if (strArg[1] == "END")
			Get_ConnectEnd();
		// No END echo loop.
	}
	else if (strArg[0] == "STATUS" && strArg[1] == "REQUEST")
	{
		Get_StatusRequest();
	}
	else if (strArg[0] == "JUDGE" && strArg[1] == "REQUEST")
	{
		Get_JudgeRequest(strArg);
	}
	else if (strArg[0] == "LOT" && strArg[1] == "END" && strArg.size() >= 6)
	{
		for (size_t i = 0; i < m_vecJudgeReply.size();)
		{
			std::vector<std::string> strLotArg = m_vecJudgeReply[i].strArg;

			if (strLotArg[3] == strArg[3] && strLotArg[4] == strArg[4] && strLotArg[5] == strArg[5])
				m_vecJudgeReply.erase(m_vecJudgeReply.begin() + i);
			else++i;
		}
	}

	// LOT START, INFO REPLY, TIME UPDATE are logged; Dispatcher expects no acknowledgement.
}


void CDispatcher::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && m_bRunning)
	{
		DWORD dwTime = GetTickCount();

		for (size_t i = 0; i < m_vecJudgeReply.size();)
		{
			if ((DWORD)(dwTime - m_vecJudgeReply[i].dwStart) >= m_vecJudgeReply[i].dwDelay)
			{
				if (Set_JudgeReply(m_vecJudgeReply[i]))
				{
					if (m_vecJudgeReply[i].chJudge == 'G')
						++m_nJudgeGoodCount;
					else if (m_vecJudgeReply[i].chJudge == 'N')
						++m_nJudgeNGCount;
					else++m_nJudgeRepairCount;
				}

				m_vecJudgeReply.erase(m_vecJudgeReply.begin() + i);
			}
			else++i;
		}

		Update_Status();
	}

	CWnd::OnTimer(nIDEvent);
}


void CDispatcher::Get_ConnectRequest()
{
	Set_ConnectReply();
}


void CDispatcher::Get_ConnectReply()
{
	m_bConnected = TRUE;
}


void CDispatcher::Get_ConnectEnd()
{
	m_bConnected = FALSE;
	m_vecJudgeReply.clear();
}


void CDispatcher::Get_StatusRequest()
{
	Set_StatusReply();
}


void CDispatcher::Set_ConnectReply()
{
	CString strSendCmd;

	strSendCmd.Format("@CONNECT,REPLY,%s\n", (LPCSTR)m_sRosIp);
	m_bConnected = Send_Command(m_sHandlerIp, strSendCmd);
}


void CDispatcher::Set_StatusReply()
{
	CString strSendCmd;

	strSendCmd.Format("@STATUS,REPLY,%s,1\n", (LPCSTR)m_sRosIp);
	Send_Command(m_sHandlerIp, strSendCmd);
}


int CDispatcher::Get_Random(int nStart, int nEnd)
{
	if (nStart >= nEnd)
		return nStart;

	return nStart + (int)(((double) rand() / ((double) RAND_MAX + 1.0)) * (nEnd - nStart + 1));
}


void CDispatcher::Get_JudgeRequest(const std::vector<std::string>& strArg)
{
	if (!RosProtocol::JudgeRequest(strArg))
	{
		CString strError;
		if (strArg.size() != 11)
			strError.Format("Invalid JUDGE field count: received %u; expected 11 including image count.", (unsigned int)strArg.size());
		else if (!RosProtocol::Number(strArg[5], 1, 30) || !RosProtocol::Number(strArg[6], 1, 1) || !RosProtocol::Number(strArg[7], 1, 40))
			strError.Format("Invalid JUDGE index: Port=%s, Tray=%s, Module=%s; expected Port 1..30, Tray 1, Module 1..40.", strArg[5].c_str(), strArg[6].c_str(), strArg[7].c_str());
		else
			strError.Format("Invalid JUDGE image count: %s; expected integer 0..999999.", strArg[10].c_str());
		Save_DispatcherLog(strError);
		return;
	}

	int nDelayMin, nDelayMax, nGoodPercent, nNGPercent;

	if (!Get_Settings(nDelayMin, nDelayMax, nGoodPercent, nNGPercent))
		return;

	int nMode = ((CComboBox *) m_pParentDlg->GetDlgItem(IDC_MODE))->GetCurSel();

	if (nMode == 4)
	{
		Save_DispatcherLog("No-reply mode: intentional timeout.");
		return;
	}

	if (m_vecJudgeReply.size() >= 10000)
	{
		Save_DispatcherLog("Pending queue full: request rejected.");
		return;
	}

	int nRand = Get_Random(0, 99);
	char chJudge = nMode == 1 ? 'G' :
		nMode == 2 ? 'N' :
		nMode == 3 ? 'R' :
		nRand < nGoodPercent ? 'G' :
		nRand < nGoodPercent + nNGPercent ? 'N' :
		'R';
	JUDGE_PARAM stJudge;

	stJudge.dwStart = GetTickCount();
	// Floating scaling supports delay ranges larger than VS2010 RAND_MAX.
	stJudge.dwDelay = (DWORD)Get_Random(nDelayMin, nDelayMax);
	stJudge.sIp = m_sHandlerIp;
	stJudge.chJudge = chJudge;
	stJudge.strArg = strArg;
	m_vecJudgeReply.push_back(stJudge);
}


BOOL CDispatcher::Set_JudgeReply(const JUDGE_PARAM& stJudge)
{
	std::string strSendCmd = RosProtocol::JudgeReply(stJudge.strArg, (LPCSTR)m_sRosIp, stJudge.chJudge);

	return Send_Command(stJudge.sIp, CString(strSendCmd.c_str()));
}


void CDispatcher::Set_ConnectEnd()
{
	CString strSendCmd;

	strSendCmd.Format("@CONNECT,END,%s\n", (LPCSTR)m_sRosIp);
	Send_Command(m_sHandlerIp, strSendCmd);
}


