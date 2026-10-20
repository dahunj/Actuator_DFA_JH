// OCAPProcess.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CMAI2100.h"
#include "OCAPFAI.h"
#include "Common.h"
#include "LogFile.h"
#include "DataManager.h"
#include "afxdialogex.h"
#include <math.h>


// OCAPProcess 대화 상자입니다.
// 동적 생성하는 OCAP 전용 컨트롤 ID (다른 대화상자와 공유하지 않음)
#define IDC_OCAP_CODE_PREV  61000
#define IDC_OCAP_CODE_NEXT  61001
#define IDC_OCAP_CODE_VIEW  61002
#define IDC_OCAP_CODE_FIND  61003

OCAPFAI g_dlgOCAP_FAI;

IMPLEMENT_DYNAMIC(OCAPFAI, CDialogEx)

OCAPFAI::OCAPFAI(CWnd* pParent /*=NULL*/)
	: CDialogEx(OCAPFAI::IDD, pParent)
{
	m_bError = FALSE;
	m_bSetting = TRUE;
	m_nCodePage = 0;
}

OCAPFAI::~OCAPFAI()
{
}

void OCAPFAI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < 2; i++) DDX_Control(pDX, IDC_GROUP_0 + i, m_Group[i]);
	for (int i = 0; i < 7; i++) DDX_Control(pDX, IDC_LABEL_0 + i, m_Label[i]);
	for (int i = 0; i < 9; i++) DDX_Control(pDX, IDC_STC_OCAP_VALUE_0 + i, m_stcOption[i]);
	DDX_Control(pDX, IDC_GRD_OCAP_DATA, m_grdData);
}


BEGIN_MESSAGE_MAP(OCAPFAI, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_OCAP_CODE_PREV, IDC_OCAP_CODE_FIND, OnCodeButton)
	ON_BN_CLICKED(IDCANCEL, &OCAPFAI::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK,		&OCAPFAI::OnBnClickedOk)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_STC_OCAP_VALUE_0, IDC_STC_OCAP_VALUE_8, OnStcOptionClick)
END_MESSAGE_MAP()


BOOL OCAPFAI::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowPos(this, 0, 75, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	Initial_Controls();
	Initial_CodeControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL OCAPFAI::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void OCAPFAI::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow) {
		Display_Option();
		Initial_NameGrid(&m_grdData, 51, 416);
		Load_CodeData();
		Display_CodePage();
		Show_CodeControls();
		Display_Status();
	} else {
	}
}

// OCAPProcess 메시지 처리기입니다.

void OCAPFAI::OnBnClickedCancel()
{
	BOOL bChanged = FALSE;
	CString str;
	for (int i=0; i<100; i++) {
		if (m_sCode[i] != gCap.sFAICode[i] || m_sName[i] != gCap.sFAIName[i] || m_nMZCnt[i] != gCap.nCodeMZCnt[i] || m_dDefect[i] != gCap.dCodeDefect[i]) { bChanged = TRUE; break; }
	}
	for (int i=0; i<10 && !bChanged; i++) {
		int nIx = m_nCodePage * 10 + i;
		m_edtCode[i].GetWindowText(str); if (str != m_sCode[nIx]) bChanged = TRUE;
		m_edtName[i].GetWindowText(str); if (str != m_sName[nIx]) bChanged = TRUE;
		CString strCount; strCount.Format("%d", m_nMZCnt[nIx]);
		m_edtMZCnt[i].GetWindowText(str); if (str != strCount) bChanged = TRUE;
		strCount.Format("%0.3lf", m_dDefect[nIx]);
		m_edtDefect[i].GetWindowText(str); if (str != strCount) bChanged = TRUE;
	}
	if (bChanged && AfxMessageBox("저장하지 않은 코드 설정을 취소하고 닫습니까?", MB_YESNO) != IDYES) return;
	ShowWindow(SW_HIDE);
}


