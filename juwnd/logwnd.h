#pragma once

namespace ju{
	//显示 Log 窗口，如果是第一次调用，内部会创建它，一个应用程序只能有一个 Log 窗口，
	//后续调用这个函数，都返回同一个实例，
	JUBASE_API void LogShowWindow(bool exit = true);
	JUBASE_API void LogHideWindow();
	JUBASE_API void LogSetFontSize(int size);
	JUBASE_API void LogSetBkColor(int color);
	JUBASE_API void LogSetMaxLine(int count);
	JUBASE_API void LogSet(int color,LPCWSTR app = 0);
	JUBASE_API void Log(LPCWSTR str);
	JUBASE_API void Log(LPCSTR str);
	JUBASE_API void Log(LPCWSTR str,int color,LPCWSTR app = (LPCWSTR)INVALID_HANDLE_VALUE);
	JUBASE_API void Log(LPCSTR str,int color,LPCWSTR app = (LPCWSTR)INVALID_HANDLE_VALUE);
	JUBASE_API void LogFormat(LPCWSTR fms,...);
	JUBASE_API void LogFormat(LPCSTR fms,...);
	JUBASE_API void LogClear();
}