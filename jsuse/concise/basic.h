#pragma once
#define DEFTYPE(name) typedef struct name##__{int unused;}name##__,*name
#define WINMAIN		int __stdcall wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
#define MAIN		int _tmain(int argc, _TCHAR* argv[])
#define CONASSERT(msg) _ASSERT_EXPR(0,msg)
//明确一个 0 的类型, 而不是指针, 避免重载函数调用不明.
#define IntZero (int)0
#define UIntZero (uint)0
#define usexpstyle comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#ifndef uint
typedef unsigned int uint;
#endif
#ifndef ushort
typedef unsigned short ushort;
#endif
#ifndef uchar
typedef unsigned char uchar;
#endif
#ifndef ulong
typedef unsigned long ulong;
#endif
#ifndef uint64
typedef unsigned __int64 uint64;
#endif
#ifndef int64
typedef __int64 int64;
#endif

namespace cs{
	//使用系统内存分配函数, 从程序的默认堆上分配内存
	//在默认堆上分配内存, 这样分配的内存, 可以跨DLL使用和分配.
	//分配一定数量的内存,当size = 0时,这个函数仍然会分配一个1字节的内存,返回一个有效的内存指针.
	CONCISE_API void* SYSAlloc(int size);
	//重新分配一个内存指针的大小,当p=0时,这个函数作用和MemoryAlloc相同.
	CONCISE_API void* SYSRealloc(void* p,int size);
	//释放已经分配的内存.成功返回true,失败或p为0返回false .
	CONCISE_API bool SYSFree(void* p);
	//返回内存的大小,返回0如果p=0,如果p不是一个有效的内存指针,会引发一个异常.
	CONCISE_API int SYSSize(void* p);
	
	//初始化高速内存, 这个函数必须先于任何使用告诉内存的函数之前调用, 包括自动初始化的对象.
	/*@function bool SetFastMemory(int size = 16)
	@text 设置预分配高速内存的大小。预分配高速内存对应 FastAlloc，FastRealloc，FastFree，FastSize函数，这些
	函数操作的内存都是预分配的，所以速度快。系统默认的小内存需求也是从预分配内存分配，高速内存只是分配，
	重新分配和释放的速度快，不是指内存的数据处理速度快，处理速度和系统内存是一样的，但是
	分配和释放速度比系统内存可能高几十倍到上百倍。
	@text 此函数只能在程序初始化时调用一次，也就是没有任何内存函数被调用之前，一旦有高速内存函数
	被使用，后续调用会失败，重复调用也会失败。
	@param size -- int
	@text 要分配的内存大小，单位是MB，默认值是16MB，如果没有调用这个函数，系统默认分配16MB的高速内存。
	@return bool
	@text 分配是否成功。*/
	//CONCISE_API bool SetFastMemory(int size = 16);
	/*@function void* FastAlloc(int size)
	@text 使用快速分配内存, 这个内存总的可用量是通过SetFastMemory来设置的，
	默认是16M. 32位程序的地址空间只有4G，实际上当中分配超过1G的内存失败的概率
	非常高，这和系统可用的物理内存无关，而是地址空间耗尽。所以大内存分配在32位
	程序中，需要特殊的操作，不能用一般的内存分配函数。
	@text 如果分配的内存大于1M，或者预分配内存已经耗尽，此函数会调用系统内存来分配空间。
	@text 一般来说 String，Memory等对象，默认都是用高速内存来分配的，所以，如果
	这些对象有大内存需求，需要特别指明使用系统内存。
	@text Fast内存操作比Windows系统操作快近100倍, 它的策略是预分配一块内存, 注意, 
	如果这些内存没有使用, 并不会占有物理内存. 但是会占用虚拟内存页面, 如果运行了
	很多个实例, 虚拟内存页面可能会用尽. 因为虚拟内存也是有上限的。
	@param size -- int
	@text 请求的内存大小。
	@return void*
	@text 成功返回内存指针，失败返回 NULL 。
	*/
	CONCISE_API void* FastAlloc(int size);
	/*@function void* FastRealloc(void* p,int size)
	@text 重新分配一个内存指针的大小,当 p=0 时,这个函数作用和FastAlloc相同. 
	如果 size 大于 1M 或者预分配内存已经耗尽，这个函数会在系统内存里分配空间。
	*/
	CONCISE_API void* FastRealloc(void* p,int size);
	/*@function bool FastFree(void* p)
	@text 释放已经分配的内存. 因为 Fast 函数仍然可能分配系统内存，所以此函数实际上可可以用于系统内存和预分配内存。
	@return bool
	@text 成功返回true, 失败或 p 为 0 返回 false.
	*/
	CONCISE_API bool FastFree(void* p);
	//返回内存的大小,注意, 如果p 是一个无效内存, 仍然可能返回一个不可用的尺寸.
	CONCISE_API int FastSize(void* p);
	//一个内存指针是否是预分配内存，此函数只是简单的检测指针是否在预分配内存范围内，范围内的错误指针仍然返回 true。
	CONCISE_API bool IsFastMemory(void* p);

