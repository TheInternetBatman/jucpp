#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
#pragma pack(push, 1)
	//这个类用于把类成员函数转化为__stdcall函数.在64位平台直接操作这个类可能引发DEP保护.应该使用Delegate.
	class CProcToStdcall : public _class{
	private:
		BYTE    _mov;          // mov ecx, %pThis
		DWORD   _this;         //
		BYTE    _jmp;          // jmp func
		DWORD   _relproc;      // relative jmp
	public:
		template<typename T>void Bind(void* object,T proc,int n = 0){
			if(proc){
				_mov = 0xB9;
				_jmp = 0xE9;
				_this = (DWORD)(INT_PTR)object;
				_relproc = SuperCast<DWORD,T>(proc) - (DWORD)(INT_PTR)this - sizeof(CProcToStdcall);
			}else{
				if(n){
					_mov = 0xc2;
					_this = n;
				}else{
					_mov = 0xc3;
				}
			}
			FlushInstructionCache(GetCurrentProcess(),this,sizeof(CProcToStdcall));
		}
		inline bool IsStdcall(){
			return _mov==0xBA;
		}
		inline bool IsThiscall(){
			return _mov==0xB9;
		}
		template<typename T>inline void BindStd(T proc,int n = 0){
			Bind(0,proc,n);
			/*if(proc){
			_mov = 0xBA;
			_jmp = 0xE9;
			_this = (DWORD)(INT_PTR)0;
			_relproc = SuperCast<DWORD,T>(proc) - (DWORD)(INT_PTR)this - sizeof(CProcToStdcall);
			}else{
			if(n){
			_mov = 0xc2;
			_this = n;
			}else{
			_mov = 0xc3;
			}
			}
			FlushInstructionCache(GetCurrentProcess(),this,sizeof(CProcToStdcall));*/
		}
		void* GetObject(){
			return (void*)(INT_PTR)_this;
		}
		void* GetFunction(){
			return (void*)((char*)this + _relproc + sizeof(CProcToStdcall));
		}
		void operator = (CProcToStdcall& cp){
			memcpy(this,&cp,sizeof(CProcToStdcall));
			_relproc += SuperCast<DWORD,CProcToStdcall*>(&cp) - SuperCast<DWORD,CProcToStdcall*>(this);
			FlushInstructionCache(GetCurrentProcess(),this,sizeof(CProcToStdcall));
		}
		template<typename T>T Procedure(){
			return (T)(INT_PTR)this;
		}
	};
