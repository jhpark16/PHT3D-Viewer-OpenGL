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
#include "Mesh.h"

#include "ModelRead.h"

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

const float toRadians = glm::pi<float>() / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  GLuint TextureID;   // ID handle of the glyph texture
  glm::ivec2 Size;    // Size of glyph
  glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
  GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char *vShader1 = "Shaders/shader.vs";

// Fragment Shader
static const char *fShader1 = "Shaders/shader.fs";

CView::CView()
{
  
}

void CreateShaders()
{
  Shader *shader1 = new Shader();
  shader1->CreateFromFiles(vShader1, fShader1);
  shaderList.push_back(*shader1);
  Shader *shader2 = new Shader();
  shader2->CreateFromFiles("Shaders/text.vs", "Shaders/text.fs");
  shaderList.push_back(*shader2);
}

void CalcAverageNormals(unsigned int *indices, unsigned int indiceCount, GLfloat *vertices,
  unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
{
  for (size_t i = 0; i < indiceCount; i += 3) {
    unsigned int in0 = indices[i] * vLength;
    unsigned int in1 = indices[i + 1] * vLength;
    unsigned int in2 = indices[i + 2] * vLength;
    glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1],
      vertices[in1 + 2] - vertices[in0 + 2]);
    glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1],
      vertices[in2 + 2] - vertices[in0 + 2]);
    glm::vec3 normal = glm::cross(v1, v2);
    normal = glm::normalize(normal);

    in0 += normalOffset;
    in1 += normalOffset;
    in2 += normalOffset;
    vertices[in0] += normal.x;
    vertices[in0 + 1] += normal.y;
    vertices[in0 + 2] += normal.z;
    vertices[in1] += normal.x;
    vertices[in1 + 1] += normal.y;
    vertices[in1 + 2] += normal.z;
    vertices[in2] += normal.x;
    vertices[in2 + 1] += normal.y;
    vertices[in2 + 2] += normal.z;
  }

  for (size_t i = 0; i < verticeCount / vLength; i++) {
    unsigned int nOffset = i * vLength + normalOffset;
    glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
    if (vec.x != 0.0 || vec.y != 0.0 || vec.z != 0.0) {
      vec = glm::normalize(vec);
      vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
  }
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
    OpenModel(mPHT3DM, fileDlg.m_szFileName);
  }
  return 0;
}