	//FAST_MEMORY开关控制Memory开头的内存分配函数使用哪个版本(快速Fast还是系统SYS).
	CONCISE_API void* MemoryAlloc(int size,bool sys = 0);
	CONCISE_API void* MemoryRealloc(void* p,int size,bool sys = 0);
	CONCISE_API bool MemoryFree(void* p,bool sys = 0);
	CONCISE_API int MemorySize(void* p,bool sys = 0);
	//添加位
	template<typename T> inline void BitAdd(T& bit,T add){
		bit |= add;
	}
	//移除位
	template<typename T> inline void BitRemove(T& bit,T remove){
		bit = bit&~remove;
	}
	//较大值.
	template<typename T> T Maximum(T t1,T t2){
		if(t1>t2) return t1;
		else return t2;
	}
	//较小值.
	template<typename T> T Minimum(T t1,T t2){
		if(t1<t2) return t1;
		else return t2;
	}
	//是否是一个句柄(高位是否为0).
	template<typename T> bool IsHandle(T t){
		return (0xffff0000&(INT_PTR)t)!=0;
	}
	//把句柄作为纯数值来求差.
	template<typename T,typename R> inline int SubHandle(T a,R b){
		return (int)((UINT_PTR)a - (UINT_PTR)b);
	}
	//32位任意类型转换.
	template<typename OutType,typename InType> OutType Cast(InType inValue){
		return (OutType)(INT_PTR)inValue;
	}
	//转换一个类型为任意另一个类型.
	template<typename OutType,typename InType> OutType SuperCast(InType inValue){
		union{
			OutType Out;
			InType In;
		}addr;
		addr.In = inValue;
		return addr.Out;
	}

