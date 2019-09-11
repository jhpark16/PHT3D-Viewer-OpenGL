// aboutdlg.cpp : implementation of the CAboutDlg class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: CAboutDlg class provides a simple dialog class showing the 
//              about the application dialog box accessible from main help menu
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"

// Called before the dialog box is intilalized
LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  // Set the location of the dialog box to the centre of the Parent window (MainFrm)
  CenterWindow(GetParent());
	return TRUE;
}

// When the OK button is pressed
LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // Stop processing the dialog box and destroy the box
	EndDialog(wID);
	return 0;
}
