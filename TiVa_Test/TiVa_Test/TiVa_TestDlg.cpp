
// TiVa_TestDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TiVa_Test.h"
#include "TiVa_TestDlg.h"
#include "afxdialogex.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HEIGHT	480
#define WIDTH	640

#define DEFAULT 5
#define BLUE	0
#define GREEN	1
#define RED		2
#define YELLOW	3

#define HUE 0
#define SATURATION 1
#define VALUE 2

#define E_loop 5 //Erosion loop
#define D_loop 5 //Dilation loop

BYTE State = DEFAULT;

BITMAPINFO Bmapinf;
LPBYTE plmgBuf;

typedef struct C_VIDEO {
	unsigned char RGB[HEIGHT][WIDTH][3];
	unsigned char HSV[HEIGHT][WIDTH][3];
	unsigned char index[HEIGHT + 1][WIDTH + 1];
}C_VIDEO;

unsigned long cnt = 0;
unsigned long last_cnt = 0;
unsigned long cnt_Val[255] = { 0 };
unsigned long area_cnt = 0;
unsigned long area[255] = { 0 };

struct C_VIDEO Video_In, Video_Out, Tmp;



void Transform_HSV(unsigned short h, unsigned short w);
void Erosion(unsigned short h, unsigned short w, byte i);
void Dilation(unsigned short h, unsigned short w, byte i);
void Edge(unsigned short h, unsigned short w);
//Labeling
void F_Check(unsigned short h, unsigned short w);
void S_Check(unsigned short h, unsigned short w);
void Area_Find(unsigned short h, unsigned short w);
void Labeling_Color(unsigned short h, unsigned short w);


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTiVa_TestDlg 대화 상자



CTiVa_TestDlg::CTiVa_TestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TIVA_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTiVa_TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTiVa_TestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTiVa_TestDlg::Default_Button)
	ON_BN_CLICKED(IDC_BUTTON2, &CTiVa_TestDlg::Blue_Button)
	ON_BN_CLICKED(IDC_BUTTON3, &CTiVa_TestDlg::Green_Button)
	ON_BN_CLICKED(IDC_BUTTON4, &CTiVa_TestDlg::Red_Button)
	ON_BN_CLICKED(IDC_BUTTON5, &CTiVa_TestDlg::Yellow_Button)
END_MESSAGE_MAP()


// CTiVa_TestDlg 메시지 처리기

BOOL CTiVa_TestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	RECT m_Rect = { 0, 0, 640, 480 };

	AdjustWindowRect(&m_Rect, WS_OVERLAPPEDWINDOW, FALSE);
	int Rect_width = m_Rect.right - m_Rect.left;
	int Rect_height = m_Rect.bottom - m_Rect.top;
	this->SetWindowPos(NULL, 0, 0, Rect_width, Rect_height, SWP_NOSIZE);

	m_Cap = capCreateCaptureWindow(TEXT("Image Test"), WS_CHILD | WS_VISIBLE, 0, 0, 640, 480, this->m_hWnd, NULL);

	//CALLBACK
	if (capSetCallbackOnFrame(m_Cap, CallbackOnFrame) == FALSE) {
		return FALSE;
	}

	//CAM Driver
	if (capDriverConnect(m_Cap, 0) == FALSE) {
		return FALSE;
	}

	capPreviewRate(m_Cap, 33); // Frame per Second
	capOverlay(m_Cap, false);
	capPreview(m_Cap, true);  // Preview

	if (Bmapinf.bmiHeader.biBitCount != 24) {
		Bmapinf.bmiHeader.biBitCount = 24;
		Bmapinf.bmiHeader.biCompression = 0;
		Bmapinf.bmiHeader.biSizeImage = Bmapinf.bmiHeader.biWidth * Bmapinf.bmiHeader.biHeight * 3;

		capGetVideoFormat(m_Cap, &Bmapinf, sizeof(BITMAPINFO));
	}
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTiVa_TestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTiVa_TestDlg::OnPaint()
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
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTiVa_TestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTiVa_TestDlg::OnDestroy() {
	CDialog::OnDestroy();
	capDriverDisconnect(m_Cap);
	if (plmgBuf != NULL) {
		delete[] plmgBuf;
	}
}

