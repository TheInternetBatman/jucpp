#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	//class VirtualAddress
	VirtualAddress::VirtualAddress():_Size(0){
	}
	VirtualAddress::VirtualAddress(uint section):_Size(0){
		_Handle = ::VirtualAlloc(NULL,section*0x10000,MEM_RESERVE,PAGE_READWRITE);
		if(_Handle) _Size = section*0x10000;
	}
	bool VirtualAddress::Reserve(uint section){
		if(_Handle) ::VirtualFree(_Handle,0,MEM_FREE);
		_Size = section*0x10000;
		_Handle = ::VirtualAlloc(0,_Size,MEM_RESERVE,PAGE_READWRITE);
		if(_Handle==0){
			_Size = 0;
#ifdef _DEBUG
			SystemMessageBox(0,L"VirtualAddress::Reserve");
#endif
			return 0;
		}
		else return 1;
	}
	void* VirtualAddress::Commit(LPVOID p,uint length,DWORD protect){//函数分配的内存的最小单位是 64KB,section = 1,则分配64KB; section = 2则分配128KB...
		if(p==0) p = _Handle;
		if(length==0) length = _Size;
		return ::VirtualAlloc(p,length,MEM_COMMIT,protect);
	}
	bool VirtualAddress::DeCommit(LPVOID p,uint length){
		if(p==0) p = _Handle;
		if(length==0) length = _Size;
		return ::VirtualFree(p,length,MEM_DECOMMIT)!=0;
	}
	void* VirtualAddress::Desert(LPVOID p,uint length){//丢弃数据,但内存仍可用.
		if(p==0) p = _Handle;
		if(length==0) length = _Size;
		return ::VirtualAlloc(p,length,MEM_RESET,0);
	}
	bool VirtualAddress::Lock(LPVOID p,uint length){
		if(p==0) p = _Handle;
		if(length==0) length = _Size;
		return ::VirtualLock(p,length)!=0;
	}
	bool VirtualAddress::Unlock(LPVOID p,uint length){
		if(p==0) p = _Handle;
		if(length==0) length = _Size;
		return ::VirtualUnlock(p,length)!=0;
	}
	bool VirtualAddress::QueryInfo(PMEMORY_BASIC_INFORMATION pbi,void* p){
		if(p==0) p = _Handle;
		return ::VirtualQuery(p,pbi,sizeof(MEMORY_BASIC_INFORMATION))!=0;
	}
	DWORD VirtualAddress::SetProtect(DWORD protect,void* p,DWORD length){
		if(p==0) p = _Handle;
		if(length==0) length = _Size;
		DWORD old = 0;
		if(::VirtualProtect(p,length,protect,&old)) return old;
		return 0;
	}
	bool VirtualAddress::Free(){
		if(::VirtualFree(_Handle,0,MEM_RELEASE))
		{
			_Handle = 0;
			_Size = 0;
			return 1;
		}
		return 0;
	}
