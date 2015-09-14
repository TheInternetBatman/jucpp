#pragma once

namespace base{
	class BASEUI_API  WndPosRestore : public _class
	{
		IntegerList<IWnd*> wndList;
		StringMemList	wndName;
		void _OnCreate(Message*);
		void _OnDestroy(Message*);
	public:
		WndPosRestore();
		~WndPosRestore();
		//wnd是需要定位記錄的窗口，name是指定一個名稱，因為記錄需要用這個名稱來區分窗口。
		bool AddWnd(IWnd* wnd,LPCWSTR name);
		//如果WndPosRestore先于它添加的窗口銷毀，必須調用這個函數移除對它的成員函數的回調。
		bool RemoveWnd(IWnd* wnd);
		//对于没有HOOK WM_CRAET消息的窗口在合适的时候调用这个函数模拟
		void OnCreate(HWND hwnd);
		//对于没有HOOK WM_DESTROY消息的窗口在合适的时候调用这个函数模拟
		void OnDestroy(HWND hwnd);
	};
	//注册一个wnd对象，这对象必须能够接收WM_CREATE和WM_DESTROY消息，在这两个消息里分别设置和记录窗口的位置大小参数。
	void BASEUI_API WPRRegisterWnd(IWnd* wnd,LPCWSTR name);
	//解除一个wnd对象的自动位置大小功能。
	void BASEUI_API WPRUnRegisterWnd(IWnd* wnd);
}