LRESULT CALLBACK CallbackOnFrame(HWND hWnd, LPVIDEOHDR lpVHdr) {
	unsigned short w = 0, h = 0; //Width & Height
	byte i = 0; //loop or RGB

	for (h = 0; h < HEIGHT; h++) {
		for (w = 0; w < WIDTH; w++) {
			// Get Value from lpData
			Video_Out.RGB[h][w][BLUE] = Video_In.RGB[h][w][BLUE] = *(lpVHdr->lpData + (h * WIDTH + w) * 3);
			Video_Out.RGB[h][w][GREEN] = Video_In.RGB[h][w][GREEN] = *(lpVHdr->lpData + (h * WIDTH + w) * 3 + 1);
			Video_Out.RGB[h][w][RED] = Video_In.RGB[h][w][RED] = *(lpVHdr->lpData + (h * WIDTH + w) * 3 + 2);
			Transform_HSV(h, w);
		}
	}
	
	switch (State)
	{
		//Detect Blue
	case 0:
		for (h = 0; h < HEIGHT; h++) {
			for (w = 0; w < WIDTH; w++) {
				if (Video_In.HSV[h][w][HUE] < 160 && Video_In.HSV[h][w][HUE] >150 && Video_In.HSV[h][w][SATURATION] > 95 && Video_In.HSV[h][w][VALUE] > 95) {
					
					Video_In.RGB[h][w][BLUE] = 255;
					Video_In.RGB[h][w][GREEN] = 0;
					Video_In.RGB[h][w][RED] = 0;

				}
				else {
					Video_In.RGB[h][w][BLUE] = 0;
					Video_In.RGB[h][w][GREEN] = 0;
					Video_In.RGB[h][w][RED] = 0;
				}
			}
		}
		break;
		//Detect Green
	case 1:
		for (h = 0; h < HEIGHT; h++) {
			for (w = 0; w < WIDTH; w++) {
				if (Video_In.HSV[h][w][HUE] < 110 && Video_In.HSV[h][w][HUE] > 91 && Video_In.HSV[h][w][SATURATION] > 95 && Video_In.HSV[h][w][VALUE] > 95) {

					Video_In.RGB[h][w][BLUE] = 0;
					Video_In.RGB[h][w][GREEN] = 255;
					Video_In.RGB[h][w][RED] = 0;

				}
				else {
					Video_In.RGB[h][w][BLUE] = 0;
					Video_In.RGB[h][w][GREEN] = 0;
					Video_In.RGB[h][w][RED] = 0;
				}
			}
		}
		break;
		//Detect Red
	case 2:
		for (h = 0; h < HEIGHT; h++) {
			for (w = 0; w < WIDTH; w++) {
				if (Video_In.HSV[h][w][HUE] < 15 && Video_In.HSV[h][w][HUE] >= 0 && Video_In.HSV[h][w][SATURATION] > 100 && Video_In.HSV[h][w][VALUE] > 100) {//95
					
					Video_In.RGB[h][w][BLUE] = 0;
					Video_In.RGB[h][w][GREEN] = 0;
					Video_In.RGB[h][w][RED] = 255;

				}
				else {
					Video_In.RGB[h][w][BLUE] = 0;
					Video_In.RGB[h][w][GREEN] = 0;
					Video_In.RGB[h][w][RED] = 0;
				}
			}
		}
		break;
		//Detect Yellow
	case 3:
		for (h = 0; h < HEIGHT; h++) {
			for (w = 0; w < WIDTH; w++) {
				if (Video_In.HSV[h][w][HUE] < 45 && Video_In.HSV[h][w][HUE] >30 && Video_In.HSV[h][w][SATURATION] > 99 && Video_In.HSV[h][w][VALUE] > 99) {

					Video_In.RGB[h][w][BLUE] = 0;
					Video_In.RGB[h][w][GREEN] = 255;  //Green'll be used to Yellow
					Video_In.RGB[h][w][RED] = 0;

				}
				else {
					Video_In.RGB[h][w][BLUE] = 0;
					Video_In.RGB[h][w][GREEN] = 0;
					Video_In.RGB[h][w][RED] = 0;
				}
			}
		}
		
		break;
	default:
		break;
	}

	//Morphology
	Erosion(h, w, i);
	Dilation(h, w, i);

	if (State != DEFAULT) {
		if (State == YELLOW) {
			State = GREEN;
			F_Check(h, w);
			S_Check(h, w);
			Area_Find(h, w);
			Labeling_Color(h, w);
			State = YELLOW;
		}
		else {
			F_Check(h, w);
			S_Check(h, w);
			Area_Find(h, w);
			Labeling_Color(h, w);
		}
	}

	//Edge Detect
	if (State != DEFAULT) {
		if (State == YELLOW) {
			State = GREEN;
			for (h = 1; h < HEIGHT - 1; h++) {
				for (w = 1; w < WIDTH - 1; w++) {
					Edge(h, w);
				}
			}
			State = YELLOW;
		}
		else {
			for (h = 1; h < HEIGHT - 1; h++) {
				for (w = 1; w < WIDTH - 1; w++) {
					Edge(h, w);
				}
			}
		}
	}

	/*for (h = 0; h < HEIGHT; h++) {
		for (w = 0; w < WIDTH; w++) {
			// Return Value to lpData
			*(lpVHdr->lpData + (h * WIDTH + w) * 3) = Video_In.RGB[h][w][BLUE];
			*(lpVHdr->lpData + (h * WIDTH + w) * 3 + 1) = Video_In.RGB[h][w][GREEN];
			*(lpVHdr->lpData + (h * WIDTH + w) * 3 + 2) = Video_In.RGB[h][w][RED];
		}
	}
	*/

	for (h = 0; h < HEIGHT; h++) {
		for (w = 0; w < WIDTH; w++) {
			// Return Value to lpData
			*(lpVHdr->lpData + (h * WIDTH + w) * 3) = Video_Out.RGB[h][w][BLUE];
			*(lpVHdr->lpData + (h * WIDTH + w) * 3 + 1) = Video_Out.RGB[h][w][GREEN];
			*(lpVHdr->lpData + (h * WIDTH + w) * 3 + 2) = Video_Out.RGB[h][w][RED];
		}
	}
	
	return(LRESULT)true;
}


