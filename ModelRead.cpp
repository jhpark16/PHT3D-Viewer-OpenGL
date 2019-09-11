// ModuleRead.cpp : implmentation of various MODFLOW data classes
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: 
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

int Read_MODFLOW_Files(PHT3D_Model& mPHT3DM, CString sPlotDirectory)
{
  //PLOTClass& plot = mPHT3DM.PLOT;
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  //MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;
  //PHT3DClass& pht3d = mPHT3DM.PHT3D;

  TCHAR str[MAX_STR + 1], str2[MAX_STR + 1], tstr[MAX_STR + 1];
  CString tCstr, tCstr1, tCstr2;
  int iTmp, fh1, bytes_read, tIval, error_val = 0; //iTmp2, tIval2, 
  float min, max;
  //vector<float>::pointer ptr;
  FILE *fp1;
  iTmp = mf.NameFile.FindFileType(_T("LMT6"));
  if (iTmp >= 0)
  { // Read LMT6 package
    tCstr = mf.NameFile.FileName[iTmp];
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
    {
      MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0);
      //return 1;
    }
    else
    {
      do
      {
        _fgetts(tCstr.GetBuffer(MAX_STR), MAX_STR, fp1);
        tCstr.ReleaseBuffer();
        if (feof(fp1)) break;
        iTmp = _stscanf(tCstr, _T("%s %s"), tCstr1.GetBuffer(MAX_STR), tCstr2.GetBuffer(MAX_STR));
        tCstr1.ReleaseBuffer();
        tCstr2.ReleaseBuffer();
        if (iTmp == 2)

        {
          if (tCstr1[0] == _T('#'))
          {
          }
          else if (tCstr1.CompareNoCase(_T("OUTPUT_FILE_NAME")) == 0)
          {
            mf.LMT6_Name = tCstr2.Trim();
          }
          else if (tCstr1.CompareNoCase(_T("OUTPUT_FILE_UNIT")) == 0)
          {
            mf.LMT6_Unit = _ttoi(tCstr2);
          }
          else if (tCstr1.CompareNoCase(_T("OUTPUT_FILE_HEADER")) == 0)
          {
            mf.LMT6_Header = (tCstr2.CompareNoCase(_T("Standard")) == 0 ? 1 : 2);
          }
          else if (tCstr1.CompareNoCase(_T("OUTPUT_FILE_FORMAT")) == 0)
          {
            mf.LMT6_Format = (tCstr2.CompareNoCase(_T("Unformatted")) == 0 ? 1 : 2);
          }
        }
      } while (1);
      fclose(fp1);
    }
  }
  // 1. Read MODFLOW discretization file
  // 2. Read MODFLOW BAS package file
  // 3. Read MODFLOW output control file
  // 4. Read MODFLOW zone and multiplier file
  // 5. Read MODFLOW parameter values
  // 6. Read MODFLOW BCF
  // 7. Read MODFLOW LPF
  // 8. Read MODFLOW HUF
  // 9. Read MODFLOW WEL (each step 1)
  // 10. Read MODFLOW DRN (each step 2)
  // 11. Read MODFLOW RIV (each step 3)
  // 12. Read MODFLOW GHB (each step 5)
  // 13. Read MODFLOW STR
  // 14. Read MODFLOW CHD
  // read in each step
  // 15. Read MODFLOW EVT (each step 4)
  // 16. Read MODFLOW RCH (each step 6)
  // 17. Read MODFLOW FHB
  // 18. Read MODFLOW RES
  // 19. Read MODFLOW IBS
  // 20. Read MODFLOW HFB
  // 21. Read MODFLOW SFR
  iTmp = mf.NameFile.FindFileType(_T("DIS"));
  if (iTmp < 0)
  {
    MessageBox(NULL, _T("Application does not support modflow 96 or prior models"), 0, 0);
    return error_val | 16;;
  }
  else
  {
    tCstr = mf.NameFile.FileName[iTmp];
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
    {
      MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0); return -1;
    }
    else
    {
      _fgetts(str, MAX_STR, fp1);
      if (str[0] == _T('#'))
      {
        mf.DIS_HEADINGS[0] = str;
        _fgetts(str, MAX_STR, fp1);
        if (str[0] == _T('#'))
        {
          mf.DIS_HEADINGS[1] = str;
          do
          {
            _fgetts(str, MAX_STR, fp1);
          } while (str[0] == _T('#') && !feof(fp1));
        }
      }
      iTmp = _stscanf(str, _T("%d %d %d %d %d %d"), &mf.DIS_NLAY, &mf.DIS_NROW,
        &mf.DIS_NCOL, &mf.DIS_NPER, &mf.DIS_ITMUNI, &mf.DIS_LENUNI);
      if (iTmp == 6)
      {
        mf.DIS_LAYCBD.resize(mf.DIS_NLAY);
        for (int i = 0; i < mf.DIS_NLAY; i++)
        {
          _ftscanf(fp1, _T("%d"), &(mf.DIS_LAYCBD[i]));
        }
        _fgetts(str, MAX_STR, fp1); // remove linefeed
        mf.DIS_DELR.resize(mf.DIS_NCOL);
        ReadMODFLOWArray(fp1, &(mf.DIS_DELR), false, false, false);
        mf.DIS_DELC.resize(mf.DIS_NROW);
        ReadMODFLOWArray(fp1, &(mf.DIS_DELC), false, false, false);
        mf.DIS_TOP.Resize(mf.DIS_NROW, mf.DIS_NCOL);
        ReadMODFLOWArray(fp1, &(mf.DIS_TOP), true, false, false);
        mf.DIS_BOTMS.resize(mf.DIS_NLAY);
        for (int k = 0; k < mf.DIS_NLAY; k++)
        {
          mf.DIS_BOTMS[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
          ReadMODFLOWArray(fp1, &(mf.DIS_BOTMS[k]), true, false, false);
        }
        mf.DIS_STRESS_PERIOD.resize(max(mf.DIS_NPER, 1));
        if (mf.DIS_NPER == 0)
        {
          mf.DIS_STRESS_PERIOD[0].NSTP = 1;
          mf.DIS_STRESS_PERIOD[0].PERLEN = 0.0;
        }
        else
        {
          mf.BCF_bTransient = false;
          for (int k = 0; k < mf.DIS_NPER; k++)
          {
            _fgetts(str, MAX_STR, fp1);
            _stscanf(str, _T("%lf %d %lf %s"), &(mf.DIS_STRESS_PERIOD[k].PERLEN),
              &(mf.DIS_STRESS_PERIOD[k].NSTP), &(mf.DIS_STRESS_PERIOD[k].TSMULT), &tstr);
            if ((tstr[0] | 0x20) == _T('s')) mf.DIS_STRESS_PERIOD[k].bSteadyState = true;
            else if ((tstr[0] | 0x20) == _T('t'))
            {
              mf.DIS_STRESS_PERIOD[k].bSteadyState = false;
              mf.BCF_bTransient = true;
            }
            else mf.DIS_STRESS_PERIOD[k].bSteadyState = true;
          }
        }
      }
      fclose(fp1);
    }
  }
  // calculate total number of steps;
  mf.nTotalSteps = 0;
  int k = 0;
  double tTime = 0.0;
  for (int i = 0; i < mf.DIS_NPER; i++)
  {
    mf.nTotalSteps += mf.DIS_STRESS_PERIOD[i].NSTP;
    mf.Time_Period.reserve((int)(65000 + mf.nTotalSteps / 65000));
    mf.Time_Period.resize(mf.nTotalSteps);
    mf.Time_Period_Step.reserve((int)(65000 + mf.nTotalSteps / 65000));
    mf.Time_Period_Step.resize(mf.nTotalSteps);
    mf.Times_in_Period.reserve((int)(65000 + mf.nTotalSteps / 65000));
    mf.Times_in_Period.resize(mf.nTotalSteps);
    mf.Times.reserve((int)(65000 + mf.nTotalSteps / 65000));
    mf.Times.resize(mf.nTotalSteps);
    for (int j = 0; j < mf.DIS_STRESS_PERIOD[i].NSTP; j++, k++)
    {
      mf.Time_Period[k] = i + 1;
      mf.Time_Period_Step[k] = j + 1;
      mf.Times_in_Period[k] = (j + 1)*mf.DIS_STRESS_PERIOD[i].PERLEN / mf.DIS_STRESS_PERIOD[i].NSTP;
      mf.Times[k] = tTime + mf.Times_in_Period[k];
    }
    tTime += mf.DIS_STRESS_PERIOD[i].PERLEN;
  }
  // read basic package
  iTmp = mf.NameFile.FindFileType(_T("BAS6"));
  if (iTmp >= 0)
  {
    tCstr = mf.NameFile.FileName[iTmp];
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
    {
      MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0); return error_val | 8;
    }
    else
    {
      int idx = mf.AddData(_T("IBOUND"));
      //int iTmp2 = mf.Data.size();
      //mf.Data[iTmp2].Title = _T("IBOUND");
      //mf.Data.resize(iTmp2+1);
      //mf.Data_Pointer.resize(iTmp2+1);
      //mf.Data_Type.resize(iTmp2+1);
      //mf.Data_Max.resize(iTmp2+1);
      //mf.Data_Min.resize(iTmp2+1);
      mf.Data[idx].Max = 1.0;
      mf.Data[idx].Min = -1.0;
      mf.Data[idx].Data_Pointer = (void*)&mf.BAS_IBOUND;
      mf.Data[idx].Data_Type = 1; // Integer layer, row, column type
      _fgetts(str, MAX_STR, fp1);
      if (str[0] == _T('#'))
      {
        mf.BAS_HEADINGS[0] = str;
        _fgetts(str, MAX_STR, fp1);
        if (str[0] == _T('#'))
        {
          mf.BAS_HEADINGS[1] = str;
          _fgetts(str, MAX_STR, fp1);
        }
      }
      tCstr = str;
      tCstr.Trim();
      tCstr.MakeUpper();
      mf.BAS_OPTIONS_XSECTION = false;
      mf.BAS_OPTIONS_CHTOCH = false;
      mf.BAS_OPTIONS_FREE = false;
      if (tCstr.CompareNoCase(_T("XSECTION")) == 0)
      {
        mf.BAS_OPTIONS_XSECTION = true;
      }
      if (tCstr.CompareNoCase(_T("CHTOCH")) == 0)
      {
        mf.BAS_OPTIONS_CHTOCH = true;
      }
      if (tCstr.CompareNoCase(_T("FREE")) == 0)
      {
        mf.BAS_OPTIONS_FREE = true;
      }
      mf.BAS_IBOUND.resize(mf.DIS_NLAY);
      for (int k = 0; k < mf.DIS_NLAY; k++)
      {
        mf.BAS_IBOUND[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
        if (mf.BAS_OPTIONS_XSECTION)
        {
          ReadMODFLOWArray(fp1, &(mf.BAS_IBOUND[k][0]), false, true, mf.BAS_OPTIONS_FREE);
        }
        else
        {
          ReadMODFLOWArray(fp1, &(mf.BAS_IBOUND[k]), true, true, mf.BAS_OPTIONS_FREE);
        }
      }
      _fgetts(str, MAX_STR, fp1);
      min = 1e30f; max = -1e30f;
      _stscanf(str, _T("%f"), &(mf.BAS_HNOFLO));
      mf.BAS_STRT.resize(mf.DIS_NLAY);
      idx = mf.AddData(_T("Head(start)"));
      //iTmp2 = mf.Data.size();
      //mf.Data.resize(iTmp2+1);
      //mf.Data[iTmp2].Title = _T("Head(start)");
      //mf.Data_Pointer.resize(iTmp2+1);
      //mf.Data_Type.resize(iTmp2+1);
      //mf.Data_Max.resize(iTmp2+1);
      //mf.Data_Min.resize(iTmp2+1);
      mf.Data[idx].Data_Pointer = (void*)&mf.BAS_STRT;
      mf.Data[idx].Data_Type = 5; // Double layer, row, column type
      for (int k = 0; k < mf.DIS_NLAY; k++)
      {
        mf.BAS_STRT[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
        if (mf.BAS_OPTIONS_XSECTION)
        {
          ReadMODFLOWArray(fp1, &(mf.BAS_STRT[k][0]), false, false, mf.BAS_OPTIONS_FREE);
        }
        else
        {
          ReadMODFLOWArray(fp1, &(mf.BAS_STRT[k]), true, false, mf.BAS_OPTIONS_FREE);
        }
        for (int j = 0; j < mf.DIS_NROW; j++)
        {
          for (int i = 0; i < mf.DIS_NCOL; i++)
          {
            if (mf.BAS_IBOUND[k][j][i]) {
              double tval = mf.BAS_STRT[k][j][i];
              if (tval > max) max = tval;
              if (tval < min) min = tval;
            }
            else
              mf.BAS_STRT[k][j][i] = mf.BAS_HNOFLO;
          }
        }
      }
      mf.Data[idx].Max = max;
      mf.Data[idx].Min = min;
      fclose(fp1);
    }
  }
  // read bcf6 package
  iTmp = mf.NameFile.FindFileType(_T("BCF6"));
  if (iTmp >= 0)
  {
    tCstr = mf.NameFile.FileName[iTmp];
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
    {
      MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0); return error_val | 16;
    }
    else
    {
      _fgetts(str, MAX_STR, fp1);
      if (str[0] == _T('#'))
      {
        mf.BAS_HEADINGS[0] = str;
        _fgetts(str, MAX_STR, fp1);
        if (str[0] == _T('#'))
        {
          mf.BAS_HEADINGS[1] = str;
          _fgetts(str, MAX_STR, fp1);
        }
      }
      tCstr = str;
      if (1 || mf.BAS_OPTIONS_FREE)
      {
        _stscanf(str, _T("%d %f %d %lf %d %d"), &(mf.BCF_IBCFCB),
          &(mf.BCF_HDRY), &(mf.BCF_IWDFLG), &(mf.BCF_WETFCT),
          &(mf.BCF_IWETIT), &(mf.BCF_IHDWET));
        mf.BCF_Ltype.resize(mf.DIS_NLAY);
        for (int i = 0; i < mf.DIS_NLAY; i++)
        {
          _ftscanf(fp1, _T("%d"), &(mf.BCF_Ltype[i]));
        }
        _fgetts(str, MAX_STR, fp1);
        mf.BCF_TRPY.resize(mf.DIS_NLAY);
        ReadMODFLOWArray(fp1, &(mf.BCF_TRPY), false, false, mf.BAS_OPTIONS_FREE);
        mf.BCF_Sf1.resize(mf.DIS_NLAY);
        mf.BCF_HY.resize(mf.DIS_NLAY);
        mf.BCF_Vcont.resize(mf.DIS_NLAY);
        mf.BCF_Sf2.resize(mf.DIS_NLAY);
        mf.BCF_WETDRY.resize(mf.DIS_NLAY);
        min = 1e30f; max = -1e30f;
        for (int k = 0; k < mf.DIS_NLAY; k++)
        {
          if (mf.BCF_bTransient)
          {
            mf.BCF_Sf1[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
            ReadMODFLOWArray(fp1, &(mf.BCF_Sf1[k]), true, false, mf.BAS_OPTIONS_FREE);
          }
          mf.BCF_HY[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
          ReadMODFLOWArray(fp1, &(mf.BCF_HY[k]), true, false, mf.BAS_OPTIONS_FREE);
          mf.BCF_Vcont[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
          if (k < mf.DIS_NLAY - 1)
            ReadMODFLOWArray(fp1, &(mf.BCF_Vcont[k]), true, false, mf.BAS_OPTIONS_FREE);
          if (mf.BCF_bTransient && (mf.BCF_Ltype[k] == 2 || mf.BCF_Ltype[k] == 3))
          {
            mf.BCF_Sf2[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
            ReadMODFLOWArray(fp1, &(mf.BCF_Sf2[k]), true, false, mf.BAS_OPTIONS_FREE);
          }
          if (mf.BCF_IWDFLG && (mf.BCF_Ltype[k] == 1 || mf.BCF_Ltype[k] == 3))
          {
            mf.BCF_WETDRY[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
            ReadMODFLOWArray(fp1, &(mf.BCF_WETDRY[k]), true, false, mf.BAS_OPTIONS_FREE);
          }
          double thickness;
          for (int j = 0; j < mf.DIS_NROW; j++)
            for (int i = 0; i < mf.DIS_NCOL; i++)
            {
              if (k == 0)
                thickness = (mf.DIS_TOP[j][i] - mf.DIS_BOTMS[k][j][i]);
              else
                thickness = (mf.DIS_BOTMS[k - 1][j][i] - mf.DIS_BOTMS[k][j][i]);
              if (mf.BCF_Ltype[k] == 0 || mf.BCF_Ltype[k] == 2)
                mf.BCF_HY[k][j][i] /= thickness;
              if (k == mf.DIS_NLAY - 1)
                mf.BCF_Vcont[k][j][i] = mf.INACTIVECELL;
              else
                mf.BCF_Vcont[k][j][i] *= thickness;
            }
        }
        for (int k = 0; k < mf.DIS_NLAY; k++)
          for (int j = 0; j < mf.DIS_NROW; j++)
            for (int i = 0; i < mf.DIS_NCOL; i++)
            {
              double tval = mf.BCF_HY[k][j][i];
              mf.BCF_HY[k][j][i] = log10(tval);
              tval = mf.BCF_Vcont[k][j][i];
              if (tval != mf.INACTIVECELL)
                mf.BCF_Vcont[k][j][i] = log10(tval);
            }
      }
      else
      {
      }
      fclose(fp1);
      int idx = mf.AddData(_T("Horz Log K"));
      mf.Data[idx].Data_Pointer = (void*)&mf.BCF_HY;
      mf.Data[idx].Data_Type = 5; // Double Layer Row Col 
      idx = mf.AddData(_T("Vert Log K"));
      mf.Data[idx].Data_Pointer = (void*)&mf.BCF_Vcont;
      mf.Data[idx].Data_Type = 5; // Double Layer Row Col 
    }
  }
  // read lpf package (needs work)
  iTmp = mf.NameFile.FindFileType(_T("LPF"));
  if (iTmp >= 0)
  {
    tCstr = mf.NameFile.FileName[iTmp];
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
    {
      MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0); return error_val | 16;
    }
    else
    {
      _fgetts(str, MAX_STR, fp1);
      if (str[0] == _T('#'))
      {
        mf.BAS_HEADINGS[0] = str;
        _fgetts(str, MAX_STR, fp1);
        if (str[0] == _T('#'))
        {
          mf.BAS_HEADINGS[1] = str;
          _fgetts(str, MAX_STR, fp1);
        }
      }
      tCstr = str;
      if (1 || mf.BAS_OPTIONS_FREE)
      {
        _stscanf(str, _T("%d %f %d %lf %d %d"), &(mf.BCF_IBCFCB),
          &(mf.BCF_HDRY), &(mf.BCF_IWDFLG), &(mf.BCF_WETFCT),
          &(mf.BCF_IWETIT), &(mf.BCF_IHDWET));
        mf.BCF_Ltype.resize(mf.DIS_NLAY);
        for (int i = 0; i < mf.DIS_NLAY; i++)
        {
          _ftscanf(fp1, _T("%d"), &(mf.BCF_Ltype[i]));
        }
        _fgetts(str, MAX_STR, fp1);
      }
      else
      {
      }
      fclose(fp1);
    }
  }
  // Read LMT4 Files after determining the number of total steps
  if (!mf.LMT6_Name.IsEmpty())
  {
    Read_LKMT_Package(mPHT3DM, sPlotDirectory);
    //mf.bFortranBinary = true;
  }

  // Read hydraulic head.dat
  //iTmp = mf.NameFile.FindFileType(_T("XXX"));
  //if(iTmp>=0)
  //{
  //}
  //mf.Heads.resize(mf.nTotalSteps);
  //mf.Heads_Info.resize(mf.nTotalSteps);
  //for(int l=0;l<mf.nTotalSteps;l++)
  //{
  //mf.Heads[l].resize(mf.DIS_NLAY);
  //for(int k=0;k<mf.DIS_NLAY;k++)
  //{
  //mf.Heads[l][k].Resize(mf.DIS_NROW,mf.DIS_NCOL);
  //}
  //}
  //tCstr = _T("heads.dat");
  iTmp = mf.NameFile.FindFileType(_T("Head"));
  if (iTmp >= 0)
  {
    tCstr = mf.NameFile.FileName[iTmp];
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fh1 = _topen(sPlotDirectory, _O_RDONLY | _O_BINARY | _O_SEQUENTIAL)) == -1)
    {
      mf.bHeadsData = false;
    }
    else
    {
      //MODFLOW_HYDRAULIC_HEAD_Head *pHEADHead;
      bool bContinue = true;
      //double tval;
      min = 1e30f; max = -1e30f;
      int l = 0, nStrides = 0;
      bytes_read = _read(fh1, &tIval, sizeof(int));
      if (tIval == 0x2c)
      {
        mf.bFortranBinary = true;
      }
      _lseeki64(fh1, 0, SEEK_SET);
      mf.bHeadsData = true;
      int idx = mf.AddData(_T("Heads"));
      mf.Data[idx].bHeadPerLayer = true;
      mf.Data[idx].bColumnRead = false;
      mf.Data[idx].FileName = tCstr;
      mf.Data[idx].nHeadBytes = sizeof(MODFLOW_HYDRAULIC_HEAD_Head);
      mf.Data[idx].nStrideBytes = sizeof(float)*mf.DIS_NROW*mf.DIS_NCOL;
      _int64 nTimeStepBytes = (_int64)mf.DIS_NLAY*((_int64)mf.Data[idx].nHeadBytes + (_int64)mf.Data[idx].nStrideBytes);
      if (mf.bFortranBinary)
      {
        nTimeStepBytes += mf.DIS_NLAY*(sizeof(int) * 4);
      }
      mf.Data[idx].iSeekHeadOffset.resize(mf.nTotalSteps);
      mf.Data[idx].iSeekDataOffset.resize(mf.nTotalSteps);
      for (int j = 0; j < mf.nTotalSteps; j++)
      {
        mf.Data[idx].iSeekHeadOffset[j] = j*nTimeStepBytes + 1;
        mf.Data[idx].iSeekDataOffset[j] = j*nTimeStepBytes + 1 + sizeof(MODFLOW_HYDRAULIC_HEAD_Head);
        if (mf.bFortranBinary) mf.Data[idx].iSeekDataOffset[j] += sizeof(int) * 2;
      }
      mf.Data[idx].Data_Pointer = NULL;
      mf.Data[idx].Data_Type = 0;
      //mf.Data[idx].Min = min;
      //mf.Data[idx].Max = max;
      _close(fh1);
    }
  }
  // Read drawdown.dat
  //tCstr = _T("ddown.dat");
  iTmp = mf.NameFile.FindFileType(_T("Drawdown"));
  if (iTmp >= 0)
  {
    tCstr = mf.NameFile.FileName[iTmp];
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fh1 = _topen(sPlotDirectory, _O_RDONLY | _O_BINARY | _O_SEQUENTIAL)) == -1)
    {
      //MessageBox(_T("Can't open ")+tCstr); 
      mf.bDrawDownData = false;
    }
    else
    {
      //MODFLOW_HYDRAULIC_HEAD_Head *pHEADHead;
      bool bContinue = true;
      //double tval;
      min = 1e30f; max = -1e30f;
      _int64 l = 0, nStrides = 0;
      bytes_read = _read(fh1, &tIval, sizeof(int));
      if (tIval == 0x2c)
      {
        mf.bFortranBinary = true;
      }
      _lseeki64(fh1, 0, SEEK_SET);
      _close(fh1);
      int idx = mf.AddData(_T("Drawdown"));
      mf.Data[idx].bHeadPerLayer = true;
      mf.Data[idx].bColumnRead = false;
      mf.Data[idx].FileName = tCstr;
      mf.Data[idx].nHeadBytes = sizeof(MODFLOW_HYDRAULIC_HEAD_Head);
      mf.Data[idx].nStrideBytes = sizeof(float)*mf.DIS_NROW*mf.DIS_NCOL;
      _int64 nTimeStepBytes = (_int64)mf.DIS_NLAY*((_int64)mf.Data[idx].nHeadBytes + (_int64)mf.Data[idx].nStrideBytes);
      if (mf.bFortranBinary)
      {
        nTimeStepBytes += mf.DIS_NLAY*(sizeof(int) * 4);
      }
      mf.Data[idx].iSeekHeadOffset.resize(mf.nTotalSteps);
      mf.Data[idx].iSeekDataOffset.resize(mf.nTotalSteps);
      for (int j = 0; j < mf.nTotalSteps; j++)
      {
        mf.Data[idx].iSeekHeadOffset[j] = j*nTimeStepBytes + 1;
        mf.Data[idx].iSeekDataOffset[j] = j*nTimeStepBytes + 1 + sizeof(MODFLOW_HYDRAULIC_HEAD_Head);
        if (mf.bFortranBinary) mf.Data[idx].iSeekDataOffset[j] += sizeof(int) * 2;
      }
      mf.Data[idx].Data_Pointer = NULL;
      mf.Data[idx].Data_Type = 0;
      //mf.Data[idx].Min = min;
      //mf.Data[idx].Max = max;
      mf.bDrawDownData = true;
    }
  }
  // Read budget.dat
  iTmp = mf.NameFile.FindFileType(_T("BAS6"));
  if (iTmp >= 0)
  {
  }
  //tCstr = _T("budget.dat");
  iTmp = mf.NameFile.FindFileType(_T("Budget"));
  if (iTmp >= 0)
  {
    tCstr = mf.NameFile.FileName[iTmp];
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fh1 = _topen(sPlotDirectory, _O_RDONLY | _O_BINARY | _O_SEQUENTIAL)) == -1)
    {
      //MessageBox(_T("Can't open ")+tCstr); 
      mf.bBudgetData = false;
    }
    else
    {
      MODFLOW_BUDGET_Head *pBUDGET_Head;
      bool bContinue = true, bNextRead = true;
      //double tval;
      _int64 nTimeStepBytes = 0;
      _int64 nStrideBytes = 0;
      _int64 iOffset;
      //int l;//, nBudget=30;
#define MAX_BUDGET_DATA 20
      //    mf.Budget_Info.resize(MAX_BUDGET_DATA);
      vector<double> minArray, maxArray;
      //    mf.Budget.resize(mf.nTotalSteps);
      mf.Budget_Start_Index = mf.Data.size();
      //    iTmp = mf.Budget_Info.size();
      //    for(int m=0;m<iTmp;m++)
      //    {
      //      minArray[m] = 1e30;
      //      maxArray[m] = -1e30;
      //    }
      _int64 nHeadBytes = sizeof(MODFLOW_BUDGET_Head);
      nStrideBytes = sizeof(float)*mf.DIS_NLAY*mf.DIS_NROW*mf.DIS_NCOL;
      pBUDGET_Head = (MODFLOW_BUDGET_Head*)str;
      /*        for(int l=0;l<mf.nTotalSteps;l++)
      {

      }*/
      bytes_read = _read(fh1, &tIval, sizeof(int));
      if (tIval == 0x24)
      {
        mf.bFortranBinary = true;
      }
      _lseeki64(fh1, 0, SEEK_SET);
      for (int m = 0; 1; m++)
      {
        iOffset = _telli64(fh1);
        if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
        bytes_read = _read(fh1, str, sizeof(MODFLOW_BUDGET_Head));
        if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
        int bEof = eof(fh1);
        if (bEof || bytes_read != sizeof(MODFLOW_BUDGET_Head))
        {
          break;
        }
        if (pBUDGET_Head->KPER < 0)
        {
          MessageBox(NULL, _T("KPER is less than zero"), 0, 0);
          break;
        }
        if (pBUDGET_Head->KSTP < 0)
        {
          MessageBox(NULL, _T("KSTP is less than zero"), 0, 0);
          break;
        }
        if (pBUDGET_Head->NCOL < 0)
        {
          MessageBox(NULL, _T("NCOL is less than zero"), 0, 0);
          break;
        }
        if (pBUDGET_Head->NROW < 0)
        {
          MessageBox(NULL, _T("NROW is less than zero"), 0, 0);
          break;
        }
        if (pBUDGET_Head->NLAY < 0)
        {
          int nList;
          if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
          bytes_read = _read(fh1, str2, sizeof(MODFLOW_BUDGET_IBD2_Head));
          if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
          MODFLOW_BUDGET_IBD2_Head *pBudget2 = (MODFLOW_BUDGET_IBD2_Head*)str2;
          if (pBudget2->TYPE == 1)
          {
            int idx2 = mf.GetTimeStep(pBUDGET_Head->KPER, pBUDGET_Head->KSTP);
            if (idx2 < 0) MessageBox(NULL, _T("Can't find the budget time step"), 0, 0);
            if (mf.bFortranBinary)
              _lseeki64(fh1, nStrideBytes + sizeof(int) * 2, SEEK_CUR);
            else
              _lseeki64(fh1, nStrideBytes, SEEK_CUR);
            tCstr1 = CString(pBUDGET_Head->TEXT, 16).Trim();
            int idx = mf.AddBudget(tCstr1);
            idx = mf.AddData(_T("Bgt-") + tCstr1);
            mf.Data[idx].IBDType = 2;
            mf.Data[idx].FileName = tCstr;
            mf.Data[idx].nHeadBytes = nHeadBytes;
            mf.Data[idx].nStrideBytes = nStrideBytes;
            mf.Data[idx].bHeadPerLayer = false;
            mf.Data[idx].bColumnRead = false;
            mf.Data[idx].iSeekHeadOffset.resize(mf.nTotalSteps);
            mf.Data[idx].iSeekDataOffset.resize(mf.nTotalSteps);
            mf.Data[idx].iSeekHeadOffset[idx2] = iOffset + 1;
            mf.Data[idx].iSeekDataOffset[idx2] = iOffset + 1 +
              sizeof(MODFLOW_BUDGET_Head) + sizeof(MODFLOW_BUDGET_IBD2_Head);
            if (mf.bFortranBinary) mf.Data[idx].iSeekDataOffset[idx2] += sizeof(int) * 4;
            mf.Data[idx].Data_Pointer = NULL;
            mf.Data[idx].Data_Type = 0;
          }
          else if (pBudget2->TYPE == 2)
          {
            if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
            bytes_read = _read(fh1, &nList, sizeof(int));
            if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
            if (mf.bFortranBinary)
              _lseeki64(fh1, nList*(sizeof(int) + sizeof(float) + sizeof(int) * 2), SEEK_CUR);
            else
              _lseeki64(fh1, nList*(sizeof(int) + sizeof(float)), SEEK_CUR);
          }
          else if (pBudget2->TYPE == 3)
          {
            if (mf.bFortranBinary)
              _lseeki64(fh1, sizeof(float)*mf.DIS_NROW + sizeof(int) * 2, SEEK_CUR);
            else
              _lseeki64(fh1, sizeof(float)*mf.DIS_NROW, SEEK_CUR);
          }
          else if (pBudget2->TYPE == 4)
          {
            if (mf.bFortranBinary)
              _lseeki64(fh1, sizeof(float)*mf.DIS_NROW*mf.DIS_NCOL + sizeof(int) * 2, SEEK_CUR);
            else
              _lseeki64(fh1, sizeof(float)*mf.DIS_NROW*mf.DIS_NCOL, SEEK_CUR);
          }
          else if (pBudget2->TYPE == 5)
          {
            if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
            bytes_read = _read(fh1, &nList, sizeof(int));
            if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
            if (mf.bFortranBinary)
              _lseeki64(fh1, (nList - 1) * sizeof(char) * 16 + sizeof(int) * 3, SEEK_CUR); // Skip NLIST and fortran binary
            else
              _lseeki64(fh1, (nList - 1) * sizeof(char) * 16 + sizeof(int), SEEK_CUR); // Skip NLIST
          }
          else
          {
            MessageBox(NULL, _T("Unsupported budget binary format: Contact the author"), 0, 0);
          }
        }
        else
        {
          int idx2 = mf.GetTimeStep(pBUDGET_Head->KPER, pBUDGET_Head->KSTP);
          if (idx2 < 0) MessageBox(NULL, _T("Can't find the budget time step"), 0, 0);
          if (mf.bFortranBinary)
            _lseeki64(fh1, nStrideBytes + sizeof(int) * 2, SEEK_CUR);
          else
            _lseeki64(fh1, nStrideBytes, SEEK_CUR);
          tCstr1 = CString(pBUDGET_Head->TEXT, 16).Trim();
          int idx = mf.AddBudget(tCstr1);
          idx = mf.AddData(_T("Bgt-") + tCstr1);
          mf.Data[idx].IBDType = 0;
          mf.Data[idx].FileName = tCstr;
          mf.Data[idx].nHeadBytes = nHeadBytes;
          mf.Data[idx].nStrideBytes = nStrideBytes;
          mf.Data[idx].bHeadPerLayer = false;
          mf.Data[idx].bColumnRead = false;
          mf.Data[idx].iSeekHeadOffset.resize(mf.nTotalSteps);
          mf.Data[idx].iSeekDataOffset.resize(mf.nTotalSteps);
          mf.Data[idx].iSeekHeadOffset[idx2] = iOffset + 1;
          mf.Data[idx].iSeekDataOffset[idx2] = iOffset + 1 +
            sizeof(MODFLOW_BUDGET_Head);
          if (mf.bFortranBinary) mf.Data[idx].iSeekDataOffset[idx2] += sizeof(int) * 2;
          mf.Data[idx].Data_Pointer = NULL;
          mf.Data[idx].Data_Type = 0;
        }
      }
      /*
      for(int m=0;m<nBudget;m++)
      {
      if(mf.bFortranBinary)
      {
      mf.Data[mf.Budget_Start_Index+m].nTimeStepBytes =
      nBudget*(sizeof(int)*4+mf.Data[mf.Budget_Start_Index+m].nStrideBytes+mf.Data[mf.Budget_Start_Index+m].nHeadBytes);
      }
      else
      {
      mf.Data[mf.Budget_Start_Index+m].nTimeStepBytes =
      nBudget*(mf.Data[mf.Budget_Start_Index+m].nStrideBytes+mf.Data[mf.Budget_Start_Index+m].nHeadBytes);
      }
      }*/
      _close(fh1);
      mf.bBudgetData = true;
    }
  }
  //bool bDataRead;
  //ArrayF3D *Data3D;
  //Data3D = new ArrayF3D(boost::extents[mf.DIS_NLAY][mf.DIS_NROW][mf.DIS_NCOL]);
  //UpdateData(Data3D, &min, &max, &bDataRead);
  float thickness1, thickness2, dx1, dx2;
  mf.AreaRight.resize(mf.DIS_NLAY);
  for (int k = 0; k < mf.DIS_NLAY; k++)
  {
    mf.AreaRight[k].Resize(mf.DIS_NROW, mf.DIS_NCOL);
    for (int j = 0; j < mf.DIS_NROW; j++)
    {
      for (int i = 0; i < mf.DIS_NCOL; i++)
      {
        dx1 = mf.DIS_DELR[i] / 2.0f;
        if (i < mf.DIS_NCOL - 1)
          dx2 = mf.DIS_DELR[i + 1] / 2.0f;
        else
          dx2 = dx1;
        if (k == 0)
        {
          thickness1 = mf.DIS_TOP[j][i] - mf.DIS_BOTMS[k][j][i];
          if (i < mf.DIS_NCOL - 1)
          {
            thickness2 = mf.DIS_TOP[j][i + 1] - mf.DIS_BOTMS[k][j][i + 1];
          }
          else
            thickness2 = thickness1;
        }
        else
        {
          thickness1 = mf.DIS_BOTMS[k - 1][j][i] - mf.DIS_BOTMS[k][j][i];
          if (i < mf.DIS_NCOL - 1)
          {
            thickness2 = mf.DIS_BOTMS[k - 1][j][i + 1] - mf.DIS_BOTMS[k][j][i + 1];
          }
          else
            thickness2 = thickness1;
        }
        mf.AreaRight[k][j][i] = mf.DIS_DELC[j] * (thickness1*dx2 + thickness2*dx1) / (dx1 + dx2);//mf.Data
      }
    }
  }
  //mf.Calced_Start_Index = mf.Data.size();
  int idx2 = mf.FindData(_T("LKMT-QXX"));
  if (idx2 >= 0)
  {
    int idx = mf.AddData(_T("Darcy Flux Right"));
    //mf.Data[mf.Calced_Start_Index].Title = _T("Darcy Flux Right");
    //mf.Data.resize(mf.Calced_Start_Index+1);
    mf.Data[idx].FileName = mf.Data[idx2].FileName;
    mf.Data[idx].nHeadBytes = mf.Data[idx2].nHeadBytes;
    mf.Data[idx].nStrideBytes = mf.Data[idx2].nStrideBytes;
    mf.Data[idx].bHeadPerLayer = mf.Data[idx2].bHeadPerLayer;
    mf.Data[idx].bColumnRead = mf.Data[idx2].bColumnRead;
    mf.Data[idx].iSeekHeadOffset.resize(mf.nTotalSteps);
    mf.Data[idx].iSeekDataOffset.resize(mf.nTotalSteps);
    for (int j = 0; j < mf.nTotalSteps; j++)
    {
      mf.Data[idx].iSeekHeadOffset[j] = mf.Data[idx2].iSeekHeadOffset[j];
      mf.Data[idx].iSeekDataOffset[j] = mf.Data[idx2].iSeekDataOffset[j];
    }
    mf.Data[idx].Data_Pointer = mf.Data[idx2].Data_Pointer;
    mf.Data[idx].Data_Type = mf.Data[idx2].Data_Type;
  }
  mPHT3DM.PLOT.ModelType |= 0x02; // solute (0x01), flow (0x02), pht3d (0x04) and mt3dms (0x08)
  return 0;
}