void OCAPFAI::OnBnClickedOk()
{
	if (!Read_CodePage() || !Check_CodeData()) { m_bSetting = TRUE; Show_CodeControls(); return; }

	int		nData1, nData2, nData3, nData4, nData5;
	double	dData1, dData2, dData3, dData4;
	CString strData, strLog;

	CIniFileCS INI(gsCurrentDir + "\\System\\OCAP_Data.ini");
	if (!INI.Check_File()) {
		AfxMessageBox("OCAP_Data.ini File Not Found!");
		return;
	}

	m_stcOption[0].GetWindowText(strData); nData1 = atoi(strData); INI.Set_Integer("OPTION", "MZ_COUNT_01", nData1);
	m_stcOption[1].GetWindowText(strData); dData1 = atof(strData); INI.Set_Double("OPTION", "DEFECTP_01", dData1, "%0.3lf");
	m_stcOption[3].GetWindowText(strData); nData2 = atoi(strData); INI.Set_Integer("OPTION", "MZ_COUNT_02", nData2);
	m_stcOption[4].GetWindowText(strData); dData2 = atof(strData); INI.Set_Double("OPTION", "DEFECTP_02", dData2, "%0.3lf");
	m_stcOption[5].GetWindowText(strData); nData3 = atoi(strData); INI.Set_Integer("OPTION", "MZ_COUNT_03", nData3);
	m_stcOption[6].GetWindowText(strData); dData3 = atof(strData); INI.Set_Double("OPTION", "DEFECTP_03", dData3, "%0.3lf");
	m_stcOption[7].GetWindowText(strData); nData4 = atoi(strData); INI.Set_Integer("OPTION", "MZ_COUNT_04", nData4);
	m_stcOption[8].GetWindowText(strData); dData4 = atof(strData); INI.Set_Double("OPTION", "DEFECTP_04", dData4, "%0.3lf");
	m_stcOption[2].GetWindowText(strData); nData5 = atoi(strData); INI.Set_Integer("OPTION", "MZ_MIN_COUNT", nData5);

	strLog.Format("[OCAP Option] OnBnClickedOk - Data1(%d,%0.3lf) Data2(%d,%0.3lf) Data3(%d,%0.3lf)  Data4(%d,%0.3lf) Skip(%d)", nData1, dData1, nData2, dData2, nData3, dData3, nData4, dData4, nData5);
	g_objLogFile.Save_HandlerLog(strLog);

	// 코드 변경 시 이전 코드의 수량을 새 코드에 사용하지 않는다.
	CString strKey;
	for (int i=0; i<100; i++) {
		strKey.Format("%02d", i + 1);
		INI.Set_String("DATA-CODE", strKey, m_sCode[i]);
		INI.Set_String("DATA-NAME", strKey, m_sName[i]);
		INI.Set_Integer("DATA-MZ-COUNT", strKey, m_nMZCnt[i]);
		INI.Set_Double("DATA-DEFECTP", strKey, m_dDefect[i], "%0.3lf");
		strLog.Format("[OCAP Code] No(%03d) Code(%s) Name(%s) MZ(%d) Defect(%0.3lf)", i + 1, m_sCode[i], m_sName[i], m_nMZCnt[i], m_dDefect[i]);
		g_objLogFile.Save_HandlerLog(strLog);
	}
	// 저장한 파일을 다시 읽어 전체 설정값이 일치하는지 확인한다.
	::WritePrivateProfileString(NULL, NULL, NULL, gsCurrentDir + "\\System\\OCAP_Data.ini");
	BOOL bSaved = TRUE;
	for (int i=0; i<100 && bSaved; i++) {
		strKey.Format("%02d", i + 1);
		if (INI.Get_String("DATA-CODE", strKey, "#READ_ERROR#") != m_sCode[i] ||
			INI.Get_String("DATA-NAME", strKey, "#READ_ERROR#") != m_sName[i] ||
			INI.Get_Integer("DATA-MZ-COUNT", strKey, -9999) != m_nMZCnt[i] ||
			fabs(INI.Get_Double("DATA-DEFECTP", strKey, -9999.0) - m_dDefect[i]) > 0.0000001) bSaved = FALSE;
	}
	int nMZ[4] = { nData1, nData2, nData3, nData4 };
	double dDefect[4] = { dData1, dData2, dData3, dData4 };
	for (int i=0; i<4 && bSaved; i++) {
		strKey.Format("MZ_COUNT_%02d", i + 1);
		if (INI.Get_Integer("OPTION", strKey, -9999) != nMZ[i]) bSaved = FALSE;
		strKey.Format("DEFECTP_%02d", i + 1);
		if (fabs(INI.Get_Double("OPTION", strKey, -9999.0) - dDefect[i]) > 0.0000001) bSaved = FALSE;
	}
	if (INI.Get_Integer("OPTION", "MZ_MIN_COUNT", -9999) != nData5) bSaved = FALSE;
	if (!bSaved) { AfxMessageBox("OCAP_Data.ini 저장/읽기 검증에 실패했습니다. 파일 쓰기 권한을 확인하세요."); return; }
	for (int i=0; i<100; i++) {
		if (m_sCode[i] != gCap.sFAICode[i]) {
			for (int j=0; j<50; j++) gCap.nCount[j][i] = gCap.nFCount[j][i] = 0;
			for (int j=0; j<30; j++) gLot.nOcapCount[j][i] = gLot.nFOcapCount[j][i] = 0;
		}
	}
	if (!g_objDataManager.Read_OCAPData()) return;
	Load_CodeData();
	Display_CodePage();

	Initial_NameGrid(&m_grdData, 51, 416);
	Display_Option();
	Display_Status();
	AfxMessageBox("OCAP_Data.ini 저장 및 읽기 확인이 완료되었습니다.");
}

void OCAPFAI::Initial_Controls() 
{
	for (int i = 0; i < 2; i++) m_Group[i].Init_Ctrl("바탕", 12, TRUE, COLOR_DEFAULT, COLOR_DEFAULT);
	for (int i = 0; i < 6; i++) m_Label[i].Init_Ctrl("바탕", 12, FALSE, RGB(0xFF, 0xFF, 0xFF), RGB(0x80, 0x00, 0x80));
	m_Label[0].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), RGB(0x99, 0x99, 0x00));
	m_Label[4].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), RGB(0x99, 0x99, 0x00));
	m_Label[5].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), RGB(0x99, 0x99, 0x00));
	m_Label[6].Init_Ctrl("바탕", 12, TRUE, RGB(0x00, 0x00, 0x00), RGB(0x99, 0x99, 0x00));
	for (int i = 0; i < 9; i++) m_stcOption[i].Init_Ctrl("바탕", 11, TRUE, COLOR_DEFAULT, RGB(0xFF, 0xFF, 0xE0));
}