void CView::Get_PHT3D_Model(unsigned int *&indices, unsigned int &nIndices, GLfloat *&vertices,
  unsigned int &nVertices, unsigned int nVertColumn)
{
  int idx0{ 0 }, idx1{ 0 }, idx2{ 0 }, idx3{ 0 };
#define PHT3D 1
#ifdef PHT3D
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  double minVal = 0, maxVal = 0;
  OpenModel(mPHT3DM, "D:\\Programming\\WTL\\PHT3D-Viewer-OpenGL\\Example\\model1.nam");

  int nCol = mf.DIS_NCOL + 1;
  int nRow = mf.DIS_NROW + 1;
  int nLay = mf.DIS_NLAY + 1;
  float *xLoc = new float[nCol] {};
  float *yLoc = new float[nRow] {};
  float *zLoc = new float[nLay*nRow*nCol]{};
  xLoc[0] = 0; yLoc[0] = 0;
  for (int i = 0; i < nCol - 1; i++) {
    xLoc[i + 1] = xLoc[i] + mf.DIS_DELR[i];
  }
  mPHT3DM.Xmin = xLoc[0];
  mPHT3DM.Xmax = xLoc[nCol - 1];
  for (int i = 0; i < nRow - 1; i++) {
    yLoc[i + 1] = yLoc[i] + mf.DIS_DELC[i];
  }
  mPHT3DM.Ymin = yLoc[0];
  mPHT3DM.Ymax = yLoc[nRow - 1];
  // Reverse Y for correct model display
  for (int i = 0; i < (nRow - 1) / 2; i++) {
    int iTmp = yLoc[i];
    yLoc[i] = yLoc[nRow - 1 - i];
    yLoc[nRow - 1 - i] = iTmp;
  }
  int nPoints = nLay*nRow*nCol;
  delete[] indices;
  delete[] vertices;
  unique_ptr<float[]> locations(new float[4 * nPoints]);
  //float *locations = new float[3 * nPoints]{};
  unique_ptr<int[]> connectivity(new int[9 * ((nLay - 1)*(nRow - 1)*(nCol - 1))]);
  unique_ptr<double[]> dVal(new double[((nLay - 1)*(nRow - 1)*(nCol - 1))]);

  //int *connectivity = new int[9*((nLay - 1)*(nRow - 1)*(nCol - 1))]{};
  //int *connectivity = new int[(nLay-1)*(nRow-1)*(nCol-1)] {};
  // The heights are not aligned at each nodal points. So, it is necessary to estimate the height
  // at each node using interpolation of heights
  mPHT3DM.Zmin = 1e20;
  mPHT3DM.Zmax = -1e20;
  CPPMatrix2<double>& elevation = mf.DIS_TOP;
  for (int k = 0; k < nLay; k++) {
    // Takes care of four corners
    if (k < nLay - 1) {
      elevation = mf.DIS_BOTMS[k];
    }
    else {
      elevation = mf.DIS_TOP;
    }
    zLoc[k*nRow*nCol + 0 * nCol + 0] = elevation[0][0];
    zLoc[k*nRow*nCol + 0 * nCol + (nCol - 1)] = elevation[0][nCol - 2];
    zLoc[k*nRow*nCol + (nRow - 1) * nCol + 0] = elevation[nRow - 2][0];
    zLoc[k*nRow*nCol + (nRow - 1) * nCol + (nCol - 1)] = elevation[nRow - 2][nCol - 2];
    // Interpolate edges along X direction 
    for (int i = 1; i < nCol - 1; i++) {
      zLoc[k*nRow*nCol + 0 * nCol + i] = (elevation[0][i - 1] +
        elevation[0][i]) / 2.0;
      zLoc[k*nRow*nCol + (nRow - 1)*nCol + i] = (elevation[nRow - 2][i - 1] +
        elevation[nRow - 2][i]) / 2.0;
    }
    // Interpolate edges along Y direction 
    for (int j = 1; j < nRow - 1; j++) {
      zLoc[k*nRow*nCol + (j * nCol) + 0] = (elevation[j - 1][0] +
        elevation[j][0]) / 2.0;
      zLoc[k*nRow*nCol + (j * nCol) + nCol - 1] = (elevation[j - 1][nCol - 2] +
        elevation[j][nCol - 2]) / 2.0;
    }
    // Interpolate the remaining part of the 3D model
    for (int j = 1; j < nRow - 1; j++) {
      for (int i = 1; i < nCol - 1; i++) {
        zLoc[k*nRow*nCol + (j * nCol) + i] = (elevation[j - 1][i - 1] +
          elevation[j][i - 1] + elevation[j - 1][i] +
          elevation[j][i]) / 4.0;
      }
    }
    // Set the X, Y, Z of all nodal points
    for (int j = 0; j < nRow; j++) {
      for (int i = 0; i < nCol; i++) {
        int i1 = i == 0 ? i : i - 1;
        int i2 = i == nCol - 1 ? i - 1 : i;
        int j1 = j == 0 ? j : j - 1;
        int j2 = j == nRow - 1 ? j - 1 : j;
        int k1 = k == 0 ? k : k - 1;
        int k2 = k == nLay - 1 ? k - 1 : k;
        float tmp = 0;
        int nItems = 0;
        for (int k3 : {k1, k2})
          for (int j3 : {j1, j2})
            for (int i3 : {i1, i2})
            {
              if (mf.BAS_IBOUND[k3][j3][i3] > 0) {
                nItems++;
                tmp += mf.BAS_STRT[k3][j3][i3];
              }
            }
        locations[4 * (k*(nRow*nCol) + j*(nCol)+i) + 3] = nItems>0 ? tmp/nItems:0;
        locations[4 * (k*(nRow*nCol) + j*(nCol)+i) + 0] = xLoc[i]; //X value
        locations[4 * (k*(nRow*nCol) + j*(nCol)+i) + 1] = yLoc[j]; //Y value
        tmp = zLoc[k*nRow*nCol + j*nCol + i];
        locations[4 * (k*(nRow*nCol) + j*(nCol)+i) + 2] = tmp; //Z value
        if (mPHT3DM.Zmin > tmp) mPHT3DM.Zmin = tmp;
        if (mPHT3DM.Zmax < tmp) mPHT3DM.Zmax = tmp;
      }
    }
  }
  minVal = DBL_MAX;
  maxVal = -DBL_MAX;
  int idx[4];
  int numActiveCells = 0;
  for (int k = 0; k < nLay - 1; k++) {
    for (int j = 0; j < nRow - 1; j++) {
      for (int i = 0; i < nCol - 1; i++) {
        if (mf.BAS_IBOUND[k][j][i] > 0) {
          double t1 = mf.BAS_STRT[k][j][i];
          if (minVal > t1) minVal = t1;
          if (maxVal < t1) maxVal = t1;
          numActiveCells++;
        }
      }
    }
  }
  indices = new unsigned int[numActiveCells * 36];
  vertices = new GLfloat[numActiveCells * 24 * nVertColumn];
  int numCells = 0;
  for (int k = 0; k < nLay - 1; k++) {
    for (int j = 0; j < nRow - 1; j++) {
      for (int i = 0; i < nCol - 1; i++) {
        if (mf.BAS_IBOUND[k][j][i]) {
          connectivity[9 * numCells + 0] = 8;
          connectivity[9 * numCells + 1] = (k)*(nRow*nCol) + (j)*nCol + (i);  //7
          connectivity[9 * numCells + 2] = (k + 1)*(nRow*nCol) + (j)*nCol + (i); //3
          connectivity[9 * numCells + 3] = (k + 1)*(nRow*nCol) + (j + 1)*nCol + (i); // 0
          connectivity[9 * numCells + 4] = (k)*(nRow*nCol) + (j + 1)*nCol + (i); //4
          connectivity[9 * numCells + 5] = (k)*(nRow*nCol) + (j)*nCol + (i + 1); //6
          connectivity[9 * numCells + 6] = (k + 1)*(nRow*nCol) + (j)*nCol + (i + 1); //2
          connectivity[9 * numCells + 7] = (k + 1)*(nRow*nCol) + (j + 1)*nCol + (i + 1); //1
          connectivity[9 * numCells + 8] = (k)*(nRow*nCol) + (j + 1)*nCol + (i + 1); //5
          for (int k6 = 0; k6 < 6; k6++) {
            switch (k6) {
            case 0:
              idx[0] = 0; idx[1] = 1; idx[2] = 2; idx[3] = 3;
              break;
            case 1:
              idx[0] = 1; idx[1] = 5; idx[2] = 6; idx[3] = 2;
              break;
            case 2:
              idx[0] = 2; idx[1] = 6; idx[2] = 7; idx[3] = 3;
              break;
            case 3:
              idx[0] = 3; idx[1] = 7; idx[2] = 4; idx[3] = 0;
              break;
            case 4:
              idx[0] = 4; idx[1] = 5; idx[2] = 1; idx[3] = 0;
              break;
            case 5:
              idx[0] = 5; idx[1] = 4; idx[2] = 7; idx[3] = 6;
              break;
            }
            int iVertStart = numCells * 24 + k6 * 4;
            //
            int in0 = 4 * connectivity[9 * numCells + 1 + idx[0]];
            int in1 = 4 * connectivity[9 * numCells + 1 + idx[1]];
            int in2 = 4 * connectivity[9 * numCells + 1 + idx[2]];
            glm::vec3 v1(locations[in1] - locations[in0], locations[in1 + 1] - locations[in0 + 1],
              locations[in1 + 2] - locations[in0 + 2]);
            glm::vec3 v2(locations[in2] - locations[in0], locations[in2 + 1] - locations[in0 + 1],
              locations[in2 + 2] - locations[in0 + 2]);
            glm::vec3 normal = glm::cross(v1, v2);
            normal = glm::normalize(normal);

            for (int ii = 0; ii < 4; ii++) {
              int ii5 = connectivity[9 * numCells + 1 + idx[ii]];
              int iVert = (iVertStart + ii) * nVertColumn;
              float tmp = (locations[ii5 * 4 + 3] - minVal) / (maxVal - minVal);
              float r, g, b;
              if (tmp <= 0.25) {
                r = 0;
                g = max(0.0, (tmp - 0.0) / 0.25);
                b = 1;
              }
              else if (tmp <= 0.5) {
                r = 0;
                g = 1;
                b = (0.5 - tmp) / 0.25;
              }
              else if (tmp <= 0.75) {
                r = (tmp - 0.5) / 0.25;
                g = 1;
                b = 0;
              }
              else {
                r = 1;
                g = max(0.0, (1.0 - tmp) / 0.25);
                b = 0;
              }
              vertices[iVert + 0] = locations[ii5 * 4 + 0];
              vertices[iVert + 1] = locations[ii5 * 4 + 1];
              vertices[iVert + 2] = locations[ii5 * 4 + 2];
              vertices[iVert + 3] = r;
              vertices[iVert + 4] = g;
              vertices[iVert + 5] = b;
              /*vertices[iVert + 6] = normal.x;
              vertices[iVert + 7] = normal.y;
              vertices[iVert + 8] = normal.z;*/
              vertices[iVert + 6] = 0;
              vertices[iVert + 7] = 0;
              vertices[iVert + 8] = 0;
            }
            if (1) {
              // 1st
              indices[numCells * 36 + k6 * 6 + 0] = iVertStart + 0;
              indices[numCells * 36 + k6 * 6 + 1] = iVertStart + 1;
              indices[numCells * 36 + k6 * 6 + 2] = iVertStart + 2;
              // 2nd
              indices[numCells * 36 + k6 * 6 + 3] = iVertStart + 0;
              indices[numCells * 36 + k6 * 6 + 4] = iVertStart + 2;
              indices[numCells * 36 + k6 * 6 + 5] = iVertStart + 3;
            }
            else {
              // 1st
              indices[numCells * 36 + k6 * 6 + 0] = connectivity[9 * numCells + 1 + idx[0]];
              indices[numCells * 36 + k6 * 6 + 1] = connectivity[9 * numCells + 1 + idx[2]];
              indices[numCells * 36 + k6 * 6 + 2] = connectivity[9 * numCells + 1 + idx[3]];
              // 2nd
              indices[numCells * 36 + k6 * 6 + 3] = connectivity[9 * numCells + 1 + idx[0]];
              indices[numCells * 36 + k6 * 6 + 4] = connectivity[9 * numCells + 1 + idx[1]];
              indices[numCells * 36 + k6 * 6 + 5] = connectivity[9 * numCells + 1 + idx[2]];
            }
          }

/*
          double tVal = mf.BAS_STRT[k][j][i];
          dVal[numCells] = tVal;
          if (tVal < minVal) minVal = tVal;
          if (tVal > maxVal) maxVal = tVal;
          */
          numCells++;
        }
      }
    }
  }
#ifdef OLD
  for (int i = 0; i < nPoints; i++) {
    vertices[i*nVertColumn + 0] = locations[i * 4 + 0];
    vertices[i*nVertColumn + 1] = locations[i * 4 + 1];
    vertices[i*nVertColumn + 2] = locations[i * 4 + 2];
    float tmp = (locations[i * 4 + 3] - minVal) / (maxVal - minVal);
    float r, g, b;
    if (tmp <= 0.25) {
      r = 0;
      g = max(0.0, (tmp-0.0)/0.25);
      b = 1;
    }
    else if (tmp <= 0.5) {
      r = 0;
      g = 1;
      b = (0.5-tmp) / 0.25;
    }
    else if (tmp <= 0.75) {
      r = (tmp - 0.5) / 0.25;
      g = 1;
      b = 0;
    }
    else {
      r = 1;
      g = max(0.0,(1.0 - tmp) / 0.25);
      b = 0;
    }
    vertices[i*nVertColumn + 3] = r;
    vertices[i*nVertColumn + 4] = g;
    vertices[i*nVertColumn + 5] = b;
    vertices[i*nVertColumn + 6] = 0.0f;
    vertices[i*nVertColumn + 7] = 0.0f;
    vertices[i*nVertColumn + 8] = 0.0f;
  }
#endif
  nIndices = numCells * 36;
  nVertices = numCells * 24 * nVertColumn;

#else
  int nPoints = 12;
  int numCells = 2;
  //cellIdType->SetNumberOfValues(18);
  int *iCon = new int[numCells * 9]{ 8, 0, 1, 2, 3, 4, 5, 6, 7,
    8, 4, 5, 6, 7, 8, 9, 10, 11 };
  for (int i = 0; i < numCells; i++) {
    // Counter clockwise
    for (int k = 0; k < 6; k++) {
      switch (k) {
      case 0:
        idx0 = 0; idx1 = 1; idx2 = 2; idx3 = 3;
        break;
      case 1:
        idx0 = 1; idx1 = 5; idx2 = 6; idx3 = 2;
        break;
      case 2:
        idx0 = 2; idx1 = 6; idx2 = 7; idx3 = 3;
        break;
      case 3:
        idx0 = 3; idx1 = 7; idx2 = 4; idx3 = 0;
        break;
      case 4:
        idx0 = 4; idx1 = 5; idx2 = 1; idx3 = 0;
        break;
      case 5:
        idx0 = 5; idx1 = 4; idx2 = 7; idx3 = 6;
        break;
      }
      // 1st
      indices[i * 36 + k * 6 + 0] = iCon[i * 9 + 1 + idx0];
      indices[i * 36 + k * 6 + 1] = iCon[i * 9 + 1 + idx2];
      indices[i * 36 + k * 6 + 2] = iCon[i * 9 + 1 + idx3];
      // 2nd
      indices[i * 36 + k * 6 + 3] = iCon[i * 9 + 1 + idx0];
      indices[i * 36 + k * 6 + 4] = iCon[i * 9 + 1 + idx1];
      indices[i * 36 + k * 6 + 5] = iCon[i * 9 + 1 + idx2];
    }
  }
  /// cellIdType->SetArray(iCon, numCells * 9, 1);
  /// cellArr->SetCells(numCells, cellIdType);
  // Set points
  double *(val[12]);
  float *fPos = new float[nPoints * 3]{ 0, 0, 0,  0, 0, 1,  0, 1, 1,  0, 1, 0,
    1, 0, 0,  1, 0, 1,  1, 1, 1,  1, 1, 0,
    2, 0, 0,  2, 0, 1,  2, 1, 1,  2, 1, 0 };
  for (int i = 0; i < nPoints; i++) {
    vertices[i*nDataPerTri + 0] = fPos[i * 3 + 0];
    vertices[i*nDataPerTri + 1] = fPos[i * 3 + 1];
    vertices[i*nDataPerTri + 2] = fPos[i * 3 + 2];
    vertices[i*nDataPerTri + 3] = (i % 4) / 3.0f;
    vertices[i*nDataPerTri + 4] = ((4 - i) % 4) / 3.0f;
    vertices[i*nDataPerTri + 5] = ((i + 2) % 4) / 3.0f;
    vertices[i*nDataPerTri + 6] = 0.0f;
    vertices[i*nDataPerTri + 7] = 0.0f;
    vertices[i*nDataPerTri + 8] = 0.0f;
  }
  nIndices = numCells * 36;
  nVertices = nPoints * nDataPerTri;
#endif
}

