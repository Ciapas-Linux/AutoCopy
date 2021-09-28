
#include "stdafx.h"
#include <process.h>
#include "AniTrayIcon.h"

#define NIF_ALL NIF_ICON | NIF_MESSAGE | NIF_TIP 
#define TIMER_ID	200

LRESULT CALLBACK TrayWindowProc(HWND hWnd,UINT msg,WPARAM wParam , LPARAM lParam)
{
	static NOTIFYICONDATA stNotifyIconData;
	static UINT hTimer = NULL ;
	static int nTimerInterval = 0 ;
	static HICON* hIconArray = NULL ;
	static bShellIconAdded = false ;
	static int nCurrIndex = 0 ;
	static int nArraySize = 0 ;
	
	switch(msg)
	{
	case WM_CLOSE : DestroyWindow(hWnd);
		break;
	case WM_CREATE :
		{
			stNotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
			stNotifyIconData.hIcon = NULL ;
			stNotifyIconData.hWnd = NULL;
			lstrcpy(stNotifyIconData.szTip,"");
			stNotifyIconData.uCallbackMessage = 0 ;
			stNotifyIconData.uFlags = 0 ;
			stNotifyIconData.uID = 0 ;
		}
		break;
	case WM_DESTROY : 
		{
			//clean up 
			if(hTimer)
				KillTimer(hWnd,hTimer);
			if(hIconArray)
				delete [] hIconArray;
			if(bShellIconAdded)
			{
				stNotifyIconData.uFlags = NIF_ALL ;
				Shell_NotifyIcon(NIM_DELETE,&stNotifyIconData);
			}
			PostQuitMessage(0);
		}
		break;
	case WM_TIM_START : 
		{
			if(!bShellIconAdded)
			{
				stNotifyIconData.uFlags = NIF_ALL ;
				if(hIconArray)
					stNotifyIconData.hIcon = hIconArray[nCurrIndex];
				bShellIconAdded = Shell_NotifyIcon(NIM_ADD,&stNotifyIconData);
			}
		}
		break ;
	case WM_TIMER :
		{
			if(bShellIconAdded)
			{
				stNotifyIconData.uFlags = NIF_ALL ;
				if(hIconArray)
					stNotifyIconData.hIcon = hIconArray[nCurrIndex];
				Shell_NotifyIcon(NIM_MODIFY,&stNotifyIconData);
				nCurrIndex++;
				if(nArraySize == nCurrIndex)
					nCurrIndex = 0 ;
			}
		}
		break;
	case WM_TIM_CONTINUE : 
		{
			if(NULL == hTimer)	
			{
				hTimer = SetTimer(hWnd,TIMER_ID,nTimerInterval,NULL);
			}
		}
		break ;
	case WM_TIM_STOP : 
		{
			if(hTimer)	
				KillTimer(hWnd,hTimer);
			hTimer = NULL ;
		}
		break ;
	case WM_TIM_REMOVE : 
		{
			if(bShellIconAdded)
			{
				Shell_NotifyIcon(NIM_DELETE,&stNotifyIconData);
				bShellIconAdded = false;
			}			
		}
		break ;
	case WM_TIM_FRAMEINFO : 
		{
			if(hIconArray)	
			{
				delete [] hIconArray;
				hIconArray = NULL ;
			}
			nArraySize = (int)lParam;
			if( nArraySize != 0 )
			{
				hIconArray = new HICON[nArraySize];
				for(int i = 0 ; i < nArraySize; i++)
					hIconArray[i] = ((HICON*)wParam)[i];
			}
		}
		break ;
	case WM_TIM_FRAMERATE : 
		{
			//kill the old timer and set the new frame rate
			if(hTimer)
				KillTimer(hWnd,hTimer);
			nTimerInterval = (int)lParam;
			hTimer = SetTimer(hWnd,TIMER_ID,nTimerInterval,NULL);
		}
		break;
	case WM_TIM_SET_CALLBACK_WND :
		{
			if(bShellIconAdded)
			{
				stNotifyIconData.uFlags = NIF_ALL;
				stNotifyIconData.hWnd = (HWND)lParam;
				Shell_NotifyIcon(NIM_MODIFY,&stNotifyIconData);
			}
		}
		break;
	case WM_TIM_SET_CALLBACK_MSG :
		{
			if(bShellIconAdded)
			{
				stNotifyIconData.uFlags = NIF_ALL;
				stNotifyIconData.uCallbackMessage = (UINT)lParam;
				Shell_NotifyIcon(NIM_MODIFY,&stNotifyIconData);
			}
		}
		break;
	case WM_TIM_SET_ID :
		{
			if(bShellIconAdded)
			{
				stNotifyIconData.uFlags = NIF_ALL;
				stNotifyIconData.uID = (UINT)lParam;
				Shell_NotifyIcon(NIM_MODIFY,&stNotifyIconData);
			}
		}
		break;
	case WM_TIM_SET_TOOLTIP :
		{
			lstrcpy(stNotifyIconData.szTip,(LPSTR)lParam);
		}
		break ;
	default :
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}

void CreateTrayWindow(AnimatedTrayIcon* pAniWnd)
{
	HWND hTrayWindow = NULL ;
	WNDCLASSEX wndclass;
	MSG msg;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0 ;
	wndclass.hbrBackground = NULL;
	wndclass.hCursor = NULL;
	wndclass.hIcon = NULL;
	wndclass.hIconSm = NULL;
	wndclass.hInstance = GetModuleHandle(NULL);
	wndclass.lpfnWndProc = TrayWindowProc;
	wndclass.lpszClassName = "NSTrayWnd";
	wndclass.lpszMenuName = NULL;
	wndclass.style = 0 ;
	RegisterClassEx(&wndclass);

	hTrayWindow = ::CreateWindow("NSTrayWnd","",WS_POPUP,0,0,0,0,NULL,NULL,GetModuleHandle(NULL),NULL);
	pAniWnd->SetTrayWindowHandle(hTrayWindow);
	SetEvent(pAniWnd->m_hEvent);

	//start the message loop 
	while(GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

AnimatedTrayIcon::AnimatedTrayIcon()
{
	m_hWnd = NULL;
	m_hEvent = NULL;

	m_hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if(m_hEvent)
	{
		DWORD threadID;
		if( 0 != _beginthreadex(NULL,0,(unsigned int (__stdcall *)(void *))CreateTrayWindow,this,0,(unsigned int*)&threadID))
		{
			WaitForSingleObject(m_hEvent,INFINITE);
		}
	}
	CloseHandle(m_hEvent);
	m_hEvent = NULL;
}

AnimatedTrayIcon::~AnimatedTrayIcon()
{
	if(::IsWindow(m_hWnd))
		::PostMessage(m_hWnd,WM_CLOSE,0,0);
	Sleep(100);
}

void AnimatedTrayIcon::SetFrameRate(int nFrameRate)
{
	::SendMessage(m_hWnd,WM_TIM_FRAMERATE,0,(LPARAM)nFrameRate);
}

void AnimatedTrayIcon::SetFrameIconInfo(HICON* hIconArray,int nNoOfIcons)
{
	::SendMessage(m_hWnd,WM_TIM_FRAMEINFO,(WPARAM)hIconArray,(LPARAM)nNoOfIcons);
}

void AnimatedTrayIcon::Start()
{
	::SendMessage(m_hWnd,WM_TIM_START,0,0);
}

void AnimatedTrayIcon::Continue()
{
	::SendMessage(m_hWnd,WM_TIM_CONTINUE,0,0);
}

void AnimatedTrayIcon::Stop()
{
	::SendMessage(m_hWnd,WM_TIM_STOP,0,0);
}

void AnimatedTrayIcon::Remove()
{
	::SendMessage(m_hWnd,WM_TIM_REMOVE,0,0);
}

void AnimatedTrayIcon::SetTrayWindowHandle(HWND hWnd)
{
	if(!m_hWnd)
		m_hWnd = hWnd;
}

void AnimatedTrayIcon::SetTrayCallbackWindowHandle(HWND hWnd)
{
	::SendMessage(m_hWnd,WM_TIM_SET_CALLBACK_WND,0,(LPARAM)hWnd);
}

void AnimatedTrayIcon::SetTrayCallBackMessage(UINT uCallBackMsg)
{
	::SendMessage(m_hWnd,WM_TIM_SET_CALLBACK_MSG,0,(LPARAM)uCallBackMsg);
}

void AnimatedTrayIcon::SetTrayID(UINT uId)
{
	::SendMessage(m_hWnd,WM_TIM_SET_ID,0,(LPARAM)uId);
}

void AnimatedTrayIcon::SetTrayToolTip(LPSTR szTip)
{
	::SendMessage(m_hWnd,WM_TIM_SET_TOOLTIP,0,(LPARAM)szTip);
}