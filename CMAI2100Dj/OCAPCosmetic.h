#pragma once

// OCAPCosmetic 대화 상자입니다.

class OCAPCosmetic : public CDialogEx
{
	DECLARE_DYNAMIC(OCAPCosmetic)

public:
	OCAPCosmetic(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~OCAPCosmetic();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OCAP_COSMETIC_DLG };
	CGroupCS		m_Group_Cosmetic[2];
	CLabelCS		m_Label_Cosmetic[7];
	CStaticCS		m_stcOption_Cosmetic[9];
	CGridCS			m_grdData_Cosmetic;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnStcOptionClick(UINT nID);

private:
	BOOL m_bError_Cosmetic;
	BOOL m_bSetting_Cosmetic;
	int m_nCodePage_Cosmetic;
	CString m_sCode_Cosmetic[100];
	CString m_sName_Cosmetic[100];
	int m_nMZCnt_Cosmetic[100];
	double m_dDefect_Cosmetic[100];
	CStatic m_stcCodeTitle_Cosmetic[10];
	CStatic m_stcCodeNo_Cosmetic[20];
	CEdit m_edtCode_Cosmetic[20];
	CEdit m_edtName_Cosmetic[20];
	CEdit m_edtMZCnt_Cosmetic[20];
	CEdit m_edtDefect_Cosmetic[20];
	CButton m_btnCode_Cosmetic[4];
	CStatic m_stcCodeInfo_Cosmetic;
	CStatic m_stcCodeLine_Cosmetic;

	void Initial_CodeControls_Cosmetic();
	void Display_CodePage_Cosmetic();
	void Show_CodeControls_Cosmetic();
	BOOL Read_CodePage_Cosmetic();
	BOOL Check_CodeData_Cosmetic();
	void Load_CodeData_Cosmetic();
	afx_msg void OnCodeButton(UINT nID);


	void Initial_NameGrid_Cosmetic(CGridCS *pGrid, int nRows, int nCols);
	void Initial_DataGrid_Cosmetic(CGridCS *pGrid, int nRows, int nCols);

	void Initial_Controls_Cosmetic();
	void Display_Option_Cosmetic();
	void Display_Grid_Cosmetic(int nDp, int nIndex);

public:
	void Display_Status_Cosmetic();
	void Set_AddMZData_Cosmetic(int nPortNo);
	void Set_AddDEFECT_Cosmetic(CString sMZid, CString sCode);
	void Check_DEFECT_Cosmetic(int nNo);
	void Check_DEFECTF_Cosmetic(int nNo);
	void AddCarToMZ_Cosmetic(int nPortNo, int nIndex);
	void AddMZOut_Cosmetic(CString sMZid);
	void DelMZData_Cosmetic(int nMZNo);
};

extern OCAPCosmetic g_dlgOCAP_Cosmetic;