int Read_PHT3D_Model(PHT3D_Model& mPHT3DM, CString sPlotDirectory)
{
  //PLOTClass& plot = mPHT3DM.PLOT;
  //MODFLOWClass& mf = mPHT3DM.MODFLOW;
  //MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;
  PHT3DClass& pht3d = mPHT3DM.PHT3D;

  FILE *fp1;
  TCHAR str[MAX_STR + 1], tstr[MAX_STR + 1], tstr1[MAX_STR + 1], tstr2[MAX_STR + 1];
  int iTmp;
  CString tCstr, tCstr2;
  // Read pht3d_ph.dat
  tCstr = pht3d.PHC_FileName;
  iTmp = sPlotDirectory.ReverseFind(_T('\\'));
  sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
  sPlotDirectory += tCstr;
  if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
  {
    MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0);
    return -1;
  }
  else
  {
    // Delete database
    //int iLoc2;
    //tCstr = _T("  A    1010       10.0     ");
    //_stscanf(tCstr,_T("%10[\ +\-0-9]"),&iTmp);
    //xscanf(tCstr,_T("%5f %f"),&iTmp,&iLoc2);
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%d %d %d %lf %d %lf %lf %d"), &pht3d.OS, &pht3d.TMP_LOC, &pht3d.RED_MOD,
      &pht3d.TEMP, &(pht3d.ASBIN), &pht3d.EPS_AQU, &pht3d.EPS_PH, &pht3d.PACK_SZ);
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%lf"), &pht3d.CB_OFFSET);
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%d"), &pht3d.NR_SOL_MST_SPEC_EQU);
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%d"), &pht3d.NR_MIN_EQU);
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%d"), &pht3d.NR_ION_EX);// has an issue
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%d"), &pht3d.NR_SURF);
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%d %d %d %d"), &pht3d.NR_MOB_KIN,
      &pht3d.NR_MIN_KIN, &pht3d.NR_SURF_KIN, &pht3d.NR_IMOB_KIN);
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%d %d"), &pht3d.NR_OUTP_SPEC, &pht3d.PR_ALAKLINITY_FLAG);
    pht3d.Kinetic_Mobile.resize(pht3d.NR_MOB_KIN);
    for (int i = 0, n_param = 0; i < pht3d.NR_MOB_KIN; i++)
    {
      CKinetic_Mobile mobile_kinetic;
      _fgetts(str, MAX_STR, fp1);
      _stscanf(str, _T("%s %d"), tstr, &n_param);
      mobile_kinetic.Name = tstr;
      mobile_kinetic.parameter.resize(n_param);
      for (int j = 0; j < n_param; j++)
      {
        _fgetts(str, MAX_STR, fp1);
        _stscanf(str, _T("%lf"), &(mobile_kinetic.parameter[j]));
      }
      _fgetts(str, MAX_STR, fp1);
      mobile_kinetic.formula = str;
      pht3d.Kinetic_Mobile[i] = mobile_kinetic;
    }
    pht3d.Equilibrium_Component.resize(pht3d.NR_SOL_MST_SPEC_EQU);
    for (int i = 0; i < pht3d.NR_SOL_MST_SPEC_EQU; i++)
    {
      _fgetts(str, MAX_STR, fp1);
      pht3d.Equilibrium_Component[i] = str;
      pht3d.Equilibrium_Component[i].Trim();
    }
    pht3d.IMOB_KIN.resize(pht3d.NR_IMOB_KIN);
    for (int i = 0, n_param = 0; i < pht3d.NR_IMOB_KIN; i++)
    {
      CKinetic_Mobile mobile_kinetic;
      _fgetts(str, MAX_STR, fp1);
      _stscanf(str, _T("%s %d"), tstr, &n_param);
      mobile_kinetic.Name = tstr;
      mobile_kinetic.parameter.resize(n_param);
      for (int j = 0; j < n_param; j++)
      {
        _fgetts(str, MAX_STR, fp1);
        _stscanf(str, _T("%lf"), &(mobile_kinetic.parameter[j]));
      }
      _fgetts(str, MAX_STR, fp1);
      mobile_kinetic.formula = str;
      pht3d.IMOB_KIN[i] = mobile_kinetic;
    }
    pht3d.Equilibrium_Mineral.resize(pht3d.NR_MIN_EQU);
    for (int i = 0; i < pht3d.NR_MIN_EQU; i++)
    {
      _fgetts(str, MAX_STR, fp1);
      pht3d.Equilibrium_Mineral[i] = str;
      pht3d.Equilibrium_Mineral[i].Trim();
    }
    pht3d.Ion_Exchange.resize(pht3d.NR_ION_EX);
    for (int i = 0, n_stoic = 0; i < pht3d.NR_ION_EX; i++)
    {
      _fgetts(str, MAX_STR, fp1);
      iTmp = _stscanf(str, _T("%s %d %s"), tstr, &n_stoic, tstr1);
      pht3d.Ion_Exchange[i].Name = tstr;
      if (iTmp > 1) pht3d.Ion_Exchange[i].Stoic_coef = n_stoic;
      if (iTmp > 2) pht3d.Ion_Exchange[i].Name_EMS = tstr1;
    }
    if (pht3d.NR_SURF > 0)
    {
      pht3d.Surface_Complex.resize(pht3d.NR_SURF);
      for (int i = 0; i < pht3d.NR_SURF; i++)
      {
        _fgetts(str, MAX_STR, fp1);
        iTmp = _stscanf(str, _T("%s %lf %lf %s %s"), tstr, &(pht3d.Surface_Complex[i].SurfaceArea), &(pht3d.Surface_Complex[i].Mass),
          tstr1, tstr2);
        pht3d.Surface_Complex[i].Name = tstr;
        if (iTmp > 3) pht3d.Surface_Complex[i].Mineral_Name = tstr1;
        if (iTmp > 4) pht3d.Surface_Complex[i].SWITCH = tstr2;
        //_fgetts(str,MAX_STR,fp1);
      }
      _fgetts(str, MAX_STR, fp1);
      _stscanf(str, _T("%s"), pht3d.SURF_CALC_TYPE.GetBuffer(MAX_STR));
      pht3d.SURF_CALC_TYPE.ReleaseBuffer();
    }
    pht3d.Kinetic_Mineral.resize(pht3d.NR_MIN_KIN);
    for (int i = 0, n_param = 0; i < pht3d.NR_MIN_KIN; i++)
    {
      CKinetic_Mineral kinetic_mineral;
      _fgetts(str, MAX_STR, fp1);
      _stscanf(str, _T("%s %d"), tstr, &n_param);
      kinetic_mineral.Name = tstr;
      kinetic_mineral.parameter.resize(n_param);
      for (int j = 0; j < n_param; j++)
      {
        _fgetts(str, MAX_STR, fp1);
        _stscanf(str, _T("%lf"), &(kinetic_mineral.parameter[j]));
      }
      pht3d.Kinetic_Mineral[i] = kinetic_mineral;
    }
    /*pht3d_Output_Secondary_Species.resize(pht3d_NR_OUTP_SPEC);
    for(int i=0;i<pht3d_NR_OUTP_SPEC;i++)
    {
    _fgetts(str,MAX_STR,fp1);
    pht3d_Output_Secondary_Species[i] = str;
    pht3d_Output_Secondary_Species[i].Trim();
    }*/
    fclose(fp1);
  }
  mPHT3DM.PLOT.ModelType |= 0x05; // solute (0x01), flow (0x02), pht3d (0x04) and mt3dms (0x08)
  return 0;
}

