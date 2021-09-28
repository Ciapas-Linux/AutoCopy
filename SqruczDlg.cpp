// SqruczDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sqrucz.h"
#include "SqruczDlg.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//USER SECTION
#define WM_TRAY WM_USER+1///<Komunikat odbierany przez aplikacjê gdy jest w TRAY'u

extern void gLoadConfig(HWND hwnd);
extern bool wDirectoryPicker(const char* Prompt,char* Dest,HWND m_hWnd);
extern void gSaveConfig(HWND hwnd);
extern CTime gTime;
extern bool CheckTime();
extern bool gbDoneAll;
extern unsigned int guiColor;
extern CString gCSSourcePath;
extern CString gCSDestinationPath;
extern void wDisplayLastError();
extern DWORD CALLBACK lpProgressRoutine(LARGE_INTEGER TotalFileSize,
								 LARGE_INTEGER TotalBytesTransferred,
								 LARGE_INTEGER StreamSize,
								 LARGE_INTEGER StreamBytesTransferred,
								 DWORD dwStreamNumber,
								 DWORD dwCallbackReason,
								 HANDLE hSourceFile,
								 HANDLE hDestinationFile,
								 LPVOID lpData);
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSqruczDlg dialog



CSqruczDlg::CSqruczDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSqruczDlg::IDD, pParent)
	, visible(false)
	, pProgressDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSqruczDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAINDLG_DATETIMEPICKER1, TimeCtrl);
}

BEGIN_MESSAGE_MAP(CSqruczDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_TRAY , TrayMessage )
	ON_BN_CLICKED(IDC_MAINDLG_BUTTON_CLOSE, OnBnClickedMaindlgButtonClose)
	ON_BN_CLICKED(IDC_MAINDLG_BUTTON_HIDE, OnBnClickedMaindlgButtonHide)
	ON_BN_CLICKED(IDC_MAINDLG_BUTTON_GETFOLDER1, OnBnClickedMaindlgButtonGetfolder1)
	ON_BN_CLICKED(IDC_MAINDLG_BUTTON_GETFOLDER2, OnBnClickedMaindlgButtonGetfolder2)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_MAINDLG_DATETIMEPICKER1, OnDtnDatetimechangeMaindlgDatetimepicker1)
	ON_BN_CLICKED(IDC_MAINDLG_BUTTON_INFO, OnBnClickedMaindlgButtonInfo)
	ON_BN_CLICKED(IDC_MAINDLG_BUTTON_COPYNOW, OnBnClickedMaindlgButtonCopynow)
	ON_EN_CHANGE(IDC_MAINDLG_EDIT_SOURCE, OnEnChangeMaindlgEditSource)
	ON_EN_CHANGE(IDC_MAINDLG_EDIT_DEST, OnEnChangeMaindlgEditDest)
END_MESSAGE_MAP()


// CSqruczDlg message handlers

