// VTK_Viewer_WTL10.cpp : main source file for VTK_Viewer_WTL10.exe
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: This file contains the starting function (_tWinMain) and Run
//   of this application. Run function setups the MainFrame window (main window)
//   and run the main message loop. 
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "resource.h"

#include "View.h"
#include "aboutdlg.h"
#include "MainFrm.h"

// Global variable for module class of the application
CAppModule _Module;

// Setup MainFrame Window, MeesageLoop and run the message loop
int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
  // Create a message loop and add it to the app module
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

  // MainFrame Window is the parent window of the application
	CMainFrame wndMain;

  // the location and size of the main window
  CRect rect(50, 50, 1024 + 50, 768 + 50);

  // Create the window using the rect provided
	if(wndMain.CreateEx(NULL, rect) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

  // Show the main window
	wndMain.ShowWindow(nCmdShow);

  // This is the main message loop. This will end when user quits the application
	int nRet = theLoop.Run();

  // Remove the message loop from the module
	_Module.RemoveMessageLoop();
	return nRet;
}

// The starting point of ths Windows application
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
  // Initialize the COM library on the current thread
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

  // Registers and initializes common control windows classes.
  // Makes sure Comctl32.dll is loaded and registers common control classes
	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

  // Initializes the module
	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

  // Setup the winodw and start the main message loop
	int nRet = Run(lpstrCmdLine, nCmdShow);

  // Terminate the module
	_Module.Term();
  // Unregisters the COM library
	::CoUninitialize();

	return nRet;
}
