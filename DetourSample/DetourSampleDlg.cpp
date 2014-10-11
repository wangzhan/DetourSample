
// DetourSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DetourSample.h"
#include "DetourSampleDlg.h"
#include "afxdialogex.h"
#include "detours/detours.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

typedef int (__stdcall *pMessageBoxFunc)(HWND, LPCTSTR, LPCTSTR, UINT);
pMessageBoxFunc OriginalMessageBoxFunc = ::MessageBox;

int __stdcall MyMessageBox(HWND hWnd, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType)
{
	CString csText = L"已经被截获了: ";
	csText += lpszText;

	return (*OriginalMessageBoxFunc)(hWnd, csText, lpszCaption, nType);
}


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDetourSampleDlg dialog



CDetourSampleDlg::CDetourSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDetourSampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDetourSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDetourSampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CDetourSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CDetourSampleDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDetourSampleDlg::OnBnClickedButton2)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDetourSampleDlg message handlers

BOOL CDetourSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// initialize detours
	VERIFY(DetourTransactionBegin() == NO_ERROR);
	VERIFY(DetourUpdateThread(GetCurrentThread()) == NO_ERROR);
	VERIFY(DetourAttach(&(PVOID&)OriginalMessageBoxFunc, MyMessageBox) == NO_ERROR);
	VERIFY(DetourTransactionCommit() == NO_ERROR);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDetourSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDetourSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDetourSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDetourSampleDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();

	//::MessageBox(m_hWnd, L"Hello", NULL, 0);
}


void CDetourSampleDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	::MessageBox(m_hWnd, L"Hello in main thread", NULL, 0);
}


static DWORD WINAPI ThreadFunc(LPVOID lpThreadParameter)
{
	::MessageBox((HWND)lpThreadParameter, L"Hello in another thread", NULL, 0);
	return 0;
}

void CDetourSampleDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here

	CloseHandle(CreateThread(NULL, 0, ThreadFunc, m_hWnd, 0, NULL));
}


void CDetourSampleDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	VERIFY(DetourTransactionBegin() == NO_ERROR);
	VERIFY(DetourUpdateThread(GetCurrentThread()) == NO_ERROR);
	VERIFY(DetourDetach(&(PVOID&)OriginalMessageBoxFunc, MyMessageBox) == NO_ERROR);
	VERIFY(DetourTransactionCommit() == NO_ERROR);

	CDialogEx::OnClose();
}