BOOL CSqruczDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    
	//Go to tray 
	this->AddTrayIcon(true);
	this->ShowWindow(SW_HIDE);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	gLoadConfig(this->m_hWnd);
	TimeCtrl.SetFormat(_T("hh:mm:ss"));
	TimeCtrl.SetTime(&gTime);

	SetTimer(TIMER_OBJ_ID,200,NULL);
   
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSqruczDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSqruczDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{   
      CPaintDC dc1(this);
      CDC cdc1;
      cdc1.CreateCompatibleDC(&dc1);
      CBitmap bmpDlgBg;
      bmpDlgBg.LoadBitmap(IDB_BACKGROUND_BITMAP1);
      cdc1.SelectObject((HBITMAP)bmpDlgBg);
      CRect rect;
      GetClientRect(rect);
      rect.NormalizeRect();
      dc1.BitBlt(0,0,rect.Width(),rect.Height(),&cdc1,0,0,SRCCOPY);
	  dc1.SetBkMode(TRANSPARENT);
	  dc1.SetTextColor(RGB(220,240,255));
	  dc1.TextOut(5,60,"¯ród³o :");
	  dc1.TextOut(5,105,"Cel :");
	  dc1.TextOut(5,170,"Czas rozpoczêcia kopiowania :");
	  CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSqruczDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSqruczDlg::OnBnClickedMaindlgButtonClose()
{
	if(MessageBox("Napewno wy³¹czyæ program AutoCopy ???","AutoCopy!",MB_YESNO) == IDYES)
	 this->EndDialog(0);
}

void CSqruczDlg::OnBnClickedMaindlgButtonHide()
{
 this->AddTrayIcon(true);
 visible = false;
 this->ShowWindow(SW_HIDE);
return;
}

void CSqruczDlg::OnBnClickedMaindlgButtonGetfolder1()
{
 char* buff = new char[5000];
 ZeroMemory(buff,5000);
 LPCTSTR lpcPrompt = "Please select directory";
 wDirectoryPicker(lpcPrompt,buff,m_hWnd);
 gCSSourcePath = buff;
 if(gCSSourcePath.GetLength() == 0)
 {
  gCSSourcePath = "Proszê wybraæ katalog ¿ród³owy!!!";
 }else
 {
  gCSSourcePath.Append("\\");
 }
 SetDlgItemText(IDC_MAINDLG_EDIT_SOURCE,gCSSourcePath.GetBuffer());
 gCSSourcePath.ReleaseBuffer();
 delete buff;
return;
}

void CSqruczDlg::OnBnClickedMaindlgButtonGetfolder2()
{
 char* buff = new char[5000];
 ZeroMemory(buff,5000);
 LPCTSTR lpcPrompt = "Please select directory";
 wDirectoryPicker(lpcPrompt,buff,m_hWnd);
 gCSDestinationPath = buff;
 if(gCSDestinationPath.GetLength() == 0)
 {
  gCSDestinationPath = "Proszê wybraæ katalog docelowy!!!";
 }else
 {
  gCSDestinationPath.Append("\\");
 }
 SetDlgItemText(IDC_MAINDLG_EDIT_DEST,gCSDestinationPath.GetBuffer());
 gCSDestinationPath.ReleaseBuffer();
 delete buff;
return;
}

void CSqruczDlg::OnDestroy()
{   
	KillTimer(TIMER_OBJ_ID);
	gSaveConfig(this->m_hWnd);
	CDialog::OnDestroy();
}

void CSqruczDlg::OnDtnDatetimechangeMaindlgDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	this->UpdateData(true);
	TimeCtrl.GetTime(gTime);
}

void CSqruczDlg::OnTimer(UINT TimerId)
{
 //Time check fuction
 if(TimerId == TIMER_OBJ_ID)
 {
	 if(gbDoneAll == false)
	 {
	  if(CheckZeroTime() == true)
	  {
	   if(gCSDestinationPath == "Katalog docelowy!" || gCSDestinationPath == "")
       {
        AfxMessageBox("Nic z tego nie poda³eœ katalogu docelowego!");
	   return;
	   }
	   if(gCSSourcePath == "Katalog ¿ród³owy!" || gCSSourcePath == "")
       {
		AfxMessageBox("Nic z tego nie poda³eœ katalogu docelowego!");
	   return;
       }
	   if(gCSSourcePath == gCSDestinationPath)
       {
        AfxMessageBox("Co ty robisz katalog ¿ród³owy jest taki sam jak docelowy, nie bêde kopiowaæ!");
       return;
       }
	   pProgressDlg = new CProgressDlg;
       KillTimer(TIMER_OBJ_ID);
       pProgressDlg->Create(IDD_PROGRESSDLG);
       CopyData();
	   pProgressDlg->EndDialog(0);
       delete pProgressDlg;
	   if(gbDoneAll == true)
       {  
        AfxMessageBox(">>>>>>>>>>   Dane skopiowano pomyœlnie !!!   <<<<<<<<<<");
        gbDoneAll = false;//reset !!!
       }else
       {
        AfxMessageBox("B³¹d odczytu pliku!");
       }
       SetTimer(TIMER_OBJ_ID,200,NULL);
	  }
	 }
 }
return;
}

void CSqruczDlg::AddTrayIcon(bool add)
{
    NOTIFYICONDATA nid;
	nid.cbSize=sizeof(NOTIFYICONDATA); 
	nid.hWnd=this->m_hWnd; 
	nid.uID=0; 
	if (add==true)
	{
		nid.uFlags=NIF_ICON | NIF_TIP | NIF_MESSAGE ; 
		nid.uCallbackMessage = WM_TRAY; 
		nid.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME); 
		GetWindowText(nid.szTip,64);
		Shell_NotifyIcon(NIM_ADD,&nid);	
	}
	else		
	{
		nid.uFlags=0; 
		Shell_NotifyIcon(NIM_DELETE,&nid);	
	}
}

