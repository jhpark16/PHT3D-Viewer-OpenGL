// MainFrm.cpp : implmentation of the CMainFrame class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: 
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"

#include "View.h"
#include "MainFrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
  m_view.StepDemo(TRUE);
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
  m_hWndCmdBar = hWndCmdBar;

	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CreateSimpleStatusBar();
  m_StatusBar.SubclassWindow(m_hWndStatusBar);
  int arrParts[] =
  {
    ID_DEFAULT_PANE,
    IDR_DATE,
    IDR_TIME
  };
  m_StatusBar.SetPanes(arrParts, sizeof(arrParts) / sizeof(int), false);
  // set status bar pane widths using local workaround
  int arrWidths[] = { 0, 120, 120 };
  SetPaneWidths(arrWidths, sizeof(arrWidths) / sizeof(int));
  // set the status bar pane icons
  m_StatusBar.SetPaneIcon(ID_DEFAULT_PANE, AtlLoadIconImage(IDR_DEFAULT, LR_DEFAULTCOLOR));
  m_StatusBar.SetPaneIcon(IDR_DATE, AtlLoadIconImage(IDR_DATE, LR_DEFAULTCOLOR));
  m_StatusBar.SetPaneIcon(IDR_TIME, AtlLoadIconImage(IDR_TIME, LR_DEFAULTCOLOR));
  // set the status bar pane text
  TCHAR szBuffer[100];
  wsprintf(szBuffer, _T("Use Left, Middle, and Right mouse click and dragging to control the view"), 0);
  m_StatusBar.SetPaneText(ID_DEFAULT_PANE, szBuffer);

  // initialize date/time and start a 1 second timer
  SetTimer(1, 1000);
  CString str;

  // get the current date and time
  SYSTEMTIME st;
  ::GetLocalTime(&st);

  // Pane 1: Display the date
  str.Format(_T("%i/%i/%i"), st.wMonth, st.wDay, st.wYear);
  m_StatusBar.SetPaneText(IDR_DATE, str);

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, _T("VTK Viewer"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN); //, WS_EX_CLIENTEDGE

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);
  UISetCheck(IDC_VIEW_FULLSCREEN, 0);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

  //  Save the styles and position of the current window
  wndStyles = GetWindowLong(GWL_STYLE);
  wndExStyles = GetWindowLong(GWL_EXSTYLE);
  GetWindowRect(&wndRect);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document
  
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

// this workaround solves a bug in CMultiPaneStatusBarCtrl
// (in SetPanes() method) that limits the size of all panes
// after the default pane to a combined total of 100 pixels  
void CMainFrame::SetPaneWidths(int* arrWidths, int nPanes)
{
  // find the size of the borders
  int arrBorders[3];
  m_StatusBar.GetBorders(arrBorders);

  // calculate right edge of default pane
  arrWidths[0] += arrBorders[2];
  for (int i = 1; i < nPanes; i++)
    arrWidths[0] += arrWidths[i];

  // calculate right edge of remaining panes
  for (int j = 1; j < nPanes; j++)
    arrWidths[j] += arrBorders[2] + arrWidths[j - 1];

  // set the pane widths
  m_StatusBar.SetParts(m_StatusBar.m_nPanes, arrWidths);
}

LRESULT CMainFrame::OnEraseBackground(HDC hdc)
{
  //		ATLTRACE("OnEraseBackground\n");
  SetMsgHandled(TRUE);
  return TRUE; // do nothing
}

//void OnTimer(UINT uTimerID)//, TIMERPROC pTimerProc)
LRESULT CMainFrame::OnTimer(UINT /*uMsg*/, WPARAM uTimerID, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CString str;

  // get the current date and time
  SYSTEMTIME st;
  ::GetLocalTime(&st);

  // Pane 1: Display the date
  str.Format(_T("%i/%i/%i"), st.wMonth, st.wDay, st.wYear);
  //		m_StatusBar.SetPaneText(IDR_DATE, str);

  // Pane 2: Display the time
  str.Format(_T("%i:%02i:%02i"), st.wHour, st.wMinute, st.wSecond);
  m_StatusBar.SetPaneText(IDR_TIME, str);

  return 0;
}

LRESULT CMainFrame::OnViewFullscreen(UINT uCode, int nID, HWND hwndCtrl)
{
  static BOOL bNew = FALSE;	// initially windowed
  bNew = !bNew;
  m_bFullScreen = bNew;
  ATLTRACE("Fullscreen = %d\n", m_bFullScreen);
  if (m_bFullScreen) {
    ::SendMessage(m_hWndToolBar, RB_SHOWBAND, 0, FALSE);	// menubar is band #0
    ::SendMessage(m_hWndToolBar, RB_SHOWBAND, 1, FALSE);	// toolbar is band #1
    UISetCheck(ID_VIEW_TOOLBAR, FALSE);
    //			m_StatusBar.ShowWindow(SW_HIDE);
    ::ShowWindow(m_hWndStatusBar, SW_SHOWNOACTIVATE);
    //			::ShowWindow(m_hWndStatusBar, SW_HIDE);
    UISetCheck(ID_VIEW_STATUS_BAR, FALSE);
    FullScreen();
  }
  else {
    ::SendMessage(m_hWndToolBar, RB_SHOWBAND, 0, TRUE);	// menubar is band #0
    ::SendMessage(m_hWndToolBar, RB_SHOWBAND, 1, TRUE);	// toolbar is band #1
    UISetCheck(ID_VIEW_TOOLBAR, TRUE);
    ::ShowWindow(m_hWndStatusBar, SW_SHOWNOACTIVATE);
    UISetCheck(ID_VIEW_STATUS_BAR, TRUE);
    Windowed();
  }
  UISetCheck(IDC_VIEW_FULLSCREEN, bNew);
  UpdateLayout();
  return 0;
}

BOOL CMainFrame::FullScreen()
{
  ATLTRACE("FullScreen\n");
  BOOL retval;
  //  Save the styles and position of the window
  GetWindowRect(&wndRect);
  wndStyles = GetWindowLong(GWL_STYLE);
  wndExStyles = GetWindowLong(GWL_EXSTYLE);

  //  Set styles and window size for fullscreen
  SetWindowLong(GWL_STYLE, WS_POPUP);
  retval = SetWindowPos(HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
  ShowWindow(SW_SHOW);
  return retval;
}

BOOL CMainFrame::Windowed()
{
  ATLTRACE("Windowed\n");
  BOOL retval;
  //  Restore the styles and position of the window

  SetWindowLong(GWL_STYLE, wndStyles);
  SetWindowLong(GWL_EXSTYLE, wndExStyles);
  retval = SetWindowPos(HWND_NOTOPMOST, &wndRect, SWP_DRAWFRAME);
  ShowWindow(SW_SHOW);
  return retval;
}

