
// MakeCocktailToolDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MakeCocktailTool.h"
#include "MakeCocktailToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//Shading 관련변수 Ambient, Diffuse, Specular (요건 모르시면 구글링)
static const GLfloat afAmbientGreen[] = { 0.00, 0.50, 0.00, 1.00 };
static const GLfloat afAmbientBlue[] = { 0.00, 0.00, 0.50, 1.00 };
static const GLfloat afDiffuseGreen[] = { 0.00, 0.75, 0.00, 1.00 };
static const GLfloat afSpecularWhite[] = { 1.00, 1.00, 1.00, 1.00 };

#define hDetail 361


float RADIAN(int x) { return float(x)*0.017453292519943295769236907684885f; }
float RADIAN(float x) { return (x)*0.017453292519943295769236907684885f; }
double RADIAN(double x) { return (x)*0.017453292519943295769236907684885; }
float DEGREE(float x) { return (x)*57.295779513082320876798154814114f; }
double DEGREE(double x) { return (x)*57.295779513082320876798154814114; }


CMakeCocktailToolDlg::CMakeCocktailToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAKECOCKTAILTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMakeCocktailToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIEW_3D, m_view3D);
	DDX_Control(pDX, IDC_COMBO_FRUIT, m_combo_fruit);
	DDX_Control(pDX, IDC_COMBO_JUICE, m_combo_juice);
	DDX_Control(pDX, IDC_TEXT_FRUIT, m_text_fruit);
	DDX_Control(pDX, IDC_TEXT_JUICE, m_text_juice);
	DDX_Control(pDX, IDC_EDIT_JUICE_RATE, m_edit_juice_rate);
	DDX_Control(pDX, IDC_SLIDER_JUICE_RATE, m_slider_juice_rate);
	DDX_Control(pDX, IDC_TEXT_JUICE_RATE, m_text_juice_rate);

	DDX_Control(pDX, IDC_EDIT_ALCOHOL_RATE, m_edit_alcohol_rate);
	DDX_Control(pDX, IDC_SLIDER_ALCOHOL_RATE, m_slider_alcohol_rate);
	DDX_Control(pDX, IDC_TEXT_ALCOHOL_RATE, m_text_alcohol_rate);

	DDX_Control(pDX, IDC_EDIT_ALCOHOL_PERCENT, m_edit_alcohol_percent);
	DDX_Control(pDX, IDC_SLIDER_ALCOHOL_PERCENT, m_slider_alcohol_percent);
	DDX_Control(pDX, IDC_TEXT_ALCOHOL_PERCENT, m_text_alcohol_percent);
	DDX_Control(pDX, IDC_BUTTON_ACCEPT_MIX, m_button_accept_mix);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_button_reset);
	DDX_Control(pDX, IDC_TEXT_ALCOHOL_CALC, m_text_alcohol_calc);
	DDX_Control(pDX, IDC_EDIT_ALCOHOL_CALC, m_edit_alcohol_calc);
	DDX_Control(pDX, IDOK, m_button_close);
}

BEGIN_MESSAGE_MAP(CMakeCocktailToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_FRUIT, &CMakeCocktailToolDlg::OnCbnSelchangeComboFruit)
	ON_CBN_SELCHANGE(IDC_COMBO_JUICE, &CMakeCocktailToolDlg::OnCbnSelchangeComboJuice)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_JUICE_RATE, &CMakeCocktailToolDlg::OnNMCustomdrawSliderJuiceRate)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ALCOHOL_RATE, &CMakeCocktailToolDlg::OnNMCustomdrawSliderAlcoholRate)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ALCOHOL_PERCENT, &CMakeCocktailToolDlg::OnNMCustomdrawSliderAlcoholPercent)
	ON_BN_CLICKED(IDC_BUTTON_ACCEPT_MIX, &CMakeCocktailToolDlg::OnBnClickedButtonAcceptMix)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CMakeCocktailToolDlg::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// CMakeCocktailToolDlg 메시지 처리기

BOOL CMakeCocktailToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 컨트롤 글자 스타일
	InitFontSize();

	// 화면 컨트롤(버튼 등등) 배치 및 3D View Rect 설정
	SetPositionControls();

	// gl 초기화
	InitGL();

	// 원래는 onsize 에서 Resize 호출해야하나 dialog 형태이므로 
	// 크기가 고정이라 한번만 그냥 해준다.
	GLResize(theApp.m_rect3D.Width(), theApp.m_rect3D.Height());

	// 배경색 
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

	// 3D Depth 초기화
	glClearDepth(1.0);

	// 초기 위치
	GLVector3 pos(0.f, -0.3f, 0.0f);
	glTranslatef(pos.x, pos.y, pos.z);

	// 자동 회전 및 카메라 포지션 정의
	glRotatef(30.f, 1.0, 1.0, 0.0);


	return TRUE;  
}