int Read_MT3DMS_Model(PHT3D_Model& mPHT3DM, CString sPlotDirectory)
{
  PLOTClass& plot = mPHT3DM.PLOT;
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;

  FILE *fp1;
  TCHAR str[MAX_STR + 1];
  int iTmp, fh1, l, bytes_read;
  CString tCstr, tCstr2;
  double min, max;
  //vector<float>::pointer ptr;
  // Read Basic Transport Package pht3dbtn.dat
  tCstr = mt3dms.BTN_FileName;
  iTmp = sPlotDirectory.ReverseFind(_T('\\'));
  sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
  sPlotDirectory += tCstr;
  if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
  {
    MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0);
    return -1;
  }
  else
  {
    _fgetts(str, MAX_STR, fp1);
    mt3dms.BTN_HEADING[0] = str;
    _fgetts(str, MAX_STR, fp1);
    mt3dms.BTN_HEADING[1] = str;
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%10d%10d%10d%10d%10d%10d"), &mt3dms.BTN_NLAY,
      &mt3dms.BTN_NROW, &mt3dms.BTN_NCOL, &mt3dms.BTN_NPER, &mt3dms.BTN_NCOMP,
      &mt3dms.BTN_MCOMP);
    //    xscanf(str,_T("%10d %10d %10d %10d %10d %10d"),&mt3dms.BTN_NLAY,
    //      &mt3dms.BTN_NROW,&mt3dms.BTN_NCOL,&mt3dms.BTN_NPER, &mt3dms.BTN_NCOMP, 
    //      &mt3dms.BTN_MCOMP);
    if (mf.NameFile.FindFileType(_T("RCT")) >= 0)
    {
      plot.bRCT = true;
      //mt3dms.BTN_NCOMP *= 2;
    }
    _fgetts(str, MAX_STR, fp1);
    mt3dms.BTN_TUNIT = _T("    ");
    mt3dms.BTN_LUNIT = _T("    ");
    mt3dms.BTN_MUNIT = _T("    ");
    _stscanf(str, _T("%4c%4c%4c"), mt3dms.BTN_TUNIT.GetBuffer(),
      mt3dms.BTN_LUNIT.GetBuffer(), mt3dms.BTN_MUNIT.GetBuffer());
    mt3dms.BTN_TUNIT.ReleaseBuffer(); mt3dms.BTN_TUNIT.Trim();
    mt3dms.BTN_LUNIT.ReleaseBuffer(); mt3dms.BTN_LUNIT.Trim();
    mt3dms.BTN_MUNIT.ReleaseBuffer(); mt3dms.BTN_MUNIT.Trim();
    _fgetts(str, MAX_STR, fp1);
    CString tstr1[10];
    for (int i = 0; i < 10; i++) tstr1[i] = _T("  ");
    _stscanf(str, _T("%2c%2c%2c%2c%2c%2c%2c%2c%2c%2c"), tstr1[0].GetBuffer(),
      tstr1[1].GetBuffer(), tstr1[2].GetBuffer(), tstr1[3].GetBuffer(), tstr1[4].GetBuffer(),
      tstr1[5].GetBuffer(), tstr1[6].GetBuffer(), tstr1[7].GetBuffer(), tstr1[8].GetBuffer(),
      tstr1[9].GetBuffer());
    for (int i = 0; i < 10; i++)
    {
      tstr1[i].ReleaseBuffer(); tstr1[i].Trim();
      if (tstr1[i].CompareNoCase(_T("T")) == 0) mt3dms.BTN_TRNOP[i] = true;
      else mt3dms.BTN_TRNOP[i] = false;
    }
    mt3dms.BTN_LAYCON.resize(mt3dms.BTN_NLAY);
    for (int i = 0; i < mt3dms.BTN_NLAY; i = i + 40)
    {
      _fgetts(tCstr.GetBuffer(MAX_STR), MAX_STR, fp1);
      tCstr.ReleaseBuffer();
      for (int j = 0; i + j < mt3dms.BTN_NLAY && j < 40; j++)
      {
        stconvert(tCstr.Mid(j * 2, 2), _T("%2d"), &(mt3dms.BTN_LAYCON[i]));
      }
    }
    //_fgetts(str,MAX_STR,fp1);
    mt3dms.BTN_DELR.resize(mt3dms.BTN_NCOL);
    ReadMT3DMSArray(fp1, (void*)&mt3dms.BTN_DELR, false, false);
    mt3dms.BTN_DELC.resize(mt3dms.BTN_NROW);
    ReadMT3DMSArray(fp1, (void*)&mt3dms.BTN_DELC, false, false);
    mt3dms.BTN_HTOP.resize(boost::extents[mt3dms.BTN_NROW][mt3dms.BTN_NCOL]);
    //mt3dms.BTN_HTOP.Resize(mt3dms.BTN_NROW,mt3dms.BTN_NCOL);
    ArrayF2D::array_view<2>::type myview = mt3dms.BTN_HTOP[boost::indices[range(0, mt3dms.BTN_NROW)][range(0, mt3dms.BTN_NCOL)]];
    ReadMT3DMSArray(fp1, (void*)&myview, true, false);
    mt3dms.BTN_DZ.resize(boost::extents[mt3dms.BTN_NLAY][mt3dms.BTN_NROW][mt3dms.BTN_NCOL]);
    //mt3dms.BTN_DZ.resize(mt3dms.BTN_NLAY);
    for (int k = 0; k < mt3dms.BTN_NLAY; k++)
    {
      //mt3dms.BTN_DZ[k].Resize(mt3dms.BTN_NROW,mt3dms.BTN_NCOL);
      ArrayF2D::array_view<2>::type myview = mt3dms.BTN_DZ[boost::indices[k][range(0, mt3dms.BTN_NROW)][range(0, mt3dms.BTN_NCOL)]];
      ReadMT3DMSArray(fp1, (void*)&myview, true, false);
    }
    mt3dms.BTN_PRSITY.resize(boost::extents[mt3dms.BTN_NLAY][mt3dms.BTN_NROW][mt3dms.BTN_NCOL]);
    //mt3dms.BTN_PRSITY.resize(mt3dms.BTN_NLAY);
    for (int k = 0; k < mt3dms.BTN_NLAY; k++)
    {
      //mt3dms.BTN_PRSITY[k].Resize(mt3dms.BTN_NROW,mt3dms.BTN_NCOL);
      ArrayF2D::array_view<2>::type myview = mt3dms.BTN_PRSITY[boost::indices[k][range(0, mt3dms.BTN_NROW)][range(0, mt3dms.BTN_NCOL)]];
      ReadMT3DMSArray(fp1, (void*)&myview, true, false);
    }
    mt3dms.BTN_ICBUND.resize(boost::extents[mt3dms.BTN_NLAY][mt3dms.BTN_NROW][mt3dms.BTN_NCOL]);
    //mt3dms.BTN_ICBUND.resize(mt3dms.BTN_NLAY);
    for (int k = 0; k < mt3dms.BTN_NLAY; k++)
    {
      //mt3dms.BTN_ICBUND[k].Resize(mt3dms.BTN_NROW,mt3dms.BTN_NCOL);
      ArrayI2D::array_view<2>::type myview = mt3dms.BTN_ICBUND[boost::indices[k][range(0, mt3dms.BTN_NROW)][range(0, mt3dms.BTN_NCOL)]];
      ReadMT3DMSArray(fp1, (void*)&myview, true, true);
    }
    mt3dms.BTN_SCONC.resize(boost::extents[mt3dms.BTN_NCOMP][mt3dms.BTN_NLAY][mt3dms.BTN_NROW][mt3dms.BTN_NCOL]);
    //mt3dms.BTN_SCONC.resize(mt3dms.BTN_NLAY);
    for (int iComp = 0; iComp < mt3dms.BTN_NCOMP; iComp++)
    {
      for (int k = 0; k < mt3dms.BTN_NLAY; k++)
      {
        //mt3dms.BTN_SCONC[k].Resize(mt3dms.BTN_NROW,mt3dms.BTN_NCOL);
        ArrayF2D::array_view<2>::type myview = mt3dms.BTN_SCONC[boost::indices[iComp][k][range(0, mt3dms.BTN_NROW)][range(0, mt3dms.BTN_NCOL)]];
        ReadMT3DMSArray(fp1, (void*)&myview, true, false);
      }
    }
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%10f%10lf"), &mt3dms.BTN_CINACT, &mt3dms.BTN_THKMIN);
    //    xscanf(str,_T("%10f %10lf"),&mt3dms.BTN_CINACT,&mt3dms.BTN_THKMIN);
    _fgetts(str, MAX_STR, fp1);
    tCstr = _T("          ");
    _stscanf(str, _T("%10d%10d%10d%10d%10[^\n]"), &mt3dms.BTN_IFMTCN, &mt3dms.BTN_IFMTNP,
      &mt3dms.BTN_IFMTRF, &mt3dms.BTN_IFMTDP, tCstr.GetBuffer());
    //    xscanf(str,_T("%10d %10d %10d %10d %10c"),&mt3dms.BTN_IFMTCN,&mt3dms.BTN_IFMTNP,
    //      &mt3dms.BTN_IFMTRF,&mt3dms.BTN_IFMTDP,tCstr.GetBuffer());
    tCstr.ReleaseBuffer();
    tCstr.Trim();
    if (tCstr.CompareNoCase(_T("T")) == 0) mt3dms.BTN_SAVUCN = true;
    else mt3dms.BTN_SAVUCN = false;
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%10d"), &mt3dms.BTN_NPRS);
    //    xscanf(str,_T("%10d"),&mt3dms.BTN_NPRS);
    if (mt3dms.BTN_NPRS > 0)
    {
      mt3dms.BTN_TIMPRS.resize(mt3dms.BTN_NPRS);
      for (int i = 0; i < mt3dms.BTN_NPRS; i = i + 8)
      {
        _fgetts(tCstr.GetBuffer(MAX_STR), MAX_STR, fp1);
        tCstr.ReleaseBuffer(80);
        for (int k = 0; i + k < mt3dms.BTN_NPRS && k < 8; k++)
        {
          stconvert(tCstr.Mid(k * 10, 10), _T("%10lf"), &(mt3dms.BTN_TIMPRS[i + k]));
        }
      }
      mt3dms.nTotalSteps = mt3dms.BTN_NPRS;
    }
    else //if(mt3dms.BTN_NPRS<0)
    {
      //MessageBox(_T("Negative NPRS is not supported yet"));
      mt3dms.nTotalSteps = 0;
    }
    //_fgetts(str,MAX_STR,fp1);
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%10d%10d"), &mt3dms.BTN_NOBS, &mt3dms.BTN_NPROBS);
    //    xscanf(str,_T("%10d %10d"),&mt3dms.BTN_NOBS,&mt3dms.BTN_NPROBS);
    mt3dms.BTN_Observation_Well.resize(mt3dms.BTN_NOBS);
    for (int i = 0; i < mt3dms.BTN_NOBS; i++)
    {
      _fgetts(str, MAX_STR, fp1);
      _stscanf(str, _T("%10d%10d%10d"), &(mt3dms.BTN_Observation_Well[i].KOBS),
        &(mt3dms.BTN_Observation_Well[i].IOBS), &(mt3dms.BTN_Observation_Well[i].JOBS));
      //      xscanf(str,_T("%10d %10d %10d"),&(mt3dms.BTN_Observation_Well[i].KOBS),
      //        &(mt3dms.BTN_Observation_Well[i].IOBS),&(mt3dms.BTN_Observation_Well[i].JOBS));
    }
    _fgetts(str, MAX_STR, fp1);
    tCstr = _T("          ");
    _stscanf(str, _T("%10[^\n]%10d"), tCstr.GetBuffer(), &mt3dms.BTN_NPRMAS);
    //    xscanf(str,_T("%10c %10d"),tCstr.GetBuffer(),&mt3dms.BTN_NPRMAS);
    tCstr.ReleaseBuffer(10); tCstr.Trim();
    if (tCstr.CompareNoCase(_T("T")) == 0) mt3dms.BTN_CHKMAS = true;
    else mt3dms.BTN_CHKMAS = false;
    int nSteps = 0;
    for (l = 0; l < mt3dms.BTN_NPER; l++)
    {
      mt3dms.BTN_PERLEN.resize(l + 1);
      mt3dms.BTN_NSTP.resize(l + 1);
      mt3dms.BTN_TSMULT.resize(l + 1);
      _fgetts(str, MAX_STR, fp1);
      _stscanf(str, _T("%10lf%10d%10lf"), &(mt3dms.BTN_PERLEN[l]), &(mt3dms.BTN_NSTP[l]), &(mt3dms.BTN_TSMULT[l]));
      //      xscanf(str,_T("%10lf %10d %10lf"),&(mt3dms.BTN_PERLEN[l]),&(mt3dms.BTN_NSTP[l]),&(mt3dms.BTN_TSMULT[l]));
      if (mt3dms.BTN_TSMULT[l] <= 0.0)
      {
        mt3dms.BTN_TSLNGH[l].resize(mt3dms.BTN_NSTP[l]);
        for (int i = 0; i < mt3dms.BTN_NSTP[l]; i = i + 8)
        {
          _fgetts(tCstr.GetBuffer(MAX_STR), MAX_STR, fp1);
          tCstr.ReleaseBuffer(80);
          for (int k = 0; i + k < mt3dms.BTN_NSTP[l] && k < 8; k++)
          {
            stconvert(tCstr.Mid(k * 10, 10), _T("%10lf"), &(mt3dms.BTN_TSLNGH[l][i + k]));
          }
        }
        nSteps += mt3dms.BTN_NSTP[l];
      }
      else
      {
        nSteps += mt3dms.BTN_NSTP[l];
      }
      _fgetts(str, MAX_STR, fp1);
      mt3dms.BTN_DT0.resize(l + 1);      mt3dms.BTN_MXSTRN.resize(l + 1);
      mt3dms.BTN_TTSMULT.resize(l + 1);      mt3dms.BTN_TTSMAX.resize(l + 1);
      _stscanf(str, _T("%10lf%10d%10lf%10lf"), &mt3dms.BTN_DT0[l], &mt3dms.BTN_MXSTRN[l], &mt3dms.BTN_TTSMULT[l], &mt3dms.BTN_TTSMAX[l]);
      //      xscanf(str,_T("%10lf %10d %10lf %10lf"),&mt3dms.BTN_DT0[l],&mt3dms.BTN_MXSTRN[l],&mt3dms.BTN_TTSMULT[l],&mt3dms.BTN_TTSMAX[l]);
    }
    if (mt3dms.nTotalSteps == 0) mt3dms.nTotalSteps = nSteps;
    else mt3dms.nTotalSteps += mt3dms.BTN_NPER - 1;
    if (mt3dms.BTN_NPRS == 0)
    {
      if (mt3dms.BTN_NPER > 1)
      {
        mt3dms.BTN_TIMPRS.resize(mt3dms.BTN_NPER);
        mt3dms.BTN_TIMPRS[0] = mt3dms.BTN_PERLEN[0];
        for (l = 1; l < mt3dms.BTN_NPER; l++)
        {
          mt3dms.BTN_TIMPRS[l] = mt3dms.BTN_TIMPRS[l - 1] + mt3dms.BTN_PERLEN[l];
        }
        mt3dms.BTN_NPRS = mt3dms.BTN_NPER;
      }
      else
      {
        mt3dms.BTN_NPRS = 1;
        mt3dms.BTN_TIMPRS.resize(mt3dms.BTN_NPRS);
      }
    }
    else if (mt3dms.BTN_NPRS < 0)
    {
      //mt3dms.BTN_NPRS = mt3dms.nTotalSteps;
    }
    fclose(fp1);
  }
  // Read Adevection Package pht3dadv.dat
  tCstr = mt3dms.ADV_FileName;
  iTmp = sPlotDirectory.ReverseFind(_T('\\'));
  sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
  sPlotDirectory += tCstr;
  if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
  {
    MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0); return -1;
  }
  else
  {
    _fgetts(str, MAX_STR, fp1);
    _stscanf(str, _T("%10d%10lf%10d%10d"), &mt3dms.ADV_MIXELEM,
      &mt3dms.ADV_PERCEL, &mt3dms.ADV_MXPART, &mt3dms.ADV_NADVFD);
    //    xscanf(str,_T("%10d %10lf %10d %10d"),&mt3dms.ADV_MIXELEM,
    //      &mt3dms.ADV_PERCEL,&mt3dms.ADV_MXPART,&mt3dms.ADV_NADVFD);
    if (mt3dms.ADV_MIXELEM > 0)
    {
      _fgetts(str, MAX_STR, fp1);
      _stscanf(str, _T("%10d%10lf"), &mt3dms.ADV_ITRACK, &mt3dms.ADV_WD);
      //      xscanf(str,_T("%10d %10lf"),&mt3dms.ADV_ITRACK,&mt3dms.ADV_WD);
      if (mt3dms.ADV_MIXELEM == 1 || mt3dms.ADV_MIXELEM == 3)
      {
        _fgetts(str, MAX_STR, fp1);
        _stscanf(str, _T("%10lf%10d%10d%10d%10d%10d"), &mt3dms.ADV_DCEPS, &mt3dms.ADV_NPLANE,
          &mt3dms.ADV_NPL, &mt3dms.ADV_NPH, &mt3dms.ADV_NPMIN, &mt3dms.ADV_NPMAX);
        //        xscanf(str,_T("%10lf %10d %10d %10d %10d %10d"),&mt3dms.ADV_DCEPS,&mt3dms.ADV_NPLANE,
        //          &mt3dms.ADV_NPL,&mt3dms.ADV_NPH,&mt3dms.ADV_NPMIN,&mt3dms.ADV_NPMAX);
      }
      if (mt3dms.ADV_MIXELEM == 2 || mt3dms.ADV_MIXELEM == 3)
      {
        _fgetts(str, MAX_STR, fp1);
        _stscanf(str, _T("%10d%10d%10d"), &mt3dms.ADV_INTERP, &mt3dms.ADV_NLSINK,
          &mt3dms.ADV_NPSINK);
        //        xscanf(str,_T("%10d %10d %10d"),&mt3dms.ADV_INTERP,&mt3dms.ADV_NLSINK,
        //          &mt3dms.ADV_NPSINK);
      }
      if (mt3dms.ADV_MIXELEM == 3)
      {
        _fgetts(str, MAX_STR, fp1);
        _stscanf(str, _T("%10lf"), &mt3dms.ADV_DCHMOC);
        //        xscanf(str,_T("%10lf"),&mt3dms.ADV_DCHMOC);
      }
    }
    fclose(fp1);
  }
  // Read Dispersion Package pht3ddsp.dat
  // Read Generalized Conjugate Gradient Solver Package pht3dgcg.dat
  // Read Sink and Source Mixing Package pht3dssm.dat

  // Read Adevection Package pht3dadv.dat
  tCstr = "ExtraUCN.txt";
  iTmp = sPlotDirectory.ReverseFind(_T('\\'));
  sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
  sPlotDirectory += tCstr;
  if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
  {
    //MessageBox(_T("Can't open ")+tCstr);
    plot.nExtra = 0;
  }
  else
  {
    CString sLine;
    //int iSize;
    _fgetts(sLine.GetBuffer(MAX_STR), MAX_STR, fp1);
    sLine.ReleaseBuffer();
    int n = _stscanf(sLine, _T("%d"), &plot.nExtra);
    if (n == 1)
    {
      for (int i = 0; i < plot.nExtra && plot.nExtra>0; i++)
      {
        if (feof(fp1)) break;
        _fgetts(sLine.GetBuffer(MAX_STR), MAX_STR, fp1);
        sLine.ReleaseBuffer();
        plot.ExtraUCN.resize(i + 1);
        plot.ExtraUCN[i] = sLine.Trim();
      }
    }
    else
    {
      plot.ExtraUCN.resize(1);
      plot.ExtraUCN[0] = sLine.Trim();
      for (int i = 1; i < 200; i++)
      {
        plot.nExtra = i;
        if (feof(fp1)) break;
        _fgetts(sLine.GetBuffer(MAX_STR), MAX_STR, fp1);
        sLine.ReleaseBuffer();
        sLine.Trim();
        if (sLine == "") break;
        plot.ExtraUCN.resize(i + 1);
        plot.ExtraUCN[i] = sLine;
      }
    }
    fclose(fp1);
  }
  // Read MT3DMS concentrations
  MT3D_UCN_Head *pMT3DHead;
  mt3dms.Times.resize(mt3dms.nTotalSteps);
  mt3dms.Time_Period.resize(mt3dms.nTotalSteps);
  mt3dms.Time_Period_Step.resize(mt3dms.nTotalSteps);
  //if (mf.NameFile.FindFileType(_T("RCT"))>=0)
  //  mt3dms_ConcentrationRange.resize(mt3dms.BTN_NCOMP*2);
  //else
  //  mt3dms_ConcentrationRange.resize(mt3dms.BTN_NCOMP);
  mt3dms.bFortranBinary = false;
  //int nComp = mt3dms.BTN_NCOMP+mPHT3DM.N
  if (plot.bRCT)
    mt3dms.Data.resize(mt3dms.BTN_NCOMP * 2 + plot.nExtra);
  else
    mt3dms.Data.resize(mt3dms.BTN_NCOMP + plot.nExtra);
  for (int iComp = 0; iComp < mt3dms.BTN_NCOMP * 2 + plot.nExtra; iComp++)
  {
    if (plot.bRCT)
    {
      if (iComp >= mt3dms.BTN_NCOMP * 2)
      {
        tCstr.Format(mt3dms.ConcentrationFileTemplate, 50 + iComp - mt3dms.BTN_NCOMP * 2);
        tCstr += _T(".UCN");
      }
      else
      {
        tCstr.Format(mt3dms.ConcentrationFileTemplate, iComp / 2 + 1);
        if (iComp % 2 == 0)
        {
          tCstr += _T(".UCN");
        }
        else
        {
          tCstr += _T("S.UCN");
        }
      }
    }
    else
    {
      if (iComp >= (mt3dms.BTN_NCOMP + plot.nExtra))
        break;
      if (iComp >= mt3dms.BTN_NCOMP)
        tCstr.Format(mt3dms.ConcentrationFileTemplate, 50 + iComp - mt3dms.BTN_NCOMP);
      else
        tCstr.Format(mt3dms.ConcentrationFileTemplate, iComp + 1);
      tCstr += _T(".UCN");
    }
    mt3dms.Concentrations.resize(iComp + 1);
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1);
    sPlotDirectory += tCstr;
    if ((fh1 = _topen(sPlotDirectory, _O_RDONLY | _O_BINARY | _O_SEQUENTIAL)) == -1)
    {
      mt3dms.Data.resize(iComp);
      MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0);
      return -1;
    }
    else
    {
      //int idx = mt3dms.AddData(_T("Conc"));
      mt3dms.Data[iComp].bHeadPerLayer = true;
      mt3dms.Data[iComp].bColumnRead = true;
      mt3dms.Data[iComp].FileName = tCstr;
      mt3dms.Data[iComp].nStrideBytes = sizeof(float)*mt3dms.BTN_NCOL;
      mt3dms.Data[iComp].nHeadBytes = sizeof(MT3D_UCN_Head);
      mt3dms.Data[iComp].Data_Type = 0;
      mt3dms.Data[iComp].Data_Pointer = NULL;
      mt3dms.Data[iComp].Title = _T("Conc");
      int tIval, tIval2;
      bytes_read = _read(fh1, &tIval, sizeof(int));
      if (tIval == 0x2c)
      {
        mt3dms.bFortranBinary = true;
      }
      _lseeki64(fh1, 0, SEEK_SET);
      _int64 nTimeStepBytes;
      if (mt3dms.bFortranBinary)
        nTimeStepBytes = mt3dms.BTN_NLAY*(sizeof(int) * 2 + mt3dms.Data[iComp].nHeadBytes + mt3dms.BTN_NROW*(sizeof(int) * 2 + mt3dms.Data[iComp].nStrideBytes));
      else
        nTimeStepBytes = mt3dms.BTN_NLAY*(mt3dms.Data[iComp].nHeadBytes + mt3dms.BTN_NROW*mt3dms.Data[iComp].nStrideBytes);
      mt3dms.Data[iComp].iSeekHeadOffset.resize(mt3dms.nTotalSteps);
      mt3dms.Data[iComp].iSeekDataOffset.resize(mt3dms.nTotalSteps);
      for (int j = 0; j < mt3dms.nTotalSteps; j++)
      {
        mt3dms.Data[iComp].iSeekHeadOffset[j] = j*nTimeStepBytes + 1;
        mt3dms.Data[iComp].iSeekDataOffset[j] = j*nTimeStepBytes + 1 +
          sizeof(MT3D_UCN_Head);
        if (mt3dms.bFortranBinary) mt3dms.Data[iComp].iSeekDataOffset[j] += sizeof(int) * 2;
      }
      bool bContinue = true;
      //double tval;
      min = 1e30; max = -1e30;
      int nSteps = 0;
      pMT3DHead = (MT3D_UCN_Head*)str;
      for (int l = 0; l < mt3dms.nTotalSteps && bContinue; l++)
      {
        _lseeki64(fh1, mt3dms.Data[iComp].iSeekHeadOffset[l] - 1, SEEK_SET);//+l*mt3dms.Data[iComp].nTimeStepBytes
        if (mt3dms.bFortranBinary) _read(fh1, &tIval, sizeof(int));
        bytes_read = _read(fh1, str, sizeof(MT3D_UCN_Head));
        if (strncmp(pMT3DHead->TEXT, "CONCENTRATION", 10) != 0)
        {
          MessageBox(NULL, _T("The UCN header should be concentration"), 0, 0);
        }
        if (mt3dms.bFortranBinary)
        {
          _read(fh1, &tIval2, sizeof(int));
          if (tIval != tIval2)
          {
            MessageBox(NULL, _T("The record size of Fortran binary format does not match"), 0, 0);
          }
        }
        if (bytes_read != sizeof(MT3D_UCN_Head))
        {
          bContinue = false;
          mt3dms.nTotalSteps = l;
          if (l == 0) MessageBox(NULL, _T("The size of ") + tCstr + _T(" is zero"), 0, 0);
          break;
        }
        if (pMT3DHead->NROW != mt3dms.BTN_NROW || pMT3DHead->NCOL != mt3dms.BTN_NCOL)
        {
          if (l == 0) MessageBox(NULL, _T("Concentration data do not match with input file"), 0, 0);
        }
        mt3dms.Time_Period[l] = pMT3DHead->KPER;
        mt3dms.Time_Period_Step[l] = pMT3DHead->KSTP;
        mt3dms.Times[l] = pMT3DHead->TIME2;
      }
      _close(fh1);
    }
  }
  //int idx = mt3dms
  int idx = mt3dms.AddData(_T("Porosity"));
  //int iSize = mt3dms.Data.size();
  //mt3dms.Data.resize(iSize+1);
  //mt3dms.Data[iSize].Title = _T("Porosity");
  //mt3dms.Data_Pointer.resize(iSize+1);
  //mt3dms.Data_Type.resize(iSize+1);
  //mt3dms.Data_Max.resize(iSize+1);
  //mt3dms.Data_Min.resize(iSize+1);
  mt3dms.Data[idx].Data_Pointer = (void*)&(mt3dms.BTN_PRSITY);
  mt3dms.Data[idx].Data_Type = 6;
  //mt3dms.Data.resize(iTmp+1);
  mt3dms.Data[idx].FileName = _T("");
  mt3dms.Data[idx].Min = 0.0;
  mt3dms.Data[idx].Max = 1.0;
  //Add_Concentrations_to_Strings();
  plot.ModelType |= 0x09; // solute (0x01), flow (0x02), pht3d (0x04) and mt3dms (0x08)
  return 0;
}

