// Sqrucz.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Sqrucz.h"
#include "SqruczDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void wGetModulePath(CString& CSretBuf);
void gLoadConfig(HWND hwnd);
void gSaveConfig(HWND hwnd);
bool wDirectoryPicker(const char* Prompt,char* Dest,HWND m_hWnd);
void wDisplayLastError();
void wGetModuleName(CString& CSretBuf);

CTime   gTime;
CString gCSSourcePath;
CString gCSDestinationPath;
bool gbDoneAll = false;
unsigned int guiColor = 0;

void wDisplayLastError()
{
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
                FORMAT_MESSAGE_FROM_SYSTEM|
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf,
                0,
                NULL);
  MessageBox(NULL,(LPCTSTR)lpMsgBuf,"PinkCard Error !",MB_OK|MB_ICONINFORMATION);
  LocalFree( lpMsgBuf );
return;
}

DWORD CALLBACK lpProgressRoutine(LARGE_INTEGER TotalFileSize,
								 LARGE_INTEGER TotalBytesTransferred,
								 LARGE_INTEGER StreamSize,
								 LARGE_INTEGER StreamBytesTransferred,
								 DWORD dwStreamNumber,
								 DWORD dwCallbackReason,
								 HANDLE hSourceFile,
								 HANDLE hDestinationFile,
								 LPVOID lpData)
{

return PROGRESS_CONTINUE;
}

bool wDirectoryPicker(const char* Prompt,char* Dest,HWND m_hWnd)
{
 BROWSEINFO bi;
 ITEMIDLIST* pItemIDList;
 char Folder[_MAX_PATH];
 memset(&bi, 0, sizeof(bi));
 bi.hwndOwner=m_hWnd;
 bi.pszDisplayName=Folder;
 bi.lpszTitle=Prompt;
 bi.ulFlags = BIF_EDITBOX | BIF_VALIDATE | BIF_NEWDIALOGSTYLE;
 if((pItemIDList=SHBrowseForFolder(&bi))!=NULL)
 {
  SHGetPathFromIDList(pItemIDList,Dest);
  return true;
 }else return false;
return false;
}


void gSaveConfig(HWND hwnd)
{
   CString CSini;
   wGetModulePath(CSini);
   CSini.Append("my.ini");
   CString CSTime;
   CSTime = gTime.Format( "%H:%M:%S" );
   ofstream fout((LPCTSTR)CSini,ios::out|ios::trunc);
   fout << (LPCTSTR)gCSDestinationPath << endl;
   fout << (LPCTSTR)gCSSourcePath << endl;
   fout << (LPCTSTR)CSTime << endl;
   fout.close();
return;
}