void CMakeCocktailToolDlg::InitGL()
{
	// 오픈지엘 초기화 ******************************
	int nPixelFormat;

	m_hDC = ::GetDC(m_view3D.m_hWnd);

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	VERIFY(SetPixelFormat(m_hDC, nPixelFormat, &pfd));
	m_hRC = wglCreateContext(m_hDC);
	VERIFY(wglMakeCurrent(m_hDC, m_hRC));

}

void CMakeCocktailToolDlg::GLResize(int cx, int cy)
{
	GLfloat fAspect;

	if (cy == 0) 
		cy = 1;

	glViewport(0, 0, cx, cy);

	// 해상도 비율
	fAspect = (GLfloat)cx / (GLfloat)cy;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 3차원 World Matrix 선언 (1.414~ 숫자는 해상도에 대한 비율 * 0.5 (절반)
	GLfloat fHalfWorldSize = (1.4142135623730950488016887242097 * 0.5f);
	glOrtho(-fHalfWorldSize * fAspect, fHalfWorldSize*fAspect, -fHalfWorldSize,
		fHalfWorldSize, -10 * fHalfWorldSize, 10 * fHalfWorldSize);

	glMatrixMode(GL_MODELVIEW);
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMakeCocktailToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();


		// 렌더링
		wglMakeCurrent(m_hDC, m_hRC);

		Render();

		SwapBuffers(m_hDC);

		wglMakeCurrent(m_hDC, NULL);

	}
}


void CMakeCocktailToolDlg::InitFontSize()
{
	{
		static CFont font;
		LOGFONT logfont;
		m_combo_fruit.GetFont()->GetLogFont(&logfont);

		logfont.lfWeight = FW_SEMIBOLD;
		logfont.lfHeight = 20;
		font.CreateFontIndirect(&logfont);

		m_combo_fruit.SetFont(&font);
		m_combo_juice.SetFont(&font);
		m_text_fruit.SetFont(&font);
		m_text_juice.SetFont(&font);

		m_edit_juice_rate.SetFont(&font);
		m_slider_juice_rate.SetFont(&font);
		m_text_juice_rate.SetFont(&font);

		m_edit_alcohol_rate.SetFont(&font);
		m_slider_alcohol_rate.SetFont(&font);
		m_text_alcohol_rate.SetFont(&font);

		m_edit_alcohol_percent.SetFont(&font);
		m_slider_alcohol_percent.SetFont(&font);
		m_text_alcohol_percent.SetFont(&font);

		m_text_alcohol_calc.SetFont(&font);
		m_edit_alcohol_calc.SetFont(&font);

		m_button_accept_mix.SetFont(&font);
		m_button_reset.SetFont(&font);
		m_button_close.SetFont(&font);
	}

	//{// 일반적으론 18 font
	//	static CFont font;
	//	LOGFONT logfont;
	//	m_group_center.GetFont()->GetLogFont(&logfont);

	//	logfont.lfWeight = FW_SEMIBOLD;
	//	logfont.lfHeight = 18;
	//	font.CreateFontIndirect(&logfont);

	//	m_group_center.SetFont(&font);
	//}
}

