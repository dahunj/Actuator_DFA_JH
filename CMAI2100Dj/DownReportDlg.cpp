// DownReportDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CMAI2100.h"
#include "DownReportDlg.h"
#include "afxdialogex.h"

#include "MesAgent.h"
#include "LogFile.h"

CDownReportDlg g_dlgDownReport;

IMPLEMENT_DYNAMIC(CDownReportDlg, CDialogEx)

CDownReportDlg::CDownReportDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDownReportDlg::IDD, pParent)
{

}

CDownReportDlg::~CDownReportDlg()
{
}

void CDownReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LBL_TITLE, m_LabelTitle);
	for (int i = 0; i < 5; i++) DDX_Control(pDX, IDC_LABEL_0 + i, m_Label[i]);
	DDX_Control(pDX, IDC_STC_ALM_CAT, m_stcAlmCatNo);
	DDX_Control(pDX, IDC_CBO_DOWNREASONCAT, m_cboDownReasonCat);
	DDX_Control(pDX, IDC_CBO_DOWNREASON, m_cboDownReason);
	DDX_Control(pDX, IDC_CBO_DOWNACTION, m_cboDownAction);
	DDX_Control(pDX, IDC_CBO_DOWNACTIONDETAIL, m_cboDownActionDetail);
	
	DDX_Control(pDX, IDC_STC_ALARMTIME, m_stcAlmTime);

	DDX_Control(pDX, IDC_EDT_ACTION_DETAIL, m_edtActionDetail);
}


BEGIN_MESSAGE_MAP(CDownReportDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_CBO_DOWNACTIONDETAIL, &CDownReportDlg::OnCbnSelchangeCboDownactiondetail)
	ON_BN_CLICKED(IDC_BTN_REPORT, &CDownReportDlg::OnBnClickedBtnReport)
END_MESSAGE_MAP()


// CDownReportDlg 메시지 처리기입니다.


