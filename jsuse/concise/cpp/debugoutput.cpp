#include "../stdafx.h"
#include "../../concise.h"
#include <stdio.h>
namespace cs{
	static int _output_device = OUTPUT_DEVICE_OUTPUT;
	void SetOutputDevice(int device){
		_output_device = device;
	}

	void Print(LPCWSTR fms,...)	{
		Memory<wchar_t> buf;
		va_list vaList;
		va_start(vaList,fms);
		int bufSize = _vscwprintf(fms,vaList);
		if(bufSize==-1)
		{
			va_end(vaList);
			return;
		}
		buf.SetLength(bufSize+3);
		int n = vswprintf_s(buf,bufSize+1,fms,vaList);
		va_end(vaList);
		buf[bufSize] = '\r';
		buf[bufSize+1] = '\n';
		buf[bufSize+2] = 0;
		if(_output_device==OUTPUT_DEVICE_CONSOLE){
			Console* cs = GetConsole();
			if(!cs->IsShowed()) cs->Show();
			cs->Write(buf);
		}else if(IsWindow((HWND)(UINT_PTR)_output_device)){
			/*COPYDATASTRUCT cds;
			cds.dwData = 0;
			cds.lpData = buf.Handle();
			cds.cbData = buf.Length()*2 + 2;*/
			//SendMessage((HWND)(UINT_PTR)_output_device,WM_COPYDATA,(WPARAM)0,(LPARAM)&cds);
			SendMessage((HWND)(UINT_PTR)_output_device,WM_LOGPRINTTEXT,0,(LPARAM)buf.Handle());
		}else{
			::OutputDebugString(buf);
		}
	}
	void Print(LPCSTR fms,...)	{
		Memory<char> buf;
		va_list vaList;
		va_start(vaList,fms);
		int bufSize = _vscprintf(fms,vaList);
		if(bufSize==-1){
			va_end(vaList);
			return;
		}
		buf.SetLength(bufSize+3);
		int n = vsprintf_s(buf,bufSize+1,fms,vaList);
		va_end(vaList);
		buf[bufSize] = '\r';
		buf[bufSize+1] = '\n';
		buf[bufSize+2] = 0;
		if(_output_device==OUTPUT_DEVICE_CONSOLE){
			Console* cs = GetConsole();
			if(!cs->IsShowed()) cs->Show();
			cs->Write(buf);
		}else if(IsWindow((HWND)(UINT_PTR)_output_device)){
			/*COPYDATASTRUCT cds;
			cds.dwData = 0;
			cds.lpData = buf.Handle();
			cds.cbData = buf.Length() + 1;*/
			//SendMessage((HWND)(UINT_PTR)_output_device,WM_COPYDATA,(WPARAM)0,(LPARAM)&cds);
			cs::String str = buf.Handle();
			SendMessage((HWND)(UINT_PTR)_output_device,WM_LOGPRINTTEXT,0,(LPARAM)str.Handle());
		}else{
			::OutputDebugStringA(buf);
		}
	}
	void PrintLastError(LPCWSTR fms){
		PrintLastError(fms,GetLastError());
	}
	void PrintLastError(LPCWSTR fms,DWORD code){
		wchar_t str[256];
		str[0] = 0;
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,code,0,str,256,0);
		Print(fms,str);
	}
	void PrintLastError(){
		PrintLastError(GetLastError());
	}
	void PrintLastError(DWORD code){
		wchar_t buf[256];
		buf[0] = 0;
		int len = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,code,0,buf,256,0);
		if(len>254) len = 254;
		buf[len-1] = '\r';
		buf[len] = '\n';
		buf[len+1] = '\0';
		::OutputDebugString(buf);
	}
	//弹出一个信息提示窗口,显示code代码的系统描述信息.支持GetLastError,WSAGetLastError等函数.title为对话框标题，check用于指定显示条件。
	void Warning(LPCWSTR title,int code){
		wchar_t str[256];
		str[0] = 0;
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,code,0,str,256,0);
		MessageBox(0,str,title,MB_ICONWARNING);
	}
}