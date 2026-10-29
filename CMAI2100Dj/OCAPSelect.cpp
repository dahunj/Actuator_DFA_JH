#include "stdafx.h"
#include "CMAI2100.h"
#include "OCAPSelect.h"
#include "afxdialogex.h"

OCAPSelect::OCAPSelect(CWnd* pParent /*=NULL*/)
	: CDialogEx(OCAPSelect::IDD, pParent)
{
}

BEGIN_MESSAGE_MAP(OCAPSelect, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OCAP_FAI, &OCAPSelect::OnBnClickedFAI)
	ON_BN_CLICKED(IDC_BTN_OCAP_COSMETIC, &OCAPSelect::OnBnClickedCosmetic)
END_MESSAGE_MAP()

void OCAPSelect::OnBnClickedFAI()
{
	EndDialog(IDC_BTN_OCAP_FAI);
}

void OCAPSelect::OnBnClickedCosmetic()
{
	EndDialog(IDC_BTN_OCAP_COSMETIC);
}
