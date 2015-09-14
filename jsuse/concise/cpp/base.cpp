#include "../stdafx.h"
#include "../../concise.h"
namespace cs{
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
#define PROGRESS_INDEX_FASTMEM_HANDLE 0
	FastMemory* initFastMem(uint mb){
		static FastMemory fm;
		void** fmadr = getProgressMemSpace(PROGRESS_INDEX_FASTMEM_HANDLE);
		if(!fmadr[PROGRESS_INDEX_FASTMEM_HANDLE]){
			fmadr[PROGRESS_INDEX_FASTMEM_HANDLE] = &fm;
			fm.CreateMB(mb);
		}
		return (FastMemory*)fmadr[PROGRESS_INDEX_FASTMEM_HANDLE];
	}
#define PROGRESS_INDEX_EXECMEM_HANDLE 1
	ExecMemory* initExecMem(uint mb){
		static ExecMemory em;
		void** fmadr = getProgressMemSpace(PROGRESS_INDEX_EXECMEM_HANDLE);
		if(!fmadr[PROGRESS_INDEX_EXECMEM_HANDLE]){
			fmadr[PROGRESS_INDEX_EXECMEM_HANDLE] = &em;
			em.Create(mb);
		}
		return (ExecMemory*)fmadr[PROGRESS_INDEX_EXECMEM_HANDLE];
	}
	FastMemory* GetGlobalFastMemory(){
		static FastMemory* pfm = initFastMem(FASTMEM_SIZE_MB);
		return pfm;
	}
	ExecMemory* GetGlobalExecMemory(){
		static ExecMemory* pem = initExecMem(EXECMEM_SIZE);
		return pem;
	}
	void InitConciseLib(uint fastMemSize,uint execMemSize){
		initFastMem(fastMemSize);
		initExecMem(execMemSize);
	}
	/*bool SetFastMemory(int size){
		if(size<=0) return 0;
		if(!GetGlobalFastMemory()->IsNull()) return 0;
		return GetGlobalFastMemory()->Create(size);
	}*/
	//分配一定数量的内存,当size = 0时,这个函数仍然会分配一个1字节的内存,返回一个有效的内存指针.
	void* FastAlloc(int size){
		FastMemory* csn = GetGlobalFastMemory();
		return csn->Alloc(size);
	}
	//重新分配一个内存指针的大小,当p=0时,这个函数作用和MemoryAlloc相同.
	void* FastRealloc(void* p,int size){
		FastMemory* csn = GetGlobalFastMemory();
		return csn->ReAlloc(p,size);
	}
	//释放已经分配的内存.成功返回true,失败或p为0返回false.
	bool FastFree(void* p){
		FastMemory* csn = GetGlobalFastMemory();
		return csn->Free(p);
	}
	//返回内在的大小,返回0如果p=0,如果p不是一个有效的内存指针,会引发一个异常.
	int FastSize(void* p){
		FastMemory* csn = GetGlobalFastMemory();
		return csn->GetLength(p);
	}
	bool IsFastMemory(void* p){
		FastMemory* csn = GetGlobalFastMemory();
		return csn->IsFastMemory(p);
	}
	void* SYSAlloc(int size){
		return ::HeapAlloc(GetProcessHeap(),0,size);
	}
	bool SYSFree(void* p){
		if(p) return ::HeapFree(GetProcessHeap(),0,p)!=0;
		else return false;
	}
	void* SYSRealloc(void* p,int size){
		if(p) return ::HeapReAlloc(GetProcessHeap(),0,p,size);
		else return ::HeapAlloc(GetProcessHeap(),0,size);
	}
	int SYSSize(void* p){
		if(p) return (int)::HeapSize(GetProcessHeap(),0,p);
		else return 0;
	}
	void* MemoryAlloc(int size,bool sys){
		if(sys) return ::HeapAlloc(GetProcessHeap(),0,size);
		FastMemory* csn = GetGlobalFastMemory();
		return csn->Alloc(size);
	}
	void* MemoryRealloc(void* p,int size,bool sys){
		if(sys){
			if(p) return ::HeapReAlloc(GetProcessHeap(),0,p,size);
			else return ::HeapAlloc(GetProcessHeap(),0,size);
		}
		FastMemory* csn = GetGlobalFastMemory();
		return csn->ReAlloc(p,size);
	}
	bool MemoryFree(void* p,bool sys){
		if(sys){
			if(p) return ::HeapFree(GetProcessHeap(),0,p)!=0;
			else return false;
		}
		FastMemory* csn = GetGlobalFastMemory();
		return csn->Free(p);
	}
	int MemorySize(void* p,bool sys){
		if(sys){
			if(p) return (int)::HeapSize(GetProcessHeap(),0,p);
			else return 0;
		}
		FastMemory* csn = GetGlobalFastMemory();
		return csn->GetLength(p);
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