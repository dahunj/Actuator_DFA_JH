#include "stdafx.h"
#include "ROSEmulatorDlg.h"
#include "Dispatcher.h"
#include "RosNetwork.h"
BEGIN_MESSAGE_MAP(CROSEmulatorDlg, CDialog)
	ON_BN_CLICKED(IDC_START, OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedBtnConnect2)
	ON_BN_CLICKED(IDC_CONNECT, OnBnClickedBtnConnectRequest)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CROSEmulatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDlgItemText(IDC_LOCAL, ROS_DEFAULT_IP);
	SetDlgItemText(IDC_HANDLER, HANDLER_DEFAULT_IP);
	SetDlgItemInt(IDC_ROS_PORT, ROS_DEFAULT_LISTEN_PORT);
	SetDlgItemInt(IDC_MIN, 100);
	SetDlgItemInt(IDC_MAX, 1000);
	SetDlgItemInt(IDC_GOOD, 34);
	SetDlgItemInt(IDC_NG, 33);
	CComboBox * mode = (CComboBox *) GetDlgItem(IDC_MODE);
	mode->AddString("Random G / N / R");
	mode->AddString("Always G");
	mode->AddString("Always N");
	mode->AddString("Always R");
	mode->AddString("No reply (timeout)");
	mode->SetCurSel(0);
	g_objDispatcher.Initialize(this);
	Save_DispatcherLog("Ready. Set the ROS local IP, UDP port, and Handler IP, then click Start.");

	return TRUE;
}


void CROSEmulatorDlg::Save_DispatcherLog(CString s)
{
	SYSTEMTIME t;
	GetLocalTime( & t);
	CString line;

	line.Format("%02d:%02d:%02d.%03d  %s", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, (LPCSTR)s);
	CListBox * box = (CListBox *) GetDlgItem(IDC_LOG);

	if (box->GetCount() >= 2000)
		box->DeleteString(0);
	int i = box->AddString(line);

	box->SetTopIndex(i);
	box->SetHorizontalExtent(1600);
}


void CROSEmulatorDlg::OnBnClickedBtnConnect()
{
	g_objDispatcher.Connect();
}


void CROSEmulatorDlg::OnBnClickedBtnConnect2()
{
	g_objDispatcher.Terminate();
}


void CROSEmulatorDlg::OnBnClickedBtnConnectRequest()
{
	g_objDispatcher.Set_ConnectRequest();
}


void CROSEmulatorDlg::OnCancel()
{
	g_objDispatcher.Terminate();
	CDialog::OnCancel();
}


void CROSEmulatorDlg::OnDestroy()
{
	g_objDispatcher.Terminate();

	if (g_objDispatcher.GetSafeHwnd())
		g_objDispatcher.DestroyWindow();
	g_objDispatcher.Set_ParentDlg(NULL);
	CDialog::OnDestroy();
}


