
// MakeCocktailToolDlg.h: 헤더 파일
//

#pragma once


// CMakeCocktailToolDlg 대화 상자
class CMakeCocktailToolDlg : public CDialogEx
{
// 생성입니다.
public:
	CMakeCocktailToolDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAKECOCKTAILTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

public:
	CComboBox m_combo_fruit;
	CComboBox m_combo_juice;
	CStatic m_text_fruit;
	CStatic m_text_juice;

	CEdit m_edit_juice_rate;
	CSliderCtrl m_slider_juice_rate;
	CStatic m_text_juice_rate;

	CEdit m_edit_alcohol_rate;
	CSliderCtrl m_slider_alcohol_rate;
	CStatic m_text_alcohol_rate;

	CEdit m_edit_alcohol_percent;
	CSliderCtrl m_slider_alcohol_percent;
	CStatic m_text_alcohol_percent;

	CStatic m_text_alcohol_calc;
	CEdit m_edit_alcohol_calc;

	CButton m_button_accept_mix;
	CButton m_button_reset;

	CButton m_button_close;

public:
	/////////////////////////////////////////////////////////////////
	// 오픈지엘 초기화 핸들변수
	HGLRC m_hRC;
	HDC m_hDC;
	int m_pfIndex;
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 3D View Control
	CStatic m_view3D;
	/////////////////////////////////////////////////////////////////



public:
	void InitGL();
	

	void SetPositionControls();
	void InitFontSize();

	void ResetAll();

public:
	////////////////////////////////////////////////////////////////////////////////////////////
	// opengl 화면에 그리는 함수
	void Render(void);
	void RenderEmptyCup();
	void RenderJuiceLiquid();
	void RenderAlcoholLiquid();
	void RenderCylinder(GLVector3 vecNormal, float fRadiusBottom,
		float fRadiusTop, float fHeight, BOOL bCover, GLVector3 color, float alpha);
	////////////////////////////////////////////////////////////////////////////////////////////

	void GLResize(int cx, int cy);

protected:
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCbnSelchangeComboFruit();
	afx_msg void OnCbnSelchangeComboJuice();
	afx_msg void OnNMCustomdrawSliderJuiceRate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderAlcoholRate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderAlcoholPercent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonAcceptMix();
	afx_msg void OnBnClickedButtonReset();
};
