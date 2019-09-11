// CPPMatrix2.h : interface of the CPPMatrix2 class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: A simple 2D Matrix class for 2D matrix operations
//   It is a template based 2D matrix constructed using vector STL
//
/////////////////////////////////////////////////////////////////////////////

#include <vector>

template <class T>
class CPPMatrix2
{
public:
  // Number of row and columns
  int nRows, nCols;
  vector<vector<T> > mVec;
  CPPMatrix2() : nRows(0), nCols(0)
  {
  }

  // Matrix constructor with number of rows and columns
  CPPMatrix2(int rows, int cols)
  {
    Resize(rows, cols);
  }

  ~CPPMatrix2()
  {
    clear();
  }

  // Resize the matrix
  void Resize(int rows, int cols)
  {
    nRows = rows;
    nCols = cols;
    mVec.resize(nRows);
    for (int i = 0; i < nRows; i++)
    {
      mVec[i].resize(nCols);
    }
  }

  // Clear the 2D matrix
  void clear()
  {
    mVec.clear();
    nRows = 0;
    nCols = 0;
  }

  CPPMatrix2(const CPPMatrix2<T> &M)
  {
    nRows = M.nRows;
    nCols = M.nCols;
    mVec = M.mVec;
  }

  // Copy constructor
  CPPMatrix2<T>& operator= (const CPPMatrix2<T> &M)
  {
    nRows = M.nRows;
    nCols = M.nCols;
    mVec = M.mVec;
    return *this;
  }

  vector<T>& operator[] (int row)
  {
    try
    {
      return mVec[row];
    }
    catch (...)
    {
      mVec[row].resize(row + 1);
      return mVec[row];
    }
  }

  void Dump(void)
  {
    cout << "\t[\n";
    for (int row = 0; row < nRows; row++)
    {
      for (int col = 0; col < nCols; col++)
        cout << "\t\t" << mVec[row][col] << " ";
      cout << "\n";
    }
    cout << "\t]\n";
  }

  vector<T> Row(int num)
  {
    vector<T> Result(nCols);
    for (int x = 0; x < nCols; x++)
      Result[x] = (mVec[num][x]);
    return Result;
  }

  vector<T> Col(int num)
  {
    vector<T> Result(nRows);
    for (int y = 0; y < nRows; y++)
      Result[y] = (mVec[y][num]);
    return Result;
  }

  CPPMatrix2<T> Transpose(void)
  {
    CPPMatrix2<T> M_Trans(nCols, nRows);
    for (int row = 0; row < M_Trans.Rows(); row++)
      for (int mrow = 0; mrow < nRows; mrow++)
        M_Trans[row][mrow] = mVec[mrow][row];
    return M_Trans;
  }

  // Scale the whole matrix
  CPPMatrix2<T> Scale(T factor)
  {
    CPPMatrix2 Result(nRows, nCols);
    for (int row = 0; row < Result.Rows(); row++)
      for (int col = 0; col < Result.Cols(); col++)
        Result[row][col] = factor*(mVec[row][col]);
    return Result;
  }

  // Add two matrices
  CPPMatrix2<T> operator+ (CPPMatrix2<T> &M)
  {
    CPPMatrix2<T> M2 = *this;
    CPPMatrix2<T> Result(M2.Rows(), M2.Cols());

    for (int row = 0; row < M2.Rows(); row++)
      for (int col = 0; col < M2.Cols(); col++)
        Result[row][col] = M2[row][col] + M[row][col];

    return Result;
  }

  // Subtract a matrix from the source matrix
  CPPMatrix2<T> operator- (CPPMatrix2<T> &M)
  {
    CPPMatrix2<T> M2 = *this;
    CPPMatrix2<T> Result(M2.Rows(), M2.Cols());

    for (int row = 0; row < M2.Rows(); row++)
      for (int col = 0; col < M2.Cols(); col++)
        Result[row][col] = M2[row][col] - M[row][col];

    return Result;
  }

  // Matrix multiplication. This is different from element-wise multiplication
  CPPMatrix2<T> operator* (CPPMatrix2<T> &M)
  {
    CPPMatrix2<T> M2 = *this;
    CPPMatrix2<T> Result(M2.Rows(), M.Cols());
    T Temp;

    for (int row = 0; row < Result.Rows(); row++)
      for (int col = 0; col < Result.Cols(); col++)
      {
        Temp = 0;
        vector<T> row_vector = M2.Row(row);
        vector<T> col_vector = M.Col(col);
        for (int x = 0; x < row_vector.size(); x++)
          Temp += (row_vector[x] * col_vector[x]);
        Result[row][col] = Temp;
      }

    return Result;
  }

  T CofactorSign(int row, int col)
  {
    return(pow(-1.0, row) * pow(-1.0, col));
  }

  CPPMatrix2<T> Minor(int row, int col)
  {
    CPPMatrix2<T> result((Rows() - 1), (Cols() - 1));
    int xpt = 0, ypt = 0;

    for (int y = 0; y < (Rows()); y++)
    {
      if (y != row)
      {
        for (int x = 0; x < (Cols()); x++)
        {
          if (x != col)
          {
            result[ypt][xpt] = m[y][x];
            xpt++;
          }
        }
        ypt++;
      }
      xpt = 0;
    }

    return result;
  }

  // Calculate the determinant of the 2D matrix
  T Det(void)
  {
    T result = 0; CPPMatrix2<T> Temp;

    if (Rows() == 2)
    {
      result = (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
    }
    else
    {
      for (int col = 0; col < Cols(); col++)
      {
        Temp = Minor(0, col);
        result += (Temp.Det() * m[0][col] * CofactorSign(0, col));
      }
    }
    return result;
  }
};