void CMakeCocktailToolDlg::SetPositionControls()
{
	this->MoveWindow(0, 0, theApp.m_dialogSize.cx, theApp.m_dialogSize.cy);

	::GetClientRect(this->m_hWnd, theApp.m_rectClient);

	int nMargin = 10;

	CRect rect3D;
	{
		rect3D.right = theApp.m_rectClient.right - nMargin;
		rect3D.left = rect3D.right - 640;
		rect3D.top = nMargin;
		rect3D.bottom = rect3D.top + 480;

		m_view3D.MoveWindow(rect3D);
		theApp.m_rect3D = rect3D;

	}

	CRect rectFruitText;
	CRect rectFruitCombo;
	{// 과일 텍스트 & 콤보박스
		rectFruitText.left = nMargin * 2;
		rectFruitText.right = rectFruitText.left + 80;
		rectFruitText.top = nMargin + 3;
		rectFruitText.bottom = rectFruitText.top + 20;

		m_text_fruit.MoveWindow(rectFruitText);


		rectFruitCombo.left = rectFruitText.right + nMargin * 2;
		rectFruitCombo.right = rectFruitCombo.left + 150;
		rectFruitCombo.top = nMargin;
		rectFruitCombo.bottom = rectFruitCombo.top + 20;

		m_combo_fruit.MoveWindow(rectFruitCombo);

		m_combo_fruit.AddString(L"없음");
		m_combo_fruit.AddString(L"코코넛");
		m_combo_fruit.AddString(L"오디");
		m_combo_fruit.AddString(L"파인애플");
		m_combo_fruit.AddString(L"키위");
		m_combo_fruit.AddString(L"블루베리");
		m_combo_fruit.AddString(L"오렌지");
		m_combo_fruit.AddString(L"체리");
		m_combo_fruit.AddString(L"라임");
		m_combo_fruit.AddString(L"레몬");

		m_combo_fruit.SetCurSel(0);
	}

	CRect rectJuiceText;
	CRect rectJuiceCombo;
	{// 음료 텍스트 & 콤보박스
		rectJuiceText.left = nMargin * 2;
		rectJuiceText.right = rectJuiceText.left + 80;
		rectJuiceText.top = rectFruitCombo.bottom + nMargin * 1.5 + 3;
		rectJuiceText.bottom = rectJuiceText.top + 20;

		m_text_juice.MoveWindow(rectJuiceText);


		rectJuiceCombo.left = rectFruitText.right + nMargin * 2;
		rectJuiceCombo.right = rectJuiceCombo.left + 150;
		rectJuiceCombo.top = rectFruitCombo.bottom + nMargin * 1.5;
		rectJuiceCombo.bottom = rectJuiceCombo.top + 20;

		m_combo_juice.MoveWindow(rectJuiceCombo);

		m_combo_juice.AddString(L"없음");
		m_combo_juice.AddString(L"사이다");
		m_combo_juice.AddString(L"콜라");
		m_combo_juice.AddString(L"오렌지주스");
		m_combo_juice.AddString(L"크랜베리주스");
		m_combo_juice.AddString(L"커피");
		m_combo_juice.AddString(L"우유");
		m_combo_juice.AddString(L"포도주스");

		m_combo_juice.SetCurSel(0);

	}


	CRect rectJuiceRateText;
	{// 주스 비율 타이틀 텍스트
		rectJuiceRateText.left = nMargin * 2;
		rectJuiceRateText.right = rectJuiceRateText.left + 80;
		rectJuiceRateText.top = rectJuiceCombo.bottom + nMargin * 3;
		rectJuiceRateText.bottom = rectJuiceRateText.top + 20;

		m_text_juice_rate.MoveWindow(rectJuiceRateText);
	}

	CRect rectJuiceRateSlider;
	CRect rectJuiceRateEdit;
	{// 주스 비율 슬라이더, Edit
		rectJuiceRateSlider.left = nMargin * 1.5;
		rectJuiceRateSlider.right = rectJuiceRateSlider.left + 185;
		rectJuiceRateSlider.top = rectJuiceRateText.bottom + nMargin * 1.5;
		rectJuiceRateSlider.bottom = rectJuiceRateSlider.top + 30;

		rectJuiceRateEdit.left = rectJuiceRateSlider.right + nMargin;
		rectJuiceRateEdit.right = rectJuiceRateEdit.left + 60;
		rectJuiceRateEdit.top = rectJuiceRateText.bottom + nMargin * 1.5;
		rectJuiceRateEdit.bottom = rectJuiceRateEdit.top + 30;

		m_slider_juice_rate.SetTic(1);
		m_slider_juice_rate.SetRange(0, 50);
		m_slider_juice_rate.SetPos(theApp.m_juiceRate);

		m_slider_juice_rate.MoveWindow(rectJuiceRateSlider);


		m_edit_juice_rate.MoveWindow(rectJuiceRateEdit);

		CString strText;
		strText.Format(L"%d %%", theApp.m_juiceRate);
		m_edit_juice_rate.SetWindowText(strText);
	}


	CRect rectAlcoholRateText;
	{// 알코올 비율 타이틀 텍스트
		rectAlcoholRateText.left = nMargin * 2;
		rectAlcoholRateText.right = rectAlcoholRateText.left + 80;
		rectAlcoholRateText.top = rectJuiceRateEdit.bottom + nMargin * 3;
		rectAlcoholRateText.bottom = rectAlcoholRateText.top + 20;

		m_text_alcohol_rate.MoveWindow(rectAlcoholRateText);
	}


	CRect rectAlcoholRateSlider;
	CRect rectAlcoholRateEdit;
	{// 알코올 비율 슬라이더, Edit
		rectAlcoholRateSlider.left = nMargin * 1.5;
		rectAlcoholRateSlider.right = rectAlcoholRateSlider.left + 185;
		rectAlcoholRateSlider.top = rectAlcoholRateText.bottom + nMargin * 1.5;
		rectAlcoholRateSlider.bottom = rectAlcoholRateSlider.top + 30;

		rectAlcoholRateEdit.left = rectAlcoholRateSlider.right + nMargin;
		rectAlcoholRateEdit.right = rectAlcoholRateEdit.left + 60;
		rectAlcoholRateEdit.top = rectAlcoholRateText.bottom + nMargin * 1.5;
		rectAlcoholRateEdit.bottom = rectAlcoholRateEdit.top + 30;

		m_slider_alcohol_rate.SetTic(1);
		m_slider_alcohol_rate.SetRange(0, 50);
		m_slider_alcohol_rate.SetPos(theApp.m_juiceRate);

		m_slider_alcohol_rate.MoveWindow(rectAlcoholRateSlider);


		m_edit_alcohol_rate.MoveWindow(rectAlcoholRateEdit);

		CString strText;
		strText.Format(L"%d %%", theApp.m_alcoholRate);
		m_edit_alcohol_rate.SetWindowText(strText);
	}


	CRect rectAlcoholPercentText;
	{// 알코올 퍼센트 타이틀 텍스트
		rectAlcoholPercentText.left = nMargin * 2;
		rectAlcoholPercentText.right = rectAlcoholPercentText.left + 200;
		rectAlcoholPercentText.top = rectAlcoholRateEdit.bottom + nMargin * 3;
		rectAlcoholPercentText.bottom = rectAlcoholPercentText.top + 20;

		m_text_alcohol_percent.MoveWindow(rectAlcoholPercentText);
	}


	CRect rectAlcoholPercentSlider;
	CRect rectAlcoholPercentEdit;
	{// 알코올 비율 슬라이더, Edit
		rectAlcoholPercentSlider.left = nMargin * 1.5;
		rectAlcoholPercentSlider.right = rectAlcoholPercentSlider.left + 185;
		rectAlcoholPercentSlider.top = rectAlcoholPercentText.bottom + nMargin * 1.5;
		rectAlcoholPercentSlider.bottom = rectAlcoholPercentSlider.top + 30;

		rectAlcoholPercentEdit.left = rectAlcoholRateSlider.right + nMargin;
		rectAlcoholPercentEdit.right = rectAlcoholPercentEdit.left + 60;
		rectAlcoholPercentEdit.top = rectAlcoholPercentText.bottom + nMargin * 1.5;
		rectAlcoholPercentEdit.bottom = rectAlcoholPercentEdit.top + 30;

		m_slider_alcohol_percent.SetTic(1);
		m_slider_alcohol_percent.SetRange(0, 100);
		m_slider_alcohol_percent.SetPos(theApp.m_alcoholPercent);

		m_slider_alcohol_percent.MoveWindow(rectAlcoholPercentSlider);


		m_edit_alcohol_percent.MoveWindow(rectAlcoholPercentEdit);

		CString strText;
		strText.Format(L"%d %%", theApp.m_alcoholPercent);
		m_edit_alcohol_percent.SetWindowText(strText);
	}


	CRect rectAlcoholCalcText;
	CRect rectAlcoholCalcEdit;
	{// 알코올 도수계산값 타이틀 텍스트
		rectAlcoholCalcText.left = nMargin * 2;
		rectAlcoholCalcText.right = rectAlcoholCalcText.left + 100;
		rectAlcoholCalcText.top = rectAlcoholPercentEdit.bottom + nMargin * 3 + 5;
		rectAlcoholCalcText.bottom = rectAlcoholCalcText.top + 20;

		m_text_alcohol_calc.MoveWindow(rectAlcoholCalcText);


		rectAlcoholCalcEdit.left = rectAlcoholCalcText.right + nMargin;
		rectAlcoholCalcEdit.right = rectAlcoholCalcEdit.left + 140;
		rectAlcoholCalcEdit.top = rectAlcoholPercentEdit.bottom + nMargin * 3;
		rectAlcoholCalcEdit.bottom = rectAlcoholCalcText.top + 30;

		m_edit_alcohol_calc.MoveWindow(rectAlcoholCalcEdit);

	}



	CRect rectAcceptButton;
	CRect rectResetButton;
	{// 적용 && 초기화 버튼
		rectAcceptButton.left = nMargin * 1.5;
		rectAcceptButton.right = rectAcceptButton.left + 125;
		rectAcceptButton.top = rect3D.bottom - 40;
		rectAcceptButton.bottom = rect3D.bottom;

		rectResetButton.left = rectAcceptButton.right + nMargin;
		rectResetButton.right = rectResetButton.left + 120;
		rectResetButton.top = rect3D.bottom - 40;
		rectResetButton.bottom = rect3D.bottom;

		m_button_accept_mix.MoveWindow(rectAcceptButton);
		m_button_reset.MoveWindow(rectResetButton);
	}



	CRect rectCloseButton;
	{// 종료버튼
		rectCloseButton.right = theApp.m_rectClient.right - nMargin;
		rectCloseButton.left = rectCloseButton.right - 150;
		rectCloseButton.bottom = theApp.m_rectClient.bottom - nMargin;
		rectCloseButton.top = rectCloseButton.bottom - 40;

		m_button_close.MoveWindow(rectCloseButton);
	}





}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMakeCocktailToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CMakeCocktailToolDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
}