int OpenModel(PHT3D_Model& mPHT3DM, CString sSelectedFile)
{
  PLOTClass& plot = mPHT3DM.PLOT;
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;

  int iTmp, iTmp2, iStart;
  CString tCstr, sPlotDirectory;
  // Read Name file
  sPlotDirectory = sSelectedFile;
  mf.NameFile.clear();
  iTmp = Read_Name_File(mPHT3DM, sPlotDirectory);
  if (iTmp < 0) return -4;
  mPHT3DM.PLOT.bReady = false;
  mPHT3DM.clear();
  if ((iTmp = mf.NameFile.FindFileType(_T("PHC"))) >= 0)
  {
    // PHT3D case
    mPHT3DM.PHT3D.PHC_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("BTN"));
    if (iTmp >= 0) mt3dms.BTN_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("ADV"));
    if (iTmp >= 0) mt3dms.ADV_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("DSP"));
    if (iTmp >= 0) mt3dms.DSP_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("SSM"));
    if (iTmp >= 0) mt3dms.SSM_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("GCG"));
    if (iTmp >= 0) mt3dms.GCG_FileName = mf.NameFile.FileName[iTmp];
    mt3dms.ConcentrationFileTemplate = _T("PHT3D%03d");
    // Read PHT3D model
    iTmp = Read_PHT3D_Model(mPHT3DM, sPlotDirectory);
    if (iTmp < 0)
      return iTmp;
    //else 
    //  UpdatePlotMenu(mPHT3DM.PLOT.PlotMode);
    //Read MT3DMS
    iTmp = Read_MT3DMS_Model(mPHT3DM, sPlotDirectory);
    if (iTmp < 0) return -3;
    //Read Modflow
    iTmp = sPlotDirectory.ReverseFind(_T('.'));
    tCstr = sPlotDirectory.Left(iTmp + 1);
    tCstr += _T("nam");
    iTmp = Read_Name_File(mPHT3DM, tCstr);
    if (iTmp >= 0)
      iTmp2 = Read_MODFLOW_Files(mPHT3DM, sPlotDirectory);
    else
    {
      mf.DIS_NLAY = mt3dms.BTN_NLAY;
      mf.DIS_NROW = mt3dms.BTN_NROW;
      mf.DIS_NCOL = mt3dms.BTN_NCOL;
    }
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    tCstr = sPlotDirectory.Left(iTmp);
    iStart = 0;
    if (iTmp > MAX_TITLE) { iStart = iTmp - MAX_TITLE; }
    mPHT3DM.modelType = _T("PHT3D - ") + tCstr.Mid(iStart);
    mPHT3DM.PLOT.bFlowPlot = false;
  }
  else if (mf.NameFile.FindFileType(_T("VDF")) >= 0)
  {
    // Seawat case
    iTmp = mf.NameFile.FindFileType(_T("BTN"));
    if (iTmp >= 0) mt3dms.BTN_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("ADV"));
    if (iTmp >= 0) mt3dms.ADV_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("DSP"));
    if (iTmp >= 0) mt3dms.DSP_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("SSM"));
    if (iTmp >= 0) mt3dms.SSM_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("GCG"));
    if (iTmp >= 0) mt3dms.GCG_FileName = mf.NameFile.FileName[iTmp];
    mt3dms.ConcentrationFileTemplate = _T("MT3D%03d");
    //Read MT3DMS
    Read_MT3DMS_Model(mPHT3DM, sPlotDirectory);
    //Read Modflow
    iTmp2 = Read_MODFLOW_Files(mPHT3DM, sPlotDirectory);
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    tCstr = sPlotDirectory.Left(iTmp);
    iStart = 0;
    if (iTmp > MAX_TITLE) { iStart = iTmp - MAX_TITLE; }
    mPHT3DM.modelType = _T("SEAWAT - ") + tCstr.Mid(iStart);
    mPHT3DM.PLOT.bFlowPlot = false;
  }
  else if (mf.NameFile.FindFileType(_T("BTN")) >= 0)
  {  //MT3DMS case (read mtn file)
    iTmp = mf.NameFile.FindFileType(_T("BTN"));
    if (iTmp >= 0) mt3dms.BTN_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("ADV"));
    if (iTmp >= 0) mt3dms.ADV_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("DSP"));
    if (iTmp >= 0) mt3dms.DSP_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("SSM"));
    if (iTmp >= 0) mt3dms.SSM_FileName = mf.NameFile.FileName[iTmp];
    iTmp = mf.NameFile.FindFileType(_T("GCG"));
    if (iTmp >= 0) mt3dms.GCG_FileName = mf.NameFile.FileName[iTmp];
    //mt3dms.BTN_FileName=_T("mtmsbtn1.dat");
    //mt3dms.ADV_FileName=_T("mtmsadv1.dat");
    //mt3dms.DSP_FileName=_T("mtmsdsp1.dat");
    //mt3dms.SSM_FileName=_T("mtmsssm1.dat");
    //mt3dms.GCG_FileName=_T("mtmsgcg1.dat");
    mt3dms.ConcentrationFileTemplate = _T("MT3D%03d");
    //Read MT3DMS
    Read_MT3DMS_Model(mPHT3DM, sPlotDirectory);
    //Read Modflow
    iTmp = sPlotDirectory.ReverseFind(_T('.'));
    tCstr = sPlotDirectory.Left(iTmp + 1);
    tCstr += _T("nam");
    iTmp = Read_Name_File(mPHT3DM, tCstr);
    if (iTmp >= 0)
      iTmp2 = Read_MODFLOW_Files(mPHT3DM, sPlotDirectory);
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    tCstr = sPlotDirectory.Left(iTmp);
    iStart = 0;
    if (iTmp > MAX_TITLE) { iStart = iTmp - MAX_TITLE; }
    mPHT3DM.modelType = _T("MT3DMS - ") + tCstr.Mid(iStart);
    mPHT3DM.PLOT.bFlowPlot = false;
  }
  else if (mf.NameFile.FindFileType(_T("BAS")) >= 0 || mf.NameFile.FindFileType(_T("BAS6")) >= 0)
  {// pure modflow case
    iTmp2 = Read_MODFLOW_Files(mPHT3DM, sPlotDirectory);
    if (iTmp2 == 0)
    {
      // Update MT3DMS basic parameters
      mt3dms.BTN_NLAY = mf.DIS_NLAY;
      mt3dms.BTN_NROW = mf.DIS_NROW;
      mt3dms.BTN_NCOL = mf.DIS_NCOL;
    }
    else
      return -2;
    iTmp = sPlotDirectory.ReverseFind(_T('\\'));
    tCstr = sPlotDirectory.Left(iTmp);
    iStart = 0;
    if (iTmp > MAX_TITLE) { iStart = iTmp - MAX_TITLE; }
    mPHT3DM.modelType = _T("MODFLOW - ") + tCstr.Mid(iStart);
    mPHT3DM.PLOT.bFlowPlot = true;
  }
  Add_Concentrations_to_Strings(mPHT3DM);
  if (!mf.bLMT6Data && !mf.bHeadsData && !mf.bDrawDownData && !mf.bBudgetData)
  {
    mf.nTotalSteps = 0;
  }
  if (mPHT3DM.PLOT.ModelType & 0x02 && !(mPHT3DM.PLOT.ModelType & 0x08))// solute (0x01), flow (0x02), pht3d (0x04) and mt3dms (0x08)
  {
    if (mt3dms.BTN_HTOP.shape()[0] == 0)
    {
      mt3dms.BTN_HTOP.resize(boost::extents[mt3dms.BTN_NROW][mt3dms.BTN_NCOL]);
      mt3dms.BTN_DZ.resize(boost::extents[mt3dms.BTN_NLAY][mt3dms.BTN_NROW][mt3dms.BTN_NCOL]);
      //mt3dms.BTN_DZ.resize(mt3dms.BTN_NLAY);
      for (int k = 0; k < mt3dms.BTN_NLAY; k++)
      {
        //mt3dms.BTN_DZ[k].Resize(mt3dms.BTN_NROW,mt3dms.BTN_NCOL);
        for (int j = 0; j < mt3dms.BTN_NROW; j++)
        {
          //mt3dms.BTN_HTOP[j]=mf.DIS_TOP[j];
          for (int i = 0; i < mt3dms.BTN_NCOL; i++)
          {
            mt3dms.BTN_HTOP[j][i] = mf.DIS_TOP[j][i];
            if (k == 0)
              mt3dms.BTN_DZ[k][j][i] = mf.DIS_TOP[j][i] - mf.DIS_BOTMS[k][j][i];
            else
              mt3dms.BTN_DZ[k][j][i] = mf.DIS_BOTMS[k - 1][j][i] - mf.DIS_BOTMS[k][j][i];
          }
        }
      }
      mt3dms.BTN_DELC.resize(mt3dms.BTN_NROW);
      for (int j = 0; j < mt3dms.BTN_NROW; j++)
      {
        mt3dms.BTN_DELC[j] = mf.DIS_DELC[j];
      }
      mt3dms.BTN_DELR.resize(mt3dms.BTN_NCOL);
      for (int i = 0; i < mt3dms.BTN_NCOL; i++)
      {
        mt3dms.BTN_DELR[i] = mf.DIS_DELR[i];
      }
    }
  }
  //update cell dimensions
  mPHT3DM.PLOT.Z.resize(mt3dms.BTN_NLAY);
  for (int k = 0; k < mt3dms.BTN_NLAY; k++)
  {
    mPHT3DM.PLOT.Z[k].Resize(mt3dms.BTN_NROW, mt3dms.BTN_NCOL);
    for (int j = 0; j < mt3dms.BTN_NROW; j++)
    {
      for (int i = 0; i < mt3dms.BTN_NCOL; i++)
      {
        if (k == 0)
        {
          mPHT3DM.PLOT.Z[k][j][i] = mt3dms.BTN_HTOP[j][i] - mt3dms.BTN_DZ[k][j][i] / 2.0f;
        }
        else
        {
          mPHT3DM.PLOT.Z[k][j][i] = mPHT3DM.PLOT.Z[k - 1][j][i] - mt3dms.BTN_DZ[k - 1][j][i] / 2.0 - mt3dms.BTN_DZ[k][j][i] / 2.0;
        }
      }
    }
  }
  mPHT3DM.PLOT.X.resize(mt3dms.BTN_NCOL);
  mPHT3DM.PLOT.Y.resize(mt3dms.BTN_NROW);
  mPHT3DM.PLOT.X[0] = mt3dms.BTN_DELR[0] / 2.0;
  mPHT3DM.PLOT.Y[0] = mt3dms.BTN_DELC[0] / 2.0;
  for (int i = 1; i < mt3dms.BTN_NCOL; i++)
  {
    mPHT3DM.PLOT.X[i] = mPHT3DM.PLOT.X[i - 1] + mt3dms.BTN_DELR[i - 1] / 2.0 + mt3dms.BTN_DELR[i] / 2.0;
  }
  for (int j = 1; j < mt3dms.BTN_NROW; j++)
  {
    mPHT3DM.PLOT.Y[j] = mPHT3DM.PLOT.Y[j - 1] + mt3dms.BTN_DELC[j - 1] / 2.0 + mt3dms.BTN_DELC[j] / 2.0;
  }
  double tValue, tRange;
  if (mt3dms.nTotalSteps > 0)
  {
    tValue = max(fabs(mt3dms.Times[mt3dms.nTotalSteps - 1]), fabs(mt3dms.Times[0]));
    if (mt3dms.nTotalSteps == 1)
      tRange = max(0.01, fabs(tValue)*0.2);
    else
      tRange = mt3dms.Times[mt3dms.nTotalSteps - 1] - mt3dms.Times[0];
    mPHT3DM.PLOT.ConcentrationTimeFormat = _T("(%d,%d) %8lg");//+GetNumFormat(tValue,tRange/mt3dms.nTotalSteps);
  }
  if (mf.nTotalSteps > 0)
  {
    tValue = fabs(mf.Times[mf.nTotalSteps - 1]) + fabs(mf.Times[0]);
    if (mt3dms.nTotalSteps == 1)
      tRange = max(0.01, fabs(tValue)*0.2);
    else
      tRange = mf.Times[mf.nTotalSteps - 1] - mf.Times[0];
    mPHT3DM.PLOT.FlowTimeFormat = _T("(%d,%d) %8lg");//+GetNumFormat(tValue,tRange/mf.nTotalSteps);
  }
  /*if(mt3dms.nTotalSteps<1)
  mPHT3DM.PLOT.bFlowPlot = true;
  if(mf.nTotalSteps<1)
  mPHT3DM.PLOT.bFlowPlot = false;*/
  for (int i = 0; i < mf.Data.size(); i++)
  {
    CString tCstr2 = mf.Data[i].Title;
    if (tCstr2 == "LKMT-RCH" || tCstr2 == "LKMT-QXX" || tCstr2 == "LKMT-QYY" || tCstr2 == "LKMT-QZZ")
    {
      mf.Data[i].HelpText = "Volumetric net flow rate of MT3D flow linkage file (L^3/T)";
    }
    else if (tCstr2 == "LKMT-THKSAT")
    {
      mf.Data[i].HelpText = "Cell satirated thickness: Confined cell -111, Inactive 1.E30 (L)";
    }
    else if (tCstr2 == "LKMT-CNH")
    {
      mf.Data[i].HelpText = "Constant Head Boundary: Flow rate (L^3/T), positive value if flow is out of the cell";
    }
    else if (tCstr2 == "Bgt-CONSTANT HEAD" || tCstr2 == "Bgt-FLOW RIGHT FACE" ||
      tCstr2 == "Bgt-FLOW LOWER FACE" || tCstr2 == "Bgt-RECHARGE")
    {
      mf.Data[i].HelpText = "Volumetric net flow rate of budget file(L^3/T)";
    }
    else if (tCstr2 == "Heads")
    {
      mf.Data[i].HelpText = "Head(L), L is typically meter";
    }
    else if (tCstr2 == "Drawdown")
    {
      mf.Data[i].HelpText = "Drawdown(L), L is typically meter";
    }
    else if (tCstr2 == "Head(start)")
    {
      mf.Data[i].HelpText = "Initial head(L), L is typically meter";
    }
    else if (tCstr2 == "IBOUND")
    {
      mf.Data[i].HelpText = "Flow boundary condition: 1 Active, 0 Inactive, -1 Fixed";
    }
    else if (tCstr2 == "Horz Log K")
    {
      mf.Data[i].HelpText = "Horizontal Log K (L/T)";
    }
    else if (tCstr2 == "Vert Log K")
    {
      mf.Data[i].HelpText = "Vertical Log K (L/T)";
    }
  }

  return 0;
}