void OCAPFAI::Display_Option()
{
	CString strData;
	strData.Format("%d", gCap.nGiMZCnt[0]);	     m_stcOption[0].SetWindowText(strData);
	strData.Format("%0.3lf", gCap.dGiDefect[0]); m_stcOption[1].SetWindowText(strData);
	strData.Format("%d", gCap.nGiMZCnt[1]);	     m_stcOption[3].SetWindowText(strData);
	strData.Format("%0.3lf", gCap.dGiDefect[1]); m_stcOption[4].SetWindowText(strData);
	strData.Format("%d", gCap.nGiMZCnt[2]);	     m_stcOption[5].SetWindowText(strData);
	strData.Format("%0.3lf", gCap.dGiDefect[2]); m_stcOption[6].SetWindowText(strData);
	strData.Format("%d", gCap.nGiMZCnt[3]);	     m_stcOption[7].SetWindowText(strData);
	strData.Format("%0.3lf", gCap.dGiDefect[3]); m_stcOption[8].SetWindowText(strData);
	strData.Format("%d", gCap.nGiMinCnt);	     m_stcOption[2].SetWindowText(strData);
}

void OCAPFAI::OnStcOptionClick(UINT nID)
{
	int		nData = 0;
	double	dData = 0.0;
	int ID = nID - IDC_STC_OCAP_VALUE_0;

	CString strOld, strNew, strLog;
	m_stcOption[ID].GetWindowText(strOld);
	if (g_objCommon.Show_NumPad(strOld, strNew) != IDOK) return;

	if (ID == 0 || ID == 3 || ID == 5 || ID == 7) { nData = atoi(strNew); if (nData < 0 || nData > 40) { AfxMessageBox("연속 매거진 수량 Error.."); return; } }
	if (ID == 1 || ID == 4 || ID == 6 || ID == 8) { dData  = atof(strNew); if (dData <= 0.0 || dData >= 100.0) { AfxMessageBox("기준 불량율 Error.."); return; } }
	if (ID == 2) { nData = atoi(strNew); if (nData <= 0 || nData > 300) { AfxMessageBox("매거진 최소수량 Error.."); return; } }

	m_stcOption[ID].SetWindowText(strNew);
}

void OCAPFAI::Initial_NameGrid(CGridCS *pGrid, int nRows, int nCols)
{
	pGrid->Set_RowCount(nRows);
	pGrid->Set_ColCount(nCols);
	pGrid->Set_FixRowCount(1);
	pGrid->Set_FixColCount(3);

	long lTotalW = pGrid->Get_GridWidth();
	long lTotalH = pGrid->Get_GridHeight();
	long lCellW = lTotalW / nCols;
	long lCellH = 26;
	long  lWidth[16]= { 60,    60,      100,          60,     60,      60,    50,   50,    50,    50,    50,    50,    50,     50,    50,     50};
	CString sTxt[16]= {"날짜","시간","배출배거진ID", "투입", "완공", "양품", "RN", "RR", "치명", "BCR", "MES", "MC", "치수", "7FAI", "Tilt", "Gap"};
	long  lWidthName[2]= { 65, 120 };

	for (int i=0; i<nRows; i++) {
		pGrid->Set_RowHeight(i, lCellH);
		for (int j=0; j<16; j++) {
			if(i==0) {
				pGrid->Set_ColWidth(j, lWidth[j]); pGrid->Set_CellText(i, j, sTxt[j]);
				pGrid->Set_CellBackClr(i, j, RGB(0xCC, 0xCC, 0xCC));
			}
		}

		int nW = 0, nName=0; 
		for (int j=16; j<216; j++) {
			if(i==0) {
				pGrid->Set_ColWidth(j, lWidthName[nW]);
				if (nW == 0) { CString str; str.Format("FAI-%03d", nName + 1); pGrid->Set_CellText(i, j, str); }
				else		 pGrid->Set_CellText(i, j, gCap.sFAIName[nName]);
				nW++; if (nW == 2) { nW = 0; nName++; }
				if (j>=16 && j<=29) pGrid->Set_CellBackClr(i, j, RGB(0xFF, 0xFF, 0xCC));
				if (j>=30 && j<=45) pGrid->Set_CellBackClr(i, j, RGB(0x99, 0xFF, 0x99));
				if (j>=46 && j<=215) pGrid->Set_CellBackClr(i, j, RGB(0x33, 0xFF, 0x33));
			}
		}
		nW = 0; nName=0; 
		for (int j=216; j<416; j++) {
			if(i==0) {
				pGrid->Set_ColWidth(j, lWidthName[nW]);
				if (nW == 0) { CString str; str.Format("ERR-%03d", nName + 1); pGrid->Set_CellText(i, j, str); }
				else		 pGrid->Set_CellText(i, j, gCap.sFAIName[nName]);
				nW++; if (nW == 2) { nW = 0; nName++; }
				if (j>=216 && j<=229) pGrid->Set_CellBackClr(i, j, RGB(0xCC, 0xCC, 0xCC));
				if (j>=230 && j<=245) pGrid->Set_CellBackClr(i, j, RGB(0x99, 0x99, 0x99));
				if (j>=246 && j<=415) pGrid->Set_CellBackClr(i, j, RGB(0x66, 0x66, 0x66));
			}
		}
	}
}

void OCAPFAI::Initial_DataGrid(CGridCS *pGrid, int nRows, int nCols)
{
	pGrid->Set_RowCount(nRows);
	pGrid->Set_ColCount(nCols);
	pGrid->Set_FixRowCount(0);
	pGrid->Set_FixColCount(0);

	long lTotalW = pGrid->Get_GridWidth();
	long lTotalH = pGrid->Get_GridHeight();
	long lCellW = lTotalW / nCols;
	long lCellH = (lTotalH / nRows);
	long  lWidth[50]= { 35, 105, 50, 33, 50};
	for (int i=5; i<nCols; i=i+2) {
		lWidth[i] = lWidth[3];
	}
	for (int i=6; i<nCols; i=i+2) {
		lWidth[i] = lWidth[4];
	}

	for (int i=0; i<nRows; i++) {
		pGrid->Set_RowHeight(i, lCellH);
		for (int j=0; j<nCols; j++) {
			if(i==0) {
				pGrid->Set_ColWidth(j, lWidth[j]);
				pGrid->Set_CellBackClr(i, j, RGB(0xF0, 0xF0, 0xC0));
			}
		}
	}
}

