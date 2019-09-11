// ModelRead.h : interface or class definitiions of MODFLOW models
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2015
// Description: Defines various structures and classes for Groundwater models.
//   Three main classes (MODFLOWClass, PHT3DClass, MT3DMSClass) to host model
//   data are defined here. All other classes and data strctures are used by 
//   these three main classes. Some Boost::multi_array matrix types are declares
//   for 2D, 3D, and 4D matrix data.
/////////////////////////////////////////////////////////////////////////////

#pragma once

// The following data structure is fixed in size. So, packing should be aligned
// to 1 byte boundary.
#pragma pack(push,1)
// Header of the UCN Files produced by MT3DMS 3D models
struct MT3D_UCN_Head
{
  _int32 NTRANS; // Number of Transport steps
  _int32 KSTP;   // Number of steps per period
  _int32 KPER;   // Number of periods
  float TIME2;   // Current time of the simulation
  char TEXT[16]; // Text to indicate the head
  _int32 NCOL;   // Number of columns of the model
  _int32 NROW;   // Number of rows of the model
  _int32 ILAY;   // Index of the layer of the model
};

struct MT3D_LKMT3_Head
{
  char VERSION[11];
  _int32 MTWEL;  // Number of wells
  _int32 MTDRN;  // Numeer of drains
  _int32 MTRCH;  // Number of recharge
  _int32 MTEVT;  // Number of evaporation
  _int32 MTRIV;  // Number of rivers
  _int32 MTGHB;  // Number of Generalized Head boundaries
  _int32 MTCHD;  // Number CHD
  _int32 MTISS;  // Number of ISS
  _int32 MTNPER; // Number of periods
};

struct MT3D_LKMT4_Head
{
  char VERSION[11];
  _int32 MTWEL;
  _int32 MTDRN;
  _int32 MTRCH;
  _int32 MTEVT;
  _int32 MTRIV;
  _int32 MTGHB;
  _int32 MTCHD;
  _int32 MTISS;
  _int32 NPERFL;
  _int32 MTSTR;
  _int32 MTRES;
  _int32 MTFHB;
  _int32 MTDRT;
  _int32 MTETS;
  _int32 MTTLK;
  _int32 MTIBS;
  _int32 MTLAK;
  _int32 MTMNW;
  _int32 MTSWT;
  _int32 MTSFR;
  _int32 MTUZF;
  void clear(void) {
    std:fill(VERSION, VERSION+11,0);
    MTWEL = 0;
    MTDRN = 0;
    MTRCH = 0;
    MTEVT = 0;
    MTRIV = 0;
    MTGHB = 0;
    MTCHD = 0;
    MTISS = 0;
    NPERFL = 0;
    MTSTR = 0;
    MTRES = 0;
    MTFHB = 0;
    MTDRT = 0;
    MTETS = 0;
    MTTLK = 0;
    MTIBS = 0;
    MTLAK = 0;
    MTMNW = 0;
    MTSWT = 0;
    MTSFR = 0;
    MTUZF = 0;
  }

  MT3D_LKMT4_Head() {
    clear();
  }
};

struct MT3D_LKMT4_SubHead
{
  _int32 KPER;
  _int32 KSTP;
  _int32 NCOL;
  _int32 NROW;
  _int32 NLAY;
  char LABEL[16];
};

struct MT3D_LKMT4_SubHead2
{
  _int32 KPER;
  _int32 KSTP;
  _int32 NCOL;
  _int32 NROW;
  _int32 NLAY;
  char LABEL[16];
  _int32 N;
};

// MT3DMS list structure.
// It is composed of K(layer), I(column), and J(row), and Q(value)
struct MT3D_List
{
  _int32 K;
  _int32 I;
  _int32 J;
  float Q;
};

struct MT3D_Group_List
{
  _int32 K;
  _int32 I;
  _int32 J;
  float Q;
  _int32 IGROUP;
  float Q2;
};

// Head of MODFLOW FLOW files
struct MODFLOW_HYDRAULIC_HEAD_Head
{
  _int32 KSTP;
  _int32 KPER;
  float PERTIM;
  float TOTIM;
  char TEXT[16];
  _int32 NCOL;
  _int32 NROW;
  _int32 ILAY;
};

// Head of MODFLOW budget files
struct MODFLOW_BUDGET_IBD2_Head
{
  _int32 TYPE;
  float DELT;
  float PERTIM;
  float TOTIM;
};

// Head of MODFLOW budget files
struct MODFLOW_BUDGET_Head
{
  _int32 KSTP;
  _int32 KPER;
  char TEXT[16];
  _int32 NCOL;
  _int32 NROW;
  _int32 NLAY;
};

#pragma pack(pop) 

// Color class to handle colour mixture
class CColor
{
public:
  unsigned char R, G, B;
  CColor()
  {
    CColor(0, 0, 0);
  }
  CColor(int iR, int iG, int iB)
  {
    R = iR;
    G = iG;
    B = iB;
  }
  CColor Mix(CColor c2, float fraction_c1)
  {
    CColor clr;
    float fraction_c2 = 1.0f - fraction_c1;
    clr.R = (unsigned char)(R*fraction_c1 + c2.R*fraction_c2);
    clr.G = (unsigned char)(G*fraction_c1 + c2.G*fraction_c2);
    clr.B = (unsigned char)(B*fraction_c1 + c2.B*fraction_c2);
    return clr;
  }
};

// Cube strcture to represent a cell of the 3D models
class CCubeD
{
public:
  double left;
  double right;
  double top;
  double bottom;
  double front;
  double back;
  void clear(void){
    left = 0;
    right = 0;
    top = 0;
    bottom = 0;
    front = 0;
    back = 0;
  }
  CCubeD() {
    clear();
  }
};

// Mobile kinetic elements for PHT3D
class CKinetic_Mobile
{
public:
  CString Name;
  vector<double> parameter;
  CString formula;
};

