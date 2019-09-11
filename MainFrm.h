// MainFrm.h : interface of the CMainFrame class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: MainFrame is the main window receiving command messages 
//   from menu, messages to update controls and system messages from Windows
//   It also handles idle operation.
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

// CMainFrame is the main Window receiving messges from Windows
class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, public CIdleHandler
{
public:
  // Defines GetWndClassInfo for this class. GetWndClassInfo returns 
  // CFrameWndClassInfo (various class settings of this window)
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  // CView is the main viewer implementation handling screen output 
	CView m_view;
  // Handles top command bar window
	CCommandBarCtrl m_CmdBar;
  HWND m_hWndCmdBar;
  // Handles and update status bar
  CMultiPaneStatusBarCtrl m_StatusBar;
  // Boolean variables for full screen, showing command bar below the main menu
  // and status bar at the bottom of main window.
  BOOL m_bFullScreen;
  BOOL m_bCmdBar;
  BOOL m_bStatusBar;
  // Saved Windows styles
  LONG wndStyles;
  LONG wndExStyles;
  RECT wndRect;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

  // Create message maps for CUpdateUI<CMainFrame> which 
  // updates UI elements such as menu and controls
	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
    UPDATE_ELEMENT(IDC_VIEW_FULLSCREEN, UPDUI_MENUPOPUP)
  END_UPDATE_UI_MAP()

  // Process Windows message and call function to handle command and 
  // system messages such as WM_TIMER.
  // You can use many different types of handlers. You can use any type of
  // Handlers but they are all designed to serve a different purpose
  // For example, you can use either COMMAND_ID_HANDLER or MESSAGE_HANDLER
  // to process ID_APP_ABOUT but COMMAND_ID_HANDLER offers a clean interface
  // for the handling function compared with MESSAGE_HANDLER
  	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MSG_WM_ERASEBKGND(OnEraseBackground)
    //MSG_WM_TIMER(OnTimer)
    MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		//COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    // Send the unprocessed Windows messages to CView message map #1.
    // Some command message can be handled better in CView than CMainFrame.
    CHAIN_MSG_MAP_ALT_MEMBER(m_view, 1)
  END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  // Prototypes of message hanlders
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  void SetPaneWidths(int* arrWidths, int nPanes);
  LRESULT OnEraseBackground(HDC hdc);
  LRESULT OnTimer(UINT /*uMsg*/, WPARAM uTimerID, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnViewFullscreen(UINT uCode, int nID, HWND hwndCtrl);
  BOOL FullScreen();
  BOOL Windowed();
};

