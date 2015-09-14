#pragma once
/* 在窗口中输出信息会自动同步，所以也就可能存在死锁问题。
*/
namespace cs{
#define WM_LOGPRINTTEXT	(WM_USER+0x137)
	enum{OUTPUT_DEVICE_CONSOLE,OUTPUT_DEVICE_OUTPUT};
	CONCISE_API void SetOutputDevice(int device = OUTPUT_DEVICE_OUTPUT);
	//设置输出窗口的句柄，这个窗口将会接收到 WM_COPYDATA/WM_LOGPRINTTEXT 消息。
	CONCISE_API inline void SetOutputDevice(HWND wnd){SetOutputDevice((int)(UINT_PTR)wnd);}
	//打印格式化信息与consol的print相同
	CONCISE_API void Print(LPCWSTR inf,...);
	//打印格式化信息与consol的print相同
	CONCISE_API void Print(LPCSTR inf,...);
	//打印GetLastError的描述信息。
	CONCISE_API void PrintLastError();
	//打印WSAGetLastError或者GetLastError的描述信息
	CONCISE_API void PrintLastError(DWORD code);
	//打印GetLastError的描述信息。
	CONCISE_API void PrintLastError(LPCWSTR fms);
	//打印WSAGetLastError或者GetLastError的描述信息
	CONCISE_API void PrintLastError(LPCWSTR fms,DWORD code);
#ifdef _DEBUG
#define PrintD Print
#define PrintLastErrorD PrintLastError
#else
#define PrintD(inf,...){((void)0);}
#define PrintLastErrorD(fms){((void)0);}
#endif
	//弹出一个信息提示窗口,显示code代码的系统描述信息.支持GetLastError,WSAGetLastError等函数.title为对话框标题。
	CONCISE_API void Warning(LPCWSTR title,int code);
	//弹出一个信息提示窗口,显示内容为GetLastError的描述。
	inline CONCISE_API void Warning(LPCWSTR title){Warning(title,GetLastError());}
}