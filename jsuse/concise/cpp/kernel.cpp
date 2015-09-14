#include "../stdafx.h"
#include "../../concise.h"

#include "psapi.h"
#pragma comment(lib,"psapi.lib")

namespace cs{
	HANDLE DuplicateCurrentThread(){
		HANDLE handle = 0;
		DuplicateHandle(::GetCurrentProcess(),::GetCurrentThread(),::GetCurrentProcess(),&handle,DUPLICATE_SAME_ACCESS,1,DUPLICATE_SAME_ACCESS);
		return handle;
	}
	int SystemMessageBox(HWND hWnd ,LPCWSTR title,DWORD style,DWORD code){
		wchar_t str[256];
		str[0] = 0;
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,code,0,str,256,0);
		return MessageBox(hWnd,str,title,style);
	}
	int GetSystemMessage(String& str,DWORD code){
		str.SetCubage(256);
		int len = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,code,0,str.Handle(),256,0);
		str.Realize();
		return len;
	}
	bool Guid::Create(){
		return CoCreateGuid((GUID*)this)==S_OK;
	}
	void Guid::ToByteString(String& str){
		str = L"";
		unsigned char buf[16];
		memcpy(buf,this,16);
		String num;
		for(int i=0;i<16;i++){
			num.Format(L"%02X",buf[i]);
			str += num;
		}
	}
	void Guid::FromByteString(LPCWSTR str){
		String num;
		unsigned char buf[16];
		for(int i=0;i<16;i++){
			num.CopyFrom(str+i*2,2);
			buf[i] = num.ToInt32(16);
		}
		memcpy(this,buf,16);
	}
	void Guid::ToString(String& str){
		str = L"";
		String num;
		num.Format(L"%08X",Data1);
		str += num;
		str += L"-";
		num.Format(L"%04X",Data2);
		str += num;
		str += L"-";
		num.Format(L"%04X",Data3);
		str += num;
		str += L"-";
		int i=0;
		for(;i<2;i++){
			num.Format(L"%02X",Data4[i]);
			str += num;
		}
		str += L"-";
		for(;i<8;i++){
			num.Format(L"%02X",Data4[i]);
			str += num;
		}
	}
	Event::Event(){
	}
	Event::~Event(){
		::CloseHandle(_Handle);
	}
	bool Event::Create(bool autosignal,bool signal,LPCWSTR name){
		if(_Handle){
			PrintD(L"%s",L"Event.Create: the instance has created");
			return 0;
		}
		_Handle = ::CreateEvent(0,!autosignal,signal,name);
		return _Handle!=NULL;
	}
	bool Event::Open(LPCWSTR name){
		if(_Handle){
			PrintD(L"%s",L"Event.OpenExist: the instance has created");
			return 0;
		}
		_Handle = ::OpenEvent(EVENT_ALL_ACCESS,0,name);
		return _Handle!=NULL;
	}
	void Event::Close(){
		if(!_Handle) return;
		::CloseHandle(_Handle);
		_Handle = 0;
	}
	void Event::Signal(bool signaled){
		if(signaled) SetEvent(_Handle);
		else ResetEvent(_Handle);
	}
	DWORD Event::Wait(int ms){
		return ::WaitForSingleObject(_Handle,ms);
	}

	/*_StaticThread::_StaticThread(){
		_evExit.Create(1,0);
	}
	//等待线程结束，除非主动结束线程（调用Close），否则线程不会随线程函数返回而结束。
	DWORD _StaticThread::Wait(uint ms){
		LocalCriticalSection lcs(_Cs);
		if(!_Handle||!_Handle->Handle) return WAIT_FAILED;
		return ::WaitForSingleObject(_Handle->Handle,ms);
	}
	bool _StaticThread::Pause(){
		LocalCriticalSection lcs(_Cs);
		if(!_Handle||!_Handle->Handle) return 0;
		//从RUN切换到Suspend
		if(THREAD_STATUS_RUNNING==::InterlockedCompareExchange(&_Handle->Status,THREAD_STATUS_SUSPEND,THREAD_STATUS_RUNNING)){
			::SuspendThread(_Handle->Handle);
			return 1;
		}
		else
			return 0;
	}
	Event* _StaticThread::Start(bool wait){
		LocalCriticalSection lcs(_Cs);
		if(!_Handle) return 0;
		if(_Handle->Handle==0){
			//_evExit.Signal(0);
			_Handle->Handle = ::CreateThread(0,0,_Handle->routine,_Handle,CREATE_SUSPENDED,&_Handle->Id);
		}
		while(1){
			//Suspend to Running
			if(THREAD_STATUS_SUSPEND==::InterlockedCompareExchange(&_Handle->Status,THREAD_STATUS_RUNNING,THREAD_STATUS_SUSPEND)){
				//_evExit.Signal(0);
				DWORD count = ::ResumeThread(_Handle->Handle);
				if(count==-1||count>1){
					cs::Print(L"resume failed: %d,%d",count,_Handle->Handle);
					//_ASSERT(0);
				}
				return &_evExit;
			}
			if(wait) ::Sleep(100);
			else return 0;
		}
	}
	bool _StaticThread::IsRunning(){
		LocalCriticalSection lcs(_Cs);
		if(!_Handle||!_Handle->Handle) return 0;
		return (THREAD_STATUS_RUNNING==::InterlockedCompareExchange(&_Handle->Status,THREAD_STATUS_RUNNING,THREAD_STATUS_RUNNING));
	}
	inline void _StaticThread::SetContinue(bool cont){
		LocalCriticalSection lcs(_Cs);
		if(_Handle&&_Handle->Handle) ::InterlockedCompareExchange(&_Handle->Status,THREAD_FLAG_CONTINUE,_Handle->Status);
	}
	void _StaticThread::SetClose(){
		LocalCriticalSection lcs(_Cs);
		if(!_Handle||!_Handle->Handle) return;
		::InterlockedCompareExchange(&_Handle->Flag,THREAD_FLAG_EXIT,_Handle->Flag);
		::ResumeThread(_Handle->Handle);
	}
	void _StaticThread::Close(uint timeOut){
		LocalCriticalSection lcs(_Cs);
		if(!_Handle) return;
		if(!_Handle->Handle){
			delete _Handle;
			_Handle = 0;
			return;
		}
		if(timeOut==0){
			::TerminateThread(_Handle->Handle,-1);
		}else{
			::InterlockedCompareExchange(&_Handle->Flag,THREAD_FLAG_EXIT,_Handle->Flag);
			::ResumeThread(_Handle->Handle);
			DWORD waitRst = WaitForSingleObject(_Handle->Handle,timeOut);
			if(WAIT_TIMEOUT==waitRst){
				TerminateThread(_Handle->Handle,-1);
			}else if(waitRst!=WAIT_OBJECT_0){
				_ASSERT(0);
			}
		}
		CloseHandle(_Handle->Handle);
		delete _Handle;
		_Handle = 0;
	}
	bool _StaticThread::Stop(DWORD code){
		LocalCriticalSection lcs(_Cs);
		if(!_Handle||!_Handle->Handle) return 0;
		_StaticThreadParam* handle = _Handle;
		_Handle = 0;//为了避免异步函数调用引发异常（因为Exit和CloseHandle都会导致_Handle可能失效），预先置零）。
		::TerminateThread(handle->Handle,code);
		::CloseHandle(handle->Handle);
		return 1;
	}
	DWORD _StaticThread::GetID(){
		LocalCriticalSection lcs(_Cs);
		if(!_Handle||!_Handle->Handle) return -1;
		return _Handle->Id;
	}
	_StaticThread::~_StaticThread(){
		Close();
	}

	//TaskPool
	void _TaskPool::_onClean(){
		uint count = 0;
		while(1){
			if(_exitCleanThread) break;
			Sleep(100);
			count++;
			if(count>=_cleanPeroid) count = 0;
			else continue;
			LocalCriticalSection lcs(_cs);
			for(int i=_threadList.Count()-1;i>=0;i--){
				if(!_threadList[i].IsRunning()) _threadList.Delete(i);
			}
		}
	}
	//返回一个空闲的线程对象，如果所有激活线程都在运行，则返回一个新线程。
	_StaticThread* _TaskPool::_getThread(){
		if(_threadList.Count()>=_maxThreadCount) return false;
		//LocalCriticalSection lcs(_cs);这个函数在使用之前已经锁定，这里无需锁定
		for(uint i=0;i<_threadList.Count();i++){
			_StaticThread& st = _threadList[i];
			if(st.IsRunning()) continue;
			return _threadList.Element(i);
		}
		return _threadList.Add();
	}
	_TaskPool::_TaskPool(){
		_cleanPeroid = 100;
		_exitCleanThread = 0;
		_maxThreadCount = 1000;
	}
	void _TaskPool::StartCleanThread(uint period){
		_cleanPeroid = period*10;
		_cleanThread.Call<void>(this,&_TaskPool::_onClean);
	}
	void _TaskPool::SetMaxThread(int maxCount){
		LocalCriticalSection lcs(_cs);
		_maxThreadCount = maxCount;
	}
	int _TaskPool::GetTaskCount(){
		int n = 0;
		for(uint i=0;i<_threadList.Count();i++){
			if(_threadList[i].IsRunning()) 
				n++;
		}
		return n;
	}
	int _TaskPool::GetIdleCount(){
		int n = 0;
		for(uint i=0;i<_threadList.Count();i++){
			if(!_threadList[i].IsRunning()) 
				n++;
		}
		return n;
	}
	int _TaskPool::GetThreadCount(){
		return _threadList.Count();
	}
	void _TaskPool::Close(uint timeOut){
		_exitCleanThread = 1;
		_cleanThread.Wait(timeOut);
		for(uint i=0;i<_threadList.Count();i++){
			_StaticThread& st = _threadList[i];
			_threadList[i].SetClose();
		}
		for(uint i=0;i<_threadList.Count();i++){
			_StaticThread& st = _threadList[i];
			_threadList[i].Close(timeOut);
		}
		_threadList.Clear();
	}
	_TaskPool::~_TaskPool(){
		Close();
	}*/
	int GetCurrentExeFileName(String& str){
		str.SetCubage(260);
		int len = ::GetModuleFileName(0,str.Handle(),260);
		str.Realize();
		return len;
	}
}