// kinetic mineral elements for PHT3D
class CKinetic_Mineral
{
public:
  CString Name;
  vector<double> parameter;
};

// ion exchange elements for PHT3D
class CIon_Exchange
{
public:
  CString Name;
  int Stoic_coef;
  CString Name_EMS;
};

// surface elements for PHT3D
class CSurface_Complex
{
public:
  CString Name;
  double SurfaceArea, Mass;
  CString Mineral_Name;
  CString SWITCH;
};

// locations of observational wells
class CObservation_Well
{
public:
  int KOBS, IOBS, JOBS;
};

// The range of concentrations
class CConcentrationRange
{
public:
  double Min, Max;
};

class CHydraulic_Head_Info
{
public:
  double KSTP, KPER, PERTIM, TOTIM;
};

class CScatterPlotData
{
public:
  double X;
  double Y;
};

class CStressPeriod
{
public:
  double PERLEN, TSMULT;
  int NSTP;
  bool bSteadyState;
};

class CMT3D_List
{
public:
  int K, I, J;
  float R;
};

// Solution master species for PhreeqC or PHT3D thermodynamic database
class CSolutionMasterSpecies
{
public:
  CString Element;
  CString MasterSpecies;
  double Alkalinity;
  CString Gfw_formula;
  double Element_gfw;
  bool bActive;
};

// Solution species for PhreeqC or PHT3D thermodynamic database
class CSolutionSpecies
{
public:
  vector<CString> MasterSpecies;
  vector<double> MasterSpeciesCoefficients;
  CString Species;
  double SpeciesCoefficient;
  vector<CString> Lines;
  bool bActive;
};

// Equilibrium phases for PhreeqC or PHT3D thermodynamic database
class CPhases
{
public:
  CString Name;
  vector<CString> MasterSpecies;
  vector<double> MasterSpeciesCoefficients;
  CString Species;
  double SpeciesCoefficient;
  vector<CString> Lines;
  bool bActive;
};

// Exchange master species for PhreeqC or PHT3D thermodynamic database
class CExchangeMasterSpecies
{
public:
  vector<CString> Species;
  CString MainSpecies;
  vector<CString> Lines;
  bool bActive;
};

// Exchange species for PhreeqC or PHT3D thermodynamic database
class CExchangeSpecies
{
public:
  vector<CString> Species;
  CString MainSpecies;
  vector<CString> Lines;
  bool bActive;
};

// Surface master species for PhreeqC or PHT3D thermodynamic database
class CSurfaceMasterSpecies
{
public:
  vector<CString> Species;
  CString MainSpecies;
  vector<CString> Lines;
  bool bActive;
};

// Surface species for PhreeqC or PHT3D thermodynamic database
class CSurfaceSpecies
{
public:
  CString Element;
  double Alkalinity;
  bool bActive;
};

// Reaction rates for PhreeqC or PHT3D
class CRates
{
public:
  CString Element;
  double Alkalinity;
  bool bActive;
};

// Sit models for PhreeqC or PHT3D
class CSit
{
public:
  CString Species1;
  CString Species2;
  double Coefficient;
  bool bActive;
};

// Main keywords for PhreeqC
static const TCHAR *PHREEQC_Keywords[] = { _T("SOLUTION_MASTER_SPECIES"),_T("SIT"),_T("SOLUTION_SPECIES"),_T("PHASES"),_T("EXCHANGE_MASTER_SPECIES")
,_T("EXCHANGE_SPECIES"),_T("SURFACE_MASTER_SPECIES"),_T("SURFACE_SPECIES"),_T("RATES") };

// Thermodyanic database of PhreeqC
class CThermodynamicDatabase
{
public:
  vector<CSolutionMasterSpecies> SOLUTION_MASTER_SPECIES;
  vector<CSolutionSpecies> SOLUTION_SPECIES;
  vector<CPhases> PHASES;
  vector<CExchangeMasterSpecies> EXCHANGE_MASTER_SPECIES;
  vector<CExchangeSpecies> EXCHANGE_SPECIES;
  vector<CSurfaceMasterSpecies> SURFACE_MASTER_SPECIES;
  vector<CSurfaceSpecies> SURFACE_SPECIES;
  vector<CRates> RATES;
  vector<CSit> SIT;
  int SIT_TYPE;// epsilon=1 or epsilon1=2
  int FindTitle(CString tCstr)
  {
    for (int i = 0; i < (sizeof PHREEQC_Keywords) / (sizeof PHREEQC_Keywords[0]); i++)
    {
      if (tCstr.CompareNoCase(PHREEQC_Keywords[i]) == 0) return i;
    }
    return -1;
  }
  int FindMasterSpecies(CString tCstr)
  {
    for (int i = 0; i < SOLUTION_MASTER_SPECIES.size(); i++)
    {
      if (tCstr.CompareNoCase(SOLUTION_MASTER_SPECIES[i].MasterSpecies) == 0) return i;
    }
    return -1;
  }
  int FindSolutionSpecies(CString tCstr)
  {
    for (int i = SOLUTION_SPECIES.size() - 1; i >= 0; i--)
    {
      if (tCstr.CompareNoCase(SOLUTION_SPECIES[i].Species) == 0) return i;
    }
    return -1;
  }
  int FindSolutionMasterSpecies(CString tCstr)
  {
    for (int i = 0; i < SOLUTION_SPECIES.size(); i++)
    {
      if (tCstr.CompareNoCase(SOLUTION_SPECIES[i].Species) == 0) return i;
    }
    return -1;
  }
  int FindPhase(CString tCstr)
  {
    for (int i = 0; i < PHASES.size(); i++)
    {
      if (tCstr.CompareNoCase(PHASES[i].Species) == 0) return i;
    }
    return -1;
  }
};