BOOL CDownReportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	Initial_Controls();

	gDown.bDownClear = FALSE;
	gDown.bDownHappen = FALSE;
	gDown.bPDT = FALSE;

	
	CString strErrPick, strMiddle, strMiddleMsg;

	if (gData.nLanguage == 0) strErrPick = gsCurrentDir + "\\System\\ErrorList_KOR.ini";
	else					  strErrPick = gsCurrentDir + "\\System\\ErrorList_ENG.ini";
	CIniFileCS INI(strErrPick);
	if (!INI.Check_File()) {
		AfxMessageBox("ErrorList.ini File Not Found!!!");
		return FALSE;
	}
	m_cboDownReason.ResetContent();
	for(int i = 0; i < 5; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_01", strMiddle, "");
		m_cboDownReason.AddString(strMiddleMsg);
	}

	for(int i = 0; i < 7; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_02", strMiddle, "");
		m_cboDownReason.AddString(strMiddleMsg);
	}

	for(int i = 0; i < 6; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_03", strMiddle, "");
		m_cboDownReason.AddString(strMiddleMsg);
	}

	for(int i = 0; i < 9; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_04", strMiddle, "");
		m_cboDownReason.AddString(strMiddleMsg);
	}

	for(int i = 0; i < 5; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_05", strMiddle, "");
		m_cboDownReason.AddString(strMiddleMsg);
	}

	for(int i = 0; i < 4; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_06", strMiddle, "");
		m_cboDownReason.SetWindowText(strMiddleMsg);
	}

	for(int i = 0; i < 7; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_07", strMiddle, "");
		m_cboDownReason.AddString(strMiddleMsg);
	}

	for(int i = 0; i < 5; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_08", strMiddle, "");
		m_cboDownReason.SetWindowText(strMiddleMsg);
	}

	for(int i = 0; i < 1; i++)
	{
		strMiddle.Format("%02d", i+1);
		strMiddleMsg = INI.Get_String("CAT_TYPE_99", strMiddle, "");
		m_cboDownReason.AddString(strMiddleMsg);
	}

	CString sAction, sDetail, sKey;

	m_cboDownAction.ResetContent();
	m_cboDownActionDetail.ResetContent();
	for(int i = 1; ;i++)
	{
		sKey.Format("%02d", i);
		sAction = INI.Get_String("DOWN_ACTION", sKey, "");
		sDetail = INI.Get_String("ACTION_DETAIL", sKey, "");

		m_cboDownAction.AddString(sAction);
		m_cboDownActionDetail.AddString(sDetail);

		if(sAction == "") break;
	}	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDownReportDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSKEYDOWN)
	{
		//if (pMsg->wParam == VK_F4)
		//{
		//	// Alt + F4 차단
		//	return TRUE;
		//}
	}

	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDownReportDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow) 
	{
		g_objMesAgent.Set_EquipState(eEquipState::DOWN);	
		g_objMesAgent.Set_UnitState(eEquipState::DOWN);
		
		gDown.bDownClear = FALSE;
		gDown.bDownHappen = FALSE;

		CString sTempCat;
		sTempCat.Format("00004%s", gAlm.sAlmCatMajor);

		m_stcAlmCatNo.SetWindowText(sTempCat);
		m_edtActionDetail.SetWindowText("");

		CString strErrPick, strMiddle, strMiddleMsg;

		if (gData.nLanguage == 0) strErrPick = gsCurrentDir + "\\System\\ErrorList_KOR.ini";
		else					  strErrPick = gsCurrentDir + "\\System\\ErrorList_ENG.ini";
		CIniFileCS INI(strErrPick);
		if (!INI.Check_File()) {
			AfxMessageBox("ErrorList.ini File Not Found!!!");
			return;
		}

		
		////////////////////// Down Action 
		int nIndex = 0;
		gAlm.sAlmCatMajor.Empty();
		gAlm.sAlmCatMiddle.Empty();

		CString strCat, strTemp;
		strCat = INI.Get_String("CAT_ID_MATCH", gDown.strErrNo, "");
		AfxExtractSubString(gAlm.sAlmCatMajor, strCat, 0, '-');
		AfxExtractSubString(gAlm.sAlmCatMiddle, strCat, 1, '-');

		for (std::map<CString, CString>::const_iterator it = m_mssDownAction.begin();
			it != m_mssDownAction.end();
			++it)
		{
			const CString& strKey   = it->first;
			const CString& strValue = it->second;

			strTemp = strKey.Right(2);			
			if (strTemp == gAlm.sAlmCatMajor)
			{
				nIndex = m_cboDownReasonCat.FindStringExact(-1, strValue);
				m_cboDownReasonCat.SetCurSel(nIndex);
			}
		}

		CString strCatMsg, strMajorNo;
		strMajorNo.Format("CAT_TYPE_%s", gAlm.sAlmCatMajor);
		strCatMsg = INI.Get_String(strMajorNo, gAlm.sAlmCatMiddle, "");

		nIndex = m_cboDownReason.FindStringExact(-1, strCatMsg);
		m_cboDownReason.SetCurSel(nIndex);


		SYSTEMTIME time;
		GetLocalTime(&time);

		CString strTime;
		strTime.Format("%04d-%02d-%02d %s", time.wYear, time.wMonth, time.wDay, gData.sAlarmTime[0]);
		m_stcAlmTime.SetWindowText(strTime);

		m_strAlmStart.Format("%04d%02d%02d%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

		
	} 
	else
	{
		KillTimer(0);
	}

	
}


void CDownReportDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);
}


