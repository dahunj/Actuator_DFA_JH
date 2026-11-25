#pragma once
class CROSEmulatorDlg : public CDialog
{
public:

	CROSEmulatorDlg() : CDialog(IDD_MAIN)
	{
	}

	enum
	{
		IDD = IDD_MAIN
	};
	void Save_DispatcherLog(CString strLog);
protected:

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK()
	{
	}

	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnConnect2();
	afx_msg void OnBnClickedBtnConnectRequest();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

};
