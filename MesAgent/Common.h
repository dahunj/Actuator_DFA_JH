// Common.h : 헤더 파일
//
#pragma once

// CCommon

class CCommon : public CWnd
{
	DECLARE_DYNAMIC(CCommon)

public:
	CCommon();
	virtual ~CCommon();

private:
	CServerSocketCS m_Server;
	CClientSocketCS m_Client;
	int			m_nClientIdx;
	BOOL		m_socketOpen;

protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL Read_Config();
	void Delete_LogAll();
	void Delete_LogFile(CString sPath);
	void DoEvents(int nSleep = 0);

	void Set_LabelPrinter(CString strXml);
	void Send_Command(CString sSend);
};

extern CCommon g_objCommon;

///////////////////////////////////////////////////////////////////////////////