void CView::CreateObjects()
{
  unsigned int nTriangles{ 100 }, nIndices{ 0 }, nVertices{ 0 };
  unsigned int idxNormal{ 6 }, idxData{ 3 }, nDataPerTri{ 9 };
#define NEWMODEL 1

#ifdef NEWMODEL
  unsigned int *indices;
  GLfloat *vertices;
  indices = new unsigned int[nTriangles*100];
  vertices = new GLfloat[nTriangles *nDataPerTri];

  Get_PHT3D_Model(indices, nIndices, vertices, nVertices, nDataPerTri);
#else
  nTriangles = 4;
  nIndices = nTriangles * 3;
  nVertices = nTriangles * nDataPerTri;
  unsigned int indices[] = {
    0, 3, 1,
    1, 3, 2,
    2, 3, 0,
    0, 1, 2
  };

  GLfloat vertices[] = {
    // X,    y,     z,    r,    g,   b,   nx (normal x), ny, nz     
    -1.0f, -1.0f, 0.0f,  1.f,  0.f, 0.f,  0.f, 0.f, 0.f,
    0.0f, -1.0f, 1.0f,  0.0f, 1.f, 0.f,  0.f, 0.f, 0.f,
    1.0f, -1.0f, 0.0f,  0.f,  0.f, 1.f,  0.f, 0.f, 0.f,
    0.0f, 1.0f, 0.0f,  1.0f,  1.f, 1.f,  0.f, 0.f, 0.f };
#endif
  CalcAverageNormals(indices, nIndices, vertices, nVertices, nDataPerTri, idxNormal);

  Mesh *obj1 = new Mesh();
  obj1->CreateMesh(vertices, indices, nVertices, nIndices);
  meshList.push_back(obj1);
  Mesh *obj2 = new Mesh();
  for (int i = 0; i < nVertices; i += nDataPerTri) {
    vertices[i + idxData] = 0.1f;
    vertices[i + idxData + 1] = 0.1f;
    vertices[i + idxData + 2] = 0.1f;
  }
  obj2->CreateMesh(vertices, indices, nVertices, nIndices);
  meshList.push_back(obj2);
  delete[] indices;
  delete[] vertices;
  Mesh *obj3 = new Mesh();
  int nVertices2 = 10*9;
  int nIndices2 = 24;
  unsigned int indices2[] = {
    0, 1, 3,
    0, 3, 2,
    2, 3, 5,
    2, 5, 4,
    4, 5, 7,
    4, 7, 6,
    6, 7, 9,
    6, 9, 8
  };

  GLfloat vertices2[] = {
    // X,    y,     z,    r,    g,   b,   nx (normal x), ny, nz     
    0.7f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.f, 0.f, 0.f,
    0.8f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.f, 0.f, 0.f,
    0.7f,-0.25f, 0.0f,  0.0f, 1.0f, 1.0f,  0.f, 0.f, 0.f,
    0.8f,-0.25f, 0.0f,  0.0f, 1.0f, 1.0f,  0.f, 0.f, 0.f,
    0.7f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.f, 0.f, 0.f,
    0.8f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.f, 0.f, 0.f,
    0.7f, 0.25f, 0.0f,  1.0f, 1.0f, 0.0f,  0.f, 0.f, 0.f,
    0.8f, 0.25f, 0.0f,  1.0f, 1.0f, 0.0f,  0.f, 0.f, 0.f,
    0.7f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.f, 0.f, 0.f,
    0.8f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.f, 0.f, 0.f  };
  CalcAverageNormals(indices2, nIndices2, vertices2, nVertices2, nDataPerTri, idxNormal);

  obj3->CreateMesh(vertices2, indices2, nVertices2, nIndices2);
  meshList.push_back(obj3);
}

