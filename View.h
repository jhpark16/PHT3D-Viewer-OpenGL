// View.h : interface of the CView class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: 
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Light.h"
#include "Shader.h"

class CView : public CWindowImpl<CView>//, public VTK_Operation
{
public:
	DECLARE_WND_CLASS(NULL)

  HGLRC m_hRC;			// Handle to RC
  Camera camera;
  Light mainLight;
  glm::mat4 projection;
  GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformAmbientIntensity = 0,
    uniformAmbientColour = 0, uniformDirection = 0, uniformDiffuseIntensity = 0,
    uniformVColourFrac = 0;
  GLfloat fFovy = 45.0f; // Field-of-view
  GLfloat fZNear = 0.1f;  // Near clipping plane
  GLfloat fZFar = 100.0f;  // Far clipping plane
  GLfloat fAspect = 1.25;
  LARGE_INTEGER start;
  LARGE_INTEGER frequency;


	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_SIZE(OnSize)
    MSG_WM_ERASEBKGND(OnEraseBackground)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MSG_WM_SYSCOMMAND(OnSysCommand)
    MSG_WM_ACTIVATEAPP(OnActivateApp)
    //		MSG_WM_TIMER(OnTimer)
    MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_LBUTTONUP(OnLButtonUp)
    MSG_WM_MBUTTONDOWN(OnMButtonDown)
    MSG_WM_MBUTTONUP(OnMButtonUp)
    MSG_WM_RBUTTONDOWN(OnRButtonDown)
    MSG_WM_RBUTTONUP(OnRButtonUp)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MESSAGE_HANDLER(WM_CHAR, OnChar)
    MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
    MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
    ALT_MSG_MAP(1)
    COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
    COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
  END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
  CView();
  LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  BOOL PreviousInstanceFound(LPCTSTR lpClassName, LPCTSTR lpWindowName);
  static BOOL AllowMultipleInstances();
  HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
    DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0, LPVOID lpCreateParam = NULL);
  LRESULT OnCreate(LPCREATESTRUCT lParam);
  void OnDestroy();
  LRESULT OnSize(UINT uMsg, CSize size);
  LRESULT OnEraseBackground(HDC hdc);
  LRESULT OnSysCommand(UINT uMsg, CPoint point);
  LRESULT OnActivateApp(BOOL activated, DWORD minimized);
  virtual void ActivateApp();
  virtual void DeactivateApp();
  void OnTimer(UINT uTimerID, TIMERPROC pTimerProc);
  void StepDemo(int step);
  void StartClock();
  void StopClock();
  void OnLButtonDblClk(UINT uMsg, CPoint point);
  void OnLButtonDown(UINT uMsg, CPoint point);
  void OnMButtonDown(UINT uMsg, CPoint point);
  void OnRButtonDown(UINT uMsg, CPoint point);
  void OnLButtonUp(UINT uMsg, CPoint point);
  void OnMButtonUp(UINT uMsg, CPoint point);
  void OnRButtonUp(UINT uMsg, CPoint point);
  void OnMouseMove(UINT uMsg, CPoint point);

  void OnInit(void);
  void OnRender(void);
  void OnResize(int cx, int cy);
  LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  void ResetModelData();
  void CreateObjects();
  void Get_PHT3D_Model(unsigned int *&indices, unsigned int &nIndices, GLfloat *&vertices,
    unsigned int &nVertices, unsigned int nDataPerTri);
  PHT3D_Model& GetPHT3DM(void); 

private:
  //bool bInitialized;
  PHT3D_Model mPHT3DM;
  BOOL disableActivateApp;
  BOOL m_bClockRunning;
  SYSTEMTIME m_stLastTime;
//  VTK_Operation* pvtkDemo;
  HWND m_hWndParent;
};

void CalcAverageNormals(unsigned int *indices, unsigned int indiceCount, GLfloat *vertices,
  unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset);

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

