
// TiVa_TestDlg.h : 헤더 파일
//

#pragma once
#pragma comment(lib, "vfw32.lib")
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "vfw.h"
#include "afxwin.h"

#define _DEF_WEBCAM 1000

LRESULT CALLBACK CallbackOnFrame(HWND hwnd, LPVIDEOHDR lpVHdr);


// CTiVa_TestDlg 대화 상자
class CTiVa_TestDlg : public CDialogEx
{
// 생성입니다.
public:
	CTiVa_TestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TIVA_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void OnDestroy();

public:

	HWND m_Cap;
	CStatic m_stDisplay;
	afx_msg void Default_Button();
	afx_msg void Blue_Button();
	afx_msg void Green_Button();
	afx_msg void Red_Button();
	afx_msg void Yellow_Button();
};