void CMakeCocktailToolDlg::RenderCylinder(GLVector3 vecNormal, 
						float fRadiusBottom, float fRadiusTop, float fHeight, BOOL bCover, GLVector3 color, float alpha)
{
	float fAlpha = alpha;
	int nHdetail = hDetail;

	GLVector3 vecRot[hDetail];
	for (int k = 0; k < nHdetail; k++)
	{
		vecRot[k].x = sin(RADIAN((float)k));
		vecRot[k].y = 0.f;
		vecRot[k].z = cos(RADIAN((float)k));
	}

	// 원기둥의 vertex
	GLVector3 ptCylinder[2][hDetail];

	for (int h = 0; h < nHdetail; h++)
	{
		// Z 축방향의 VecDir 방향으로 회전할 Center (구의 Center 아님)
		GLVector3 ptBottomPos;
		ptBottomPos.x = vecRot[h].x * fRadiusBottom;
		ptBottomPos.y = vecRot[h].y * fRadiusBottom;
		ptBottomPos.z = vecRot[h].z * fRadiusBottom;

		ptCylinder[0][h] = ptBottomPos;

		GLVector3 ptTopPos;
		ptTopPos.x = vecRot[h].x * fRadiusTop + vecNormal.x * fHeight;
		ptTopPos.y = vecRot[h].y * fRadiusTop + vecNormal.y * fHeight;
		ptTopPos.z = vecRot[h].z * fRadiusTop + vecNormal.z * fHeight;

		//ptTopPos.x = ptBottomPos.x + vecNormal.x * fHeight;
		//ptTopPos.y = ptBottomPos.y + vecNormal.y * fHeight;
		//ptTopPos.z = ptBottomPos.z + vecNormal.z * fHeight;

		ptCylinder[1][h] = ptTopPos;
	}

	srand((unsigned)time(NULL));

	for (int h = 0; h < nHdetail - 1; h++)
	{
		// 반시계방향으로 폴리곤 생성 (1개)
		GLVector3 vecNormal;
		{
			GLVector3 pt0 = ptCylinder[0][h];
			GLVector3 pt1 = ptCylinder[0][h + 1];
			GLVector3 pt2 = ptCylinder[1][h];

			GLVector3 vec0;
			vec0.x = pt1.x - pt0.x;
			vec0.y = pt1.y - pt0.y;
			vec0.z = pt1.z - pt0.z;

			GLVector3 vecNormal0;
			theApp.NormalizeVector(vecNormal0, vec0);


			GLVector3 vec1;
			vec1.x = pt2.x - pt0.x;
			vec1.y = pt2.y - pt0.y;
			vec1.z = pt2.z - pt0.z;

			GLVector3 vecNormal1;
			theApp.NormalizeVector(vecNormal1, vec1);

			theApp.CrossProduct(vecNormal, vecNormal0, vecNormal1);


			// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
			glColor4f(color.x, color.y, color.z, fAlpha);
			glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
			glVertex3f(pt0.x, pt0.y, pt0.z);
			glVertex3f(pt1.x, pt1.y, pt1.z);
			glVertex3f(pt2.x, pt2.y, pt2.z);
		}


		// 반시계방향으로 폴리곤 생성 (2개)
		{
			GLVector3 pt0 = ptCylinder[1][h];
			GLVector3 pt1 = ptCylinder[1][h + 1];
			GLVector3 pt2 = ptCylinder[0][h + 1];

			GLVector3 vec0;
			vec0.x = pt1.x - pt0.x;
			vec0.y = pt1.y - pt0.y;
			vec0.z = pt1.z - pt0.z;

			GLVector3 vecNormal0;
			theApp.NormalizeVector(vecNormal0, vec0);


			GLVector3 vec1;
			vec1.x = pt2.x - pt0.x;
			vec1.y = pt2.y - pt0.y;
			vec1.z = pt2.z - pt0.z;

			GLVector3 vecNormal1;
			theApp.NormalizeVector(vecNormal1, vec1);

			glColor4f(color.x, color.y, color.z, fAlpha);
			glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
			glVertex3f(pt0.x, pt0.y, pt0.z);
			glVertex3f(pt1.x, pt1.y, pt1.z);
			glVertex3f(pt2.x, pt2.y, pt2.z);
		}

		// 천장 Draw
		if(bCover)
		{
			GLVector3 pt0 = ptCylinder[1][h];
			GLVector3 pt1 = ptCylinder[1][h + 1];
			GLVector3 pt2(0, fHeight, 0);


			//pt0.y += fHeight;
			//pt1.y += fHeight;
			//pt2.y += fHeight;

			GLVector3 vec0;
			vec0.x = pt1.x - pt0.x;
			vec0.y = pt1.y - pt0.y;
			vec0.z = pt1.z - pt0.z;

			GLVector3 vecNormal0;
			theApp.NormalizeVector(vecNormal0, vec0);


			GLVector3 vec1;
			vec1.x = pt2.x - pt0.x;
			vec1.y = pt2.y - pt0.y;
			vec1.z = pt2.z - pt0.z;

			GLVector3 vecNormal1;
			theApp.NormalizeVector(vecNormal1, vec1);

			GLVector3 vecNormal;
			theApp.CrossProduct(vecNormal, vecNormal0, vecNormal1);


			// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
			glColor4f(color.x, color.y, color.z, fAlpha);
			glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
			glVertex3f(pt0.x, pt0.y, pt0.z);
			glVertex3f(pt1.x, pt1.y, pt1.z);
			glVertex3f(pt2.x, pt2.y, pt2.z);
		}

		// 바닥 Draw
		{
			GLVector3 pt1 = ptCylinder[0][h];
			GLVector3 pt0 = ptCylinder[0][h + 1];
			GLVector3 pt2(0, 0, 0);

			GLVector3 vec0;
			vec0.x = pt1.x - pt0.x;
			vec0.y = pt1.y - pt0.y;
			vec0.z = pt1.z - pt0.z;

			GLVector3 vecNormal0;
			theApp.NormalizeVector(vecNormal0, vec0);


			GLVector3 vec1;
			vec1.x = pt2.x - pt0.x;
			vec1.y = pt2.y - pt0.y;
			vec1.z = pt2.z - pt0.z;

			GLVector3 vecNormal1;
			theApp.NormalizeVector(vecNormal1, vec1);

			GLVector3 vecNormal;
			theApp.CrossProduct(vecNormal, vecNormal0, vecNormal1);


			// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
			glColor4f(color.x, color.y, color.z, fAlpha);
			glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
			glVertex3f(pt0.x, pt0.y, pt0.z);
			glVertex3f(pt1.x, pt1.y, pt1.z);
			glVertex3f(pt2.x, pt2.y, pt2.z);
		}

	}

}




