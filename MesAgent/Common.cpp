// Common.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "Common.h"
#include "Wininet.h"
#include <windows.h>
#include <winspool.h>
#include <afxwin.h> 
#include "LogFile.h"

#define	LABEL_PRINTER_IP	"192.168.254.254" // Label Printer Port
#define LABEL_PRINTER_PORT	9100		// Label Printer Port

#define DELETE_LOG_DAY	 180

// CCommon
CCommon g_objCommon;

IMPLEMENT_DYNAMIC(CCommon, CWnd)

CCommon::CCommon()
{
}

CCommon::~CCommon()
{
}

BEGIN_MESSAGE_MAP(CCommon, CWnd)
END_MESSAGE_MAP()

// CCommon 메시지 처리기입니다.

BOOL CCommon::Read_Config()
{
	CIniFileCS INI(gsCurrentDir + "\\Config.ini");
	if (!INI.Check_File()) { AfxMessageBox("Config.ini File Not Found!!!"); return FALSE; }

	gData.nHostPort = INI.Get_Integer("DATA", "HOST_PORT", 0);
	gData.sEquipId = INI.Get_String("DATA", "EQUIP_ID", "");
	gData.bHandlerLog = INI.Get_Bool("DATA", "HANDLER_LOG", FALSE);
	gData.bHostLog = INI.Get_Bool("DATA", "HOST_LOG", FALSE);
	gData.sErrFile = INI.Get_String("DATA", "ERROR_FILE", "");
	gData.bJahwa = INI.Get_Bool("DATA", "JAHWA", FALSE);

	return TRUE;
}

void CCommon::Delete_LogAll()
{
	Delete_LogFile(gsCurrentDir + "\\Handler");
	Delete_LogFile(gsCurrentDir + "\\Host");
	Delete_LogFile(gsCurrentDir + "\\MES");
}

void CCommon::Delete_LogFile(CString sPath)
{
	CString strFindPath, strFilePath, strFileName;
	strFindPath.Format("%s\\*.*", sPath);

	CFileFind Finder;
	BOOL bContinue = Finder.FindFile(strFindPath, NULL);

	CTime DelTime =  CTime::GetCurrentTime() - CTimeSpan(DELETE_LOG_DAY, 0, 0, 0);

	while (bContinue) {
		bContinue = Finder.FindNextFile();

		if (Finder.IsDots()) continue;
		if (Finder.IsDirectory()) continue;

		strFileName = Finder.GetFileName();
		strFilePath.Format("%s\\%s", sPath, strFileName);

		if (strFileName.GetLength() < 8) DeleteFile(strFilePath);	// 불필요한 파일 삭제

		int nYear = atoi(strFileName.Left(4));
		int nMonth = atoi(strFileName.Mid(4, 2));
		int nDay = atoi(strFileName.Mid(6, 2));

		if (nYear > 2000 && nYear < 3000 && nMonth > 0 && nMonth < 13 && nDay > 0 && nDay < 32) {
			CTime LogTime(nYear, nMonth, nDay, 0, 0, 0, 0);
			if (LogTime > DelTime) continue;
		}

		DeleteFile(strFilePath);
	}
}

void CCommon::DoEvents(int nSleep)
{
	MSG msg;
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (nSleep > 0) Sleep(nSleep);
}

///////////////////////////////////////////////////////////////////////////////
void CCommon::Set_LabelPrinter(CString strXml)
{
	CString zplCode = strXml;

	CString strSend;
    
	strSend.Format("SEND,ZPL Code Send To Print,\"%s\"", zplCode); 

    Send_Command(strSend);
}

///////////////////////////////////////////////////////////////////////////////
void CCommon::Send_Command(CString sSend)
{
	 CString sHCACK = "0";

    // 소켓 열기
    if (!m_socketOpen)
    {
        if (!m_Client.Open_Socket(LABEL_PRINTER_IP, LABEL_PRINTER_PORT, this))
        {
            AfxMessageBox("Print Connect Fail");
            g_objLogFile.Save_CommLog("Print Connect Fail");
            sHCACK = "1";  // 일반 오류
            gMes.sLabelPrintHCK = sHCACK;
            return;
        }
        m_socketOpen = true;
        Sleep(300);
    }

    CString strSendSocket;
    strSendSocket.Format("%s\n", sSend);

    char chSend[1001] = { 0 };
    int nLength = strSendSocket.GetLength();

    memcpy(chSend, (LPSTR)(LPCSTR)strSendSocket, nLength);
    chSend[nLength] = '\0';

    if (!m_Client.Write_Socket((BYTE*)chSend, nLength))
    {
        int errorCode = WSAGetLastError();
        CString strError;
        strError.Format("ZPL Data Send Error. Error Code: %d", errorCode);
        AfxMessageBox(strError);
        g_objLogFile.Save_CommLog(strError);

        sHCACK = (errorCode == WSAETIMEDOUT) ? "5" : "2";

        CString strFinalFailLog;
        strFinalFailLog.Format("ZPL 전송 실패 - ErrorCode: %d (Timeout: %s)", errorCode, (errorCode == WSAETIMEDOUT ? "Yes" : "No"));
        g_objLogFile.Save_CommLog(strFinalFailLog);
    }

    // 소켓 닫기
    if (m_socketOpen)
    {
        m_Client.Close_Socket();
        m_socketOpen = false;
    }

    gMes.sLabelPrintHCK = sHCACK;
}

///////////////////////////////////////////////////////////////////////////////////