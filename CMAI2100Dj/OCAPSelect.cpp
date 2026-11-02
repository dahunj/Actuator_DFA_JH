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

void OCAPSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LABEL_0, m_lblTitle);
	DDX_Control(pDX, IDC_BTN_OCAP_FAI, m_btnFAI);
	DDX_Control(pDX, IDC_BTN_OCAP_COSMETIC, m_btnCosmetic);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
}

BOOL OCAPSelect::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_lblTitle.Init_Ctrl("πŸ≈¡", 16, TRUE, RGB(255, 255, 255), RGB(128, 0, 128));
	m_btnFAI.Init_Ctrl("πŸ≈¡", 18, TRUE, RGB(0, 0, 255), COLOR_DEFAULT, 0, 0);
	m_btnCosmetic.Init_Ctrl("πŸ≈¡", 18, TRUE, RGB(128, 0, 128), COLOR_DEFAULT, 0, 0);
	m_btnClose.Init_Ctrl("πŸ≈¡", 14, TRUE, RGB(0, 0, 0), COLOR_DEFAULT, 0, 0);
	return TRUE;
}