// The data structure to handle the content of a name file
class CNameFile
{
public:
  vector<CString> FileType, FileName;
  vector<int> FileUnitNo;
  CNameFile() {
    clear();
  }
  ~CNameFile()
  {
    FileType.empty();
    FileName.empty();
    FileUnitNo.empty();
  }
  void clear()
  {
    FileType.clear();
    FileName.clear();
    FileUnitNo.clear();
  }

  // Add a filename to the data structure
  int Add(CString typeName, int iUnit, CString fileName)
  {
    int idx = FindFileType(typeName);
    if (idx < 0)
    {
      idx = FileType.size();
      FileType.resize(idx + 1);
      FileType[idx] = typeName;
      FileUnitNo.resize(idx + 1);
      FileUnitNo[idx] = iUnit;
      FileName.resize(idx + 1);
      FileName[idx] = fileName;
    }
    return idx;
  }

  // Find a file type from the data structure
  int FindFileType(CString tStr)
  {
    int nSize = FileType.size();
    for (int i = 0; i < nSize; i++)
    {
      if (FileType[i].CompareNoCase(tStr) == 0)
      {
        return i;
      }
    }
    return -1; // Could not find the file type
  }

  // Find a file unit number from the data structure
  int FindFileUnit(int iUnit)
  {
    int nSize = FileUnitNo.size();
    for (int i = 0; i < nSize; i++)
    {
      if (FileUnitNo[i] == iUnit)
      {
        return i;
      }
    }
    return -1; // Could not find the file type
  }
};

//
class PHT3D_PLOT_Info
{
public:
  CString FileName;
  CString Title;
  CString HelpText;
  bool bHeadPerLayer;
  bool bColumnRead;
  int nHeadBytes;
  int IBDType;
  vector<_int64> iSeekHeadOffset;
  vector<_int64> iSeekDataOffset;
  int nStrideBytes;
  //_int64 nTimeStepBytes;
  double Min, Max;
  int Data_Type;
  void* Data_Pointer;
  PHT3D_PLOT_Info()
  {
    nHeadBytes = -1;
    Min = 0;
    Max = 0;
  }
};

// PHT3D model class
// PHT3D is a reactive transport model handling various kinetic and equilibrium chemical reactions
// and solute transport. It is a combination of MT3DMS and PhreeqC.
// The data structure is very simple because underlying MT3DMS data is handled separately.
class PHT3DClass
{
public:
  CThermodynamicDatabase Thermo_DB;
  CString PHC_FileName;
  int OS, TMP_LOC, RED_MOD;
  double TEMP;
  int ASBIN;
  double EPS_AQU, EPS_PH;
  int PACK_SZ;
  double CB_OFFSET;
  int NR_SOL_MST_SPEC_EQU;
  int NR_MIN_EQU;
  int NR_ION_EX;
  int NR_MOB_KIN;
  int NR_SURF;
  int NR_MIN_KIN;
  int NR_SURF_KIN;
  int NR_IMOB_KIN;
  int NR_OUTP_SPEC, PR_ALAKLINITY_FLAG;
  CString SURF_CALC_TYPE;
  vector<CKinetic_Mobile> Kinetic_Mobile;
  vector<CKinetic_Mobile> IMOB_KIN;
  vector<CSurface_Complex> Surface_Complex;
  vector<CString> Equilibrium_Component;
  vector<CString> Equilibrium_Mineral;
  vector<CIon_Exchange> Ion_Exchange;
  vector<CKinetic_Mineral> Kinetic_Mineral;
  vector<CString> Output_Secondary_Species;
  vector<CString> SELECTED_OUTPUT_Heads;

  void clear()
  {
    Kinetic_Mobile.clear();
    IMOB_KIN.clear();
    Surface_Complex.clear();
    Equilibrium_Component.clear();
    Equilibrium_Mineral.clear();
    Ion_Exchange.clear();
    Kinetic_Mineral.clear();
    Output_Secondary_Species.clear();
    SELECTED_OUTPUT_Heads.clear();
    OS = 0;
    TMP_LOC = 0;
    RED_MOD = 0;;
    TEMP = 0;
    ASBIN  = 0;
    EPS_AQU = 0;
    EPS_PH = 0;
    PACK_SZ = 0;
    CB_OFFSET = 0;
    NR_SOL_MST_SPEC_EQU =0;
    NR_MIN_EQU = 0;
    NR_ION_EX = 0;
    NR_MOB_KIN = 0;
    NR_SURF = 0;
    NR_MIN_KIN = 0;
    NR_SURF_KIN = 0;
    NR_IMOB_KIN = 0;
    NR_OUTP_SPEC = 0;
    PR_ALAKLINITY_FLAG = 0;
  }
  PHT3DClass() 
  {
    clear();
  }
};