int stconvert(const TCHAR *_Str, const TCHAR *_Format, void *pointer)
{
  CString fmt = _Format;
  int iRes;
  switch (fmt[fmt.GetLength() - 1])
  {
  case _T('c'):
  case _T('C'):
    iRes = _stscanf(_Str, _Format, pointer);
    break;
  case _T('d'):
  case _T('i'):
  case _T('o'):
  case _T('u'):
  case _T('x'):
    iRes = _stscanf(_Str, _Format, pointer);
    if (iRes <= 0)
    {
      if ((fmt[fmt.GetLength() - 2] | 0x20) == _T('l'))
      {
        *((_int64*)pointer) = 0;
      }
      else
      {
        *((int*)pointer) = 0;
      }
    }
    break;
  case _T('e'):
  case _T('E'):
  case _T('f'):
  case _T('g'):
  case _T('G'):
    iRes = _stscanf(_Str, _Format, pointer);
    if (iRes <= 0)
    {
      if ((fmt[fmt.GetLength() - 2] | 0x20) == _T('l'))
      {
        *((double*)pointer) = 0;
      }
      else
      {
        *((float*)pointer) = 0;
      }
    }
    break;
  case _T('s'):
  case _T('S'):
    iRes = _tcslen(_Str);
    for (int i = 0; i < iRes; i++)
    {
      *((TCHAR*)pointer + i) = _Str[i];
    }
    *((TCHAR*)pointer + iRes) = _T('\0');
    iRes = 1;
    break;
  default:
    iRes = _stscanf(_Str, _Format, pointer);
    break;
  }
  return iRes;
}

