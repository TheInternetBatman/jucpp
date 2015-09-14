#pragma once

namespace cs{
	class CONCISEUI_API  WndPosRestore : public _class{
		DWORD _flag;
		IntegerList<IWnd*> _wndList;
		StringMemList	_wndName;
		void _OnCreate(Message*,IWnd*);
		void _OnDestroy(Message*,IWnd*);
	public:
		WndPosRestore();
		~WndPosRestore();
		void SetFlag(DWORD flag){
			_flag = flag;
		}
		//wnd是需要定位記錄的窗口，name是指定一個名稱，因為記錄需要用這個名稱來區分窗口。
		bool AddWnd(IWnd* wnd,LPCWSTR name);
		//如果WndPosRestore先于它添加的窗口銷毀，必須調用這個函數移除對它的成員函數的回調。
		bool RemoveWnd(IWnd* wnd);
		//对于没有HOOK WM_CRAET消息的窗口在合适的时候调用这个函数模拟
		void OnCreate(HWND hwnd);
		//对于没有HOOK WM_DESTROY消息的窗口在合适的时候调用这个函数模拟
		void OnDestroy(HWND hwnd);
	};
#define WPR_FLAG_LEFT	1	//记录 X 位置
#define WPR_FLAG_TOP	2	//记录 Y 位置
#define WPR_FLAG_WIDTH	4	//记录宽度
#define WPR_FLAG_HEIGHT	8	//记录高度
	//注册一个wnd对象，这对象必须能够接收WM_CREATE和WM_DESTROY消息，在这两个消息里分别设置和记录窗口的位置大小参数。
	void CONCISEUI_API SetWndRememberPos(IWnd* wnd,LPCWSTR name,DWORD flag = 0xf);
	//解除一个wnd对象的自动位置大小功能。
	//void CONCISEUI_API SetWndRememberPos(IWnd* wnd);
}