#pragma pack(pop)
	void* _CProcedure(void* p){
		//static CodeMemory cm;
		if(p){
			GetGlobalExecMemory()->Free((CProcToStdcall*)p);
			return 0;
		}
		else
			return GetGlobalExecMemory()->Alloc(10);
	}
	void _InitCProcdure(void* obj,void* func,void* pointer,int n){
		((CProcToStdcall*)pointer)->Bind(obj,func,n);
	}
	void _InitStdProcdure(void* func,void* pointer,int n){
		((CProcToStdcall*)pointer)->BindStd(func,n);
	}
	_StdClassProc::~_StdClassProc(){
		if(_Handle) _CProcedure(_Handle);
	}
	void _StdClassProc::Release(){
		if(_Handle) _CProcedure(_Handle);
	}
	bool _StdClassProc::IsEmpty(){
		if(_Handle==NULL) return 0;
		return ((*(char*)_Handle)==(char)0xc2)||((*(char*)_Handle)==(char)0xc3);
	}
	bool _StdClassProc::IsThiscall(){
		if(_Handle==NULL) return 0;
		return ((CProcToStdcall*)_Handle)->IsThiscall();
	}
	bool _StdClassProc::IsStdcall(){
		if(_Handle==NULL) return 0;
		return ((CProcToStdcall*)_Handle)->IsStdcall();
	}
	void _StdClassProc::operator = (_StdClassProc& scp)
	{
		if(scp._Handle==0)
		{
			if(_Handle!=0)
			{
				_CProcedure(_Handle);
				_Handle = 0;
			}
			return;
		}
		if(_Handle==0) _Handle = _CProcedure();
		*(CProcToStdcall*)_Handle = *(CProcToStdcall*)scp._Handle;
		FlushInstructionCache(GetCurrentProcess(),this,sizeof(CProcToStdcall));
	}
	void* _StdClassProc::GetObject()
	{
		return ((CProcToStdcall*)_Handle)->GetObject();
	}
	void* _StdClassProc::GetFunction()
	{
		return ((CProcToStdcall*)_Handle)->GetFunction();
	}
	bool _StdClassProc::operator == (_StdClassProc& scp)
	{
		if(GetObject()!=scp.GetObject()) return false;
		if(GetFunction()!=scp.GetFunction()) return false;
		return true;
	}
	//class _Delegate
	bool _delegate::Delete(uint index)
	{
		if(index>=_Ptrs.Count()) return false;
		_CProcedure(_Ptrs[index]);
		return _Ptrs.Delete(index)!=0;
	}
	void* _delegate::GetObject(uint index)
	{
		if(index>=_Ptrs.Count()) return 0;
		return ((CProcToStdcall*)_Ptrs[index])->GetObject();
	}
	void* _delegate::GetFunction(uint index)
	{
		if(index>=_Ptrs.Count()) return 0;
		return ((CProcToStdcall*)_Ptrs[index])->GetFunction();
	}
	void _delegate::operator = (_delegate& dt)
	{
		if(&dt==this) return;
		for(uint i=0;i<_Ptrs.Count();i++)
		{
			_CProcedure(_Ptrs[i]);
		}
		_Ptrs.Clear();
		_Ptrs.Expand(dt.Count());
		for(uint i=0;i<dt.Count();i++)
		{
			void* p = _CProcedure();
			_InitCProcdure(dt.GetObject(i),dt.GetFunction(i),p);
			_Ptrs[i] = p;
		}
	}
	_delegate::~_delegate()
	{
		for(uint i=0;i<_Ptrs.Count();i++)
		{
			_CProcedure(_Ptrs[i]);
		}
	}
	APIHook::APIHook():_orgFunc(0){
	}
	APIHook::~APIHook(){
		if(_orgFunc) GetGlobalExecMemory()->Free(_orgFunc);
	}
	bool APIHook::SetHook(void* api,void* hook,int pSize){
		if(_orgFunc==NULL){
			_orgFunc = (uchar*)GetGlobalExecMemory()->Alloc(16);
			if(_orgFunc==NULL) return 0;
		}
		HANDLE hProcess = GetCurrentProcess();
		DWORD dwOldProtect;
		//读取原函数的首10个字节，
		if(!VirtualProtect((void*)api,10, PAGE_EXECUTE_WRITECOPY, &dwOldProtect)) return 0;
		DWORD size = 0;
		//把 API 函数的首 10 个字节写入 _orgFunc，用来呼叫原函数。_orgFunc结构的有效
		//大小是15字节，后5个字节完成一个跳转，跳到原函数的第11个字节。这样呼叫 _orgFunc
		//和呼叫原函数，在汇编代码是一样的，只是第11个字节多了一个跳转，等于把连续的
		//函数，分在两个地方执行。
		ReadProcessMemory(hProcess,api,_orgFunc,10,&size);
		if(size==0) return 0;
		//借助_StdClassProc把原函数的前10个字节改为Function结构，这样调用原函数（api）将会
		//被跳转到绑定的函数（hook）。
		_StdClassProc scp;
		scp.Attach(api);
		scp.BindStd(hook,pSize);
		scp.Detach();
		//生成原函数调用，原理就是呼叫这个地址，会跳转到 API 的 15字节之后 
		*(_orgFunc+10) = 0xe9;
		//10是原函数的相对入口的偏移，15是本结构的大小(不是分配的内存大小，本结构分配了16字节内存，但是只使用了15字节）
		DWORD addr = *(DWORD*)(_orgFunc+11) = (DWORD)(UINT_PTR)api + 10 - 15 - (UINT_PTR)_orgFunc;
		FlushInstructionCache(GetCurrentProcess(),_orgFunc,16);
		FlushInstructionCache(GetCurrentProcess(),api,10);
		return 1;
	}
	bool APIHook::UnHook(){
		if(!_orgFunc) return 0;
		//函数原地址
		uchar* api = (uchar*)(*(DWORD*)(_orgFunc+11) - 10 + 15 + (UINT_PTR)_orgFunc);
		//恢复前10个字节数据
		HANDLE hProcess = GetCurrentProcess();
		DWORD dwOldProtect;
		//读取原函数的首10个字节，
		if(!VirtualProtect((void*)api,10, PAGE_EXECUTE_WRITECOPY, &dwOldProtect)) return 0;
		DWORD size = 0;
		WriteProcessMemory(hProcess,api,_orgFunc,10,&size);
		GetGlobalExecMemory()->Free(_orgFunc);
		_orgFunc = 0;
		FlushInstructionCache(GetCurrentProcess(),api,10);
		return 1;
	}
}