#pragma once

namespace cs{
	CONCISE_API void LogShowWindow(bool exit = true);
	CONCISE_API void LogSetFontSize(int size);
	CONCISE_API void LogHideWindow();
	CONCISE_API void Log(LPCWSTR str,int color = 0xffffff,LPCWSTR app = (LPCWSTR)INVALID_HANDLE_VALUE);
	CONCISE_API void Log(LPCSTR str,int color = 0xffffff,LPCWSTR app = (LPCWSTR)INVALID_HANDLE_VALUE);
	CONCISE_API void LogSetBkColor(int color);
	CONCISE_API void LogSetMaxLine(int count);
	CONCISE_API void LogClear();
}