void CDownReportDlg::OnBnClickedBtnReport()
{
	CString strReasonCat, strReason, strActionCode, strActionDetail;

	int nIndex = m_cboDownReasonCat.GetCurSel();

	if (nIndex != CB_ERR)
	{
		m_cboDownReasonCat.GetLBText(nIndex, strReasonCat);
	}

	gAlm.sAlmCatMajor.Format("%02d", nIndex+1);

	if (!strReasonCat.IsEmpty())
	{
		// Pass
	}
	else
	{
		AfxMessageBox("Please Input Down Reason Cat");
		return;
	}


	nIndex = m_cboDownReason.GetCurSel();
	if (nIndex != CB_ERR)
	{
		m_cboDownReason.GetLBText(nIndex, strReason);
	}
	if (!strReason.IsEmpty())
	{
		// Pass
	}
	else
	{
		AfxMessageBox("Please Input Down Reason");
		return;
	}

	nIndex = m_cboDownAction.GetCurSel();
	if (nIndex != CB_ERR)
	{
		m_cboDownAction.GetLBText(nIndex, strActionCode);
	}
	if (!strActionCode.IsEmpty())
	{
		// Pass
	}
	else
	{
		AfxMessageBox("Please Input Down Action");
		return;
	}

	strActionDetail.Empty();
	m_edtActionDetail.GetWindowText(strActionDetail);
	if(strActionDetail.GetLength() < 5)
	{
		AfxMessageBox("Please Input Action Detail more than 5 string");
		return;
	}

	SYSTEMTIME time;
	GetLocalTime(&time);
	m_strAlmEnd.Format("%04d%02d%02d%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);		
	
	if(gDown.bPDT) g_objMesAgent.Set_DownActionReport(strActionCode, strActionDetail, m_strAlmStart, m_strAlmEnd, 0 , 0, gDown.strErrMsg);
	else g_objMesAgent.Set_DownActionReport(strActionCode, strActionDetail, m_strAlmStart, m_strAlmEnd, gDown.nErrorNo , atoi(gAlm.sAlmCatMajor), gDown.strErrMsg);
		
	g_objMesAgent.Set_EquipState(eEquipState::IDLE);	
	g_objMesAgent.Set_UnitState(eEquipState::IDLE);

	gDown.bDownHappen = FALSE;
	gDown.bDownClear = TRUE;
	g_objLogFile.Save_HandlerLog("[Down Report] OK button push");
	ShowWindow(SW_HIDE);
}


void CDownReportDlg::Initial_Controls() 
{
	m_LabelTitle.Init_Ctrl("바탕", 24, TRUE, RGB(0xFF, 0xFF, 0xFF),RGB(0x2F, 0x20, 0xA0));

	for (int i = 0; i < 5; i++) m_Label[i].Init_Ctrl("바탕", 16, TRUE, RGB(0xFF, 0xFF, 0xFF), RGB(0x60, 0x60, 0x60));

	m_stcAlmTime.Init_Ctrl("바탕", 16, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xF0, 0xF0));	

	m_cboDownReasonCat.Init_Ctrl("바탕", 16, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xF0, 0xF0));

	m_cboDownReason.Init_Ctrl("바탕", 16, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xF0, 0xF0));
	m_cboDownAction.Init_Ctrl("바탕", 16, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xF0, 0xF0));
	m_cboDownActionDetail.Init_Ctrl("바탕", 16, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xF0, 0xF0));

	m_stcAlmTime.Init_Ctrl("바탕", 16, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xF0, 0xF0));
	m_stcAlmCatNo.Init_Ctrl("바탕", 16, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xF0, 0xF0));

	m_edtActionDetail.Init_Ctrl("바탕", 16, TRUE, RGB(0x00, 0x00, 0x00), RGB(0xE0, 0xF0, 0xF0));
}

void CDownReportDlg::Set_DownActionCboList(CString sData)
{
	
	CString sCode, sText;

	int i = 1, j = 2;
	BOOL bCode = FALSE, bText = FALSE;
	m_cboDownReasonCat.ResetContent();
	while(TRUE)
	{
		bCode = AfxExtractSubString(sCode, sData, i, '-');
		bText = AfxExtractSubString(sText, sData, j, '-');

		if(!bCode) break;

		m_cboDownReasonCat.AddString(sText);		
		m_mssDownAction.insert(make_pair(sCode, sText));
		i += 2; j += 2;		
	}		
}


void CDownReportDlg::OnCbnSelchangeCboDownactiondetail()
{
	CString sText;
	m_cboDownActionDetail.GetWindowText(sText);
	m_edtActionDetail.SetWindowText(sText);
}