// MT3DMS data structure
class MT3DMSClass
{
public:
  // mt3dms advection package
  CString ADV_FileName;
  int ADV_MIXELEM;
  double ADV_PERCEL;
  int ADV_MXPART, ADV_NADVFD;
  int ADV_ITRACK;
  double ADV_WD;
  double ADV_DCEPS;
  int ADV_NPLANE, ADV_NPL, ADV_NPH, ADV_NPMIN, ADV_NPMAX;
  int ADV_INTERP, ADV_NLSINK, ADV_NPSINK;
  double ADV_DCHMOC;
  // mt3dms basic transport package(BTN)
  CString BTN_FileName;
  CString BTN_HEADING[2];
  int BTN_NLAY, BTN_NROW, BTN_NCOL, BTN_NPER, BTN_NCOMP, BTN_MCOMP;
  CString BTN_TUNIT, BTN_LUNIT, BTN_MUNIT;
  bool BTN_TRNOP[10];
  vector<int> BTN_LAYCON;
  vector<float> BTN_DELR;
  vector<float> BTN_DELC;
  ArrayF2D BTN_HTOP;
  //CPPMatrix2<double> BTN_HTOP;
  ArrayF3D BTN_DZ;
  ArrayF3D BTN_PRSITY;
  ArrayI3D BTN_ICBUND;
  ArrayF4D BTN_SCONC;
  //vector<CPPMatrix2<double> > BTN_DZ;
  //vector<CPPMatrix2<double> > BTN_PRSITY;
  //vector<CPPMatrix2<int> > BTN_ICBUND;
  //vector<CPPMatrix2<double> > BTN_SCONC;
  float BTN_CINACT;
  double BTN_THKMIN;
  int BTN_IFMTCN, BTN_IFMTNP, BTN_IFMTRF, BTN_IFMTDP;
  bool BTN_SAVUCN;
  int BTN_NPRS;
  vector<double> BTN_TIMPRS;
  int BTN_NOBS, BTN_NPROBS;
  vector<CObservation_Well> BTN_Observation_Well;
  bool BTN_CHKMAS;
  int BTN_NPRMAS;
  vector<double> BTN_PERLEN, BTN_TSMULT;
  vector<int> BTN_NSTP;
  vector<vector<double> > BTN_TSLNGH;
  int nTotalSteps;
  vector<double> BTN_DT0, BTN_TTSMULT, BTN_TTSMAX;
  vector<int> BTN_MXSTRN;
  // disperson package (DSP)
  CString DSP_FileName;
  CPPMatrix2<double> DSP_AL;
  vector<double> DSP_TRPT, DSP_TRPV, DSP_DMCOEF;
  // sink & source mixing package
  CString SSM_FileName;
  bool SSM_FWEL, SSM_FDRN, SSM_FRCH, SSM_FEVT, SSM_FRIV, SSM_FGHB, SSM_FNEW[4];
  int SSM_MXSS;
  int SSM_INCRCH;
  CPPMatrix2<double> SSM_CRCH;
  int SSM_INCEVT;
  CPPMatrix2<double> SSM_CEVT;
  int SSM_NSS;
  int SSM_KSS, SSM_ISS, SSM_JSS, SSM_ITYPE;
  double SSM_CSS;
  CPPMatrix2<double> SSM_CSSMS;
  // GCG package
  CString GCG_FileName;

  //LKMT4 
  MT3D_LKMT4_Head LKMT4_Head;
  int LKMT4_Start_Index;
  //vector<vector<CPPMatrix2<float> > > LKMT4_THKSAT;
  //vector<vector<CPPMatrix2<float> > > LKMT4_QX;
  //vector<vector<CPPMatrix2<float> > > LKMT4_QY;
  //vector<vector<CPPMatrix2<float> > > LKMT4_QZ;
  //vector<vector<CPPMatrix2<float> > > LKMT4_QSTO;
  vector<vector<CMT3D_List> > LKMT4_CNH;
  vector<vector<CMT3D_List> > LKMT4_WEL;
  vector<vector<CMT3D_List> > LKMT4_DRN;
  vector<vector<CMT3D_List> > LKMT4_RIV;
  vector<vector<CMT3D_List> > LKMT4_GHB;
  vector<CPPMatrix2<int> > LKMT4_IRCH;
  vector<CPPMatrix2<float> > LKMT4_RECH;
  vector<CPPMatrix2<int> > LKMT4_IEVT;
  vector<CPPMatrix2<float> > LKMT4_EVTR;
  // concentration ucn files
  CString ConcentrationFileTemplate;
  vector<vector<vector<CPPMatrix2<float> > > > Concentrations; // Component, Time, layer, row, column
                                                               //ArrayF5D *Concentrations; // Component, Time, layer, row, column
  vector<PHT3D_PLOT_Info> Data;
  //vector<int> Data_Type;
  //vector<void*> Data_Pointer;
  //vector<double> Data_Min;
  //vector<double> Data_Max;
  vector<double> Times;
  vector<int> Time_Period;
  vector<int> Time_Period_Step;
  //vector<CConcentrationRange> ConcentrationRange;
  bool bFortranBinary;

  // Seawat Model
  CString VDF_FileName;

  // Find a data entry
  int FindData(CString dataName)
  {
    for (int i = 0; i < Data.size(); i++)
    {
      if (dataName.CompareNoCase(Data[i].Title) == 0)
        return i;
    }
    return -1;
  }

  // Add a data for the plot
  int AddData(CString dataName)
  {
    int idx = FindData(dataName);
    if (idx >= 0) return idx;
    int isize = Data.size();
    Data.resize(isize + 1);
    Data[isize].Title = dataName;
    return isize;
  }