#ifdef OLD
void AddShader(GLuint theProgram, const char*shaderCode, GLenum shaderType)
{
  GLuint theShader = glCreateShader(shaderType);
  const GLchar* theCode[1];
  theCode[0] = shaderCode;
  GLint codeLength[1];
  codeLength[0] = strlen(shaderCode);

  glShaderSource(theShader, 1, theCode, codeLength);
  glCompileShader(theShader);

  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

  if (!result) {
    glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
    fprintf(stderr, "Error compiling the %d shader: '%s'\n", shaderType, eLog);
    return;
  }

  glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
  shader = glCreateProgram();

  if (!shader) {
    fprintf(stderr, "Error creating shader program!\n");
    return;
  }

  AddShader(shader, vShader, GL_VERTEX_SHADER);
  AddShader(shader, fShader, GL_FRAGMENT_SHADER);

  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  glLinkProgram(shader);
  glGetProgramiv(shader, GL_LINK_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
    fprintf(stderr, "Error linking program: '%s'\n", eLog);
    return;
  }

  glValidateProgram(shader);
  glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
    fprintf(stderr, "Error validating program: '%s'\n", eLog);
    return;
  }

  uniformModel = glGetUniformLocation(shader, "model");
}
*/
void CreateTriangle()
{
  GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f };

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}

