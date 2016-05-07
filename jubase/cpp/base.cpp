#include "../stdafx.h"
#include "../../jubase.h"
namespace ju{
#define MAPPING_NAME	L"8BD31C4B59224BCA9EBA426CF248D1AB%08X"
	void** createProgressMemSpace(){
		wchar_t name[64];
		wsprintf(name,MAPPING_NAME,GetCurrentProcessId());
		DWORD size = 4096;
		HANDLE _map = CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE|SEC_COMMIT,0,size,name);
		if(!_map) return 0;
		void** _memSpace = (void**)MapViewOfFile(_map,FILE_MAP_READ|FILE_MAP_WRITE,0,0,size);
		if(!_memSpace){
			CloseHandle(_map);
		}else{
		}
		return _memSpace;
	}
	void** getProgressMemSpace(int index){
		static void** _memSpace = createProgressMemSpace();
		return _memSpace+index;
	}
	//此函数未加锁，必须在已经同步的场合使用。
	void* mallocGlobalStaticMem(uint size){
		static void* globalStaticMem = malloc(0x10000);
		static int offset = 0;
		void* p = (char*)globalStaticMem + offset;
		offset += size;
		if(offset>0x10000){
			::MessageBox(0,L"global memory exhaust",L"error",MB_ICONERROR);
			::ExitProcess(0);
		}
		return p;
	}
#define PROGRESS_INDEX_FASTMEM_HANDLE 0
#define PROGRESS_INDEX_EXECMEM_HANDLE 1
	ExecMemory* initExecMem(uint mb){
		static ExecMemory* em = 0;
		if(em==0){
			MutexLock ml(L"{5210A89B-A05B-4c06-8911-CBFFC1FB32EF}");
			if(em==0){
				void* buf = mallocGlobalStaticMem(sizeof(ExecMemory));
				em = new (buf) ExecMemory;
			}
		}
		void** fmadr = getProgressMemSpace(PROGRESS_INDEX_EXECMEM_HANDLE);
		if(!fmadr[PROGRESS_INDEX_EXECMEM_HANDLE]){
			fmadr[PROGRESS_INDEX_EXECMEM_HANDLE] = em;
			em->Create(mb);
		}
		return (ExecMemory*)fmadr[PROGRESS_INDEX_EXECMEM_HANDLE];
	}
	ExecMemory* GetGlobalExecMemory(){
		static ExecMemory* pem = initExecMem(EXECMEM_SIZE);
		return pem;
	}
	void InitJulib(uint execMemSize){
		initExecMem(execMemSize);
	}
	void* SYSAlloc(int size){
		return malloc(size);//::HeapAlloc(GetProcessHeap(),0,size);
	}
	bool SYSFree(void* p){
		if(p) free(p);//::HeapFree(GetProcessHeap(),0,p)!=0;
		return true;
		//else return false;
	}
	void* SYSRealloc(void* p,int size){
		if(p) return realloc(p,size);//::HeapReAlloc(GetProcessHeap(),0,p,size);
		else return malloc(size);//::HeapAlloc(GetProcessHeap(),0,size);
	}
	int SYSSize(void* p){
		if(p) return (int)::HeapSize(GetProcessHeap(),0,p);
		else return 0;
	}
	//struct Rect32
	Rect32::Rect32(Rect32& rect){
		left = rect.left;
		top = rect.top;
		right = rect.right;
		bottom = rect.bottom;
	}
	void Rect32::SetLocation(int x,int y){
		int dx = x - left;
		int dy = y - top;
		left = x;
		top = y;
		right += dx;
		bottom += dy;
	}
	Rect32::Rect32(int left,int top,int right,int bottom){
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	Rect32::Rect32(Twin pt,Twin sz){
		left = pt.x;
		top = pt.y;
		right = left + sz.x;
		bottom = top + sz.y;
	}

	void Rect32::SetValue(int x,int y,int r,int b){
		left = x;
		top = y;
		right = r;
		bottom = b;
	}
	void Rect32::SetValue(Twin pt,Twin sz){
		left = pt.x;
		top = pt.y;
		right = left + sz.x;
		bottom = top + sz.y;
	}
	void Rect32::OffsetLocation(int dx,int dy){
		left += dx;
		right += dx;
		top += dy;
		bottom += dy;
	}
	void Rect32::Offset(int dx,int dy,int cx,int cy){
		left += dx;
		right += cx;
		top += dy;
		bottom += cy;
	}
	void Rect32::SetSize(int cx,int cy){
		right = left + cx;
		bottom = top + cy;
	}
	void Rect32::SetLocation(Twin pt){
		int dx = pt.x - left;
		int dy = pt.y - top;
		left = pt.x;
		top = pt.y;
		right += dx;
		bottom += dy;
	}
	void Rect32::Increase(int sl,int st,int sr,int sb,bool abs){
		if((left<right)&&(abs)){
			left -= sl;
			right += sr;
		}else{
			left += sl;
			right -= sr;
		}
		if((top>bottom)&&(abs)){
			top -= st;
			bottom -= sb;
		}else{
			top += st;
			bottom -= sb;
		}
	}
	void Rect32::Increase(int sx,int sy,bool abs){
		if((left>right)&&abs){
			left += sx;
			right -= sx;
		}else{
			left -= sx;
			right += sx;
		}
		if((top>bottom)&&abs){
			top += sy;
			bottom -= sy;
		}else{
			top -= sy;
			bottom += sy;
		}
	}
	bool Rect32::PtInRect(LPPOINT lpPt){
		if(left>lpPt->x)
			return 0;
		if(right<=lpPt->x)
			return 0;
		if(top>lpPt->y)
			return 0;
		if(bottom<=lpPt->y)
			return 0;
		return 1;
	}
	bool Rect32::PtInRect(int x,int y){
		if(left>x)
			return 0;
		if(right<=x)
			return 0;
		if(top>y)
			return 0;
		if(bottom<=y)
			return 0;
		return 1;
	}
	void Rect32::Positive(){
		int l,t,r,b;
		l = min(left,right);
		t = min(top,bottom);
		r = max(left,right);
		b = max(top,bottom);
		SetValue(l,t,r,b);
	}
	bool Rect32::Intersect(LPRECT r){
		return ::IntersectRect((LPRECT)this,(LPRECT)this,r)!=0;
	}
	bool Rect32::IsIntersect(LPRECT r){
		Rect32 rc;
		return ::IntersectRect(rc,(LPRECT)this,r)!=0;
	}
}