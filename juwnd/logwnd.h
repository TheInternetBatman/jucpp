#pragma once

namespace ju{
	//显示 Log 窗口，如果是第一次调用，内部会创建它，一个应用程序只能有一个 Log 窗口，
	//后续调用这个函数，都返回同一个实例，
	JUWND_API void LogShowWindow(bool exit = true);
	JUWND_API void LogHideWindow();
	JUWND_API void LogCloseWindow();
	JUWND_API void LogSetFontSize(int size);
	JUWND_API void LogSetBkColor(int color);
	JUWND_API void LogSetMaxLine(int count);
	JUWND_API void LogSet(int color,LPCWSTR app = 0);
	JUWND_API void Log(LPCWSTR str);
	JUWND_API void Log(LPCSTR str);
	JUWND_API void Log(LPCWSTR str,int color,LPCWSTR app = (LPCWSTR)INVALID_HANDLE_VALUE);
	JUWND_API void Log(LPCSTR str,int color,LPCWSTR app = (LPCWSTR)INVALID_HANDLE_VALUE);
	JUWND_API void Logf(LPCWSTR fms,...);
	JUWND_API void Logf(LPCSTR fms,...);
	JUWND_API void LogfD(LPCWSTR fms,...);
	JUWND_API void LogfD(LPCSTR fms,...);
	JUWND_API void LogfI(LPCWSTR fms,...);
	JUWND_API void LogfI(LPCSTR fms,...);
	JUWND_API void LogfW(LPCWSTR fms,...);
	JUWND_API void LogfW(LPCSTR fms,...);
	JUWND_API void LogfE(LPCWSTR fms,...);
	JUWND_API void LogfE(LPCSTR fms,...);
	JUWND_API void Logf(int color,LPCWSTR fms,...);
	JUWND_API void Logf(int color,LPCSTR fms,...);
	JUWND_API void LogClear();

	class JUWND_API LogView : public _class{
	protected:
		void* _handle;
	public:
		LogView();
		~LogView();
		HWND Create(HWND parent);
		void Destroy();
		void SetBkColor(int bkColor);
		void SetMaxLine(int count);
		void SetFontSize(int size);
		void Log(LPCWSTR str,int color = 0xffffff);
		void Log(LPCSTR str,int color = 0xffffff);
		void Logf(int color,LPCWSTR fms,...);
		void Logf(int color,LPCSTR fms,...);
		void Logf(LPCWSTR fms,...);
		void Logf(LPCSTR fms,...);
		void Clear();
	};
}