#define HANDLE_SIZE 4	//32位系统的指针大小是4
	void pointerInfo(void* p){
		Print(L"pre:%X,cur:%X,next:%X,8(head)+len:%X",*(char**)((char*)p),(char*)p,*(char**)((char*)p+4),*(char**)((char*)p+8));
	}

	inline uint _getPointerSize(char* p){
		return *(uint*)(p+HANDLE_SIZE*2);
	}
	inline char* _getPrePointer(char* p){
		return *(char**)p;
	}
	inline char* _getNextPointer(char* p){
		return *(char**)(p+HANDLE_SIZE);
	}
	inline void _setPointer(char* p,char* pre,char* after,uint len){
		*(char**)(p) = pre;
		*(char**)(p+HANDLE_SIZE) = after;
		*(uint*)(p+HANDLE_SIZE*2) = len + HANDLE_SIZE*3;
	}
	inline void _setPointer(char* p,char* after,uint len){
		*(char**)(p+HANDLE_SIZE) = after;
		*(uint*)(p+HANDLE_SIZE*2) = len + HANDLE_SIZE*3;
	}
	inline void _setPointerAfter(char* p,char* after){
		*(char**)(p+HANDLE_SIZE) = after;
	}
	inline void _setPointerPre(char* p,char* pre){
		*(char**)p = pre;
	}
	inline void _setPointerSize(char* p,uint len){
		*(uint*)(p+HANDLE_SIZE*2) = len + HANDLE_SIZE*3;
	}
	inline char* _getMemory(char* p){
		return p+HANDLE_SIZE*3;
	}
	inline char* _getPointer(char* mem){
		return mem-HANDLE_SIZE*3;
	}
	bool _findPointer(char* mem,char* cur){
		char* p = cur;
		while(p){
			if(p==mem){
				if(_getPointerSize(p)==0) return false;
				return true;
			}
			p = _getPrePointer(p);
		}
		p = cur;
		while(p){
			p = _getNextPointer(p);
			if(p==mem) return true;
		}
		return false;
	}
	/*char* FastMemory::_findSpaceForward(uint len){
		char* p = 0,*pt;
		pt = _cur;
		while(pt){
			p = _allocIn(pt,len);
			if(p!=NULL) break;
			pt = _getPrePointer(pt);
		}
		return p;
	}
	char* FastMemory::_findSapceAfter(uint len){
		char* p = 0,*pt;
		pt = _cur;
		while(1){
			pt = _getNextPointer(pt);
			if(pt==NULL) break;
			p = _allocIn(pt,len);
			if(p!=NULL) break;
		}
		return p;
	}
	//在指定指针后面分配内存, 如果指针后面的空间不足, 返回 0 .
	char* FastMemory::_allocIn(char* pt,uint len){
		char* next = _getNextPointer(pt);
		if(next==0) next = (char*)_handle.Handle() + _handle.Length();
		uint size = _getPointerSize(pt);
		if((int)size<0){
			CONASSERT(L"fatal error: fast heap is damaged");
			return 0;
		}
		int freeLen = next - pt - size - HANDLE_SIZE*3;
		if((int)len>freeLen) return 0;
		char* p = pt + size;
		next = _getNextPointer(pt);
		if(size==0) pt = 0;
		_setPointer(p,pt,next,len);
		if(pt) _setPointerAfter(pt,p);
		if(next) _setPointerPre(next,p);
		_cur = p;
		return p;
	}
	FastMemory::FastMemory():_cur(0){
	}

	//初始化大小
	bool FastMemory::Create(uint total){
		LCS lcs(_cs);
		if(_cur) return 0;
		uint section = total/0x10000;
		if(!_handle.Reserve(section+1)){
			wchar_t buf[32];
			int n = swprintf_s(buf,32,L"申请高速内存失败 size=%d",total);
			::MessageBox(0,buf,L"FastMemory::Create",MB_ICONERROR);
			CONASSERT(L"FastMemory.Create Error");
			return 0;
		}
		if(!_handle.Commit()){
			wchar_t buf[32];
			int n = swprintf_s(buf,32,L"申请高速内存失败 size=%d",total);
			::MessageBox(0,buf,L"FastMemory::Create",MB_ICONERROR);
			CONASSERT(L"FastMemory.Create Error");
			return 0;
		}
		_cur = (char*)_handle.Handle();
		_setPointer(_cur,0,0,-HANDLE_SIZE*3);
		return true;
	}

	void* FastMemory::Alloc(uint len){
		if(len==0) return 0;
		if(len>0x100000){
			return SYSAlloc(len);
		}
		LCS lcs(_cs);
		if(_handle.IsNull()) return 0;

		char* p;
		p = _findSpaceForward(len);
		if(p!=NULL) return _getMemory(p);
		p = _findSapceAfter(len);
		if(p==NULL){
			//Print(L"FastMemory.Alloc: 可用内存不足,需求:%X",len);
			CONASSERT(L"FastMemory.Alloc: 预分配内存已经用尽");
			_cs.Unlock();
			return SYSAlloc(len);
		}
		return _getMemory(p);
	}
	uint FastMemory::GetLength(void* mem){
		if(_handle.IsNull()||mem==NULL) return 0;
		if(isSysMemoryPointer(_handle,mem)){
			return SYSSize(mem);
		}
		LCS lcs(_cs);
		return _getPointerSize(_getPointer((char*)mem))-HANDLE_SIZE*3;
	}
	void FastMemory::Close(){
		LCS lcs(_cs);
		_handle.Free();
		_cur = 0;
	}
	void* FastMemory::ReAlloc(void* mem,uint size){
		if(isSysMemoryPointer(_handle,mem)){
			void* p = SYSRealloc(mem,size);
			_ASSERT_EXPR(p!=NULL,L"FastMemory::ReAlloc: SYSRealloc failed");
			return p;
		}
		LCS lcs(_cs);
		if(_handle.IsNull()) return 0;
		uint preLen = GetLength(mem);
		if(preLen==size) return mem;
		Free(mem);
		void* p = Alloc(size);
		if(p==0){//空间不足, 切换到系统内存.
			void* cur = SYSAlloc(size);
			if(cur){
				memcpy(cur,mem,preLen);
			}
			return cur;
		}
		if(p!=mem&&mem!=0)
			memcpy(p,mem,preLen);
		return p;
	}
	//这个函数无法判定mem的有效性, 而且如果mem不正确, 可能会破坏整个内存对象. 访问释放的内存并不会引起错误.
	bool FastMemory::Free(void* mem){
		if(mem==0) return 0;
		if(isSysMemoryPointer(_handle,mem)){
			bool r = SYSFree(mem);
			_ASSERT_EXPR(r,L"FastMemory::ReAlloc: SYSFree failed");
			return r;
		}
		LCS lcs(_cs);
		if(_handle.IsNull()) return 0;
		char* pt = _getPointer((char*)mem);
#ifdef _DEBUG
		if(!_findPointer((char*)pt,_cur)){
			//Print(L"FastMemory.Free(%x):not find pointer",pt);
			CONASSERT(L"FastMemory.Free:pointer not find");
			return 0;
		}
#endif
		char* pre = _getPrePointer(pt);
		char* after = _getNextPointer(pt);
		if(pre) _setPointerAfter(pre,after);
		else{
			pre = (char*)_handle.Handle();
			_setPointer(pre,0,after,-HANDLE_SIZE*3);
		}
		if(after){
			_setPointerPre(after,pre);
		}
		_cur = pre;
		return true;
	}
#ifdef _DEBUG
	void FastMemory::PrintInfo(){
		char* p = (char*)_handle.Handle();
		while(p){
			pointerInfo(p);
			p = _getNextPointer(p);
		}
	}
#endif*/
	IMemory::IMemory():_cur(0),_length(0){
	}
	char* IMemory::_findSpaceForward(uint len){
		char* p = 0,*pt;
		pt = _cur;
		while(pt){
			p = _allocIn(pt,len);
			if(p!=NULL) break;
			pt = _getPrePointer(pt);
		}
		return p;
	}
	char* IMemory::_findSapceAfter(uint len){
		char* p = 0,*pt;
		pt = _cur;
		while(1){
			pt = _getNextPointer(pt);
			if(pt==NULL) break;
			p = _allocIn(pt,len);
			if(p!=NULL) break;
		}
		return p;
	}
	//在指定指针后面分配内存, 如果指针后面的空间不足, 返回 0 .
	char* IMemory::_allocIn(char* pt,uint len){
		char* next = _getNextPointer(pt);
		if(next==0) next = (char*)_handle + _length;
		uint size = _getPointerSize(pt);
		int freeLen = next - pt - size - HANDLE_SIZE*3;
		if((int)len>freeLen) return 0;
		char* p = pt + size;
		next = _getNextPointer(pt);
		if(size==0) pt = 0;
		_setPointer(p,pt,next,len);
		if(pt) _setPointerAfter(pt,p);
		if(next) _setPointerPre(next,p);
		_cur = p;
		return p;
	}
	void* IMemory::Alloc(uint len){
		if(len==0) return 0;
		if(len%2) len += 1;
		LCS lcs(_cs);
		if(_handle==NULL) return 0;

		char* p;
		p = _findSpaceForward(len);
		if(p!=NULL) return _getMemory(p);
		p = _findSapceAfter(len);
		if(p==NULL){
#ifdef ALERT_FASTMEM_FAILED
			::MessageBox(0,L"预分配内存已经用尽，可能存在内存泄露，应该尽快释放不使用的内存。",L"Error",MB_ICONERROR);
#ifndef _DEBUG 
	::ExitProcess(0);
#endif		
#endif
			CONASSERT(L"IMemory.Alloc: 预分配内存已经用尽");
			_cs.Unlock();
			return 0;
		}
		return _getMemory(p);
	}
	uint IMemory::GetLength(void* mem){
		if(_handle==NULL||mem==NULL) return 0;
		LCS lcs(_cs);
		return _getPointerSize(_getPointer((char*)mem))-HANDLE_SIZE*3;
	}
	void* IMemory::ReAlloc(void* mem,uint size){
		LCS lcs(_cs);
		if(_handle==NULL) return 0;
		if(size%2) size += 1;
		uint preLen = GetLength(mem);
		if(preLen==size) return mem;
		Free(mem);
		void* p = Alloc(size);
		if(p==0){//空间不足, 切换到系统内存.
			Alloc(preLen);
			return 0;
		}
		if(p!=mem&&mem!=0)
			memcpy(p,mem,preLen);
		return p;
	}
	//这个函数无法判定mem的有效性, 而且如果mem不正确, 可能会破坏整个内存对象. 访问释放的内存并不会引起错误.
	bool IMemory::Free(void* mem){
		if(mem==0) return 0;
		LCS lcs(_cs);
		if(_handle==NULL) return 0;
		char* pt = _getPointer((char*)mem);
#ifdef _DEBUG
		if(!_findPointer((char*)pt,_cur)){
			//Print(L"IMemory.Free(%x):not find pointer",pt);
			CONASSERT(L"IMemory.Free:pointer not find");
			return 0;
		}
#endif
		char* pre = _getPrePointer(pt);
		char* after = _getNextPointer(pt);
		if(pre) _setPointerAfter(pre,after);
		else{
			pre = (char*)_handle;
			_setPointer(pre,0,after,-HANDLE_SIZE*3);
		}
		if(after){
			_setPointerPre(after,pre);
		}
		_cur = pre;
		return true;
	}