void CSqruczDlg::OnClose(void)
{
 this->AddTrayIcon(true);
 visible = false;
 this->ShowWindow(SW_HIDE);
return;
}

afx_msg LRESULT CSqruczDlg::TrayMessage(WPARAM  wParam, LPARAM lParam)
{
	switch(lParam)
	{
		case WM_LBUTTONDOWN:	
			  visible = true;
	          SetForegroundWindow(); 
	          this->ShowWindow(SW_RESTORE);
 	          this->AddTrayIcon(false);
			 return 0;

		case WM_RBUTTONDOWN:	
			 return 0;
	}
	return 0;
}

afx_msg void CSqruczDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
if(!visible)
    lpwndpos->flags &= ~SWP_SHOWWINDOW;
    CDialog::OnWindowPosChanging(lpwndpos);
return afx_msg void();
}

void CSqruczDlg::OnBnClickedMaindlgButtonInfo()
{

return;
}

void CSqruczDlg::OnBnClickedMaindlgButtonCopynow()
{
 if(gCSDestinationPath == "Katalog docelowy!" || gCSDestinationPath == "")
 {
  AfxMessageBox("Nic z tego nie poda³eœ katalogu docelowego!");
  return;
 }
 if(gCSSourcePath == "Katalog ¿ród³owy!" || gCSSourcePath == "")
 {
  AfxMessageBox("Nic z tego nie poda³eœ katalogu docelowego!");
  return;
 }
 if(gCSSourcePath == gCSDestinationPath)
 {
  AfxMessageBox("Co ty robisz katalog ¿ród³owy jest taki sam jak docelowy, nie bêde kopiowaæ!");
 return;
 }
 if(GetObjNumber(gCSSourcePath) == 0)
 {
   AfxMessageBox("Nic z tego! zamiast œcie¿ki do plików ¿ród³owych wpisujesz jakieœ bzdety!");
 return;
 }
 if(GetObjNumber(gCSDestinationPath) == 0)
 {
   AfxMessageBox("Nic z tego! zamiast œcie¿ki do plików docelowych wpisujesz jakieœ bzdety!");
 return;
 }
 KillTimer(TIMER_OBJ_ID);
 pProgressDlg = new CProgressDlg;
 pProgressDlg->Create(IDD_PROGRESSDLG);
 CopyData();
 pProgressDlg->EndDialog(0);
 delete pProgressDlg;
 if(gbDoneAll == true)
 {
   AfxMessageBox(">>>>>>>>>>   Dane skopiowano pomyœlnie !!!   <<<<<<<<<<");
   gbDoneAll = false;//reset !!!
 }else
 {
   AfxMessageBox("B³¹d odczytu pliku!");
 }
 SetTimer(TIMER_OBJ_ID,200,NULL);
return;
}

bool CSqruczDlg::CheckZeroTime(void)
{
 CTime CTcurrent = CTime::GetCurrentTime();
 if(gTime == CTcurrent)
      return true; 
return false;
}

bool CSqruczDlg::CopyData(void)
{
 unsigned int iObjNum = GetObjNumber(gCSSourcePath);
 pProgressDlg->ProgresObj.SetRange32(0,iObjNum);
 pProgressDlg->ProgresObj.SetPos(0);
 pProgressDlg->ProgresObj.SetStep(1);
  if(FolderIsEmpty(gCSDestinationPath) == false)
  {
   if(DeleteFolderRec(gCSDestinationPath) == false)
   {
    return false;
   }
  }
 pProgressDlg->ProgresObj.SetPos(0);
 int ret = CopyAllObjFromDirectory(gCSSourcePath,gCSDestinationPath);
  if(ret == iObjNum)
  {
   gbDoneAll = true;
   return true;
  }else
  {
   return false;
  }
}