void Add_Concentrations_to_Strings(PHT3D_Model& mPHT3DM)
{
  MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;
  PHT3DClass& pht3d = mPHT3DM.PHT3D;

  CString tCstr;
  //mt3dms.Data.clear();
  int index = 0;
  for (int i = 0; i < pht3d.NR_MOB_KIN; i++)
  {
    tCstr.Format(_T("%s"), pht3d.Kinetic_Mobile[i].Name);
    mt3dms.Data[index++].Title = tCstr;
    if (mPHT3DM.PLOT.bRCT)
    {
      tCstr.Format(_T("%s_im"), pht3d.Kinetic_Mobile[i].Name);
      mt3dms.Data[index++].Title = tCstr;
    }
  }
  for (int i = 0; i < pht3d.NR_SOL_MST_SPEC_EQU; i++)
  {
    tCstr.Format(_T("%s"), pht3d.Equilibrium_Component[i]);
    mt3dms.Data[index++].Title = tCstr;
    if (mPHT3DM.PLOT.bRCT)
    {
      tCstr.Format(_T("%s_im"), pht3d.Equilibrium_Component[i]);
      mt3dms.Data[index++].Title = tCstr;
    }
  }
  for (int i = 0; i < pht3d.NR_IMOB_KIN; i++)
  {
    tCstr.Format(_T("%s"), pht3d.IMOB_KIN[i].Name);
    mt3dms.Data[index++].Title = tCstr;
    if (mPHT3DM.PLOT.bRCT)
    {
      tCstr.Format(_T("%s_im"), pht3d.IMOB_KIN[i].Name);
      mt3dms.Data[index++].Title = tCstr;
    }
  }
  for (int i = 0; i < pht3d.NR_MIN_EQU; i++)
  {
    tCstr.Format(_T("%s"), pht3d.Equilibrium_Mineral[i]);
    mt3dms.Data[index++].Title = tCstr;
    if (mPHT3DM.PLOT.bRCT)
    {
      tCstr.Format(_T("%s_im"), pht3d.Equilibrium_Mineral[i]);
      mt3dms.Data[index++].Title = tCstr;
    }
  }
  for (int i = 0; i < pht3d.NR_ION_EX; i++)
  {
    tCstr.Format(_T("%s"), pht3d.Ion_Exchange[i].Name + _T("(ex)"));
    mt3dms.Data[index++].Title = tCstr;
    if (mPHT3DM.PLOT.bRCT)
    {
      tCstr.Format(_T("%s_im"), pht3d.Ion_Exchange[i].Name + _T("(ex)"));
      mt3dms.Data[index++].Title = tCstr;
    }
  }
  for (int i = 0; i < pht3d.NR_SURF; i++)
  {
    tCstr.Format(_T("%s"), pht3d.Surface_Complex[i].Name);
    mt3dms.Data[index++].Title = tCstr;
    if (mPHT3DM.PLOT.bRCT)
    {
      tCstr.Format(_T("%s_im"), pht3d.Surface_Complex[i].Name);
      mt3dms.Data[index++].Title = tCstr;
    }
  }
  for (int i = 0; i < pht3d.NR_MIN_KIN; i++)
  {
    tCstr.Format(_T("%s"), pht3d.Kinetic_Mineral[i].Name + _T("(kin)"));
    mt3dms.Data[index++].Title = tCstr;
    if (mPHT3DM.PLOT.bRCT)
    {
      tCstr.Format(_T("%s_im"), pht3d.Kinetic_Mineral[i].Name + _T("(kin)"));
      mt3dms.Data[index++].Title = tCstr;
    }
  }
  if (!(mPHT3DM.PLOT.ModelType & 0x04)) // If not PHT3D
  {
    for (int i = 0; i < mt3dms.BTN_NCOMP * 2; i++)
    {
      if (i >= mt3dms.BTN_NCOMP)
      {
        if (!mPHT3DM.PLOT.bRCT)
          break;
        else
        {
          tCstr.Format(_T("IM Sol %d"), i + 1 - mt3dms.BTN_NCOMP);
          mt3dms.Data[index++].Title = tCstr;
        }
      }
      else
      {
        tCstr.Format(_T("Solute %d"), i + 1);
        mt3dms.Data[index++].Title = tCstr;
      }
    }
  }
  for (int i = 0; i < mPHT3DM.PLOT.nExtra; i++)
  {
    if (index >= mt3dms.Data.size()) break;
    mt3dms.Data[index++].Title = mPHT3DM.PLOT.ExtraUCN[i];
  }
  for (int i = 0; i < pht3d.SELECTED_OUTPUT_Heads.size(); i++)
  {
    int iSize = mt3dms.Data.size();
    mt3dms.Data[index++].Title = pht3d.SELECTED_OUTPUT_Heads[i];
  }
  if (mt3dms.Data.size() > 0)
    int idx = mt3dms.AddData(_T("Porosity"));
  // mt3dms.Data[index++].Title = "Porosity";
}

int Read_A_Line_From_Thermo_Database(PHT3D_Model& mPHT3DM, CString *tBuffer, FILE *fp1)
{
  int iTmp;
  do
  {
    _fgetts((*tBuffer).GetBuffer(MAX_STR), MAX_STR, fp1);
    (*tBuffer).ReleaseBuffer();
    if (feof(fp1)) return -3;
    (*tBuffer).Trim();
    if ((*tBuffer).IsEmpty()) continue;
    if ((*tBuffer)[0] == _T('#')) return -1;
    else if ((iTmp = mPHT3DM.PHT3D.Thermo_DB.FindTitle((*tBuffer))) >= 0)
      return iTmp;
    else return -2;
  } while (1);
}