void CreateCube(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength)
{
  GLfloat halfSideLength = edgeLength * 0.5f;

  GLfloat vertices[] =
  {
    // front face
    centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
    centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top right
    centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom right
    centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

    // back face
    centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top left
    centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
    centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
    centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom left

    // left face
    centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
    centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
    centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
    centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

    // right face
    centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
    centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
    centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
    centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

    // top face
    centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
    centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
    centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // bottom right
    centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // bottom left

    // top face
    centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // top left
    centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // top right
    centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
    centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength  // bottom left
  };

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}
#endif

BOOL CView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
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
  CClientDC dc(m_hWnd);

  // Fill in the pixel format descriptor.
  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE;

  int nPixelFormat = dc.ChoosePixelFormat(&pfd);
  ATLASSERT(nPixelFormat != 0);

  BOOL bResult = dc.SetPixelFormat(nPixelFormat, &pfd);
  ATLASSERT(bResult);

  m_hRC = dc.wglCreateContext();
  ATLASSERT(m_hRC);

  dc.wglMakeCurrent(m_hRC);
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    //glfwTerminate();
    return FALSE;
  }
  OnInit();
  // For line rendering
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // For filled polygon
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glFrontFace(GL_CCW);
  //CreateTriangle();
  dc.wglMakeCurrent(NULL);

  //bInit = true;

  //bHandled = FALSE;
  SetMsgHandled(FALSE);
  return FALSE;
}