void OCAPFAI::Display_Status()
{
	if (gCap.nMZCycle < 0 || gCap.nMZCycle > 49) return;
	int nIx = (gCap.nMZCycle + 49) % 50;
	for (int i=0; i<50; i++) {
		if (gCap.sDate[nIx].GetLength() < 1) break;
		Display_Grid(i + 1, nIx);
		nIx = (nIx + 49) % 50;
	}
}

void OCAPFAI::Display_Grid(int nDp, int nIx)
{
	if (nDp < 1 || nDp > 50 || nIx < 0 || nIx > 49) return;
	CString str;

	str.Format(_T("%s"), gCap.sDate[nIx]);
	m_grdData.Set_CellFont(nDp, 0, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 0, str);

	str.Format(_T("%s"), gCap.sTime[nIx]);
	m_grdData.Set_CellFont(nDp, 1, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 1, str);

	str.Format(_T("%s"), gCap.sMZID[nIx]);
	m_grdData.Set_CellFont(nDp, 2, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 2, str);

	str.Format(_T("%d"), gCap.nTotCount[nIx]);
	m_grdData.Set_CellFont(nDp, 3, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 3, str);

	str.Format(_T("%d"), gCap.nGoodCount[nIx] + gCap.nROSNGCount[nIx] + gCap.nROSRfCount[nIx]);
	m_grdData.Set_CellFont(nDp, 4, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 4, str);

	str.Format(_T("%d"), gCap.nGoodCount[nIx]);
	m_grdData.Set_CellFont(nDp, 5, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 5, str);

	str.Format(_T("%d"), gCap.nROSNGCount[nIx]);
	m_grdData.Set_CellFont(nDp, 6, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 6, str);

	str.Format(_T("%d"), gCap.nROSRfCount[nIx]);
	m_grdData.Set_CellFont(nDp, 7, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 7, str);

	str.Format(_T("%d"), gCap.nBCRCount[nIx] + gCap.nMESCount[nIx] + gCap.nMCCount[nIx]);
	m_grdData.Set_CellFont(nDp, 8, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 8, str);

	str.Format(_T("%d"), gCap.nBCRCount[nIx]);
	m_grdData.Set_CellFont(nDp, 9, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 9, str);

	str.Format(_T("%d"), gCap.nMESCount[nIx]);
	m_grdData.Set_CellFont(nDp, 10, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 10, str);

	str.Format(_T("%d"), gCap.nMCCount[nIx]);
	m_grdData.Set_CellFont(nDp, 11, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 11, str);

	int nA1 = gCap.nCount[nIx][0] + gCap.nCount[nIx][1] + gCap.nCount[nIx][2] + gCap.nCount[nIx][3] + gCap.nCount[nIx][4] + gCap.nCount[nIx][5] + gCap.nCount[nIx][6];
	int nA2 = gCap.nCount[nIx][7] + gCap.nCount[nIx][8] + gCap.nCount[nIx][9] + gCap.nCount[nIx][10] + gCap.nCount[nIx][11] + gCap.nCount[nIx][12] + gCap.nCount[nIx][13] + gCap.nCount[nIx][14];
	int nA3 = gCap.nCount[nIx][15] + gCap.nCount[nIx][16] + gCap.nCount[nIx][17] + gCap.nCount[nIx][18] + gCap.nCount[nIx][19];
	int nTotal = nA1 + nA2 + nA3;
	for (int i=20; i<100; i++) nTotal += gCap.nCount[nIx][i];
	str.Format(_T("%d"), nTotal);
	m_grdData.Set_CellFont(nDp, 12, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 12, str);

	str.Format(_T("%d"), nA1);
	m_grdData.Set_CellFont(nDp, 13, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 13, str);

	str.Format(_T("%d"), nA2);
	m_grdData.Set_CellFont(nDp, 14, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 14, str);

	str.Format(_T("%d"), nA3);
	m_grdData.Set_CellFont(nDp, 15, str, 10, FALSE);
	m_grdData.Set_CellText(nDp, 15, str);

	double dValue;
	int	   nNo = 16;
	for(int i=0; i<100; i++) {
		str.Format(_T("%d"), gCap.nCount[nIx][i]);
		m_grdData.Set_CellFont(nDp, nNo, str, 10, FALSE);
		m_grdData.Set_CellText(nDp, nNo, str);
		nNo++;

		dValue = gCap.nTotCount[nIx] > 0 ? (gCap.nCount[nIx][i] * 100.0) / gCap.nTotCount[nIx] : 0.0;
		str.Format(_T("%0.1f%%"), dValue);
		m_grdData.Set_CellFont(nDp, nNo, str, 10, FALSE);
		m_grdData.Set_CellText(nDp, nNo, str);
		if		((gCap.dCodeDefect[i] == -1.0 ? gCap.dGiDefect[2] : gCap.dCodeDefect[i]) <= dValue)	m_grdData.Set_CellBackClr(nDp, nNo, RGB(0xFF, 0x00, 0x00));	// 3 NG
		else if	((gCap.dCodeDefect[i] == -1.0 ? gCap.dGiDefect[1] : gCap.dCodeDefect[i]) <= dValue)	m_grdData.Set_CellBackClr(nDp, nNo, RGB(0x00, 0x00, 0xFF));	// 2 NG
		else if	((gCap.dCodeDefect[i] == -1.0 ? gCap.dGiDefect[0] : gCap.dCodeDefect[i]) <= dValue)	m_grdData.Set_CellBackClr(nDp, nNo, RGB(0x00, 0xFF, 0x00));	// 1 NG
		else									m_grdData.Set_CellBackClr(nDp, nNo, RGB(0xFF, 0xFF, 0xFF));	// 0 Empty

		if (gCap.nTotCount[nIx] < gCap.nGiMinCnt) m_grdData.Set_CellBackClr(nDp, nNo, RGB(0xFF, 0xFF, 0xFF));	// 0 Empty
		nNo++;
	}
	nNo = 216;
	for(int i=0; i<100; i++) {
		str.Format(_T("%d"), gCap.nFCount[nIx][i]);
		m_grdData.Set_CellFont(nDp, nNo, str, 10, FALSE);
		m_grdData.Set_CellText(nDp, nNo, str);
		nNo++;

		dValue = gCap.nTotCount[nIx] > 0 ? (gCap.nFCount[nIx][i] * 100.0) / gCap.nTotCount[nIx] : 0.0;
		str.Format(_T("%0.1f%%"), dValue);
		m_grdData.Set_CellFont(nDp, nNo, str, 10, FALSE);
		m_grdData.Set_CellText(nDp, nNo, str);
		if		((gCap.dCodeDefect[i] == -1.0 ? gCap.dGiDefect[3] : gCap.dCodeDefect[i]) <= dValue)	m_grdData.Set_CellBackClr(nDp, nNo, RGB(0xFF, 0x00, 0x00));	// 3 NG
		else									m_grdData.Set_CellBackClr(nDp, nNo, RGB(0xFF, 0xFF, 0xFF));	// 0 Empty
		if (gCap.nTotCount[nIx] < gCap.nGiMinCnt) m_grdData.Set_CellBackClr(nDp, nNo, RGB(0xFF, 0xFF, 0xFF));	// 0 Empty
		nNo++;
	}
}

