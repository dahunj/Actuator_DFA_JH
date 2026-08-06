// ErrorDlg.h : 헤더 파일
//
#pragma once

#include <map>
using namespace std;


class CErrorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CErrorDlg)

public:
	CErrorDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CErrorDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ERROR_DLG };
	CGroupCS	m_Group[4];
	CLabelCS	m_Label[13];
	CStaticCS	m_stcErrBack[4];
	CStaticCS	m_stcErrTitle;
	CPictureCS	m_Image;

	CStaticCS	m_stcErrPos[14];
	CStaticCS	m_stcErrNo;
	CStaticCS	m_stcAlmCatNo;


	CStaticCS	m_stcMsgBack;
	CStaticCS	m_stcErrMsg;
	CButtonCS	m_btnErrBuzzOff;
	CButtonCS	m_btnErrSkip;
	CButtonCS	m_btnErrRetry;
	CButtonCS	m_btnErrToShipTray;
	CButtonCS	m_btnErrOK;
	CButtonCS	m_btnErrSystemExit;
	CStaticCS	m_stcCarrierID;

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
	afx_msg void OnBnClickedBtnErrBuzzOff();
	afx_msg void OnBnClickedBtnErrSkip();
	afx_msg void OnBnClickedBtnErrRetry();
	afx_msg void OnBnClickedBtnErrToShipTray();
	afx_msg void OnBnClickedBtnErrOk();
	afx_msg void OnBnClickedBtnErrSystemExit();
	afx_msg void OnStnClickedCarID();
	afx_msg void OnCbnSelchangeCboDownactiondetail();

private:
	CBitmap		m_bmpImage;
	int			m_nErrNo;
	int			m_nPortNo;
	CString		m_strErrMsg;
	CString		m_strErrSubMsg;
	CString		m_strLotID;
	CString		m_strsSpcInfo;
	CString		m_strTitle;
	int			m_nBackColorLoop;
	BOOL		m_bErrShow;

	CString		m_strAlmStart;
	CString		m_strAlmEnd;

	map<CString, CString> m_mssDownAction;

private:
	void Initial_Controls();
	void ErrorTimeEdit();
	void Set_AlarmLog(int nErrNo, CString sErrMsg);
	void Set_SPCError(int nErrNo, CString sErrMsg);

public:
	void Set_ErrorNumber(int nErrNo) { m_nErrNo = nErrNo; }
	void Set_ErrorSubMessage(CString sErrMsg) { m_strErrSubMsg = sErrMsg; }

	void Set_DownActionCboList(CString sData);



};

extern CErrorDlg g_dlgError;

///////////////////////////////////////////////////////////////////////////////