void CMakeCocktailToolDlg::RenderEmptyCup()
{
	// shading 처리된 색상 변경
	static const GLfloat afDiffuseColor[] = { 0.80, 0.80, 0.8, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, afDiffuseColor);

	GLVector3 vecNormal(0.f, 1.f, 0.f);

	float fRadiusBottom = 0.15f;
	float fCupHeight = 0.6f;

	float fRadiusTop = 0.f;
	float fCurrentHeight = 0.6;

	fRadiusTop = fRadiusBottom + (fCurrentHeight / fCupHeight) * 0.1f;

	// 이 push 와 pop 이 화면 그려주는 구간
	glPushMatrix();

	glBegin(GL_TRIANGLES);

	GLVector3 color(0, 0, 0);
	RenderCylinder(vecNormal, fRadiusBottom, fRadiusTop, fCupHeight, FALSE, color, 1.f);

	glEnd();

	glPopMatrix();
}


void CMakeCocktailToolDlg::RenderJuiceLiquid()
{
	// shading 처리된 색상 변경
	static const GLfloat afDiffuseColor[] = { 1.0, 1.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, afDiffuseColor);

	GLVector3 vecNormal(0.f, 1.f, 0.f);

	float fRadiusBottom = 0.15f;
	float fCupHeight = 0.6f;

	float fRadiusTop = 0.f;

	// 주스, 알콜 비율 (맥시멈 50, 50)
	int nJuiceRate = theApp.m_juiceRate;
	int nAlcoholRate = theApp.m_alcoholRate;

	// 주스, 알콜 더해서 100으로 나누면 액체량 0~1 이고 1이면 컵에 꽉참
	float fRate = float(nJuiceRate + nAlcoholRate) / 100.f;
	float fCurrentHeight = fCupHeight * fRate;

	if (0.f == fRate)
		return;

	fRadiusTop = fRadiusBottom + (fCurrentHeight / fCupHeight) * 0.1f;

	GLVector3 color;
	color.x = float(theApp.m_colorJuice[0]) / 255.f;
	color.y = float(theApp.m_colorJuice[1]) / 255.f;
	color.z = float(theApp.m_colorJuice[2]) / 255.f;

	// 이 push 와 pop 이 화면 그려주는 구간
	glPushMatrix();

	glBegin(GL_TRIANGLES);

	RenderCylinder(vecNormal, fRadiusBottom, fRadiusTop, fCurrentHeight, FALSE, color, 0.3f);

	glEnd();

	glPopMatrix();
}