void Transform_HSV(unsigned short h, unsigned short w) {

	unsigned char RGB_MAX = 0;
	unsigned char RGB_MIN = 0;

	RGB_MIN = Video_In.RGB[h][w][RED] < Video_In.RGB[h][w][GREEN] ? (Video_In.RGB[h][w][RED] < Video_In.RGB[h][w][BLUE] ? Video_In.RGB[h][w][RED] : Video_In.RGB[h][w][BLUE]) : (Video_In.RGB[h][w][GREEN] < Video_In.RGB[h][w][BLUE] ? Video_In.RGB[h][w][GREEN] : Video_In.RGB[h][w][BLUE]);
	RGB_MAX = Video_In.RGB[h][w][RED] > Video_In.RGB[h][w][GREEN] ? (Video_In.RGB[h][w][RED] > Video_In.RGB[h][w][BLUE] ? Video_In.RGB[h][w][RED] : Video_In.RGB[h][w][BLUE]) : (Video_In.RGB[h][w][GREEN] > Video_In.RGB[h][w][BLUE] ? Video_In.RGB[h][w][GREEN] : Video_In.RGB[h][w][BLUE]);

	Video_In.HSV[h][w][VALUE] = RGB_MAX;
	if (Video_In.HSV[h][w][VALUE] == 0)
	{
		Video_In.HSV[h][w][HUE] = 0;
		Video_In.HSV[h][w][SATURATION] = 0;
		return;
	}

	Video_In.HSV[h][w][SATURATION] = 255 * long(RGB_MAX - RGB_MIN) / Video_In.HSV[h][w][VALUE];
	if (Video_In.HSV[h][w][SATURATION] == 0)
	{
		Video_In.HSV[h][w][HUE] = 0;
		return;
	}

	if (RGB_MAX == Video_In.RGB[h][w][RED]) {
		Video_In.HSV[h][w][HUE] = 0 + 43 * (Video_In.RGB[h][w][GREEN] - Video_In.RGB[h][w][BLUE]) / (RGB_MAX - RGB_MIN);
	}
	else if (RGB_MAX == Video_In.RGB[h][w][GREEN]) {
		Video_In.HSV[h][w][HUE] = 85 + 43 * (Video_In.RGB[h][w][BLUE] - Video_In.RGB[h][w][RED]) / (RGB_MAX - RGB_MIN);
	}
	else {
		Video_In.HSV[h][w][HUE] = 171 + 43 * (Video_In.RGB[h][w][RED] - Video_In.RGB[h][w][GREEN]) / (RGB_MAX - RGB_MIN);
	}
	return;
}	

