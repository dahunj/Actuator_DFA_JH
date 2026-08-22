#pragma once


#include <map>
using namespace std;

class CDownReportDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDownReportDlg)

public:
	CDownReportDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDownReportDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DOWN_REPORT_DLG };

	CLabelCS	m_LabelTitle;
	CLabelCS	m_Label[5];

	CStaticCS	m_stcAlmCatNo;

	CComboCS	m_cboDownReasonCat;
	CComboCS	m_cboDownReason;
	CComboCS	m_cboDownAction;
	CComboCS	m_cboDownActionDetail;

	CStaticCS	m_stcAlmTime;

	CEditCS		m_edtActionDetail;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CString		m_strAlmStart;
	CString		m_strAlmEnd;

	map<CString, CString> m_mssDownAction;

public:
	
	BOOL		m_bStart;

	DWORD		m_dwStartTime;
	CString		m_strStartTime;

public:
	void Initial_Controls();

	void Set_DownActionCboList(CString sData);

	afx_msg void OnCbnSelchangeCboDownactiondetail();
	afx_msg void OnBnClickedBtnReport();

};

extern CDownReportDlg g_dlgDownReport;