void CMakeCocktailToolDlg::RenderAlcoholLiquid()
{
	// shading 처리된 색상 변경
	static const GLfloat afDiffuseColor[] = { 1.0, 1.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, afDiffuseColor);

	GLVector3 vecNormal(0.f, 1.f, 0.f);

	float fRadiusBottom = 0.15f;
	float fCupHeight = 0.6f;

	float fRadiusTop = 0.f;

	// 주스, 알콜 비율 (맥시멈 50, 50)
	int nJuiceRate = theApp.m_juiceRate;
	int nAlcoholRate = theApp.m_alcoholRate;

	// 주스, 알콜 더해서 100으로 나누면 액체량 0~1 이고 1이면 컵에 꽉참
	float fRate = float(nJuiceRate + nAlcoholRate) / 100.f;
	float fCurrentHeight = fCupHeight * fRate;

	if (0.f == fRate)
		return;

	fRadiusTop = fRadiusBottom + (fCurrentHeight / fCupHeight) * 0.1f;

	GLVector3 color;
	color.x = float(theApp.m_colorFruit[0]) / 255.f;
	color.y = float(theApp.m_colorFruit[1]) / 255.f;
	color.z = float(theApp.m_colorFruit[2]) / 255.f;

	// 이 push 와 pop 이 화면 그려주는 구간
	glPushMatrix();

	glBegin(GL_TRIANGLES);

	RenderCylinder(vecNormal, fRadiusBottom, fRadiusTop, fCurrentHeight, FALSE, color, 0.15);

	glEnd();

	glPopMatrix();
}