void OCAPFAI::Set_AddMZData(int nPortNo)
{
	int nNo = nPortNo - 1;
	for (int i=0; i<50; i++) {
		if (gLot.sMZID_GD[nNo] == gCap.sMZID[i]) 
		{
			AddCarToMZ(nNo, i);
			return;
		}
	}

	if (gCap.nMZCycle < 0 || gCap.nMZCycle >= 50) gCap.nMZCycle = 0;
	DelMZData(gCap.nMZCycle);
	AddCarToMZ(nNo, gCap.nMZCycle);
}

void OCAPFAI::AddCarToMZ(int nNo, int nIdx)
{
	gCap.sMZID[nIdx]		= gLot.sMZID_GD[nNo];	//양품MZ
	gCap.nTotCount[nIdx]	= gCap.nTotCount[nIdx]	 + gLot.nCmCount[nNo];			//투입-Module수
	gCap.nGoodCount[nIdx]	= gCap.nGoodCount[nIdx]	 + gLot.nGoodCount[nNo];		//양품수
	gCap.nROSNGCount[nIdx]	= gCap.nROSNGCount[nIdx] + gLot.nRosJugCount[nNo][3];	//ROS-NG수
	gCap.nROSRfCount[nIdx]	= gCap.nROSRfCount[nIdx] + gLot.nRosJugCount[nNo][4];	//ROS-Repaier수
	gCap.nBCRCount[nIdx]	= gCap.nBCRCount[nIdx]	 + gLot.nRosJugCount[nNo][7];	//Barcode[NG수
	gCap.nMESCount[nIdx]	= gCap.nMESCount[nIdx]	 + gLot.nRosJugCount[nNo][8];	//MES[NG수
	gCap.nMCCount[nIdx]		= gCap.nMCCount[nIdx]	 + gLot.nRosJugCount[nNo][9];	//MC[NG수
	for(int i=0; i<100; i++)
	{
		gCap.nCount[nIdx][i]  = gCap.nCount[nIdx][i]  + gLot.nOcapCount[nNo][i];		//MZ,FAI별 발생수
		gCap.nFCount[nIdx][i] = gCap.nFCount[nIdx][i] + gLot.nFOcapCount[nNo][i];		//MZ,FAI별 발생수
	}
	for(int i=0; i<8; i++) 
	{
		if (gCap.sLotID[nIdx][i].GetLength() < 2) 
		{
			gCap.sLotID[nIdx][i] = gLot.sLotID[nNo];
			break;
		}
	}
}

void OCAPFAI::DelMZData(int nMZNo)
{
	if (nMZNo < 0 || nMZNo > 49) return;
	gCap.sDate[nMZNo] = gCap.sTime[nMZNo] = gCap.sMZID[nMZNo] = "";
	gCap.nTotCount[nMZNo] = gCap.nGoodCount[nMZNo] = gCap.nROSNGCount[nMZNo] = gCap.nROSRfCount[nMZNo] = gCap.nBCRCount[nMZNo] = gCap.nMESCount[nMZNo] = gCap.nMCCount[nMZNo] = 0;
	for(int j=0; j<100; j ++) {
		gCap.nCount[nMZNo][j] = gCap.nFCount[nMZNo][j] = 0;
	}
	for(int j=0; j<8; j ++) gCap.sLotID[nMZNo][j] = "";
}

