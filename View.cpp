// View.cpp : implementation of the CView class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: 
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "View.h"

CView::CView()//:VTK_Operation(m_hWnd, m_hWndParent)
{
  //StepObjects(0);
}

BOOL CView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CView::OnPaint(UINT /*uMsg*/, WPARAM hdc, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	//TODO: Add your drawing code here
  RECT rc;
  GetClientRect(&rc);

  pvtkDemo->Render();
  SetMsgHandled(TRUE);

	return 0;
}

BOOL CView::PreviousInstanceFound(LPCTSTR lpClassName, LPCTSTR lpWindowName)
{
  HWND hwnd = FindWindow(lpClassName, lpWindowName);

  if (hwnd)
  {
    if (!CView::AllowMultipleInstances())
    {
      FLASHWINFO flashinfo;

      flashinfo.cbSize = sizeof(flashinfo);
      flashinfo.hwnd = hwnd;
      flashinfo.dwFlags = FLASHW_ALL;
      flashinfo.uCount = 2;
      flashinfo.dwTimeout = 0;

      FlashWindowEx(&flashinfo);
    }
    return TRUE;
  }

  return FALSE;
}

BOOL CView::AllowMultipleInstances()
{
  return FALSE;
}

HWND CView::Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName,
  DWORD dwStyle, DWORD dwExStyle, UINT nID, LPVOID lpCreateParam)
{
  ATLTRACE("Create\n");
  m_hWndParent = hWndParent;
  if (PreviousInstanceFound(GetWndClassInfo().m_wc.lpszClassName, szWindowName) && !AllowMultipleInstances())
    return NULL;

  // 		HWND hwnd = CWindowImpl<CView, CWindow, CVTKWinTraits>::Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
  HWND hwnd = CWindowImpl<CView>::Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
  if (!hwnd)
    return NULL;

  return hwnd;
}

LRESULT CView::OnCreate(LPCREATESTRUCT lParam)
{
  ATLTRACE("OnCreate\n");
  //		SetTimer(1, 1);
  //bInitialized = false;
  m_bClockRunning = TRUE;
  pvtkDemo = new VTK_Operation(m_hWnd, m_hWndParent);
  pvtkDemo->WidgetInteractiveOff();
  //pvtkDemo->StepObjects(0);
  SetMsgHandled(FALSE);
  //bInitialized = true;
  return FALSE;
}

void CView::OnDestroy()
{
  ATLTRACE("OnDestroy\n");
  KillTimer(1);
  delete pvtkDemo;
  pvtkDemo = nullptr;
  PostQuitMessage(0);
  SetMsgHandled(FALSE);
}

LRESULT CView::OnSize(UINT uMsg, CSize size)
{
//  if (true || bInitialized) {
    ATLTRACE("OnSize\n");
    if (pvtkDemo) pvtkDemo->OnSize(size);
    Invalidate();
    SetMsgHandled(TRUE);
//  }
  return TRUE;
}

LRESULT CView::OnEraseBackground(HDC hdc)
{
  //		ATLTRACE("OnEraseBackground\n");
  SetMsgHandled(TRUE);
  return TRUE; // do nothing
}

LRESULT CView::OnSysCommand(UINT uMsg, CPoint point)
{
  ATLTRACE("OnSysCommand\n");
  if (((uMsg & 0xFFF0) == SC_SCREENSAVE) || ((uMsg & 0xFFF0) == SC_MONITORPOWER))
  {
    SetMsgHandled(TRUE);
  }
  else
    SetMsgHandled(FALSE);

  return 0;
}

LRESULT CView::OnActivateApp(BOOL activated, DWORD minimized)
{
  ATLTRACE("OnActivateApp: %d\n", (BOOL)activated);
  if (activated)
    ActivateApp();
  else
    DeactivateApp();

  return 0;
}

void CView::ActivateApp()
{
}

void CView::DeactivateApp()
{
}

void CView::OnTimer(UINT uTimerID, TIMERPROC pTimerProc)
{
  if (1 != uTimerID)
  {
    ATLTRACE("OnTimer: m_bClockRunning = %d\n", m_bClockRunning);
    SetMsgHandled(FALSE);
  }
  else
  {
    // If the clock is running, get the current time & redraw our
    // window so the new time appears.
    if (m_bClockRunning)
    {
      ATLTRACE("OnTimer: m_bClockRunning = %d\n", m_bClockRunning);
      SetMsgHandled(FALSE);
      GetLocalTime(&m_stLastTime);
      pvtkDemo->OnTimer(m_hWnd, uTimerID);
      pvtkDemo->StepObjects(m_stLastTime.wSecond);
      RedrawWindow();
    }
  }
}