void CMakeCocktailToolDlg::Render(void)
{
	// 컬러 뎁스 재초기화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// gl Matrix 시작 합니다. (반대로 종료합니다. glPopMatrix) 
	// push 와 pop 은 쌍을 맞춰야합니다.

	glPushMatrix();

	// gl Depth Lighting On (Shadering)
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING); //use vertex colors

	// 0 번째 빛 근원 On
	glEnable(GL_LIGHT0);

	// 상단 글로벌 변수 선언 주석 참조 (Shading 관련 변수 입니다.)
	glMaterialfv(GL_BACK, GL_AMBIENT, afAmbientGreen);
	glMaterialfv(GL_BACK, GL_DIFFUSE, afDiffuseGreen);
	glMaterialfv(GL_FRONT, GL_AMBIENT, afAmbientBlue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, afSpecularWhite);
	glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
	//Shading 관련 변수 사용 끝


	// 알파블랜딩
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 빈컵 그리기
	RenderEmptyCup();

	// gl Depth Lighting On (Shadering)
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING); //use vertex colors
	glDisable(GL_LIGHT0);

	// 아래 두개가 혼합 (alpha 적용으로) 되어 음료색상이 결정된다.
	RenderJuiceLiquid(); // 주스색 적용
	RenderAlcoholLiquid(); // 과일색 적용

	glPopMatrix();

}


int CMakeCocktailToolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void CMakeCocktailToolDlg::OnCbnSelchangeComboFruit()
{
	int nCur = m_combo_fruit.GetCurSel();

	theApp.m_fruitType = eFruitType(nCur);


	int color[3] = { 0, };

	if (E_FRUIT_0 == theApp.m_fruitType)
	{
		color[0] = 190;
		color[1] = 190;
		color[2] = 190;
	}
	else if (E_FRUIT_1 == theApp.m_fruitType)
	{
		color[0] = 120;
		color[1] = 20;
		color[2] = 23;
	}
	else if (E_FRUIT_2 == theApp.m_fruitType)
	{
		color[0] = 252;
		color[1] = 216;
		color[2] = 32;
	}
	else if (E_FRUIT_3 == theApp.m_fruitType)
	{
		color[0] = 148;
		color[1] = 198;
		color[2] = 66;
	}
	else if (E_FRUIT_4 == theApp.m_fruitType)
	{
		color[0] = 96;
		color[1] = 57;
		color[2] = 149;
	}
	else if (E_FRUIT_5 == theApp.m_fruitType)
	{
		color[0] = 255;
		color[1] = 192;
		color[2] = 41;
	}
	else if (E_FRUIT_6 == theApp.m_fruitType)
	{
		color[0] = 247;
		color[1] = 61;
		color[2] = 100;
	}
	else if (E_FRUIT_7 == theApp.m_fruitType)
	{
		color[0] = 185;
		color[1] = 198;
		color[2] = 101;
	}
	else if (E_FRUIT_8 == theApp.m_fruitType)
	{
		color[0] = 251;
		color[1] = 237;
		color[2] = 112;
	}

	theApp.m_colorFruit[0] = color[0];
	theApp.m_colorFruit[1] = color[1];
	theApp.m_colorFruit[2] = color[2];
}