void gLoadConfig(HWND hwnd)
{
 CString CSini;
 wGetModulePath(CSini);
 CSini.Append("my.ini");
 CString CSTemp;
 ifstream fin((LPCTSTR)CSini);
 if(!fin)
 {
  gCSDestinationPath = "Katalog docelowy!";
  SetDlgItemText(hwnd,IDC_MAINDLG_EDIT_DEST,(LPCTSTR)gCSDestinationPath);
  gCSSourcePath = "Katalog ¿ród³owy!";
  SetDlgItemText(hwnd,IDC_MAINDLG_EDIT_SOURCE,(LPCTSTR)gCSSourcePath);
  return;
 }
 fin.getline(CSTemp.GetBuffer(1024),1024,'\n');//Destination path
 CSTemp.ReleaseBuffer();
 if(CSTemp.GetLength() > 0)
 {
  gCSDestinationPath = CSTemp;
  SetDlgItemText(hwnd,IDC_MAINDLG_EDIT_DEST,(LPCTSTR)gCSDestinationPath);
 }else
 {
  gCSDestinationPath = "Katalog docelowy!";
  SetDlgItemText(hwnd,IDC_MAINDLG_EDIT_DEST,(LPCTSTR)gCSDestinationPath);
 }
 CSTemp = "";
 fin.getline(CSTemp.GetBuffer(1024),1024,'\n');  //Source path
 CSTemp.ReleaseBuffer();
 if(CSTemp.GetLength() > 0)
 {
  gCSSourcePath = CSTemp;
  SetDlgItemText(hwnd,IDC_MAINDLG_EDIT_SOURCE,(LPCTSTR)gCSSourcePath);
 }else
 {
  gCSSourcePath = "Katalog ¿ród³owy!";
  SetDlgItemText(hwnd,IDC_MAINDLG_EDIT_SOURCE,(LPCTSTR)gCSSourcePath);
 }
 CSTemp = "";
 fin.getline(CSTemp.GetBuffer(1024),1024,'\n');  //time
 CSTemp.ReleaseBuffer();
 if(CSTemp.GetLength() > 0 &&  CSTemp.GetLength() <= 8)
 {
  CSTemp.TrimRight();
  CString CSHH(CSTemp.Left(2));//Get... Minute
  CString CSMM(CSTemp.Mid(3,2));//Hour
  CString CSSS(CSTemp.Right(2));//Seconds
  CTime Current = CTime::GetCurrentTime();//Get current date time
  CString CSYY(Current.Format("%Y"));//year
  CString CSMO(Current.Format("%m"));//month
  CString CSDD(Current.Format("%d"));//day
  CTime TempTime(atoi((LPCTSTR)CSYY),atoi((LPCTSTR)CSMO),atoi((LPCTSTR)CSDD),atoi((LPCTSTR)CSHH),atoi((LPCTSTR)CSMM),atoi((LPCTSTR)CSSS));
  gTime = TempTime;
 }else
 {
  CTime Current = CTime::GetCurrentTime();//Get current date time
  CString CSYY(Current.Format("%Y"));//year
  CString CSMO(Current.Format("%m"));//month
  CString CSDD(Current.Format("%d"));//day
  CSTemp = "00";
  CTime TempTime(atoi((LPCTSTR)CSYY),atoi((LPCTSTR)CSMO),atoi((LPCTSTR)CSDD),atoi((LPCTSTR)CSTemp),atoi((LPCTSTR)CSTemp),atoi((LPCTSTR)CSTemp));
  gTime = TempTime;
 }
 fin.close();
return;
}

//atoi( const char *string );
//CTime(
//   int nYear,
//   int nMonth,
//   int nDay,
//   int nHour,
//   int nMin,
//   int nSec,
//   int nDST = -1 

void wGetModulePath(CString& CSretBuf)
{
 char chtmp[201];
 ZeroMemory(chtmp,sizeof(chtmp));
 GetModuleFileName(NULL,(LPTSTR)chtmp,(DWORD)200);
 CSretBuf = chtmp;
 CSretBuf = CSretBuf.Left(CSretBuf.ReverseFind('\\') + 1);
return;
}

void wGetModuleName(CString& CSretBuf)
{
 char chtmp[201];
 ZeroMemory(chtmp,sizeof(chtmp));
 GetModuleFileName(NULL,(LPTSTR)chtmp,(DWORD)200);
 CSretBuf = chtmp;
 int ret = 0;
 ASSERT(ret = CSretBuf.ReverseFind( '\\' ));
 CSretBuf = CSretBuf.Right((CSretBuf.GetLength() - ret) - 1);
return;
}

// CSqruczApp

BEGIN_MESSAGE_MAP(CSqruczApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSqruczApp construction

CSqruczApp::CSqruczApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSqruczApp object

CSqruczApp theApp;


// CSqruczApp initialization

BOOL CSqruczApp::InitInstance()
{
	
	CString CSExeName;
	wGetModuleName(CSExeName);
		
	HANDLE         hProcessSnap = NULL; 
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
    
	CString CSMname;
	PROCESSENTRY32 pe32      = {0}; 
    pe32.dwSize = sizeof(PROCESSENTRY32);  
	int iRunCounter = 0;
	if (Process32First(hProcessSnap, &pe32)) 
    { 
     while(Process32Next(hProcessSnap, &pe32))
	 {
		 CSMname = pe32.szExeFile;
		 if(CSMname == CSExeName)iRunCounter++;
	 }
	}
	if(iRunCounter > 1)
	{
	 AfxMessageBox("Program ju¿ zosta³ wczeœniej uruchomiony!!!");
	 return FALSE;
	}
  
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();


	CSqruczDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
