
// TiVa_Test.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CTiVa_TestApp:
// �� Ŭ������ ������ ���ؼ��� TiVa_Test.cpp�� �����Ͻʽÿ�.
//

class CTiVa_TestApp : public CWinApp
{
public:
	CTiVa_TestApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CTiVa_TestApp theApp;