void CView::OnInit(void)
{
#ifdef OLD
    //m_fAngle = 0.0f;

    //glClearColor(0.000f, 0.000f, 0.000f, 1.0f); //Background color

    // Activate lighting and a light source
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    // Define material parameters
    static GLfloat glfMatAmbient[] = { 0.000f, 0.450f, 1.000f, 1.0f };
    static GLfloat glfMatDiffuse[] = { 0.000f, 0.000f, 0.580f, 1.0f };
    static GLfloat glfMatSpecular[] = { 1.000f, 1.000f, 1.000f, 1.0f };
    static GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
    static GLfloat fShininess = 128.000f;

    // Set material parameters
    glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);
    glMaterialf(GL_FRONT, GL_SHININESS, fShininess);

    CreateCube(0, 0, 0, 1.0);
    CompileShaders();
#else
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  CreateObjects();
  CreateShaders();

  camera = Camera(glm::vec3(0.f, 0.0f, 10.f), glm::vec3(0.f, 1.f, 0.f), -90.f, 0.f, 0.1f, 0.1f);
  //brickTexture = Texture("Textures/brick.png");
  //brickTexture.LoadTexture();
  //dirtTexture = Texture("Textures/dirt.png");
  //dirtTexture.LoadTexture();

  //mainLight = Light(1.f, 1.f, 1.f, 0.3f, 2.f, -1.f, -2.f, 1.f);
  //mainLight = Light(1.f, 1.f, 1.f, 0.3f, -1.f, 1.f, 1.f, 1.0f);
  mainLight = Light(1.f, 1.f, 1.f, 0.4f, -1.f, 1.f, 0.0f, 1.0f);

  //projection = glm::perspective(glm::radians(fFovy), (GLfloat)800 / 600, fZNear, fZFar);

#ifdef TEST
  glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(800), 0.0f, static_cast<GLfloat>(600));
  shaderList[1].UseShader();
  glUniformMatrix4fv(glGetUniformLocation(shaderList[1].GetShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
#endif

  // FreeType
#define FONT 1
#ifdef FONT
  FT_Library ft;
  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&ft))
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

  // Load font as face
  FT_Face face;
  if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

  // Set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, 48);

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Load first 128 characters of ASCII set
  for (GLubyte c = 0; c < 128; c++)
  {
    // Load character glyph 
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
      continue;
    }
    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Now store character for later use
    Character character = {
      texture,
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      face->glyph->advance.x
    };
    Characters.insert(std::pair<GLchar, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
#endif
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&start);
#endif
}

void CView::OnDestroy()
{
  ATLTRACE("OnDestroy\n");
  KillTimer(1);
  //delete pvtkDemo;
  //pvtkDemo = nullptr;
  PostQuitMessage(0);
  SetMsgHandled(FALSE);
}

LRESULT CView::OnSize(UINT uMsg, CSize size)
{
  ATLTRACE("OnSize\n");
  CClientDC dc(m_hWnd);
  dc.wglMakeCurrent(m_hRC);
  OnResize(size.cx, size.cy);
  dc.wglMakeCurrent(NULL);
  Invalidate();
  SetMsgHandled(TRUE);
  //  }
  return TRUE;
}

