#pragma once
namespace cs{
	/*主窗口接收设定的消息，lParam为鼠标消息*/
	class CONCISEUI_API  NotifyIcon : public _class
	{
	protected:
		IntegerDiction<DWORD> _nis;
	public:
		Object<NOTIFYICONDATA> Param;

		NotifyIcon();
		//一个实例必须绑定唯一的窗口，需要再次绑定新的窗口，调用Close函数，但是实例生成的图标会被释放
		bool Create(HWND hWnd);
		//回复初始状态
		void Close();
		bool Add(UINT id,LPCWSTR name,DWORD msgId,ResID ri);
		//hIcon需要手动清理
		bool Add(UINT id,LPCWSTR name,DWORD msgId,HICON hIcon);
		bool ShowBallon(UINT id,LPCWSTR tip,LPCWSTR title,DWORD niif);
		//手动设置Param后，调用Shell_NotifyIcon。
		bool ShellNotify(DWORD nim);
		bool Delete(UINT id);
		~NotifyIcon();
	};
}