int GetTimeIndex(PHT3D_Model& mPHT3DM, double val, double factor)
{
  MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;

  double val1, val2;
  val /= factor;
  for (int i = 0; i < mt3dms.nTotalSteps; i++)
  {
    if (i != 0)
      val1 = fabs(val - mt3dms.Times[i - 1]);
    else
      val1 = 1e30;
    val2 = fabs(val - mt3dms.Times[i]);
    if (val <= mt3dms.Times[i])
    {
      if (val1 < val2) return i - 1;
      else return i;
    }
  }
  return -1;
}

int GetStringTokens(CString *cStr, vector<CString> *stArr, CString *spaIncString)
{
  int nToken = 0, curPos = 0, nLen;
  CString separators = _T(" ,\t\n");
  //TCHAR *token;
  CString resToken, tCstr;
  //token = _tcstok(str, seps ); // C4996
  // Note: strtok is deprecated; consider using strtok_s instead
  resToken = (*cStr).Tokenize(separators, curPos);
  while (resToken != _T(""))
  {
    tCstr = resToken.SpanIncluding(*spaIncString);//_T("+-0123456789.")
    nLen = tCstr.GetLength();
    if (nLen > 0 && nLen < resToken.GetLength())
    {
      nToken = nToken + 2;
      stArr->resize(nToken);
      (*stArr)[nToken - 2] = tCstr;
      (*stArr)[nToken - 1] = resToken.Mid(nLen);
    }
    else
    {
      nToken++;
      stArr->resize(nToken);
      (*stArr)[nToken - 1] = resToken;
    }
    //_tprintf_s(_T("Resulting token: %s\n"), resToken);
    resToken = (*cStr).Tokenize(separators, curPos);
  };

  /*while( token != NULL )
  {
  nToken++;
  stArr->resize(nToken);
  (*stArr)[nToken-1].Tokenize = token;
  token = _tcstok( NULL, seps ); // C4996
  }*/
  return nToken;
}

void ReadMODFLOWArray(FILE *fp1, void* rArray, bool bMatrix, bool bInt, bool bFreeFormat)
{
  CString cStr, fmtin;
  int iprn, iRead, nFmtCol, nWidth, locat;
  double cnstnt;
  _fgetts(cStr.GetBuffer(MAX_STR), MAX_STR, fp1);
  cStr.ReleaseBuffer();
  fmtin = cStr.Left(10).Trim();
  locat = 0;
  if (fmtin.CompareNoCase(_T("CONSTANT")) == 0)
  {
    //locat = 0; do nothing
    iRead = _stscanf(cStr, _T("%*s %lf"), &cnstnt);
    bFreeFormat = true;
  }
  else if (fmtin.CompareNoCase(_T("INTERNAL")) == 0)
  {
    //locat = in file
    bFreeFormat = true;
    locat = 1;
    iRead = _stscanf(cStr, _T("%*s %lf %s %d"), &cnstnt, fmtin.GetBuffer(), &iprn);
    fmtin.ReleaseBuffer();
  }
  else if (fmtin.CompareNoCase(_T("EXTERNAL")) == 0)
  {
    bFreeFormat = true;
    locat = 1;
    // locat = external 
  }
  else if (fmtin.CompareNoCase(_T("OPEN/CLOSE")) == 0)
  {
    bFreeFormat = true;
    locat = 1;
    // locat = open close
  }
  else // fixed format
  {
    bFreeFormat = false;
    fmtin = _T("                    ");
    _stscanf(cStr, _T("%10d%10lf%20[^\n]%10d"), &locat, &cnstnt, fmtin.GetBuffer(), &iprn);
    //xscanf(cStr,_T("%10d %10lf %20c %10d"),&locat,&cnstnt,fmtin.GetBuffer(),&iprn);
    fmtin.ReleaseBuffer();
    fmtin = fmtin.Trim();
  }
  CPPMatrix2<double>* pMatrix = (CPPMatrix2<double>*) rArray;
  CPPMatrix2<int>* pIMatrix = (CPPMatrix2<int>*) rArray;
  vector<double>* pVector = (vector<double>*) rArray;
  vector<int>* pIVector = (vector<int>*) rArray;
  if (locat == 0)
  {
    if (bMatrix)
    {
      int nCol = pMatrix->nCols;
      int nRow = pMatrix->nRows;
      for (int j = 0; j < nRow; j++)
      {
        for (int i = 0; i < nCol; i++)
        {
          if (bInt)
            (*pIMatrix)[j][i] = cnstnt;
          else
            (*pMatrix)[j][i] = cnstnt;
        }
      }
    }
    else
    {
      int n = pVector->size();
      for (int i = 0; i < n; i++)
      {
        if (bInt)
          (*pIVector)[i] = cnstnt;
        else
          (*pVector)[i] = cnstnt;
      }
    }
  }
  else if (bFreeFormat)
  {
    if (bMatrix)
    {
      int nCol = pMatrix->nCols;
      int nRow = pMatrix->nRows;
      for (int m = 0; m < nRow; m++)
      {
        for (int l = 0; l < nCol; l = l++)
        {
          if (bInt)
            _ftscanf(fp1, _T("%d"), &(*pIMatrix)[m][l]);
          else
            _ftscanf(fp1, _T("%lf"), &(*pMatrix)[m][l]);
        }
      }
    }
    else
    {
      int n = pVector->size();
      for (int i = 0; i < n; i++)
      {
        if (bInt)
        {
          _ftscanf(fp1, _T("%d"), &((*pIVector)[i]));
        }
        else
        {
          _ftscanf(fp1, _T("%lf"), &((*pVector)[i]));
        }
      }
    }
    _fgetts(cStr.GetBuffer(MAX_STR), MAX_STR, fp1);
    cStr.ReleaseBuffer();
  }
  else // Formatted
  {
    iRead = _stscanf(fmtin, _T("(%d"), &nFmtCol);
    if (iRead == 1 && nFmtCol > 0)
    {
      _stscanf(fmtin, _T("(%d%*c%d."), &nFmtCol, &nWidth);
    }
    else
      return;
    if (bInt)
    {
      fmtin.Format(_T("%%%dd"), nWidth);
    }
    else
    {
      fmtin.Format(_T("%%%dlf"), nWidth);
    }
    if (bMatrix)
    {
      int nCol = pMatrix->nCols;
      int nRow = pMatrix->nRows;
      TCHAR *strBuffer = new TCHAR[nFmtCol*nWidth + 2];
      TCHAR *tBuf = new TCHAR[nWidth + 1];
      TCHAR *mfmtin = (TCHAR*)fmtin.GetString();
      tBuf[nWidth] = '\0';
      for (int j = 0; j < nRow; j++)
      {
        for (int l = 0; l < nCol; l = l + nFmtCol)
        {
          _fgetts(strBuffer, nFmtCol*nWidth + 2, fp1);
          if (_tcschr(strBuffer, _T('\n')) == NULL)
          {
            do {
              _fgetts(tBuf, nWidth + 1, fp1);
            } while (_tcschr(tBuf, _T('\n')) == NULL);
          }
          for (int k = 0; l + k < nCol && k < nFmtCol; k++)
          {
            _tcsncpy(tBuf, strBuffer + k*nWidth, nWidth);
            if (bInt)
              _stscanf(tBuf, mfmtin, &(*pIMatrix)[j][l + k]);
            //stconvert(cStr.Mid(k*nWidth,nWidth),fmtin,&(*pIMatrix)[j][l+k]);
            else
              _stscanf(tBuf, mfmtin, &(*pMatrix)[j][l + k]);
            //stconvert(cStr.Mid(k*nWidth,nWidth),fmtin,&(*pMatrix)[j][l+k]);
          }
        }
      }
      fmtin.ReleaseBuffer();
      delete tBuf;
      delete strBuffer;
    }
    else
    {
      int n = pVector->size();
      for (int i = 0; i < n; i = i + nFmtCol)
      {
        _fgetts(cStr.GetBuffer(n*nWidth + MAX_STR), n*nWidth + MAX_STR, fp1);
        cStr.ReleaseBuffer(n*nWidth);
        for (int j = 0; i + j < n && j < nFmtCol; j++)
        {
          if (bInt)
          {
            stconvert(cStr.Mid(j*nWidth, nWidth), fmtin, &((*pIVector)[i + j]));
          }
          else
          {
            stconvert(cStr.Mid(j*nWidth, nWidth), fmtin, &((*pVector)[i + j]));
          }
        }
      }
    }
  }
}

void ReadMT3DMSArray(FILE *fp1, void* rArray, bool bMatrix, bool bInt)
{
  CString cStr, fmtin;
  int iread, iprn, nFmtCol, nWidth, nRead; //iRead, 
  double cnstnt;
  _fgetts(cStr.GetBuffer(MAX_STR), MAX_STR, fp1);
  fmtin = _T("                    ");
  nRead = _stscanf(cStr, _T("%10d%10lf%20[^\n]%10d"), &iread, &cnstnt, fmtin.GetBuffer(), &iprn);
  //xscanf(cStr,_T("%10d %10lf %20c %10d"),&iread,&cnstnt,fmtin.GetBuffer(),&iprn);
  fmtin.ReleaseBuffer();
  fmtin = fmtin.Trim();
  nRead = _stscanf(fmtin, _T("(%d"), &nFmtCol);
  if (nRead == 1 && nFmtCol > 0)
  {
    nRead = _stscanf(fmtin, _T("(%d%*c%d."), &nFmtCol, &nWidth);
    if (nRead == 1)
      nRead = _stscanf(fmtin, _T("(%d%*2c%d."), &nFmtCol, &nWidth);
  }
  if ((iread == 100 || iread == 101) && (nFmtCol < 1 || nWidth < 1)) return;
  ArrayF2D::array_view<2>::type *pMatrix = (ArrayF2D::array_view<2>::type *) rArray;
  ArrayI2D::array_view<2>::type *pIMatrix = (ArrayI2D::array_view<2>::type *) rArray;
  vector<float>* pVector = (vector<float>*) rArray;
  vector<int>* pIVector = (vector<int>*) rArray;
  switch (iread)
  {
  case 0:
    if (bMatrix)
    {
      int nRow = pMatrix->shape()[0];
      int nCol = pMatrix->shape()[1];
      for (int j = 0; j < nRow; j++)
      {
        for (int i = 0; i < nCol; i++)
        {
          if (bInt)
            (*pIMatrix)[j][i] = cnstnt;
          else
            (*pMatrix)[j][i] = cnstnt;
        }
      }
    }
    else
    {
      int n = pVector->size();
      for (int i = 0; i < n; i++)
      {
        if (bInt)
          (*pIVector)[i] = cnstnt;
        else
          (*pVector)[i] = cnstnt;
      }
    }
    break;
  case 1: // Due to MODFLOW bug.
  case 100:
    if (bInt)
    {
      fmtin.Format(_T("%%%dd"), nWidth);
    }
    else
    {
      fmtin.Format(_T("%%%df"), nWidth);
    }
    if (bMatrix)
    {
      int nRow = pMatrix->shape()[0];
      int nCol = pMatrix->shape()[1];
      for (int j = 0; j < nRow; j++)
      {
        for (int l = 0; l < nCol; l = l + nFmtCol)
        {
          _fgetts(cStr.GetBuffer(nCol*nWidth + MAX_STR), nCol*nWidth + MAX_STR, fp1);
          cStr.ReleaseBuffer(nCol*nWidth);
          for (int k = 0; l + k < nCol && k < nFmtCol; k++)
          {
            if (bInt)
              stconvert(cStr.Mid(k*nWidth, nWidth), fmtin, &(*pIMatrix)[j][l + k]);
            else
              stconvert(cStr.Mid(k*nWidth, nWidth), fmtin, &(*pMatrix)[j][l + k]);
          }
        }
      }
    }
    else
    {
      int n = pVector->size();
      for (int i = 0; i < n; i = i + nFmtCol)
      {
        _fgetts(cStr.GetBuffer(n*nWidth + MAX_STR), n*nWidth + MAX_STR, fp1);
        cStr.ReleaseBuffer(n*nWidth);
        for (int j = 0; i + j < n && j < nFmtCol; j++)
        {
          if (bInt)
          {
            stconvert(cStr.Mid(j*nWidth, nWidth), fmtin, &((*pIVector)[i + j]));
          }
          else
          {
            stconvert(cStr.Mid(j*nWidth, nWidth), fmtin, &((*pVector)[i + j]));
          }
        }
      }
    }
    break;
  case 101:
    int nblock, i_start, i_end, j_start, j_end;
    double tmp_val;
    _fgetts(cStr.GetBuffer(MAX_STR), MAX_STR, fp1);
    cStr.ReleaseBuffer();
    _stscanf(cStr, _T("%d"), &nblock);
    for (int k = 0; k < nblock; k++)
    {
      _fgetts(cStr.GetBuffer(MAX_STR), MAX_STR, fp1);
      cStr.ReleaseBuffer();
      _stscanf(cStr, _T("%d %d %d %d %d"), &i_start, &i_end, &j_start, &j_end, &tmp_val);
      if (bMatrix)
      {
        int nRow = pMatrix->shape()[0];
        int nCol = pMatrix->shape()[1];
        for (int j = j_start - 1; j < j_end; j++)
        {
          for (int i = i_start - 1; i < i_end; i++)
          {
            if (bInt)
              (*pIMatrix)[j][i] = tmp_val;
            else
              (*pMatrix)[j][i] = tmp_val;
          }
        }
      }
      else
      {
        int n = pVector->size();
        if (i_start == i_end)
        {
          i_start = j_start; i_end = j_end;
        }
        for (int i = i_start - 1; i < i_end; i++)
        {
          if (bInt)
            _ftscanf(fp1, _T("%d"), &((*pIVector)[i]));
          else
            _ftscanf(fp1, _T("%f"), &((*pVector)[i]));
        }
      }
      _fgetts(cStr.GetBuffer(MAX_STR), MAX_STR, fp1);
      cStr.ReleaseBuffer();
    }
    break;
  case 102: //fixed format

    break;
  case 103: //free format
    break;
  default:
    break;
  }
}

void Read_LKMT_Layers(PHT3D_Model& mPHT3DM, int fh1, vector<vector<CPPMatrix2<float> > > *Value, int l, bool *bContinue)
{
  //PLOTClass& plot = mPHT3DM.PLOT;
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  //MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;
  //PHT3DClass& pht3d = mPHT3DM.PHT3D;

  int tIval;
  TCHAR str[MAX_STR];
  _int64 iSeekOffset;
  //double tval;
  //vector<float>::pointer ptr;
  iSeekOffset = _telli64(fh1);
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  int bytes_read = _read(fh1, str, sizeof(MT3D_LKMT4_SubHead));
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  if (bytes_read != sizeof(MT3D_LKMT4_SubHead))
  {
    *bContinue = false; return;
  }
  MT3D_LKMT4_SubHead *pLKMT4SubHead = (MT3D_LKMT4_SubHead*)str;
  if (pLKMT4SubHead->NROW != mf.DIS_NROW || pLKMT4SubHead->NCOL != mf.DIS_NCOL)
    MessageBox(NULL, _T("Head data do not match with input file"), 0, 0);
  CString tCstr = L"LKMT-" + CString(pLKMT4SubHead->LABEL, 16).Trim();
  int idx = mf.AddData(tCstr);
  if (l == 0)
  {
    mf.Data[idx].iSeekHeadOffset.resize(mf.nTotalSteps);
    mf.Data[idx].iSeekDataOffset.resize(mf.nTotalSteps);
    mf.Data[idx].bHeadPerLayer = false;
    mf.Data[idx].bColumnRead = false;
    mf.Data[idx].nHeadBytes = sizeof(MT3D_LKMT4_SubHead);
    mf.Data[idx].nStrideBytes = sizeof(float)*mf.DIS_NLAY*mf.DIS_NROW*mf.DIS_NCOL;
    mf.Data[idx].Data_Pointer = (void*)Value;
    mf.Data[idx].Data_Type = 0; // time, layer, row, column type
    mf.Data[idx].FileName = mf.LMT6_Name;
  }
  int idx2 = mf.GetTimeStep(pLKMT4SubHead->KPER, pLKMT4SubHead->KSTP);
  if (idx2 >= 0)
  {
    mf.Data[idx].iSeekHeadOffset[idx2] = iSeekOffset + 1;
    mf.Data[idx].iSeekDataOffset[idx2] = iSeekOffset + 1 + sizeof(MT3D_LKMT4_SubHead);
  }
  else
    MessageBox(NULL, _T("Could not find MODFLOW Time PER and STP!!"), 0, 0);
  if (mf.bFortranBinary)
  {
    mf.Data[idx].iSeekDataOffset[idx2] += 2 * sizeof(int);
    _lseeki64(fh1, mf.Data[idx].nStrideBytes + 2 * sizeof(int), SEEK_CUR);
  }
  else
  {
    _lseeki64(fh1, mf.Data[idx].nStrideBytes, SEEK_CUR);
  }
}

