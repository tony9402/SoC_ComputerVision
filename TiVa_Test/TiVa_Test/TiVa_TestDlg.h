
// TiVa_TestDlg.h : ��� ����
//

#pragma once
#pragma comment(lib, "vfw32.lib")
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "vfw.h"
#include "afxwin.h"

#define _DEF_WEBCAM 1000

LRESULT CALLBACK CallbackOnFrame(HWND hwnd, LPVIDEOHDR lpVHdr);


// CTiVa_TestDlg ��ȭ ����
class CTiVa_TestDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CTiVa_TestDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TIVA_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