void OCAPFAI::AddMZOut(CString sMZid)
{
	if (gCap.sMZID[gCap.nMZCycle].GetLength() < 1) return;

	SYSTEMTIME time;
	GetLocalTime(&time);

	gCap.sDate[gCap.nMZCycle].Format("%02d/%02d", time.wMonth, time.wDay);
	gCap.sTime[gCap.nMZCycle].Format("%02d:%02d", time.wHour,  time.wMinute);

	CString strLog;
	if (gCap.sMZID[gCap.nMZCycle] != sMZid) {
		strLog.Format("[OCAP Option] MZ Out - Cycle[%d] Data-MZ[%s] Read-MZ[%s]", gCap.nMZCycle, gCap.sMZID[gCap.nMZCycle], sMZid);
		g_objLogFile.Save_HandlerLog(strLog);
	}
	g_objLogFile.Save_OCAPDataLog(gCap.nMZCycle);

	gCap.nMZCycle++;
	if (gCap.nMZCycle >= 50) gCap.nMZCycle = 0;

	m_bError = FALSE;
	Check_DEFECT(3); if (m_bError) return;
	Check_DEFECT(2); if (m_bError) return;
	Check_DEFECT(1); if (m_bError) return;
	Check_DEFECTF(4);
}

void OCAPFAI::Set_AddDEFECT(CString sMZid, CString sCode)
{
	if (gCap.nMZCycle < 0 || gCap.nMZCycle > 49) return;
	if (sMZid.GetLength() < 2 || sCode.GetLength() < 1 ) return;

	for (int i=0; i<50; i++) {
		if (sMZid == gCap.sMZID[i]) 
		{
			for(int j=0; j<100; j++)
			{
				if (sCode == gCap.sFAICode[j]) 
				{
					gCap.nCount[i][j]++;
					return;
				}
			}
			return;
		}
	}
}

void OCAPFAI::Check_DEFECT(int nNo)
{
	int nX = nNo - 1;
	if (nX < 0 || nX > 3) return;

	int nDefCount;
	double dPer = 0.0;
	CString sDefect = "";

	for (int i=0; i<100; i++) {
		if (gCap.sFAICode[i].GetLength() < 1) continue;
		int nMZCnt = gCap.nCodeMZCnt[i] < 0 ? gCap.nGiMZCnt[nX] : gCap.nCodeMZCnt[i];
		if (nMZCnt <= 0 || nMZCnt > 40) continue;
		double dDefect = gCap.dCodeDefect[i] == -1.0 ? gCap.dGiDefect[nX] : gCap.dCodeDefect[i];

		nDefCount = 0;
		int nMZno = gCap.nMZCycle - 1;	if (nMZno < 0) nMZno = 49;
		for (int j=0; j<nMZCnt; j++) {
			if (gCap.sDate[nMZno].IsEmpty() || gCap.nTotCount[nMZno] <= 0 || gCap.nTotCount[nMZno] < gCap.nGiMinCnt) break;

			dPer = (gCap.nCount[nMZno][i] * 100.0) / gCap.nTotCount[nMZno];
			if (dPer <= 0.0 || dPer < dDefect) break;

			nDefCount++;
			if (nDefCount == 1) {
				gCap.sAlmMZID = gCap.sMZID[nMZno];
				gCap.sAlmFAIName.Format("FAI-%s", gCap.sFAIName[i]);
				gCap.dAlmDefect  = dPer;
				gCap.nAlmCount   = nMZCnt;
			}
			nMZno--;
			if (nMZno < 0) nMZno = 49;
		}
		if (nDefCount >= nMZCnt) {
			sDefect = sDefect + gCap.sAlmFAIName + ". ";
		}
	}

	if (sDefect.GetLength() > 1) {
		m_bError = TRUE;
		gAlm.sAlmLotID[0] = sDefect;
		g_objCommon.Show_Error(9180 + nX+1);
		return;
	}
}

void OCAPFAI::Check_DEFECTF(int nNo)
{
	int nX = nNo - 1;
	if (nX < 0 || nX > 3) return;

	int nDefCount;
	double dPer = 0.0;
	CString sDefect = "";

	for (int i=0; i<100; i++) {
		if (gCap.sFAICode[i].GetLength() < 1) continue;
		int nMZCnt = gCap.nCodeMZCnt[i] < 0 ? gCap.nGiMZCnt[nX] : gCap.nCodeMZCnt[i];
		if (nMZCnt <= 0 || nMZCnt > 40) continue;
		double dDefect = gCap.dCodeDefect[i] == -1.0 ? gCap.dGiDefect[nX] : gCap.dCodeDefect[i];

		nDefCount = 0;
		int nMZno = gCap.nMZCycle - 1;	if (nMZno < 0) nMZno = 49;
		for (int j=0; j<nMZCnt; j++) {
			if (gCap.sDate[nMZno].IsEmpty() || gCap.nTotCount[nMZno] <= 0 || gCap.nTotCount[nMZno] < gCap.nGiMinCnt) break;

			dPer = (gCap.nFCount[nMZno][i] * 100.0) / gCap.nTotCount[nMZno];
			if (dPer <= 0.0 || dPer < dDefect) break;

			nDefCount++;
			if (nDefCount == 1) {
				gCap.sAlmMZID = gCap.sMZID[nMZno];
				gCap.sAlmFAIName.Format("ERR-%s", gCap.sFAIName[i]);
				gCap.dAlmDefect  = dPer;
				gCap.nAlmCount   = nMZCnt;
			}
			nMZno--;
			if (nMZno < 0) nMZno = 49;
		}
		if (nDefCount >= nMZCnt) {
			sDefect = sDefect + gCap.sAlmFAIName + ". ";
		}
	}

	if (sDefect.GetLength() > 1) {
		m_bError = TRUE;
		gAlm.sAlmLotID[0] = sDefect;
		g_objCommon.Show_Error(9184);
		return;
	}
}


