#pragma once
/*这里是点和矩形的定义.*/
namespace cs{
#pragma pack(push,2)
	//这相当于点
	typedef union CONCISE_API Twin32{
		struct{__int32 x,y;};
		struct{__int64 Value;};

		inline Twin32(){}
		inline Twin32(int x,int y){this->x = x;this->y = y;}
		inline operator POINT(){return *(LPPOINT)this;}
		inline operator LPPOINT(){return (LPPOINT)this;}
		inline operator SIZE(){return *(LPSIZE)this;}
		inline operator LPSIZE(){return (LPSIZE)this;}
		inline SIZE ToSize(){return *(LPSIZE)this;}
		inline POINT ToPoint(){return *(LPPOINT)this;}
		inline LPSIZE ToSizePointer(){return (LPSIZE)this;}
		inline LPPOINT ToPointPointer(){return (LPPOINT)this;}
		inline void operator = (POINT& pt){x = pt.x;y = pt.y;}
		inline void operator = (SIZE& sz){x = sz.cx;y = sz.cy;}
		inline void operator = (LPPOINT pp){x = pp->x;y = pp->y;}
		inline void operator = (LPSIZE ps){x = ps->cx;y = ps->cy;}
		inline bool operator == (POINT& pt){return (x==pt.x)&&(y==pt.y);}
		inline bool operator == (SIZE& sz){return (x==sz.cx)&&(y==sz.cx);}
		inline bool operator == (Twin32& tw){return (x==tw.x)&&(y==tw.y);}
		inline bool operator != (POINT& sz){return (x!=sz.x)||(y!=sz.y);}
		inline bool operator != (SIZE& sz){return (x!=sz.cx)||(y!=sz.cx);}
		inline bool operator != (Twin32& tw){return (x!=tw.x)||(y!=tw.y);}
		inline void SetValue(int x,int y){this->x = x;this->y = y;}
		inline void SetValue(__int64 value){Value = value;}
		inline void Offset(int dx,int dy){x += dx;y += dy;}

		void* operator new(size_t size){return MemoryAlloc((int)size);}
		void operator delete(void* p){MemoryFree(p);}
	}Twin;
	//这相当于16位点
	typedef union CONCISE_API Twin16{
		struct{__int16 x,y;};
		struct{__int32 Value;};

		inline Twin16(){}
		inline Twin16(INT_PTR value){this->Value = (int)value;}
		inline Twin16(short x,short y){this->x = x;this->y = y;}
		inline void SetValue(int value){this->Value = value;}
		inline void SetValue(short x,short y){this->x = x;this->y = y;}
		inline void operator = (int n){Value = n;}
		inline void operator = (LPARAM lParam){Value = (int)lParam;}
		inline void operator = (WPARAM wParam){Value = (int)wParam;}
		inline void operator = (Twin& tw){x = tw.x;y = tw.y;}
		inline void Offset(int x,int y){this->x += x;this->y += y;}
		inline operator Twin (){return Twin(x,y);}
		inline operator int (){return Value;}

		void* operator new(size_t size){return MemoryAlloc((int)size);}
		void operator delete(void* p){MemoryFree(p);}
	}Twin16;
	//矩形定义.
	typedef struct CONCISE_API Rect32 : public _struct{
	public:
		__int32 left,top,right,bottom;

		Rect32(){}
		Rect32(Rect32& rect);
		Rect32(int left,int top,int right,int bottom);
		Rect32(Twin pt,Twin sz);
		inline operator LPRECT(){return (LPRECT)&left;}
		inline operator RECT(){return *((LPRECT)this);}
		inline void operator = (const RECT* pr){memcpy(this,pr,16);}
		inline Twin Size(){return Twin(right - left,bottom - top);}
		inline int Width(){return right - left;}
		inline int Height(){return bottom - top;}
		inline Twin LeftTop(){return *((Twin*)&left);}
		inline Twin RightBottom(){return *((Twin*)&right);}
		inline void operator = (Rect32& r){::MoveMemory(&left,&r.left,sizeof(Rect32));}
		inline void operator = (RECT& r){::MoveMemory(&left,&r,sizeof(Rect32));}
		inline bool operator != (Rect32& r){return memcmp(&left,&r.left,sizeof(RECT))!=0;}
		inline bool operator != (RECT& r){return memcmp(&left,&r,sizeof(RECT))!=0;}
		inline bool operator == (Rect32& r){return !memcmp(&left,&r.left,sizeof(RECT));}
		inline bool operator == (RECT& r){return !memcmp(&left,&r,sizeof(RECT));}
		inline void OffsetSize(int dx,int dy){right += dx;bottom += dy;}
		inline void SetSize(Twin sz){right = left + sz.x;bottom = top + sz.y;}
		inline bool IsEmpty(){return (left==right)|(top==bottom);}

		void SetValue(int x,int y,int r,int b);
		void SetValue(Twin pt,Twin sz);
		void SetSize(int cx,int cy);
		void SetLocation(int x,int y);
		void SetLocation(Twin pt);
		//平移一个矩形,4个参数是各点的变化量.
		void Offset(int dleft,int dtop,int dright,int dbottom);
		//仅仅移位置(整体).
		void OffsetLocation(int dx,int dy);
		//增大(或缩小)矩形,sx是左右分别扩充的量,sy上下分别扩充的量.负值对应缩小.abs决定是否考虑左右关系,left实际上可以在
		//right右边,abs=1时,会考虑这个效果,当sx为正时,矩形总是增大.
		void Increase(int sx,int sy,bool abs = 0);
		//这个函数和上一个函数相同,但可以分别调整4个边的大小.
		void Increase(int sl,int st,int sr,int sb,bool abs = 0);
		//重置左右和上下的位置,使left<right,top<bottom.
		void Positive();
		//判断一个点是否在矩形内部.
		bool PtInRect(LPPOINT pt);
		bool PtInRect(int x,int y);
		//当前矩形和 r 相交，结果存储在当前矩形，这个函数在内部调用API函数IntersectRect
		bool Intersect(LPRECT r);
		//当前矩形和指定矩形是否相交，当前矩形不会改变。
		bool IsIntersect(LPRECT r);
	}Rect;
#pragma pack(pop)
}