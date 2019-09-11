// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: This is also a good place to include header files that are neeed
//    to be accessed globally.
/////////////////////////////////////////////////////////////////////////////


#pragma once

// Change these values to use different versions
#define WINVER		0x0601
#define _WIN32_WINNT	0x0601
#define _WIN32_IE	0x0700
#define _RICHEDIT_VER	0x0500

#define MAX_STR 1000
#define MAX_TITLE 75

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include <atlcrack.h>

#include "atlwfile.h"

using namespace std;

#include <vector>
#include <stdarg.h>
#include <io.h>
#include <fcntl.h>
#include <limits>
//#include <sys/stat.h>
#include <math.h>

// Boost multi_array declaration for various 2D, 3D, and 4D matrix structures
#include "boost/multi_array.hpp"
typedef boost::multi_array_types::index_range range;
//boost::multi_array<int, 2>::extent_gen extentsI2D;
//boost::multi_array<int, 3>::extent_gen extentsI3D;
//boost::multi_array<int, 4>::extent_gen extentsI4D;
typedef boost::multi_array<int, 2> ArrayI2D;
typedef boost::multi_array<int, 3> ArrayI3D;
typedef boost::multi_array<int, 4> ArrayI4D;
//boost::multi_array<float, 2>::extent_gen extentsF2D;
//boost::multi_array<float, 3>::extent_gen extentsF3D;
//boost::multi_array<float, 4>::extent_gen extentsF4D;
typedef boost::multi_array<float, 2> ArrayF2D;
typedef boost::multi_array<float, 3> ArrayF3D;
typedef boost::multi_array<float, 4> ArrayF4D;
//boost::multi_array<double, 2>::extent_gen extentsD2D;
//boost::multi_array<double, 3>::extent_gen extentsD3D;
//boost::multi_array<double, 4>::extent_gen extentsD4D;
typedef boost::multi_array<double, 2> ArrayD2D;
typedef boost::multi_array<double, 3> ArrayD3D;
typedef boost::multi_array<double, 4> ArrayD4D;

#define OpenVR 1

#include "CPPMatrix2.h"
#include "ModelRead.h"
#include "VTK_Operation.h"

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