  void clear()
  {
    BTN_LAYCON.clear();
    BTN_DELR.clear();
    BTN_DELC.clear();
    BTN_HTOP.resize(boost::extents[0][0]);
    BTN_DZ.resize(boost::extents[0][0][0]);
    BTN_PRSITY.resize(boost::extents[0][0][0]);
    BTN_ICBUND.resize(boost::extents[0][0][0]);
    BTN_SCONC.resize(boost::extents[0][0][0][0]);
    BTN_TIMPRS.clear();
    BTN_Observation_Well.clear();
    BTN_PERLEN.clear();
    BTN_TSMULT.clear();
    BTN_TSLNGH.clear();
    BTN_NSTP.clear();
    DSP_AL.clear();
    DSP_TRPT.clear();
    DSP_TRPV.clear();
    DSP_DMCOEF.clear();
    SSM_CRCH.clear();
    SSM_CEVT.clear();
    SSM_CSSMS.clear();
    //LKMT4_THKSAT.clear();
    //LKMT4_QX.clear();
    //LKMT4_QY.clear();
    //LKMT4_QZ.clear();
    //LKMT4_QSTO.clear();
    LKMT4_CNH.clear();
    LKMT4_WEL.clear();
    LKMT4_DRN.clear();
    LKMT4_RIV.clear();
    LKMT4_GHB.clear();
    LKMT4_IRCH.clear();
    LKMT4_RECH.clear();
    LKMT4_IEVT.clear();
    LKMT4_EVTR.clear();
    Concentrations.clear();
    Times.clear();
    Time_Period.clear();
    Time_Period_Step.clear();
    Data.clear();

    BTN_NCOMP = 0;
    BTN_NPRS = 0;
    ADV_MIXELEM = 0;
    ADV_PERCEL = 0;
    ADV_MXPART = 0, ADV_NADVFD = 0;
    ADV_ITRACK = 0;
    ADV_WD = 0;
    ADV_DCEPS = 0;
    ADV_NPLANE = 0;
    ADV_NPL = 0;
    ADV_NPH = 0;
    ADV_NPMIN = 0;
    ADV_NPMAX = 0;
    ADV_INTERP = 0;
    ADV_NLSINK = 0;
    ADV_NPSINK = 0;
    ADV_DCHMOC = 0;
    BTN_NLAY = 0;
    BTN_NROW = 0;
    BTN_NCOL = 0;
    BTN_NPER = 0;
    BTN_NCOMP = 0;
    BTN_MCOMP = 0;
    BTN_TRNOP[10] = {};
    BTN_CINACT = 0;
    BTN_THKMIN = 0;
    BTN_IFMTCN = 0;
    BTN_IFMTNP = 0;
    BTN_IFMTRF = 0;
    BTN_IFMTDP = 0;
    BTN_SAVUCN = false;
    BTN_NPRS = 0;
    BTN_NOBS = 0;
    BTN_NPROBS = 0;
    BTN_CHKMAS = false;
    BTN_NPRMAS = 0;
    nTotalSteps = 0;
    SSM_FWEL = false;
    SSM_FDRN = false;
    SSM_FRCH = false;
    SSM_FEVT = false;
    SSM_FRIV = false;
    SSM_FGHB = false;
    std::fill(BTN_TRNOP, BTN_TRNOP + 10, 0);
    std::fill(SSM_FNEW, SSM_FNEW + 4, 0);
    SSM_MXSS = 0;
    SSM_INCRCH = 0;
    SSM_INCEVT = 0;
    SSM_NSS = 0;
    SSM_KSS = 0;
    SSM_ISS = 0;
    SSM_JSS = 0;
    SSM_ITYPE = 0;
    SSM_CSS = 0;
    LKMT4_Start_Index = 0;
    bFortranBinary = false;
  }
  MT3DMSClass()
  {
    clear();
  }
};

class MODFLOWClass
{
public:
  // Modflow Name File
  CNameFile NameFile;
  // Modflow MODFLOW.Heads
  //vector<vector<CPPMatrix2<float> > > MODFLOW.Heads;
  //vector<CHydraulic_Head_Info> MODFLOW.Heads_Info;
  //vector<vector<CPPMatrix2<float> > > MODFLOW.DrawDown;
  //vector<CHydraulic_Head_Info> MODFLOW.DrawDown_Info;
  //vector<vector<vector<CPPMatrix2<float> > > > MODFLOW.Budget;
  vector<CString> Budget_Info;
  int Budget_Start_Index;
  int Calced_Start_Index;
  // Modflow discritization
  CString DIS_HEADINGS[2];
  int DIS_NLAY, DIS_NROW, DIS_NCOL, DIS_NPER, DIS_ITMUNI, DIS_LENUNI;
  vector<int> DIS_LAYCBD;
  vector<double> DIS_DELR;
  vector<double> DIS_DELC;
  CPPMatrix2<double> DIS_TOP;
  vector<CPPMatrix2<double> > DIS_BOTMS;
  vector<CStressPeriod> DIS_STRESS_PERIOD;
  int BCF_IBCFCB;
  float BCF_HDRY;
  float INACTIVECELL;
  int BCF_IWDFLG;
  double BCF_WETFCT;
  int BCF_IWETIT;
  int BCF_IHDWET;
  vector<int> BCF_Ltype;
  vector<double> BCF_TRPY;
  bool BCF_bTransient;
  vector<CPPMatrix2<double> > BCF_Sf1;
  vector<CPPMatrix2<double> > BCF_HY;
  vector<CPPMatrix2<double> > BCF_Vcont;
  vector<CPPMatrix2<double> > BCF_Sf2;
  vector<CPPMatrix2<double> > BCF_WETDRY;

  vector<CPPMatrix2<double> > AreaRight;
  vector<CPPMatrix2<double> > AreaBottom;
  vector<CPPMatrix2<double> > AreaFront;

  float LPF_HDRY;

  //  int MODFLOW.NPLOT;
  int nTotalSteps;
  bool bFortranBinary;
  vector<PHT3D_PLOT_Info> Data;
  //  vector<CString> MODFLOW.Data;
  //  vector<int> MODFLOW.Data_Type;
  //  vector<void*> MODFLOW.Data_Pointer;
  //  vector<double> MODFLOW.Data_Min;
  //  vector<double> MODFLOW.Data_Max;
  vector<int> Time_Period;
  vector<int> Time_Period_Step;
  vector<double> Times;
  vector<double> Times_in_Period;
  // Modflow BAS package
  CString BAS_HEADINGS[2];
  bool BAS_OPTIONS_XSECTION;
  bool BAS_OPTIONS_CHTOCH;
  bool BAS_OPTIONS_FREE;
  vector<CPPMatrix2<int> > BAS_IBOUND; // boundary condition
  float BAS_HNOFLO;
  vector<CPPMatrix2<double> > BAS_STRT; // initial head
  CString LMT6_Name;
  int LMT6_Version;
  int LMT6_Unit;
  int LMT6_Header;
  int LMT6_Format;
  bool bHeadsData;
  bool bDrawDownData;
  bool bBudgetData;
  bool bLMT6Data;