void CMakeCocktailToolDlg::OnCbnSelchangeComboJuice()
{
	int nCur = m_combo_juice.GetCurSel();

	theApp.m_juiceType = eJuiceType(nCur);


	int color[3] = { 0, };

	if (E_JUICE_0 == theApp.m_juiceType)
	{
		color[0] = 228;
		color[1] = 229;
		color[2] = 225;
	}
	else if (E_JUICE_1 == theApp.m_juiceType)
	{
		color[0] = 38;
		color[1] = 26;
		color[2] = 22;
	}
	else if (E_JUICE_2 == theApp.m_juiceType)
	{
		color[0] = 255;
		color[1] = 195;
		color[2] = 51;
	}
	else if (E_JUICE_3 == theApp.m_juiceType)
	{
		color[0] = 255;
		color[1] = 51;
		color[2] = 89;
	}
	else if (E_JUICE_4 == theApp.m_juiceType)
	{
		color[0] = 74;
		color[1] = 48;
		color[2] = 35;
	}
	else if (E_JUICE_5 == theApp.m_juiceType)
	{
		color[0] = 233;
		color[1] = 233;
		color[2] = 234;
	}
	else if (E_JUICE_6 == theApp.m_juiceType)
	{
		color[0] = 127;
		color[1] = 47;
		color[2] = 65;
	}

	theApp.m_colorJuice[0] = color[0];
	theApp.m_colorJuice[1] = color[1];
	theApp.m_colorJuice[2] = color[2];

}


void CMakeCocktailToolDlg::OnNMCustomdrawSliderJuiceRate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	int nPos = m_slider_juice_rate.GetPos();

	theApp.m_juiceRate = nPos;

	CString strText;
	strText.Format(L"%d %%", theApp.m_juiceRate);
	m_edit_juice_rate.SetWindowText(strText);



	*pResult = 0;
}


void CMakeCocktailToolDlg::OnNMCustomdrawSliderAlcoholRate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	int nPos = m_slider_alcohol_rate.GetPos();

	theApp.m_alcoholRate = nPos;

	CString strText;
	strText.Format(L"%d %%", theApp.m_alcoholRate);
	m_edit_alcohol_rate.SetWindowText(strText);




	*pResult = 0;
}


void CMakeCocktailToolDlg::OnNMCustomdrawSliderAlcoholPercent(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	int nPos = m_slider_alcohol_percent.GetPos();

	theApp.m_alcoholPercent = nPos;

	CString strText;
	strText.Format(L"%d %%", theApp.m_alcoholPercent);
	m_edit_alcohol_percent.SetWindowText(strText);


	*pResult = 0;
}


void CMakeCocktailToolDlg::OnBnClickedButtonAcceptMix()
{
	if (E_FRUIT_NONE == theApp.m_fruitType)
	{
		MessageBox(L"과일 선택을 해주세요!");
		return;
	}

	if (E_JUICE_NONE == theApp.m_juiceType)
	{
		MessageBox(L"주스 선택을 해주세요!");
		return;
	}

	if (0 == theApp.m_juiceRate && 0 == theApp.m_alcoholRate)
	{
		MessageBox(L"알코올과 음료를 넣어주세요!");
		return;
	}

	//////////////////////////////////////////////////////////////////
	// 도수계산
	{
		float fPercent = (theApp.m_alcoholPercent * theApp.m_alcoholRate) /
							(theApp.m_juiceRate + theApp.m_alcoholRate);

		CString strText;
		strText.Format(L"%.2f %%", fPercent);

		m_edit_alcohol_calc.SetWindowText(strText);
	}
	//////////////////////////////////////////////////////////////////


	Invalidate();
}


void CMakeCocktailToolDlg::ResetAll()
{
	theApp.m_fruitType = E_FRUIT_NONE;
	theApp.m_juiceType = E_JUICE_NONE;

	theApp.m_juiceRate = 0;
	theApp.m_alcoholRate = 0;
	theApp.m_alcoholPercent = 0;

	m_combo_fruit.SetCurSel(0);
	m_combo_juice.SetCurSel(0);

	m_slider_juice_rate.SetPos(0);
	m_slider_alcohol_rate.SetPos(0);
	m_slider_alcohol_percent.SetPos(0);

	m_edit_juice_rate.SetWindowText(L"0 %%");
	m_edit_alcohol_rate.SetWindowText(L"0 %%");
	m_edit_alcohol_percent.SetWindowText(L"0 %%");
	m_edit_alcohol_calc.SetWindowText(L"");

	theApp.m_colorFruit[0] = 0;
	theApp.m_colorFruit[1] = 0;
	theApp.m_colorFruit[2] = 0;

	theApp.m_colorJuice[0] = 0;
	theApp.m_colorJuice[1] = 0;
	theApp.m_colorJuice[2] = 0;
}

void CMakeCocktailToolDlg::OnBnClickedButtonReset()
{
	int nAnswer = AfxMessageBox(L"초기화 하시겠습니까?", MB_YESNO);

	if (IDYES == nAnswer)
	{
		ResetAll();

		Invalidate();
	}
}
