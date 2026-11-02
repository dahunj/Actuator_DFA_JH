#pragma once

class OCAPSelect : public CDialogEx
{
public:
	OCAPSelect(CWnd* pParent = NULL);
	enum { IDD = IDD_OCAP_SELECT_DLG };

protected:
	CLabelCS m_lblTitle;
	CButtonCS m_btnFAI;
	CButtonCS m_btnCosmetic;
	CButtonCS m_btnClose;
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedFAI();
	afx_msg void OnBnClickedCosmetic();
};