  int AddBudget(CString dataName)
  {
    int idx = FindBudget(dataName);
    if (idx >= 0) return idx;
    int isize = Budget_Info.size();
    Budget_Info.resize(isize + 1);
    Budget_Info[isize] = dataName;
    return isize;
  }

  int FindBudget(CString dataName)
  {
    for (int i = 0; i < Budget_Info.size(); i++)
    {
      if (dataName.CompareNoCase(Budget_Info[i]) == 0)
        return i;
    }
    return -1;
  }

  int AddData(CString dataName)
  {
    int idx = FindData(dataName);
    if (idx >= 0) return idx;
    int isize = Data.size();
    Data.resize(isize + 1);
    Data[isize].Title = dataName;
    return isize;
  }

  int FindData(CString dataName)
  {
    for (int i = 0; i < Data.size(); i++)
    {
      if (dataName.CompareNoCase(Data[i].Title) == 0)
        return i;
    }
    return -1;
  }

  int FindTimeIndex(double mTime)
  {
    int i, nTimes = Times.size();
    for (i = 0; i < nTimes; i++)
    {
      if (Times[i] >= mTime)
        break;
    }
    if (i == nTimes)
      return -1;
    else
      return i;
  }
 
  int GetTimeStep(int iPeriod, int iStep)
  {
    for (int i = 0; i < Time_Period.size(); i++)
    {
      if (Time_Period[i] == iPeriod && Time_Period_Step[i] == iStep) return i;
    }
    return -1;
  }

  void clear()
  {
    // NameFile should not be cleared by clear()
    // NameFile.clear();
    Budget_Info.clear();
    Budget_Start_Index = 0;
    Calced_Start_Index = 0;
    // Modflow discritization
    for(int i=0; i<2;i++) DIS_HEADINGS[i] = "";
    DIS_NLAY = 0;
    DIS_NROW = 0;
    DIS_NCOL = 0;
    DIS_NPER = 0;
    DIS_ITMUNI = 0;
    DIS_LENUNI = 0;
    DIS_LAYCBD.clear();
    DIS_DELR.clear();
    DIS_DELC.clear();
    DIS_TOP.clear();
    DIS_BOTMS.clear();
    DIS_STRESS_PERIOD.clear();
    BCF_IBCFCB = 0;
    BCF_HDRY = 0;
    INACTIVECELL = 1.e30f;
    BCF_IWDFLG = 0;
    BCF_WETFCT = 0;
    BCF_IWETIT = 0;
    BCF_IHDWET = 0;
    BCF_Ltype.clear();
    BCF_TRPY.clear();
    BCF_bTransient = false;
    BCF_Sf1.clear();
    BCF_HY.clear();
    BCF_Vcont.clear();
    BCF_Sf2.clear();
    BCF_WETDRY.clear();

    AreaRight.clear();
    AreaBottom.clear();
    AreaFront.clear();

    LPF_HDRY = 0;

    //  int MODFLOW.NPLOT;
    nTotalSteps = 0;
    bFortranBinary = false;
    Data.clear();
    Time_Period.clear();
    Time_Period_Step.clear();
    Times.clear();
    Times_in_Period.clear();
    // Modflow BAS package
    for (int i = 0; i<2; i++) BAS_HEADINGS[i] = "";
    BAS_OPTIONS_XSECTION = false;
    BAS_OPTIONS_CHTOCH = false;
    BAS_OPTIONS_FREE = false;
    BAS_IBOUND.clear(); // boundary condition
    BAS_HNOFLO = 0;
    BAS_STRT.clear(); // initial head
    LMT6_Name = "";
    LMT6_Version = 0;
    LMT6_Unit = 0;
    LMT6_Header = 0;
    LMT6_Format = 0;
    bHeadsData = 0;
    bDrawDownData = 0;
    bBudgetData = 0;
    bLMT6Data = 0;
  }

  MODFLOWClass() {
    clear();
  }
};

// Plot class is a helper class to hold configuration and the current data 
// of the 3D model. For example, it has numerical format to display numerical
// axis labels and the range of X-axis and tick intervals. 
class PLOTClass
{
public:
  vector<CScatterPlotData> ScatterPlotData;
  vector<CString> ExtraUCN;
  CString MainTitle;
  int nExtra;
  bool bNewScale;
  bool bMovie;
  // Range and intervals of X and Y axes 
  double min_Xval, xRange, xInterval, min_Yval, yRange, yInterval;
  // Number of ticks on X and Y axes
  int nXTicks, nYTicks;
  // The minimum and maximum of the colormap 
  double cMapMin, cMapRange;
  // The label formats of axes
  CString xFormat, yFormat;
  // boost::multi_array pointer type for 2D float array
  ArrayF2D *Data_2D;
  // 1D vector
  vector<float> Data_1D;
  // Current plotmode (It can be areal view(0), cross-sectional along X (1), etc.
  int PlotMode;
  // Index of the current simulation time of the flow model
  int IndexFlowTime;
  int nFlowData;
  int IndexFlowData;
  int IndexConcTime;
  int IndexConcentration;
  int IndexLayer;
  int IndexRow;
  int IndexColumn;
  CString ConcentrationTimeFormat;
  CString FlowTimeFormat;
  int nArrowInterval;
  bool bRCT;
  bool bClientUpdated;
  bool bReady;
  bool bShowGrid;
  bool bShowToolTip;
  bool bFlowPlot;
  int ModelType;
  float ScaleFactor;
  CPoint PointOrigin;
  int HighlightLayerIndex;
  int HighlightRowIndex;
  int HighlightColumnIndex;
  int HighlightTimeIndex;