	//对象可以被简单复制的情况，从这一个结构派生。
	struct CONCISE_API _struct{
		_struct(){}
		void* operator new(size_t size){
			return MemoryAlloc((int)size);
		}
		void operator delete(void* p){
			MemoryFree(p);
		}
		inline void* operator new(size_t,void* where){
			return where;
		}
		inline void operator delete(void*,void*){}
		inline void* operator new(size_t size,LPCSTR lpszFileName,int nLine){  
			return MemoryAlloc((int)size);
		}
		inline void operator delete(void* p, LPCSTR lpszFileName, int nLine){  
			MemoryFree(p);
		}
	};
	//这个类用于避免用全局的new来生成对象.这样的对象可以在DLL中生成,但在应用程序中释放.所有类型都由它派生.
	class CONCISE_API _class{
	public:
		_class(){}
		inline void* operator new(size_t size){
			return MemoryAlloc((int)size);
		}
		inline void operator delete(void* p){
			MemoryFree(p);
		}
		inline void* operator new(size_t,void* where){
			return where;
		}
		inline void operator delete(void*,void*){}
		inline void* operator new(size_t size,LPCSTR lpszFileName,int nLine){  
			return MemoryAlloc((int)size);
		}
		inline void operator delete(void* p, LPCSTR lpszFileName, int nLine){  
			MemoryFree(p);
		}
	private:
		//重载这两个函数来复制类.(如果对象可能作为参数传递则应该重载这两个函数,否则编译器可能提示访问).
		_class(const _class& obj){};				
		//对于绝大多数类,如果你需要用它作为参数,应该采用传地址的方式.
		void operator = (const _class& obj){};	
	};
	//Object可以用来包装一个类, 你可以自由使用包装后的对象作为函数参数或者返回值, 
	//而不用考虑复制构造和析构, 因为这个类自动处理资源释放. 这个类相当于对包装对象的
	//一个引用, 比如Object<A> a = b. a和b实际上包装的是同一个对象.
	template<typename T>class CONCISETL_API Object : public _struct{
	private:
		struct _H{
			T* obj;
			int icount;
		};
		_H* h;
	public:
		inline Object():h(0){
		}
		//构造复制
		inline Object(Object& a):h(0){
			if(!a.h) return;
			h = a.h;
			h->icount++;
		}
		//自动生成一个实例
		inline Object(bool create):h(0){
			if(create) New(new T);
		}
		//初始化包装一个对象, obj不要在外面释放, 它会被自动管理.
		inline Object(T* obj):h(0){
			New(obj);
		}
		//包装一个对象, obj不要在外面释放, 它会被自动管理.
		bool New(T* obj = 0){
			if(h){
				h->icount--;
				if(h->icount==0){
					delete h->obj;
					delete h;
				}
			}
			h = new _H;
			if(h==NULL) return 0;
			if(obj==NULL) obj = new T;
			h->obj = obj;
			h->icount = 1;
			return 1;
		}
		//赋值
		void operator = (Object& a){
			if(h){
				h->icount--;
				if(h->icount==0){
					delete h->obj;
					delete h;
				}
			}
			h = a.h;
			if(h) h->icount++;
		}
		//解除绑定, 如果没有其它Object绑定这个对象, 对象将被释放.
		void Null(){
			if(!h) return;
			h->icount--;
			if(h->icount==0){
				delete h->obj;
				delete h;
			}
			h = 0;
		}
		//是否已经绑定一个对象.
		inline bool IsNull(){
			return h==NULL;
		}
		//引用绑定的对象, 这能够让绑定的对象自动暴露给外部.
		inline T* operator -> (){
			return h->obj;
		}
		//显示返回绑定的对象.
		inline T* Handle(){
			return h->obj;
		}
		inline void Zero(){
			if(h) ::ZeroMemory(h->obj,sizeof(*h->obj));
		}
		inline DWORD Size(){
			return sizeof(T);
		}
		inline ~Object(){
			Null();
		}
	};
	//这个基类含有一个特定类型的句柄.
	template<typename T> class CONCISETL_API HandleType : public _class{
	public:
		//初始化为 0 .
		HandleType():_Handle(0){
			_ASSERT(sizeof(T)==sizeof(void*));
		}		
		//直接转化为相应的类型.
		inline operator T (){
			return _Handle;
		}	
		//用指针的方式调用结构的成员，一般来说HandleType都是指针.
		/*（这引起VAssistX的不兼容）
		inline T operator -> (){
		return _Handle;
		}*/
		//明确获取句柄.
		inline T Handle(){
			return _Handle;
		}	
		//是否为空(0),有些类型不支持这个特性,所以不要在某些类型上调用这个函数,或者重载一个函数.
		inline bool IsNull(){
			return _Handle==0;
		}		
		//绑定到这个对象，如果没有重写这个函数，不能保证资源被正确释放
		inline T Attach(T handle){
			T h = _Handle;
			_Handle = handle;
			return h;
		}
		//把句柄剥离这个类的包装，如果没有重写这个函数，不能保证资源被正确释放
		inline T Detach(){
			T handle = _Handle;
			_Handle = 0;
			return handle;
		}

		T _Handle;
	};
#define AutoStruct(T,v) T v;::ZeroMemory(&v,sizeof(T))
#define AutoStruct2(T,v) T v;::ZeroMemory(&v,sizeof(T));v.cbSize = sizeof(v)
	//主要用于Windows API常用的一类结构,特点是首4字节设为结构的大小(一般叫作cbSize成员).
	//它的作用是把结构用0初始化,然后设置cbSize成员为结构的大小.在API中这是相当重复的工作.
	/*这个对象在MFC出现莫名的问题，编译器识别大小错误4字节，导致改写了外部的内容。
	template<typename T,bool size = false> struct CONCISETL_API AutoStruct : public T,public _class{
		AutoStruct(){
			RtlZeroMemory(this,sizeof(T));
			if(size) *((size_t*)this) = sizeof(T);
		}
		void Zero(){
			RtlZeroMemory(this,sizeof(T));
		}
		operator T& (){
			return *((T*)this);
		}
		void operator = (T& t){
			*((T*)this) = t;
		}
	};*/
}