unsigned int CSqruczDlg::CopyAllObjFromDirectory(const CString& CSSourceFolderPath , const CString&  CSDestinationFolderPath)
{
 CFileFind finder;
 CString CStmp;
 BOOL bWorking = finder.FindFile(CSSourceFolderPath + "*.*");
 int cnt = 0;

 while (bWorking)//main loop
 {
  bWorking = finder.FindNextFile();
  CStmp = finder.GetFileName();

  if(finder.IsDots()) //skip dots
	  continue;

  if(finder.IsDirectory()) //directory
  {
    CString CSSrc = CSSourceFolderPath;
	CSSrc.Append(CStmp);
    CSSrc.Append("\\");
	CString CSDst = CSDestinationFolderPath;
	CSDst.Append(CStmp);
	CSDst.Append("\\");
	 if(CreateDirectory((LPCTSTR)CSDst,NULL) == false)
	 {
	  return -1;
	 }else
	 {
      int res = CopyAllObjFromDirectory(CSSrc,CSDst);//Make recurency controll !!!!!!!!
	  if (res == -1) return -1;
	  cnt = cnt + res;
	  continue;
	 }
  }

  //no dots no directory so copy one file obj..
  pProgressDlg->ProgresObj.StepIt();
  if(CopyFile(CSSourceFolderPath,CSDestinationFolderPath,CStmp) == false)
  {
   return -1;
  }
  cnt++;
 }
return cnt;
}

bool CSqruczDlg::CopyFile(const CString& CSSourcePath , const CString& CSDestinationPath , const CString& CSFileName)
{
    LPBOOL pbCancel = false;
    CString CSSrc(CSSourcePath);
	CSSrc.Append(CSFileName);
	CString CSDest(CSDestinationPath);
	CSDest.Append(CSFileName);
	BOOL bret = CopyFileEx((LPCTSTR)CSSrc,
		                   (LPCTSTR)CSDest,
						    lpProgressRoutine,
						    NULL,
						    pbCancel,
						    0);
if(bret == false)
 return false;
else return true;
}

unsigned int CSqruczDlg::GetObjNumber(const CString&  CSSourceFolderPath)
{
 CFileFind finder;
 CString CStmp;
 BOOL bWorking = finder.FindFile(CSSourceFolderPath + "*.*");
 int cnt = 0;

 while (bWorking)//main loop
 {
  bWorking = finder.FindNextFile();
  CStmp = finder.GetFileName();

  if(finder.IsDots()) //skip dots
	  continue;

  if(finder.IsDirectory()) //directory
  {
    CString CSSrc = CSSourceFolderPath;
	CSSrc.Append(CStmp);
    CSSrc.Append("\\");
	{
     unsigned int res = GetObjNumber(CSSrc);//Make recurency controll !!!!!!!!
	 cnt = cnt + res;
	 continue;
	}
  }else
  {
   cnt++;
  }
 }
return cnt;
}

bool CSqruczDlg::DeleteFolderRec(CString szPath)
{
	CFileFind ff;
	CString path = szPath;
	if(path.Right(1) != "\\")
		path += "\\";
	path += "*.*";
	BOOL res = ff.FindFile(path);
	while(res)
	{
		res = ff.FindNextFile();
		if(ff.IsDots())
			continue;
		if(ff.IsDirectory())
		{
         path = ff.GetFilePath();
		 DeleteFolderRec(path);
		 RemoveDirectory(path);
		 continue;
		}
		pProgressDlg->ProgresObj.StepIt();
		DeleteFile(ff.GetFilePath());
	}
return true;
}

bool CSqruczDlg::FolderIsEmpty(CString szPath)
{
    CFileFind ff;
	CString path = szPath;
	if(path.Right(1) != "\\")
		path += "\\";
	path += "*.*";
	BOOL res = ff.FindFile(path);
    bool found = false;
	while(res)
	{
		res = ff.FindNextFile();
		if (ff.IsDots())
         continue;
		if(ff.IsDirectory())
		{
		 found = true;
		 break;
		}
		found = true;
	}
if(found == true)
 return false;
else
 return true;
}

void CSqruczDlg::OnEnChangeMaindlgEditSource()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
    CString rString;
	GetDlgItemText(IDC_MAINDLG_EDIT_SOURCE,rString);
    gCSSourcePath = rString;
return;
}

void CSqruczDlg::OnEnChangeMaindlgEditDest()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString rString;
	GetDlgItemText(IDC_MAINDLG_EDIT_DEST,rString);
    gCSDestinationPath = rString;
return;
}
