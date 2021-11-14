
// MakeCocktailTool.h: PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 주 기호입니다.

enum eFruitType
{
	E_FRUIT_NONE = 0,
	E_FRUIT_0, //코코넛
	E_FRUIT_1, //오디
	E_FRUIT_2, //파인애플
	E_FRUIT_3, //키위
	E_FRUIT_4, //블루베리
	E_FRUIT_5, //오렌지
	E_FRUIT_6, //체리
	E_FRUIT_7, //라임
	E_FRUIT_8 //레몬
};


enum eJuiceType
{
	E_JUICE_NONE = 0,
	E_JUICE_0, //사이다
	E_JUICE_1, //콜라
	E_JUICE_2, //오렌지주스
	E_JUICE_3, //크랜베리주스
	E_JUICE_4, //커피
	E_JUICE_5, //우유
	E_JUICE_6 //포도주스
};

// 글로벌 구조체
struct GLVector3
{
	float x;
	float y;
	float z;

	GLVector3()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	GLVector3(float nx, float ny, float nz)
	{
		x = nx;
		y = ny;
		z = nz;
	}
};

class CMakeCocktailToolApp : public CWinApp
{
public:
	CMakeCocktailToolApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.




public:
	eFruitType m_fruitType; // 현재 과일 타입
	eJuiceType m_juiceType; // 현재 음료 타입
	int		   m_juiceRate; // 0 ~ 50 % (현재 음료 량)
	int		   m_alcoholRate; // 0 ~ 50 % (현재 알코올 량)
	int		   m_alcoholPercent; // 0 ~ 100 % (현재 알코올 도수)

	int		   m_colorFruit[3]; // 과일색
	int		   m_colorJuice[3]; // 음료색

public:
	CSize		m_dialogSize; // 대화상자 사이즈
	CRect		m_rectClient; // 대화상자 Rect 변수
	CRect		m_rect3D; // 3D화면 Rect 변수

public:
	// 수학연산 함수 (외적 벡터)
	void	CrossProduct(GLVector3 &vecOutput, GLVector3& vec0, GLVector3& vec1);

	// normalize (벡터, 스칼라) (스칼라는 길이가 있죠, 벡터는 길이가 없고 방향만 있어요)
	// 길이가 있는 스칼라 값을 정규화 해서 벡터로 바꾼다.
	void	NormalizeVector(GLVector3 &rfVectorResult, GLVector3 &rfVectorSource);



	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CMakeCocktailToolApp theApp;
