#pragma once

class OCAPSelect : public CDialogEx
{
public:
	OCAPSelect(CWnd* pParent = NULL);
	enum { IDD = IDD_OCAP_SELECT_DLG };

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedFAI();
	afx_msg void OnBnClickedCosmetic();
};