void Erosion(unsigned short h, unsigned short w, byte i) {
	for (h = 0; h < HEIGHT; h++) {
		for (w = 0; w < WIDTH; w++) {
			Tmp.RGB[h][w][State] = 0;
		}
	}
	for (i = 0; i < E_loop; i++) {
		for (h = 0; h < HEIGHT; h++) {
			for (w = 0; w < WIDTH; w++) {
				Tmp.RGB[h][w][State] = Video_In.RGB[h][w][State];
			}
		}
		for (h = 1; h < HEIGHT - 1; h++) {
			for (w = 1; w < WIDTH - 1; w++) {
				if (Video_In.RGB[h][w][State] != 0) {
					if (Tmp.RGB[h - 1][w - 1][State] == 0 || Tmp.RGB[h][w - 1][State] == 0 || Tmp.RGB[h + 1][w - 1][State] == 0 ||
						Tmp.RGB[h - 1][w][State] == 0 || Tmp.RGB[h + 1][w][State] == 0 ||
						Tmp.RGB[h - 1][w + 1][State] == 0 || Tmp.RGB[h + 1][w + 1][State] == 0) {
						Video_In.RGB[h][w][State] = 0;
					}
				}
			}
		}
	}
}

void Dilation(unsigned short h, unsigned short w, byte i) {
	for (i = 0; i < D_loop; i++) {
		for (h = 0; h < HEIGHT; h++) {
			for (w = 0; w < WIDTH; w++) {
				Tmp.RGB[h][w][State] = Video_In.RGB[h][w][State];
			}
		}
		for (h = 1; h < HEIGHT - 1; h++) {
			for (w = 1; w < WIDTH - 1; w++) {
				if (Video_In.RGB[h][w][State] == 0) {
					if (Tmp.RGB[h - 1][w - 1][State] != 0 || Tmp.RGB[h][w - 1][State] != 0 || Tmp.RGB[h + 1][w - 1][State] != 0 ||
						Tmp.RGB[h - 1][w][State] != 0 || Tmp.RGB[h + 1][w][State] != 0 ||
						Tmp.RGB[h - 1][w + 1][State] != 0 || Tmp.RGB[h + 1][w + 1][State] != 0) {
						Video_In.RGB[h][w][State] = 255;
					}
				}
			}
		}
	}
}

void Edge(unsigned short h, unsigned short w) {

		if (Video_In.RGB[h][w][State] == 0 && Video_In.RGB[h][w + 1][State] == 255) {
			Video_Out.RGB[h][w][0] = Video_Out.RGB[h][w][1] = Video_Out.RGB[h][w][2] = 255;
		}
		else if (Video_In.RGB[h][w][State] == 255 && Video_In.RGB[h][w + 1][State] == 0) {
			Video_Out.RGB[h][w + 1][0] = Video_Out.RGB[h][w + 1][1] = Video_Out.RGB[h][w + 1][2] = 255;
		}
		if (Video_In.RGB[h][w][State] == 255 && Video_In.RGB[h + 1][w][State] == 0) {
			Video_Out.RGB[h][w][0] = Video_Out.RGB[h][w][1] = Video_Out.RGB[h][w][2] = 255;
		}
		else if (Video_In.RGB[h][w][State] == 0 && Video_In.RGB[h + 1][w][State] == 255) {
			Video_Out.RGB[h + 1][w][0] = Video_Out.RGB[h + 1][w][1] = Video_Out.RGB[h + 1][w][2] = 255;
	}
}