void CView::OnResize(int cx, int cy) {
  RECT rc;
  GetClientRect(&rc);
  if (rc.right - rc.left > 0) {
    // Calculate viewport aspect
    fAspect = (GLfloat)(rc.right - rc.left) / (GLfloat)(rc.bottom - rc.top);
    projection = glm::perspective(glm::radians(fFovy), fAspect, fZNear, fZFar);
    // Setup Viewport size
    glViewport(0, 0, (rc.right - rc.left), (rc.bottom - rc.top));

  }
}

void CView::OnRender(void)
{
  CClientDC dc(m_hWnd);
  RECT rc;
  GetClientRect(&rc);

#ifdef OLD
    // Clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(shader);

    glm::mat4 model{ 1.0f };
    //model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
    model = glm::rotate(model, curAngle*toRadians, glm::vec3(0.0f, 1.0f, 1.0f));
    //model = glm::scale(model, glm::vec3(curSize, curSize, 1.0f));

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_QUADS, 0, 24);
    glBindVertexArray(0);
    //glDisableClientState(GL_VERTEX_ARRAY);
    glUseProgram(0);
#else
    // Clear window
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // VTK colour
    glClearColor(.1f, .2f, .4f, 1.0f);
    // Desert colour
    //glClearColor(0.93f, 0.79f, 0.686f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderList[0].UseShader();
    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
    uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
    uniformDirection = shaderList[0].GetDirectionLocation();
    uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();
    uniformVColourFrac = shaderList[0].GetVColourFracLocation();

    mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColour,
      uniformDiffuseIntensity, uniformDirection);

    glm::mat4 model{ 1.0f };
    glm::mat4 model1{ 1.0f };
    glm::mat4 modelRotate, modelScale, modelTrans;
    if (0) {
      model = glm::translate(model, glm::vec3(0.f, 0.f, -2.5f));
      model = glm::rotate(model, glm::radians(25.0f), glm::vec3(1.0f, 1.0f, 0.0f));
      model = glm::scale(model, glm::vec3(0.0001f, 0.0001f, 0.0001f));
    }
    else {
      float factor = 6.0/max(mPHT3DM.Xmax - mPHT3DM.Xmin, mPHT3DM.Ymax - mPHT3DM.Ymin);
      //model = glm::rotate(model, glm::radians(30.0f), glm::vec3(-1.0f, 1.0f, 0.0f));
      modelRotate = glm::rotate(model1, glm::radians(-30.0f), glm::vec3(1.0f, 0.4f, 0.3f));
      modelScale = glm::scale(model1, glm::vec3(factor, factor, factor));
      //model = glm::translate(model, glm::vec3(-2.0f, -6.0f, -2.5f));
      modelTrans = glm::translate(model1, glm::vec3(-(mPHT3DM.Xmax - mPHT3DM.Xmin)/2.0,
        -(mPHT3DM.Ymax - mPHT3DM.Ymin)/2.0, -((mPHT3DM.Zmax - mPHT3DM.Zmin)/2.0)));
      model = model*modelRotate*modelScale*modelTrans;
    }

    glUniform1f(uniformVColourFrac, 0.0f);
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // Hidden line removal with Polygon Offset
    if (0) {
      // Draw Lines
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      meshList[1]->RenderMesh();
      // Draw filled polygons
      //glClear(GL_DEPTH_BUFFER_BIT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(1, 1);
      meshList[0]->RenderMesh();
      glDisable(GL_POLYGON_OFFSET_FILL);
    }
    else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      meshList[0]->RenderMesh();
    }

    model = glm::mat4(1.0f);
    glUniform1f(uniformVColourFrac, 1.0f);
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    meshList[2]->RenderMesh();

