#include "../stdafx.h"
#include "../../jubase.h"

namespace ju{
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
	inline void _setPointer(char* p,uint len){
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
#define research_count 128
#define large_small_divider	102400
#define mem_head_size 12
	IMemory::IMemory():_curLarge(0),_curSmall(0),_count(0),_length(0),
		_handle(0),_total_used(0),_total_alloced(0){
	}
	char* IMemory::_findLargeSpace(uint len){
		char* pt;
		pt = _curLarge;
		while(1){
			char* p = _allocIn(pt,len);
			if(p!=NULL){
				_curLarge = p;
				return p;
			}
			char* npt = _getNextPointer(pt);
			if(npt==NULL) return 0;
			if((uint)(UINT_PTR)npt<(uint)(UINT_PTR)_handle||(uint)(UINT_PTR)npt>(uint)(UINT_PTR)(npt+_length)){
				CONASSERT(L"内存结构被破坏");
			}
			pt = npt;
		}
	}
	char* IMemory::_findSmallSpace(uint len){
		char* pt;
		pt = _curSmall;
		while(1){
			char* p = _allocIn(pt,len);
			if(p!=NULL){
				_curSmall = p;
				return p;
			}
			char* npt = _getNextPointer(pt);
			if(npt==NULL) return 0;
			if((uint)(UINT_PTR)npt<(uint)(UINT_PTR)_handle||(uint)(UINT_PTR)npt>(uint)(UINT_PTR)(npt+_length)){
				CONASSERT(L"错误的pt");
			}
			pt = npt;
		}
	}
	char* IMemory::_allocIn(char* pt,uint len){
		char* next = _getNextPointer(pt);
		uint size = _getPointerSize(pt);
		int freelen;
		if(next==0){
			freelen = (char*)_handle + _length - pt - size - mem_head_size;
		}else{
			freelen = next - pt - size - mem_head_size;
		}
		if((int)len>freelen) return 0;

		char* p = pt + size;
		if(p==_handle) pt = 0;
		_setPointer(p,pt,next,len);
		if(pt) _setPointerAfter(pt,p);
		if(next) _setPointerPre(next,p);
		return p;
	}
	void* IMemory::Alloc(uint len){
		if(len==0) return 0;
		if(len%2) len += 1;
		LCS lcs(_cs);
		if(_handle==NULL) return 0;

		if(_count>=100){
			_curSmall = (char*)_handle;
			_curLarge = (char*)_handle;
			_count = 0;
		}
		char* p;
		if(len>=large_small_divider){
			p = _findLargeSpace(len);
		}else{
			p = _findSmallSpace(len);
		}
		if(p==NULL&&_count>0){
			_curSmall = (char*)_handle;
			_curLarge = (char*)_handle;
			if(len>=large_small_divider){
				p = _findLargeSpace(len);
			}else{
				p = _findSmallSpace(len);
			}
		}
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
		_total_used += len;
		_count++;
		return _getMemory(p);
	}
	uint IMemory::GetLength(void* mem){
		if(_handle==NULL||mem==NULL) return 0;
		LCS lcs(_cs);
		return _getPointerSize(_getPointer((char*)mem))-HANDLE_SIZE*3;
	}
	void* IMemory::ReAlloc(void* mem,uint len){
		LCS lcs(_cs);
		if(_handle==NULL) return 0;
		if(len%2) len += 1;
		uint preLen = GetLength(mem);
		if(preLen==len) return mem;
		if(mem){
			char* pt = _getPointer((char*)mem);
			int size = _getPointerSize(pt);
			char* next = _getNextPointer(pt);
			if(next==0) next = (char*)_handle + _length;
			int freeLen = next - pt - size - HANDLE_SIZE*3;
			if((int)len<=freeLen){
				_setPointer(pt,len);
				return mem;
			}
			Free(mem);
		}
		void* p = Alloc(len);
		if(p==0){//空间不足
			return 0;
		}
		if(p!=mem&&mem!=0){
			uint clen = len<preLen?len:preLen;
			memcpy(p,mem,clen);
		}
		return p;
	}
	//这个函数无法判定mem的有效性, 而且如果mem不正确, 可能会破坏整个内存对象. 访问释放的内存并不会引起错误.
	bool IMemory::Free(void* mem){
		if(mem==0) return 0;
		LCS lcs(_cs);
		if(_handle==NULL) return 0;
		char* pt = _getPointer((char*)mem);
#ifdef _DEBUG
		if(!_findPointer((char*)pt,_curLarge)){
			CONASSERT(L"IMemory.Free:pointer not find");
			return 0;
		}
#endif
		uint size = _getPointerSize(pt);
		char* pre = _getPrePointer(pt);
		char* after = _getNextPointer(pt);
		if(pre){
			_setPointerAfter(pre,after);
		}else{
			pre = (char*)_handle;
			_setPointer(pre,0,after,-HANDLE_SIZE*3);
		}
		if(after){
			_setPointerPre(after,pre);
		}

		if(size>=large_small_divider){
			_curLarge = pre;
			if((uint)(UINT_PTR)_curLarge<(uint)(UINT_PTR)_curSmall){
				_curSmall = _curLarge;
			}
		}else{
			_curSmall = pre;
		}
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
		_curLarge = (char*)_handle;
		_curSmall = _curLarge;
		_setPointer(_curLarge,0,0,-HANDLE_SIZE*3);
		_length = size;
		return 1;
	}
	bool ExecMemory::Close(){
		LCS lcs(_cs);
		if(_handle==NULL) return false;
		UnmapViewOfFile(_handle);
		CloseHandle(_map);
		_length = 0;
		_curLarge = _curSmall = 0;
		return 1;
	}
	bool FastMemory::Create(uint total){
		LCS lcs(_cs);
		if(_curLarge) return 0;
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
		_curLarge = (char*)_handle;
		_curSmall = _curLarge;
		_setPointer(_curLarge,0,0,-HANDLE_SIZE*3);
		return true;
	}
	bool FastMemory::IsFastMemory(void* mem){
		UINT p = (UINT)(UINT_PTR)mem;
		UINT p0 = (UINT)(UINT_PTR)_handle;
		UINT p1 = p0 + _length;
		return (p<=p0||p>=p1);
	}
}