void F_Check(unsigned short h, unsigned short w) {
	for (h = 0; h < HEIGHT + 1; h++) {
		for (w = 0; w < WIDTH + 1; w++) {
			Tmp.index[h][w] = { 0 };
		}
	}
	for (cnt = 0; cnt < 255; cnt++) {
		cnt_Val[cnt] = { 0 };
	}
	cnt = 0;
	for (h = 1; h < HEIGHT + 1; h++) {
		for (w = 1; w < WIDTH + 1; w++) {
			if (Video_In.RGB[h - 1][w - 1][State] == 255) {
				if (Tmp.index[h - 1][w] == 0 && Tmp.index[h][w - 1] == 0) {
					++cnt;
					Tmp.index[h][w] += cnt;
					cnt_Val[cnt] = cnt;
				}
				else if (Tmp.index[h - 1][w] != 0 && Tmp.index[h][w - 1] == 0) {
					Tmp.index[h][w] = Tmp.index[h - 1][w];
				}
				else if (Tmp.index[h - 1][w] == 0 && Tmp.index[h][w - 1] != 0) {
					Tmp.index[h][w] = Tmp.index[h][w - 1];
				}
				else if (Tmp.index[h - 1][w] == Tmp.index[h][w - 1] && Tmp.index[h - 1][w] != 0) {
					Tmp.index[h][w] = Tmp.index[h - 1][w];
				}
				else if (Tmp.index[h - 1][w] != Tmp.index[h][w - 1] && Tmp.index[h - 1][w] != 0 && Tmp.index[h][w - 1] != 0) {
					if (Tmp.index[h - 1][w] > Tmp.index[h][w - 1]) {
						Tmp.index[h][w] = Tmp.index[h][w - 1];
						cnt_Val[Tmp.index[h - 1][w]] = cnt_Val[Tmp.index[h - 1][w]] > Tmp.index[h][w - 1] ? Tmp.index[h][w - 1] : cnt_Val[Tmp.index[h - 1][w]];
					}
					else if (Tmp.index[h - 1][w] < Tmp.index[h][w - 1]) {
						Tmp.index[h][w] = Tmp.index[h - 1][w];
						cnt_Val[Tmp.index[h][w - 1]] = cnt_Val[Tmp.index[h][w - 1]] > Tmp.index[h - 1][w] ? Tmp.index[h - 1][w] : cnt_Val[Tmp.index[h][w - 1]];
					}
				}
				else {
					Tmp.index[h][w] = 0;
				}
				last_cnt = cnt;
			}
		}
	}
}

void S_Check(unsigned short h, unsigned short w) {
	for (cnt = last_cnt; cnt >= 1; cnt--) {
		if (cnt_Val[cnt] != cnt) {
			for (h = 1; h < HEIGHT + 1; h++) {
				for (w = 1; w < WIDTH + 1; w++) {
					if (Tmp.index[h][w] == cnt) {
						Tmp.index[h][w] = cnt_Val[cnt];
					}
				}
			}
		}
	}
}

void Area_Find(unsigned short h, unsigned short w) {
	for (h = 1; h < HEIGHT + 1; h++) {
		for (w = 1; w < WIDTH + 1; w++) {
			area[Tmp.index[h][w]] = 0;
		}
	}
	for (cnt = 1; cnt <= last_cnt; cnt++) {
		for (h = 1; h < HEIGHT + 1; h++) {
			for (w = 1; w < WIDTH + 1; w++) {
				if (Tmp.index[h][w] != 0) {
					area[Tmp.index[h][w]]++;
				}
			}
		}
	}
	area_cnt = 0;
	short Max = 0;
	for (cnt = 1; cnt <= last_cnt; cnt++) {
		if (area[cnt] > Max) {
			Max = area[cnt];
			area_cnt = cnt;
		}
	}
}

void Labeling_Color(unsigned short h, unsigned short w) {
	for (h = 1; h < HEIGHT + 1; h++) {
		for (w = 1; w < WIDTH + 1; w++) {
			if (Tmp.index[h][w] != area_cnt) {
				Video_In.RGB[h - 1][w - 1][State] = 0;
			}
			else {
				Video_In.RGB[h - 1][w - 1][State] = 255;
			}
		}
	}
}


void CTiVa_TestDlg::Default_Button()
{
	State = 5;
}


void CTiVa_TestDlg::Blue_Button()
{
	State = BLUE;
}


void CTiVa_TestDlg::Green_Button()
{
	State = GREEN;
}


void CTiVa_TestDlg::Red_Button()
{
	State = RED;
}


void CTiVa_TestDlg::Yellow_Button()
{
	State = YELLOW;
}
