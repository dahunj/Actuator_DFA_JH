#include "stdafx.h"
#include "ROSEmulatorDlg.h"


class CROSEmulatorApp : public CWinApp
{
public:

	virtual BOOL InitInstance()
	{
		CWinApp::InitInstance();

		if (!AfxSocketInit())
		{
			AfxMessageBox("Winsock initialization failed.");

			return FALSE;
		}

		CROSEmulatorDlg dlg;
		m_pMainWnd = &dlg;
		dlg.DoModal();
		m_pMainWnd = NULL;

		return FALSE;
	}
};
CROSEmulatorApp theApp;
