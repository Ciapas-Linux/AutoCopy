
//Tray Icon Messages 
#define WM_TIM_START			WM_USER + 1	//starts animation
#define WM_TIM_CONTINUE			WM_USER + 2	//continues a paused animation
#define WM_TIM_STOP			WM_USER + 3	//stops(pauses) animation
#define WM_TIM_REMOVE			WM_USER + 4	//removes icon from tray
#define WM_TIM_FRAMEINFO		WM_USER + 5	//to set the frame icons in the tray
#define WM_TIM_FRAMERATE		WM_USER + 6	//to set the frame rate
#define WM_TIM_SET_CALLBACK_WND		WM_USER + 7	//to set the call back window
#define WM_TIM_SET_CALLBACK_MSG		WM_USER + 8	//to set the call back message id
#define WM_TIM_SET_ID			WM_USER + 9	//to set the tray icon id
#define WM_TIM_SET_TOOLTIP		WM_USER + 10 	//to set the tool tip for the icon

class AnimatedTrayIcon
{
	public :
	AnimatedTrayIcon();
	~AnimatedTrayIcon();
	
	//set methods 
	void SetFrameRate(int nFrameRate);		//sets the frame rate for animation . time in milliseconds
	void SetFrameIconInfo(HICON* hIconArray,int nNoOfIcons); //sets the icons for the different frames 
	void SetTrayWindowHandle(HWND hWnd);	//sets the hWnd used for tray icon
	void SetTrayCallbackWindowHandle(HWND hWnd);
	void SetTrayCallBackMessage(UINT uCallBackMsg);
	void SetTrayID(UINT uId);
	void SetTrayToolTip(LPSTR szTip);

	//status manipulation methods 
	void Start();							//start animation
	void Stop();							//stop animation
	void Remove();							//remove icon from tray
	void Continue();						//continue stopped animation

	HANDLE m_hEvent;						//event object used for syncing .This should've been a private member
	
	private :
	HWND m_hWnd;							//hWnd which handles tray status notifications
};