// 기존 OCAP 리소스 내부에 동적으로 생성하여 다른 화면 리소스는 변경하지 않는다.
static CRect OCAP_CodeRect(CWnd* pWnd, int nX, int nY, int nW, int nH)
{
	CRect rect(nX, nY, nX + nW, nY + nH);
	::MapDialogRect(pWnd->GetSafeHwnd(), &rect);
	return rect;
}

void OCAPFAI::Initial_CodeControls()
{
	CString sTitle[5] = { "No.", "불량 Code", "불량 Name", "연속 매거진 횟수", "기준 불량율 (%)" };
	int nX[5] = { 25, 75, 360, 690, 875 };
	int nW[5] = { 45, 270, 315, 170, 170 };
	for (int i=0; i<5; i++) {
		m_stcCodeTitle[i].Create(sTitle[i], WS_CHILD | SS_CENTERIMAGE, OCAP_CodeRect(this, nX[i], 89, nW[i], 18), this);
		m_stcCodeTitle[i].SetFont(GetFont());
	}
	for (int i=0; i<10; i++) {
		int nY = 112 + i * 28;
		m_stcCodeNo[i].Create("", WS_CHILD | SS_CENTERIMAGE, OCAP_CodeRect(this, 25, nY, 45, 21), this);
		m_edtCode[i].Create(WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, OCAP_CodeRect(this, 75, nY, 270, 21), this, 61100 + i * 4);
		m_edtName[i].Create(WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, OCAP_CodeRect(this, 360, nY, 315, 21), this, 61101 + i * 4);
		m_edtMZCnt[i].Create(WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, OCAP_CodeRect(this, 690, nY, 170, 21), this, 61102 + i * 4);
		m_edtDefect[i].Create(WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, OCAP_CodeRect(this, 875, nY, 170, 21), this, 61103 + i * 4);
		m_stcCodeNo[i].SetFont(GetFont());
		m_edtCode[i].SetFont(GetFont()); m_edtCode[i].LimitText(80);
		m_edtName[i].SetFont(GetFont()); m_edtName[i].LimitText(80);
		m_edtMZCnt[i].SetFont(GetFont()); m_edtMZCnt[i].LimitText(3);
		m_edtDefect[i].SetFont(GetFont()); m_edtDefect[i].LimitText(7);
	}
	m_stcCodeInfo.Create("", WS_CHILD, OCAP_CodeRect(this, 25, 403, 1020, 40), this);
	m_stcCodeInfo.SetFont(GetFont());
	CString sButton[4] = { "이전 10개", "다음 10개", "이력 보기", "빈 코드로 이동" };
	int nBtnX[4] = { 25, 135, 930, 700 };
	int nBtnW[4] = { 100, 100, 125, 180 };
	for (int i=0; i<4; i++) {
		m_btnCode[i].Create(sButton[i], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, OCAP_CodeRect(this, nBtnX[i], 452, nBtnW[i], 26), this, IDC_OCAP_CODE_PREV + i);
		m_btnCode[i].SetFont(GetFont());
	}
	m_grdData.MoveWindow(OCAP_CodeRect(this, 13, 83, 1071, 360));
	m_Label[3].SetWindowText("공통 연속 수량");
}

void OCAPFAI::Load_CodeData()
{
	for (int i=0; i<100; i++) {
		m_sCode[i] = gCap.sFAICode[i];
		m_sName[i] = gCap.sFAIName[i];
		m_nMZCnt[i] = gCap.nCodeMZCnt[i];
		m_dDefect[i] = gCap.dCodeDefect[i];
	}
}

void OCAPFAI::Display_CodePage()
{
	CString str;
	for (int i=0; i<10; i++) {
		int nIx = m_nCodePage * 10 + i;
		str.Format("%03d", nIx + 1); m_stcCodeNo[i].SetWindowText(str);
		m_edtCode[i].SetWindowText(m_sCode[nIx]);
		m_edtName[i].SetWindowText(m_sName[nIx]);
		str.Format("%d", m_nMZCnt[nIx]); m_edtMZCnt[i].SetWindowText(str);
		str.Format("%0.3lf", m_dDefect[nIx]); m_edtDefect[i].SetWindowText(str);
	}
	str.Format("%d / 10 페이지  (%03d ~ %03d)    -1: 위의 공통 조건 사용 / 0: 알람 제외 / 1~40: 코드별 연속 매거진 횟수\r\n불량율: -1은 공통 조건 / 0초과~100.000은 개별 기준(소수 3자리). 횟수와 불량율은 각각 독립 적용하며, [저장]으로 100개를 저장합니다.", m_nCodePage + 1, m_nCodePage * 10 + 1, m_nCodePage * 10 + 10);
	m_stcCodeInfo.SetWindowText(str);
	m_btnCode[0].EnableWindow(m_nCodePage > 0);
	m_btnCode[1].EnableWindow(m_nCodePage < 9);
}