  CCubeD RealPlotBound;
  vector<CRect> boxD;
  vector<CRect> boxClient;
  CRect boxCMapLegend;
  vector<CRect> box;
  vector<CPPMatrix2<double> > Z;
  vector<double> X;
  vector<double> Y;

  void clear()
  {
    ScatterPlotData.clear();
    ExtraUCN.clear();
    MainTitle = _T("PHT3D");
    nExtra = 0;
    bNewScale = 0;
    bMovie = 0;
    min_Xval = 0;
    xRange = 0;
    xInterval = 0;
    min_Yval = 0;
    yRange = 0;
    yInterval = 0;
    nXTicks = 0;
    nYTicks = 0;
    cMapMin = 0;
    cMapRange = 0;
    xFormat = "";
    yFormat = "";
    Data_2D = nullptr;
    Data_1D.clear();
    //vector<PHT3D_Info> Info;
    PlotMode = 0;
    IndexFlowTime = 0;
    nFlowData = 0;
    IndexFlowData = 0;
    IndexConcTime = 0;
    IndexConcentration = 0;
    IndexLayer = 0;
    IndexRow = 0;
    IndexColumn = 0;
    ConcentrationTimeFormat = "";
    FlowTimeFormat = "";
    nArrowInterval = 0;
    bRCT = 0;
    bClientUpdated = 0;
    bReady = 0;
    bShowGrid = 0;
    bShowToolTip = 0;
    bFlowPlot = 0;
    ModelType = 0;
    ScaleFactor = 0;
    PointOrigin.SetPoint(0,0);
    HighlightLayerIndex = -1;
    HighlightRowIndex = -1;
    HighlightColumnIndex = -1;
    HighlightTimeIndex = -1;

    RealPlotBound.clear();
    boxD.clear();
    boxClient.clear();
    boxCMapLegend.SetRectEmpty();
    box.clear();
    Z.clear();
    X.clear();
    Y.clear();
  }
  PLOTClass()
  {
    clear();
  }
};

// PHT3D_Model is the general model class containing MODFLOWClass,
// MT3DMSClass, PHT3DClass and PLOTClass. This is the main model class
// used by this plotting application.
class PHT3D_Model
{
public:
  //Main types of the model, PHT3D, MT3DMS, or pure MODFLOW
  CString modelType;
  // Selected name file from OpenFile
  CString sSelectedFile;
  PHT3DClass PHT3D;
  MT3DMSClass MT3DMS;
  MODFLOWClass MODFLOW;
  PLOTClass PLOT;

  PHT3D_Model()
  {
    PLOT.clear();
  }

  ~PHT3D_Model()
  {
    clear();
  }

  int Find_Selected_Output_Heads(CString tCstr)
  {
    CString SELECTED_OUTPUT_Heads[] = { _T("soln"),_T("time"),_T("charge"),_T("pct_err") };
    for (int i = 0; i < 4; i++)
    {
      if (tCstr.CompareNoCase(SELECTED_OUTPUT_Heads[i]) == 0) return i;
    }
    return -1;
  }

  void UpdatePlotIndices()
  {
    PLOT.IndexFlowData = min(PLOT.IndexFlowData, MODFLOW.Data.size() - 1);
    PLOT.IndexFlowTime = min(PLOT.IndexFlowTime, MODFLOW.nTotalSteps - 1);
    PLOT.IndexConcentration = min(PLOT.IndexConcentration, MT3DMS.Data.size() - 1);
    PLOT.IndexConcTime = min(PLOT.IndexConcTime, MT3DMS.nTotalSteps - 1);
    PLOT.IndexLayer = min(PLOT.IndexLayer, MT3DMS.BTN_NLAY - 1);
    PLOT.IndexRow = min(PLOT.IndexRow, MT3DMS.BTN_NROW - 1);
    PLOT.IndexColumn = min(PLOT.IndexColumn, MT3DMS.BTN_NCOL - 1);
  }

  // Clears all main classes
  void clear()
  {
    PLOT.clear();
    PHT3D.clear();
    MT3DMS.clear();
    MODFLOW.clear();
  }

