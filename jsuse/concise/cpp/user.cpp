#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	bool CreateXPStyleManifest(){
		wchar_t ch[265];
		DWORD len = ::GetModuleFileName(GetModuleHandle(0),ch,265);
		if(len==0) return 0;
		::RtlCopyMemory(ch+len,L".manifest",20);
		File fs;
		if(!fs.Create(ch,CREATE_NEW)) return 0;
		String str = L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n\
<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">\r\n\
  <trustInfo xmlns=\"urn:schemas-microsoft-com:asm.v3\">\r\n\
    <security>\r\n\
      <requestedPrivileges>\r\n\
        <requestedExecutionLevel level=\"asInvoker\" uiAccess=\"false\"></requestedExecutionLevel>\r\n\
      </requestedPrivileges>\r\n\
    </security>\r\n\
  </trustInfo>\r\n\
  <dependency>\r\n\
    <dependentAssembly>\r\n\
      <assemblyIdentity type=\"win32\" name=\"Microsoft.Windows.Common-Controls\" version=\"6.0.0.0\" processorArchitecture=\"*\" publicKeyToken=\"6595b64144ccf1df\" language=\"*\"></assemblyIdentity>\r\n\
    </dependentAssembly>\r\n\
  </dependency>\r\n\
</assembly>";
		Memory<char> buf;
		len = str.ToMultiByte(&buf,CP_UTF8);
		uchar head[3];
		head[0] = 0xef;
		head[1] = 0xbb;
		head[2] = 0xbf;
		fs.SetPointer(0);
		fs.Write(head,3);
		fs.Write(buf.Handle(),len);
		fs.Close();
		return 1;
	}
	bool SetPower(PowerMode mode,bool force){
		TOKEN_PRIVILEGES tkp;
		HANDLE hToken;
		::OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);
		::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;  
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
		::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES) NULL, 0);
		DWORD flag = 0;
		SetProcessShutdownParameters(0,SHUTDOWN_NORETRY);
		switch(mode)
		{
		case Power_Logoff:
			flag = EWX_LOGOFF;
			break;
		case Power_ReBoot:
			flag = EWX_REBOOT;
			break;
		case Power_ShutDown:
			flag = EWX_POWEROFF;
			break;
		case Power_Sleep:;
			return ::SetSystemPowerState(0,force)!=0;
		case Power_Suspend:
			return ::SetSystemPowerState(1,force)!=0;
		default:
			return 0;
		}
		if(force) flag |= EWX_FORCE;
		return ::ExitWindowsEx(flag,SHTDN_REASON_MAJOR_OPERATINGSYSTEM|SHTDN_REASON_MINOR_UPGRADE|SHTDN_REASON_FLAG_PLANNED)!=0;
	}
	//Timer
	Timer::Timer()
	{
		_Timer.Bind(this,&Timer::_TimerProc);
	}
	bool Timer::Start(int ms)
	{
		if(_Handle) return 0;
		_Handle = SetTimer(0,0,ms,_Timer.Procedure());
		return _Handle!=0;
	}
	void Timer::_TimerProc(HWND hWnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
	{
		OnTimer(_Handle,this);
	}
	void Timer::Stop()
	{
		::KillTimer(0,_Handle);
		_Handle = 0;
	}
	Timer::~Timer()
	{
		::KillTimer(0,_Handle);
	}
	//Screen
	bool Screen::GetScreen(int index){
		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		if(::EnumDisplaySettings(0,index,&dm)==0) return 0;
		Width = dm.dmPelsWidth;
		Height = dm.dmPelsHeight;
		Color = dm.dmBitsPerPel;
		Frequency = dm.dmDisplayFrequency;
		return 1;
	}
	LONG Screen::SetScreen(bool test){
		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
		dm.dmBitsPerPel = Color;
		dm.dmDisplayFrequency = Frequency;
		dm.dmPelsHeight = Height;
		dm.dmPelsWidth = Width;
		DWORD flag;
		if(test) flag = CDS_TEST;
		else flag = 0;
		return ::ChangeDisplaySettings(&dm,flag);
	}
	bool SetProgramAutoRun(bool autorun,LPCWSTR keyName,LPCWSTR file,LPCWSTR arg,bool alluser){
		String fileName(MAX_PATH);
		if(file==0){
			if(::GetModuleFileNameW(::GetModuleHandle(NULL),fileName,MAX_PATH)==0)
				return 0;
			fileName.Realize();
		}
		else fileName = file;
		fileName.Insert(L"\"",0);
		fileName += L"\"";
		if(arg!=0){
			fileName += L" ";
			fileName += arg;
		}
		file = fileName;
		String key;
		if(!keyName){
			key = fileName;
			FPToTitle(key);
			keyName = key;
		}
		HKEY root;
		if(alluser) root = HKEY_LOCAL_MACHINE;
		else root = HKEY_CURRENT_USER;
		Registry r;
		if(!r.Create(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",root,1)) return 0;
		if(autorun)
			return r.SetStrValue(keyName,file);
		else
			return r.DelValue(keyName);
	}
	bool getRegAutoRun(HKEY root,LPCWSTR key,String& val){
		Registry r;
		if(!r.Create(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",root)) return 0;
		return r.GetStrValue(key,val);
	}
	bool GetProgramAutoRun(LPCWSTR keyName,LPCWSTR file){
		String fileName(MAX_PATH);
		if(file==0){
			if(::GetModuleFileNameW(::GetModuleHandle(NULL),fileName,MAX_PATH)==0)
				return 0;
			fileName.Realize();
		}
		else fileName = file;
		fileName.Insert(L"\"",0);
		fileName += L"\"";
		file = fileName;
		String key;
		if(!keyName){
			key = fileName;
			FPToTitle(key);
			keyName = key;
		}
		String val;
		if(getRegAutoRun(HKEY_LOCAL_MACHINE,keyName,val)){
			//if(val==fileName){
				return 1;
			//}
		}
		if(getRegAutoRun(HKEY_CURRENT_USER,keyName,val)){
			//if(val==fileName){
				return 1;
			//}
		}
		return 0;
	}
#define ACCESSAPPINF_TYPE_GETNAME	0
#define ACCESSAPPINF_TYPE_SETNAME	1
#define ACCESSAPPINF_TYPE_GETFOLDER	2
#define ACCESSAPPINF_TYPE_SETFOLDER	3
	LPCWSTR accessAppInf(int type,LPCWSTR val = 0){
		static String _app_name,_app_folder;
		if(type==ACCESSAPPINF_TYPE_GETNAME){
			return _app_name;
		}else if(type==ACCESSAPPINF_TYPE_SETNAME){
			_app_name = val;
			return _app_name;
		}else if(type==ACCESSAPPINF_TYPE_GETFOLDER){
			return _app_folder;
		}else if(type==ACCESSAPPINF_TYPE_SETFOLDER){
			_app_folder = val;
			return _app_folder;
		}
		return 0;
	}
	void App::SetAppName(LPCWSTR name){
		accessAppInf(ACCESSAPPINF_TYPE_SETNAME,name);
	}
	LPCWSTR App::GetAppName(){
		LPCWSTR name = accessAppInf(ACCESSAPPINF_TYPE_GETNAME);
		if(name==0||name[0]==0){
			String path;
			GetCurrentExeFileName(path);
			FPToTitle(path);
			return accessAppInf(ACCESSAPPINF_TYPE_SETNAME,path);
		}
		return name;
	}
	void App::SetAppDirectory(LPCWSTR folder){
		accessAppInf(ACCESSAPPINF_TYPE_SETFOLDER,folder);
	}
	LPCWSTR App::GetAppDirectory(){
		LPCWSTR name = accessAppInf(ACCESSAPPINF_TYPE_GETFOLDER);
		if(name==0||name[0]==0){
			String path;
			GetCurrentExeFileName(path);
			FPToParent(path);
			return accessAppInf(ACCESSAPPINF_TYPE_SETFOLDER,path);
		}
		return name;
	}
}