static BOOL OCAP_ReadMZCount(CString str, int& nValue)
{
	str.TrimLeft(); str.TrimRight();
	if (str.IsEmpty()) return FALSE;
	int nStart = str[0] == '-' ? 1 : 0;
	if (str.GetLength() == nStart) return FALSE;
	for (int i=nStart; i<str.GetLength(); i++) if (str[i] < '0' || str[i] > '9') return FALSE;
	nValue = atoi(str);
	return nValue >= -1 && nValue <= 40;
}

static BOOL OCAP_ReadDefectRate(CString str, double& dValue)
{
	str.TrimLeft(); str.TrimRight();
	if (str == "-1" || str == "-1.000") { dValue = -1.0; return TRUE; }
	if (str.IsEmpty()) return FALSE;
	int nPoint = -1, nDigits = 0;
	for (int i=0; i<str.GetLength(); i++) {
		if (str[i] == '.' && nPoint < 0) { nPoint = i; continue; }
		if (str[i] < '0' || str[i] > '9') return FALSE;
		nDigits++;
	}
	if (nDigits == 0 || (nPoint >= 0 && str.GetLength() - nPoint - 1 > 3)) return FALSE;
	dValue = atof(str);
	return dValue > 0.0 && dValue <= 100.0;
}

BOOL OCAPFAI::Read_CodePage()
{
	CString sCode[10], sName[10], str;
	int nCount[10];
	double dDefect[10];
	for (int i=0; i<10; i++) {
		m_edtCode[i].GetWindowText(sCode[i]); sCode[i].TrimLeft(); sCode[i].TrimRight();
		m_edtName[i].GetWindowText(sName[i]); sName[i].TrimLeft(); sName[i].TrimRight();
		m_edtMZCnt[i].GetWindowText(str);
		if (!OCAP_ReadMZCount(str, nCount[i])) {
			AfxMessageBox("연속 매거진 횟수는 -1, 0 또는 1~40 정수로 입력하세요.");
			m_edtMZCnt[i].SetFocus(); return FALSE;
		}
		m_edtDefect[i].GetWindowText(str);
		if (!OCAP_ReadDefectRate(str, dDefect[i])) {
			AfxMessageBox("기준 불량율은 -1 또는 0초과~100.000으로 입력하세요. 소수 3자리까지 가능합니다.");
			m_edtDefect[i].SetFocus(); return FALSE;
		}
		if (sCode[i].FindOneOf("=;\r\n\"\t,") >= 0 || sName[i].FindOneOf("=;\r\n\"\t,") >= 0) {
			AfxMessageBox("Code / Name에 = ; 쉼표 따옴표 탭 문자는 사용할 수 없습니다.");
			m_edtCode[i].SetFocus(); return FALSE;
		}
	}
	for (int i=0; i<10; i++) {
		int nIx = m_nCodePage * 10 + i;
		m_sCode[nIx] = sCode[i]; m_sName[nIx] = sName[i]; m_nMZCnt[nIx] = nCount[i]; m_dDefect[nIx] = dDefect[i];
	}
	return TRUE;
}

BOOL OCAPFAI::Check_CodeData()
{
	CString str;
	for (int i=0; i<100; i++) {
		if (m_sCode[i].IsEmpty()) continue;
		if (m_sName[i].IsEmpty()) {
			str.Format("%03d번 불량 Name을 입력하세요.", i + 1);
			AfxMessageBox(str); m_nCodePage = i / 10; Display_CodePage(); return FALSE;
		}
		for (int j=i+1; j<100; j++) {
			if (m_sCode[i] != m_sCode[j]) continue;
			str.Format("%03d번과 %03d번 불량 Code가 중복됩니다.", i + 1, j + 1);
			AfxMessageBox(str); m_nCodePage = j / 10; Display_CodePage(); return FALSE;
		}
	}
	return TRUE;
}

void OCAPFAI::Show_CodeControls()
{
	int nShow = m_bSetting ? SW_SHOW : SW_HIDE;
	for (int i=0; i<5; i++) m_stcCodeTitle[i].ShowWindow(nShow);
	for (int i=0; i<10; i++) {
		m_stcCodeNo[i].ShowWindow(nShow);
		m_edtCode[i].ShowWindow(nShow); m_edtName[i].ShowWindow(nShow); m_edtMZCnt[i].ShowWindow(nShow); m_edtDefect[i].ShowWindow(nShow);
	}
	m_stcCodeInfo.ShowWindow(nShow);
	for (int i=0; i<4; i++) if (i != 2) m_btnCode[i].ShowWindow(nShow);
	m_btnCode[2].SetWindowText(m_bSetting ? "이력 보기" : "코드 설정");
	m_grdData.ShowWindow(m_bSetting ? SW_HIDE : SW_SHOW);
}

void OCAPFAI::OnCodeButton(UINT nID)
{
	if (!Read_CodePage()) return;
	if (nID == IDC_OCAP_CODE_PREV && m_nCodePage > 0) m_nCodePage--;
	if (nID == IDC_OCAP_CODE_NEXT && m_nCodePage < 9) m_nCodePage++;
	if (nID == IDC_OCAP_CODE_FIND) {
		for (int i=0; i<100; i++) if (m_sCode[i].IsEmpty()) { m_nCodePage = i / 10; break; }
	}
	if (nID == IDC_OCAP_CODE_VIEW) {
		m_bSetting = !m_bSetting;
		if (!m_bSetting) Display_Status();
		Show_CodeControls();
	}
	Display_CodePage();
}

void OCAPFAI::OnClose()
{
	OnBnClickedCancel();
}