#ifdef FONT
    float width = rc.right - rc.left;
    float height = rc.bottom - rc.top;
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
    shaderList[1].UseShader();
    glUniformMatrix4fv(glGetUniformLocation(shaderList[1].GetShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Configure VAO/VBO for texture quads
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    RenderText(shaderList[1], "MODFLOW Model", width/2-width*0.12f, height*0.92f, width*0.0007f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shaderList[1], "Head (ft)", width*0.87f, height*0.78f, width*0.0005f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shaderList[1], "3396.0", width*0.905f, height*0.240f, width*0.00035f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shaderList[1], "3888.3", width*0.905f, height*0.407f, width*0.00035f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shaderList[1], "4380.7", width*0.905f, height*0.573f, width*0.00035f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(shaderList[1], "4873.0", width*0.905f, height*0.740f, width*0.00035f, glm::vec3(1.0f, 1.0f, 1.0f));
    glDisable(GL_BLEND);
#endif

    glUseProgram(0);
#endif
}

LRESULT CView::OnPaint(UINT /*uMsg*/, WPARAM hdc, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
/*
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);
  double deltaTime = ((double)(current.QuadPart - start.QuadPart) / (double)frequency.QuadPart);
  start = current;
  */
  CPaintDC dc(m_hWnd);
  dc.wglMakeCurrent(m_hRC);
  OnRender();
  dc.SwapBuffers();
  dc.wglMakeCurrent(NULL);
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

LRESULT CView::OnEraseBackground(HDC hdc)
{
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
      //pvtkDemo->OnTimer(m_hWnd, uTimerID);
      //pvtkDemo->StepObjects(m_stLastTime.wSecond);
      RedrawWindow();
    }
  }
}

void CView::StepDemo(int step)
{
  //pvtkDemo->OnTimer(m_hWnd, step);
  //pvtkDemo->StepObjects(m_stLastTime.wSecond);
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
  //pvtkDemo->OnLButtonDown(m_hWnd, uMsg, point);
}

void CView::OnLButtonDown(UINT uMsg, CPoint point)
{
  //		SetFocus();
  //		ATLTRACE("view::OnLButtonDown: %d, %d, %d\n", uMsg, point.x, point.y);
  //pvtkDemo->OnLButtonDown(m_hWnd, uMsg, point);
}

void CView::OnMButtonDown(UINT uMsg, CPoint point)
{
  //		SetFocus();
  //		ATLTRACE("view::OnMButtonDown: %d, %d, %d\n", uMsg, point.x, point.y);
  //pvtkDemo->OnMButtonDown(m_hWnd, uMsg, point);
}

void CView::OnRButtonDown(UINT uMsg, CPoint point)
{
  //		SetFocus();
  //		ATLTRACE("view::OnRButtonDown: %d, %d, %d\n", uMsg, point.x, point.y);
  //pvtkDemo->OnRButtonDown(m_hWnd, uMsg, point);
}

void CView::OnLButtonUp(UINT uMsg, CPoint point)
{
  //		ATLTRACE("view::OnLButtonUp: %d, %d, %d\n", uMsg, point.x, point.y);
  //pvtkDemo->OnLButtonUp(m_hWnd, uMsg, point);
}

void CView::OnMButtonUp(UINT uMsg, CPoint point)
{
  //		ATLTRACE("view::OnMButtonUp: %d, %d, %d\n", uMsg, point.x, point.y);
  //pvtkDemo->OnMButtonUp(m_hWnd, uMsg, point);
}

void CView::OnRButtonUp(UINT uMsg, CPoint point)
{
  //		ATLTRACE("view::OnRButtonUp: %d, %d, %d\n", uMsg, point.x, point.y);
  //pvtkDemo->OnRButtonUp(m_hWnd, uMsg, point);
}

void CView::OnMouseMove(UINT uMsg, CPoint point)
{
  //		ATLTRACE("view::OnMouseMove: %d, %d, %d\n", uMsg, point.x, point.y);
  //pvtkDemo->OnMouseMove(m_hWnd, uMsg, point);
}

LRESULT CView::OnChar(UINT uMsg, WPARAM charCode, LPARAM lParam, BOOL& bHandled)
{
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
  if (toupper(charCode) == 'W') {
    camera.keyControl(toupper(charCode));
  }
  else if (toupper(charCode) == 'S') {
    camera.keyControl(toupper(charCode));
  }
  else if (toupper(charCode) == 'A') {
    camera.keyControl(toupper(charCode));
  }
  else if (toupper(charCode) == 'D') {
    camera.keyControl(toupper(charCode));
  }
  else if (toupper(charCode) == 'T' || charCode == VK_UP) {
    camera.mouseControl(0, 10);
  }
  else if (toupper(charCode) == 'G' || charCode == VK_DOWN) {
    camera.mouseControl(0, -10);
  }
  else if (toupper(charCode) == 'F' || charCode == VK_LEFT) {
    camera.mouseControl(-10, 0);
  }
  else if (toupper(charCode) == 'H' || charCode == VK_RIGHT) {
    camera.mouseControl(10, 0);
  }
  return 0;
}

void CView::ResetModelData()
{
  
}

LRESULT CView::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: add code to initialize document
  //pvtkDemo->FileNew();
  return 0;
}

PHT3D_Model& CView::GetPHT3DM() {
  return mPHT3DM;
}

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
  // Activate corresponding render state	
  shaderList[1].UseShader();
  glUniform3f(glGetUniformLocation(shaderList[1].GetShaderID(), "textColor"), 
    color.x, color.y, color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  // Iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++)
  {
    Character ch = Characters[*c];

    GLfloat xpos = x + ch.Bearing.x * scale;
    GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    // Update VBO for each character
    GLfloat vertices[6][4] = {
      { xpos,     ypos + h,   0.0, 0.0 },
      { xpos,     ypos,       0.0, 1.0 },
      { xpos + w, ypos,       1.0, 1.0 },

      { xpos,     ypos + h,   0.0, 0.0 },
      { xpos + w, ypos,       1.0, 1.0 },
      { xpos + w, ypos + h,   1.0, 0.0 }
    };
    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

