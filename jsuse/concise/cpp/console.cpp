#include "../stdafx.h"
#include "../../concise.h"
namespace cs{
	Console::Console():_hWrite(0),_hRead(0){
	}
	bool Console::Show(){
		if(_hWrite) return false;
		int r = AllocConsole(); 
		_hRead = GetStdHandle(STD_INPUT_HANDLE);
		_hWrite = GetStdHandle(STD_OUTPUT_HANDLE);
		return r!=0;
	}
	void Console::Close(){
		CloseHandle(_hRead);
		_hRead = 0;
		CloseHandle(_hWrite);
		_hWrite = 0;
		FreeConsole();
	}
	void Console::SetTextColor(int color){
		color &= 0x7;
		SetConsoleTextAttribute(_hWrite,FOREGROUND_INTENSITY | color);
	}
	void Console::SetCloseMenuDisable(bool disable){
		HWND console = GetConsoleWindow();
		HMENU menu = GetSystemMenu(console, FALSE);
		//DWORD state = MF_BYCOMMAND;
		if(disable){
			//state |= MF_DISABLED|MF_GRAYED;
			::DeleteMenu(menu,SC_CLOSE,MF_BYCOMMAND);
		}else{
		}
		//::EnableMenuItem(menu,SC_CLOSE,state);
		//DrawMenuBar(console);
	}
	void Console::WriteLn(LPCWSTR str){
		String s = str;
		s += L"\r\n";
		DWORD len;
		WriteConsole(_hWrite,s,s.Length(),&len,NULL);
	}
	DWORD Console::ReadLn(String& str){
		str = L"";
		wchar_t buf[128];
		while(true){
			DWORD len = 0;
			ReadConsole(_hRead, buf,128,&len, NULL);
			str.CopyFrom(buf,len,str.Length());
			if(len<128){
				break;
			}else if(buf[127]=='\n'){
				break;
			}
		}
		return str.Length();
	}
	Console* GetConsole(){
		static Console _console;
		return &_console;
	}
}