void CView::StepDemo(int step)
{
  pvtkDemo->OnTimer(m_hWnd, step);
  pvtkDemo->StepObjects(m_stLastTime.wSecond);
  RedrawWindow();
}

void CView::StartClock()
{
  m_bClockRunning = TRUE;
  GetLocalTime(&m_stLastTime);
  RedrawWindow();
}

void CView::StopClock()
{
  m_bClockRunning = FALSE;
  RedrawWindow();
}

void CView::OnLButtonDblClk(UINT uMsg, CPoint point)
{
  //		ATLTRACE("OnLButtonDblClk: %d, %d, %d\n", uMsg, point.x, point.y);
  pvtkDemo->OnLButtonDown(m_hWnd, uMsg, point);
}

void CView::OnLButtonDown(UINT uMsg, CPoint point)
{
  //		SetFocus();
  //		ATLTRACE("view::OnLButtonDown: %d, %d, %d\n", uMsg, point.x, point.y);
  pvtkDemo->OnLButtonDown(m_hWnd, uMsg, point);
}

void CView::OnMButtonDown(UINT uMsg, CPoint point)
{
  //		SetFocus();
  //		ATLTRACE("view::OnMButtonDown: %d, %d, %d\n", uMsg, point.x, point.y);
  pvtkDemo->OnMButtonDown(m_hWnd, uMsg, point);
}

void CView::OnRButtonDown(UINT uMsg, CPoint point)
{
  //		SetFocus();
  //		ATLTRACE("view::OnRButtonDown: %d, %d, %d\n", uMsg, point.x, point.y);
  pvtkDemo->OnRButtonDown(m_hWnd, uMsg, point);
}

void CView::OnLButtonUp(UINT uMsg, CPoint point)
{
  //		ATLTRACE("view::OnLButtonUp: %d, %d, %d\n", uMsg, point.x, point.y);
  pvtkDemo->OnLButtonUp(m_hWnd, uMsg, point);
}

void CView::OnMButtonUp(UINT uMsg, CPoint point)
{
  //		ATLTRACE("view::OnMButtonUp: %d, %d, %d\n", uMsg, point.x, point.y);
  pvtkDemo->OnMButtonUp(m_hWnd, uMsg, point);
}

void CView::OnRButtonUp(UINT uMsg, CPoint point)
{
  //		ATLTRACE("view::OnRButtonUp: %d, %d, %d\n", uMsg, point.x, point.y);
  pvtkDemo->OnRButtonUp(m_hWnd, uMsg, point);
}

void CView::OnMouseMove(UINT uMsg, CPoint point)
{
  //		ATLTRACE("view::OnMouseMove: %d, %d, %d\n", uMsg, point.x, point.y);
  pvtkDemo->OnMouseMove(m_hWnd, uMsg, point);
}

LRESULT CView::OnChar(UINT uMsg, WPARAM charCode, LPARAM lParam, BOOL& bHandled)
{
  // lower 16 bits of lParma is the repeat count.
  pvtkDemo->OnChar(m_hWnd, charCode, lParam & 0xFFFF, NULL);
  return 0;
}

LRESULT CView::OnKeyUp(UINT uMsg, WPARAM charCode, LPARAM lParam, BOOL& bHandled)
{
  // lower 16 bits of lParma is the repeat count.
  //pvtkDemo->OnChar(m_hWnd, charCode, lParam & 0xFFFF, NULL);
  return 0;
}

LRESULT CView::OnKeyDown(UINT uMsg, WPARAM charCode, LPARAM lParam, BOOL& bHandled)
{
  // lower 16 bits of lParma is the repeat count.
  //pvtkDemo->OnChar(m_hWnd, charCode, lParam & 0xFFFF, NULL);
  return 0;
}

void CView::ResetModelData()
{
  
}

LRESULT CView::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: add code to initialize document
  pvtkDemo->FileNew();
  return 0;
}

LRESULT CView::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CString tCstr;
  // TODO: add code to initialize document
  CFileDialog fileDlg(true, _T("nam"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
    _T("MODFLOW Name Files\0*.nam;*.pht3dnam;*.mtn\0All Files\0*.*\0"));
  if (!mPHT3DM.sSelectedFile.IsEmpty())
  {
    int iTmp = mPHT3DM.sSelectedFile.ReverseFind(_T('\\'));
    if (iTmp > 0)
    {
      tCstr = mPHT3DM.sSelectedFile.Left(iTmp);
      fileDlg.m_ofn.lpstrInitialDir = tCstr;
    }
  }
  if (fileDlg.DoModal() == IDOK)
  {
    mPHT3DM.sSelectedFile = fileDlg.m_szFileName;
    pvtkDemo->FileOpen(mPHT3DM, fileDlg.m_szFileName);
  }
  return 0;
}

PHT3D_Model& CView::GetPHT3DM() {
  return mPHT3DM;
}