#ifdef _DEBUG
	void IMemory::PrintInfo(){
		char* p = (char*)_handle;
		while(p){
			pointerInfo(p);
			p = _getNextPointer(p);
		}
	}
#endif
	bool ExecMemory::Create(int size){
		LCS lcs(_cs);
		if(_handle!=0) return false;
		_map = ::CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_EXECUTE_READWRITE|SEC_COMMIT,0,size,0);
		if(_map==0){
			return false;
		}
		_handle = MapViewOfFile(_map,FILE_MAP_EXECUTE|FILE_MAP_ALL_ACCESS,0,0,size);
		if(_handle==NULL){
			CloseHandle(_map);
			_map = NULL;
			return false;
		}
		_cur = (char*)_handle;
		_setPointer(_cur,0,0,-HANDLE_SIZE*3);
		_length = size;
		return 1;
	}
	bool ExecMemory::Close(){
		LCS lcs(_cs);
		if(_handle==NULL) return false;
		UnmapViewOfFile(_handle);
		CloseHandle(_map);
		_length = 0;
		_cur = 0;
		return 1;
	}
	bool FastMemory::Create(uint total){
		/*LCS lcs(_cs);
		if(_handle!=0) return false;
		uint section = total/0x10000;
		if(total%0x10000) section += 1;
		uint size = section*0x10000;
		_handle = (char*)::HeapAlloc(GetProcessHeap(),0,size);
		if(_handle==0){
			return false;
		}
		_cur = (char*)_handle;
		_setPointer(_cur,0,0,-HANDLE_SIZE*3);
		_length = size;
		return 1;*/
		LCS lcs(_cs);
		if(_cur) return 0;
		uint section = total/0x10000;
		if(total%0x10000) section += 1;
		if(_handle) ::VirtualFree(_handle,0,MEM_FREE);
		_length = section*0x10000;
		_handle = ::VirtualAlloc(0,_length,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
		if(_handle==0){
			_length = 0;
			wchar_t buf[32];
			int n = swprintf_s(buf,32,L"申请高速内存失败 size=%d",total);
			::MessageBox(0,buf,L"FastMemory::Create",MB_ICONERROR);
			CONASSERT(L"FastMemory.Create Error");
			return 0;
		}
		/*if(NULL==::VirtualAlloc(_handle,_length,MEM_COMMIT,PAGE_READWRITE)){
			wchar_t buf[32];
			int n = swprintf_s(buf,32,L"申请高速内存失败 size=%d",total);
			::MessageBox(0,buf,L"FastMemory::Create",MB_ICONERROR);
			CONASSERT(L"FastMemory.Create Error");
			return 0;
		}*/
		//////////////////////
		_cur = (char*)_handle;
		_setPointer(_cur,0,0,-HANDLE_SIZE*3);
		return true;
	}
	bool FastMemory::IsFastMemory(void* mem){
		UINT p = (UINT)(UINT_PTR)mem;
		UINT p0 = (UINT)(UINT_PTR)_handle;
		UINT p1 = p0 + _length;
		return (p<=p0||p>=p1);
	}
}