  // Find the cell of the 3D model using X and Y values from the screen
  int Find(int ix, int iy)
  {
    if (PLOT.boxCMapLegend.PtInRect(CPoint(ix, iy))) return -2;
    //    if(PLOT.boxCMapLegend.PtInRect(CPoint(ix,iy))) return -3; // X axis
    //    if(PLOT.boxCMapLegend.PtInRect(CPoint(ix,iy))) return -4; // Y axis
    //    if(PLOT.boxCMapLegend.PtInRect(CPoint(ix,iy))) return -5; // Title
    return 0;// for test 
    switch (PLOT.PlotMode)
    {
    case 0:
      for (int k = 0; k < MT3DMS.BTN_NLAY; k++)
      {
        for (int j = 0; j < MT3DMS.BTN_NROW; j++)
        {
          for (int i = 0; i < MT3DMS.BTN_NCOL; i++)
          {
            int idx = k*MT3DMS.BTN_NROW*MT3DMS.BTN_NCOL + j*MT3DMS.BTN_NCOL + i;
            //              if(PLOT_boxClient[idx].PtInRect(CPoint(ix,iy)))
            //              { return idx;}
          }
        }
      }
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    }
    return -1;
  };
  double GetSetPlotData(int idx, int iData, double val, bool bSet)
  {
    /*
    switch(indexPlot)
    {
    case 1:
    switch(iData)
    {
    case 1: case 3:{ if(bSet) SL_RANGE[iData-1] = val; return SL_RANGE[iData-1]; }
    case 2: case 4:{ if(bSet) SL_RANGE[iData-1] = val; return SL_RANGE[iData-1]; }
    case 5: return SL_Avg[idx];
    case 0:
    default: return SL[indexTime][idx];
    }
    case 2:
    switch(iData)
    {
    case 1: case 3:{ if(bSet) SG_RANGE[iData-1] = val; return SG_RANGE[iData-1]; }
    case 2: case 4:{ if(bSet) SG_RANGE[iData-1] = val; return SG_RANGE[iData-1]; }
    case 5: return SG_Avg[idx];
    case 0:
    default: return SG[indexTime][idx];
    }
    case 3:
    switch(iData)
    {
    case 1: case 3:{ if(bSet) TEMPERATURE_RANGE[iData-1] = val; return TEMPERATURE_RANGE[iData-1]; }
    case 2: case 4:{ if(bSet) TEMPERATURE_RANGE[iData-1] = val; return TEMPERATURE_RANGE[iData-1]; }
    case 5: return TEMPERATURE_Avg[idx];
    case 0:
    default: return TEMPERATURE[indexTime][idx];
    }
    case 4:
    switch(iData)
    {
    case 1: case 3:{ if(bSet) PRESSURE_RANGE[iData-1] = val; return PRESSURE_RANGE[iData-1]; }
    case 2: case 4:{ if(bSet) PRESSURE_RANGE[iData-1] = val; return PRESSURE_RANGE[iData-1]; }
    case 5: return PRESSURE_Avg[idx];
    case 0:
    default: return PRESSURE[indexTime][idx];
    }
    case 5:
    switch(iData)
    {
    case 1: case 3:{ if(bSet) PCAP_RANGE[iData-1] = val; return PCAP_RANGE[iData-1]; }
    case 2: case 4:{ if(bSet) PCAP_RANGE[iData-1] = val; return PCAP_RANGE[iData-1]; }
    case 5: return PCAP_Avg[idx];
    case 0:
    default: return PCAP[indexTime][idx];
    }
    case 6:
    switch(iData)
    {
    case 1: case 3:{ if(bSet) P_REAL_RANGE[iData-1] = val; return P_REAL_RANGE[iData-1]; }
    case 2: case 4:{ if(bSet) P_REAL_RANGE[iData-1] = val; return P_REAL_RANGE[iData-1]; }
    case 5: return P_REAL_Avg[idx];
    case 0:
    default: return PRESSURE[indexTime][idx]+PCAP[indexTime][idx];
    }
    case 7:
    switch(iData)
    {
    case 1: case 3:{ if(bSet) ln_K_RANGE[iData-1] = val; return ln_K_RANGE[iData-1]; }
    case 2: case 4:{ if(bSet) ln_K_RANGE[iData-1] = val; return ln_K_RANGE[iData-1]; }
    case 5: return ln_K_Avg;
    case 0:
    default: return ln_K[idx];
    }
    case 0:
    default:
    switch(iData)
    {
    case 1: case 3:{ if(bSet) SOIL_RANGE[iData-1] = val; return SOIL_RANGE[iData-1]; }
    case 2: case 4:{ if(bSet) SOIL_RANGE[iData-1] = val; return SOIL_RANGE[iData-1]; }
    case 5: return SOIL_Avg[idx];
    case 0:
    default: return SOIL[indexTime][idx];
    }
    }*/
    return 0;
  }
};

// Type definitions for various 2D and 3D matrix types using boost::multi_array
typedef struct HexaCell { int x[8]; } HexaCell;
typedef struct Coord3D { float x[3]; } Coord3D;
//typedef boost::multi_array<int, 1> HexaCell;
typedef boost::multi_array<HexaCell, 3> MFConnectivity;
typedef boost::multi_array<HexaCell, 3> MFOffsets; 
typedef boost::multi_array<Coord3D, 3> MFPoints;
typedef boost::multi_array<HexaCell, 3> MFOffsets;
typedef boost::multi_array<unsigned char, 3> MFTypes;
typedef boost::multi_array<double, 3> MFDataArray;

// Function prototypes
int stconvert(const TCHAR *_Str, const TCHAR *_Format, void *pointer);
void Add_Concentrations_to_Strings(PHT3D_Model& mPHT3DM);
int OpenModel(PHT3D_Model& mPHT3DM, CString sSelectedFile);
//int FetchARecord(int fh1, char *buffer, int *recLen);
//int ParseDelimiters(wchar_t* pStr, wchar_t* pDelimeters, wchar_t *pwch[], int maxN);
void ReadMT3DMSArray(FILE *fp1, void* rArray, bool bMatrix, bool bInt);
void ReadMODFLOWArray(FILE *fp1, void* rArray, bool bMatrix, bool bInt, bool bFreeFormat);
int GetStringTokens(CString *cStr, vector<CString> *stArr, CString *spaIncString);
int GetTimeIndex(PHT3D_Model& mPHT3DM, double val, double factor);
int Read_A_Line_From_Thermo_Database(PHT3D_Model& mPHT3DM, CString *tBuffer, FILE *fp1);
void Read_LKMT_Layers(PHT3D_Model& mPHT3DM, int fh1, vector<vector<CPPMatrix2<float> > > *Value, int l, bool *bContinue);
void Read_LKMT_Layer(PHT3D_Model& mPHT3DM, int fh1, vector<CPPMatrix2<int> > *IValue, vector<CPPMatrix2<float> > *Value, int l, bool *bContinue);
void Read_LKMT_Lists(PHT3D_Model& mPHT3DM, int fh1, vector<vector<CMT3D_List> > *Value, int l, bool *bContinue);
int Read_Name_File(PHT3D_Model& mPHT3DM, CString sPlotDirectory);
int Read_MODFLOW_Files(PHT3D_Model& mPHT3DM, CString sPlotDirectory);
int Read_MT3DMS_Model(PHT3D_Model& mPHT3DM, CString sPlotDirectory);
void Read_LKMT_Package(PHT3D_Model& mPHT3DM, CString sPlotDirectory);
int Read_PHT3D_Model(PHT3D_Model& mPHT3DM, CString sPlotDirectory);