void Read_LKMT_Layer(PHT3D_Model& mPHT3DM, int fh1, vector<CPPMatrix2<int> > *IValue, vector<CPPMatrix2<float> > *Value, int l, bool *bContinue)
{
  //PLOTClass& plot = mPHT3DM.PLOT;
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  //MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;
  //PHT3DClass& pht3d = mPHT3DM.PHT3D;

  int tIval;
  TCHAR str[MAX_STR];
  //double tval;
  vector<float>::pointer ptr;
  vector<int>::pointer ptrI;
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  int bytes_read = _read(fh1, str, sizeof(MT3D_LKMT4_SubHead));
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  if (bytes_read != sizeof(MT3D_LKMT4_SubHead))
  {
    *bContinue = false; return;
  }
  MT3D_LKMT4_SubHead *pLKMT4SubHead = (MT3D_LKMT4_SubHead*)str;
  if (pLKMT4SubHead->NROW != mf.DIS_NROW || pLKMT4SubHead->NCOL != mf.DIS_NCOL)
    MessageBox(NULL, _T("Head data do not match with input file"), 0, 0);
  CString tCstr = _T("LKMT-I-") + CString(pLKMT4SubHead->LABEL, 16).Trim();
  int idx = mf.AddData(tCstr);
  CString tCstr2 = _T("LKMT-") + CString(pLKMT4SubHead->LABEL, 16).Trim();
  int idx2 = mf.AddData(tCstr2);
  if (l == 0)
  {
    mf.Data[idx].Data_Pointer = (void*)IValue;
    mf.Data[idx].Data_Type = 1;// integer layer, row, column type
                                            //mf.Data[idx].FileName = mf.LMT6_Name;
    mf.Data[idx2].Data_Pointer = (void*)Value;
    mf.Data[idx2].Data_Type = 2;// float layer, row, column type
                                             //mf.Data[idx2].FileName = mf.LMT6_Name;
  }
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  for (int j = 0; j < mf.DIS_NROW; j++)
  {
    ptrI = &((*IValue)[l][j][0]);
    bytes_read = _read(fh1, ptrI, mf.DIS_NCOL * sizeof(int));
  }
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  for (int j = 0; j < mf.DIS_NROW; j++)
  {
    ptr = &((*Value)[l][j][0]);
    bytes_read = _read(fh1, ptr, mf.DIS_NCOL * sizeof(float));
  }
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
}

void Read_LKMT_Lists(PHT3D_Model& mPHT3DM, int fh1, vector<vector<CMT3D_List> > *Value, int l, bool *bContinue)
{
  //PLOTClass& plot = mPHT3DM.PLOT;
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  //MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;
  //PHT3DClass& pht3d = mPHT3DM.PHT3D;

  int tIval;
  TCHAR str[MAX_STR];
  double tval;
  vector<CMT3D_List>::pointer ptr;
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  int bytes_read = _read(fh1, str, sizeof(MT3D_LKMT4_SubHead2));
  if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  if (bytes_read != sizeof(MT3D_LKMT4_SubHead2))
  {
    *bContinue = false; return;
  }
  MT3D_LKMT4_SubHead2 *pLKMT4SubHead = (MT3D_LKMT4_SubHead2*)str;
  if (pLKMT4SubHead->NROW != mf.DIS_NROW || pLKMT4SubHead->NCOL != mf.DIS_NCOL)
    MessageBox(NULL, _T("Head data do not match with input file"), 0, 0);
  CString tCstr = _T("LKMT-") + CString(pLKMT4SubHead->LABEL, 16).Trim();
  int idx = mf.AddData(tCstr);
  if (l == 0)
  {
    mf.Data[idx].bHeadPerLayer = false;
    mf.Data[idx].bColumnRead = false;
    mf.Data[idx].Data_Pointer = (void*)Value;
    mf.Data[idx].Data_Type = 3;//float list type
  }
  if (pLKMT4SubHead->N > 0)
  {
    (*Value)[l].resize(pLKMT4SubHead->N);
    if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
    for (int k = 0; k < pLKMT4SubHead->N; k++)
    {
      ptr = &((*Value)[l][k]);
      bytes_read = _read(fh1, ptr, sizeof(MT3D_List));
      tval = (*Value)[l][k].R;
    }
    if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
  }
}

void Read_LKMT_Package(PHT3D_Model& mPHT3DM, CString sPlotDirectory)
{ //Read mt3d.flo
  //PLOTClass& plot = mPHT3DM.PLOT;
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;
  //PHT3DClass& pht3d = mPHT3DM.PHT3D;

  char str[MAX_STR * 2 + 1];
  int iTmp, fh1, bytes_read, tIval;
  CString tCstr, tCstr2;
  tCstr = mf.LMT6_Name;
  iTmp = sPlotDirectory.ReverseFind(_T('\\'));
  sPlotDirectory = sPlotDirectory.Left(iTmp + 1) + tCstr;
  fh1 = NULL;
  if ((fh1 = _topen(sPlotDirectory, _O_RDONLY | _O_BINARY | _O_SEQUENTIAL)) == -1)
  {
    tCstr.MakeLower();
    sPlotDirectory = sPlotDirectory.Left(iTmp + 1) + tCstr;
    fh1 = _topen(sPlotDirectory, _O_RDONLY | _O_BINARY | _O_SEQUENTIAL);
  }
  if (fh1 == -1)
  {
    MessageBox(NULL, _T("Can't open ") + tCstr, 0, 0);
    mf.bLMT6Data = false;
    return;
  }
  else
  {
    mf.bLMT6Data = true;
    bool bContinue = true;
    bytes_read = _read(fh1, &tIval, sizeof(int));
    if (tIval != 0x4433544d)
    {
      mf.bFortranBinary = true;
    }
    _lseeki64(fh1, 0, SEEK_SET);
    if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
    bytes_read = _read(fh1, str, sizeof(MT3D_LKMT3_Head));
    if (strncmp(str, "MT3D3.00", 8) == 0)
    {
      mf.LMT6_Version = 3;
    }
    else
    {
      mf.LMT6_Version = 4;
      _lseeki64(fh1, 0, SEEK_SET);
      if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
      bytes_read = _read(fh1, str, sizeof(MT3D_LKMT4_Head));
      if (bytes_read != sizeof(MT3D_LKMT4_Head))
      {
        bContinue = false;
      }
    }
    mt3dms.LKMT4_Head = *((MT3D_LKMT4_Head*)str);
    tCstr = CString(mt3dms.LKMT4_Head.VERSION, 11);
    if (mf.bFortranBinary) _read(fh1, &tIval, sizeof(int));
    // Constant head boundary Jungho MTCHD error of MODFLOW2005
    //        if(mt3dms.LKMT4_Head.MTCHD>0)
    {
      mt3dms.LKMT4_CNH.resize(mf.nTotalSteps);
    }
    if (mt3dms.LKMT4_Head.MTWEL > 0)
    {
      mt3dms.LKMT4_WEL.resize(mf.nTotalSteps);
    }
    if (mt3dms.LKMT4_Head.MTDRN > 0)
    {
      mt3dms.LKMT4_DRN.resize(mf.nTotalSteps);
    }
    if (mt3dms.LKMT4_Head.MTRCH > 0)
    {
      mt3dms.LKMT4_IRCH.resize(mf.nTotalSteps);
      mt3dms.LKMT4_RECH.resize(mf.nTotalSteps);
      for (int l = 0; l < mf.nTotalSteps; l++)
      {
        mt3dms.LKMT4_IRCH[l].Resize(mf.DIS_NROW, mf.DIS_NCOL);
        mt3dms.LKMT4_RECH[l].Resize(mf.DIS_NROW, mf.DIS_NCOL);
      }
    }
    if (mt3dms.LKMT4_Head.MTEVT > 0)
    {
      mt3dms.LKMT4_IEVT.resize(mf.nTotalSteps);
      mt3dms.LKMT4_EVTR.resize(mf.nTotalSteps);
      for (int l = 0; l < mf.nTotalSteps; l++)
      {
        mt3dms.LKMT4_IEVT[l].Resize(mf.DIS_NROW, mf.DIS_NCOL);
        mt3dms.LKMT4_EVTR[l].Resize(mf.DIS_NROW, mf.DIS_NCOL);
      }
    }
    if (mt3dms.LKMT4_Head.MTRIV > 0)
    {
      mt3dms.LKMT4_RIV.resize(mf.nTotalSteps);
    }
    if (mt3dms.LKMT4_Head.MTGHB > 0)
    {
      mt3dms.LKMT4_GHB.resize(mf.nTotalSteps);
    }
    /*double min_THKSAT=1e30, max_THKSAT=-1e30;
    double min_QX=1e30, max_QX=-1e30;
    double min_QY=1e30, max_QY=-1e30;
    double min_QZ=1e30, max_QZ=-1e30;
    double min_QSTO=1e30, max_QSTO=-1e30;
    double min_CNH=1e30, max_CNH=-1e30;
    double min_WEL=1e30, max_WEL=-1e30;
    double min_DRN=1e30, max_DRN=-1e30;
    double min_RECH=1e30, max_RECH=-1e30;
    double min_EVTR=1e30, max_EVTR=-1e30;
    double min_RIV=1e30, max_RIV=-1e30;
    double min_GHB=1e30, max_GHB=-1e30;*/
    //int k;
    //_int64 nTimeStepBytes;
    mt3dms.LKMT4_Start_Index = mf.Data.size();
    // read MT3DMS.LKMT4_THKSAT
    for (int l = 0; l < mf.nTotalSteps && bContinue; l++)
    {
      Read_LKMT_Layers(mPHT3DM, fh1, NULL, l, &bContinue);
      //int idx = mf.AddData(_T("LKMT-THKSAT"));
      // read MT3DMS.LKMT4_QX
      if (mf.DIS_NCOL > 1)
        Read_LKMT_Layers(mPHT3DM, fh1, NULL, l, &bContinue);
      // read MT3DMS.LKMT4_QY
      if (mf.DIS_NROW > 1)
        Read_LKMT_Layers(mPHT3DM, fh1, NULL, l, &bContinue);
      // read MT3DMS.LKMT4_QZ
      if (mf.DIS_NLAY > 1)
        Read_LKMT_Layers(mPHT3DM, fh1, NULL, l, &bContinue);
      if (mt3dms.LKMT4_Head.MTISS == 0)
        Read_LKMT_Layers(mPHT3DM, fh1, NULL, l, &bContinue);
      // Constant head boundary Jungho MTCHD error of MODFLOW2005
      Read_LKMT_Lists(mPHT3DM, fh1, &(mt3dms.LKMT4_CNH), l, &bContinue);
      if (mt3dms.LKMT4_Head.MTWEL > 0)
        Read_LKMT_Lists(mPHT3DM, fh1, &(mt3dms.LKMT4_WEL), l, &bContinue);
      if (mt3dms.LKMT4_Head.MTDRN > 0)
        Read_LKMT_Lists(mPHT3DM, fh1, &(mt3dms.LKMT4_DRN), l, &bContinue);
      if (mt3dms.LKMT4_Head.MTRCH > 0)
        Read_LKMT_Layer(mPHT3DM, fh1, &(mt3dms.LKMT4_IRCH), &(mt3dms.LKMT4_RECH), l, &bContinue);
      if (mt3dms.LKMT4_Head.MTEVT > 0)
        Read_LKMT_Layer(mPHT3DM, fh1, &(mt3dms.LKMT4_IEVT), &(mt3dms.LKMT4_EVTR), l, &bContinue);
      if (mt3dms.LKMT4_Head.MTRIV > 0)
        Read_LKMT_Lists(mPHT3DM, fh1, &(mt3dms.LKMT4_RIV), l, &bContinue);
      if (mt3dms.LKMT4_Head.MTGHB > 0)
        Read_LKMT_Lists(mPHT3DM, fh1, &(mt3dms.LKMT4_GHB), l, &bContinue);
    }
    _close(fh1);

  }
}

int Read_Name_File(PHT3D_Model& mPHT3DM, CString sPlotDirectory)
{
  //PLOTClass& plot = mPHT3DM.PLOT;
  MODFLOWClass& mf = mPHT3DM.MODFLOW;
  //MT3DMSClass& mt3dms = mPHT3DM.MT3DMS;
  //PHT3DClass& pht3d = mPHT3DM.PHT3D;

  FILE *fp1;
  CString tCstr, tCstr1, tCstr2, tCstr3;
  int iTmp, nUnit;
  if ((fp1 = _tfopen(sPlotDirectory, _T("rt"))) == NULL)
  {
    MessageBox(NULL, _T("Can't open ") + sPlotDirectory, 0, 0); return -1;
  }
  else
  {
    do
    {
      // Read Name File
      _fgetts(tCstr.GetBuffer(MAX_STR), MAX_STR, fp1);
      tCstr.ReleaseBuffer();
      if (feof(fp1)) break;
      iTmp = _stscanf(tCstr, _T("%s %d %s %s"), tCstr1.GetBuffer(MAX_STR), &nUnit,
        tCstr2.GetBuffer(MAX_STR), tCstr3.GetBuffer(MAX_STR));
      tCstr1.ReleaseBuffer();
      tCstr2.ReleaseBuffer(); 
      tCstr3.ReleaseBuffer();
      if (iTmp >= 3)
      {
        if (tCstr1 == "DATA(BINARY)" && (nUnit == 30 || nUnit == 51))
          iTmp = mf.NameFile.Add("Head", nUnit, tCstr2);
        else if (tCstr1 == "DATA(BINARY)" && (nUnit == 31 || nUnit == 50))
          iTmp = mf.NameFile.Add("Budget", nUnit, tCstr2);
        else if (tCstr1 == "DATA(BINARY)" && (nUnit == 52))
          iTmp = mf.NameFile.Add("Drawdown", nUnit, tCstr2);
        else
          iTmp = mf.NameFile.Add(tCstr1, nUnit, tCstr2);
      }
    } while (1);
    fclose(fp1);
  }
  return 0;
}


/*
int FetchARecord(int fh1, char *buffer, int *recLen)
{
unsigned int bytesread;
if ((bytesread = _read(fh1, buffer, 4)) <= 0)
{
return bytesread;// error
}
else
{
*recLen = *((int*)buffer);
bytesread = _read(fh1, buffer, (*recLen + 4));
if (bytesread <= 0)
{  //error
}
return bytesread;
}
}

int ParseDelimiters(wchar_t* pStr, wchar_t* pDelimeters, wchar_t *pwch[], int maxN)
{
int i, n;
size_t sLen;
sLen = wcslen(pStr);
pwch[0] = pStr;
for (i = 0; i < maxN - 1; i++)
{
if (pwch[i] >= pStr + sLen) return i;
n = wcscspn(pwch[i], pDelimeters);
pwch[i][n] = L'\0';
pwch[i + 1] = pwch[i] + n + 1;
}
return i;
}
*/
