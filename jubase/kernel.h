#pragma once
#include "function.h"
#define threadvar __declspec(thread) //线程本地存储变量
namespace ju{
	//使用 MessageBox 显示一个系统对 GetLastError,WSAGetLastError 函数返回值的描述文本对话框。
	JUBASE_API int SystemMessageBox(HWND hWnd = 0,LPCWSTR title = L"信息",DWORD style = MB_ICONINFORMATION,DWORD code = GetLastError());
	//取得系统对 GetLastError,WSAGetLastError 等函数的文本描述. code 是以上函数返回的代码.
	JUBASE_API int GetSystemMessage(String& str,DWORD code = GetLastError());
	/*对于一个阻塞的Event，在调用Wait函数之前调用Signal，通知状态仍然会改变
	多个线程等待这个Event的时候，Signal时，按线程调用Wait的先后顺序通知*/
	class JUBASE_API Event : public HandleType<HANDLE>{
	public:
		Event();
		~Event();
		HANDLE Attach(HANDLE ent){return HandleType<HANDLE>::Attach(ent);}
		HANDLE Detach(){return HandleType<HANDLE>::Detach();}
		//autosinaled：设置Wait函数调用后，Event 状态是被置为未通知状态（autoblock=1）还是维持通知状态（autoblock=0）。signaled：起始状态。
		bool Create(bool autoblock = 1,bool signal = 1,LPCWSTR name = 0);
		//打开一个存在的Event。
		bool Open(LPCWSTR name);
		//设置 Event 状态，signaled = 1，Wait函数会返回。
		void Signal(bool signaled = 1);
		//挂起调用线程。
		DWORD Wait(int ms = -1);
		//关闭对象
		void Close();
	};
	/*CriticalSection类用于多线程调用时，Lock和Unlock函数中间的代码同时只有
	一个线程在执行。但是，注意：Lock函数不阻塞单个线程的多次调用，单线程多次
	调用Lock只会增加 CRITICAL_SECTION LockCount成员的值。
	*/
	class JUBASE_API CriticalSection : public _class{
	protected:
		CRITICAL_SECTION _Handle;
	public:
		inline CriticalSection(){
			InitializeCriticalSection(&_Handle);
		}
		inline ~CriticalSection(){
			DeleteCriticalSection(&_Handle);
		}
		//开始锁定代码，调用后，在这个线程没有调用Unlock之前，其它线程对Lock的调用都会挂起。
		inline void Lock(){
			EnterCriticalSection(&_Handle);
		}
		//解除锁定，挂起的线程随机的一个被解除阻塞。
		inline void Unlock(){
			LeaveCriticalSection(&_Handle);
		}
		inline bool TryLock(){
			return 0!=TryEnterCriticalSection(&_Handle);
		}
	};
	//使用LocalCriticalSection类可以无需调用Lock和Unlock函数，自动锁定整个函数的代码。
	//此类只在函数中作为局部变量使用。
	class JUBASE_API LocalCriticalSection : public _class{
	protected:
		CriticalSection* _handle;
	public:
		inline LocalCriticalSection(CriticalSection& cs){
			_handle = &cs;
			_handle->Lock();
		}
		inline ~LocalCriticalSection(){
			_handle->Unlock();
		}
	};
	class JUBASE_API Mutex : public HandleType<HANDLE>{
	protected:
	public:
		inline Mutex(){}
		inline Mutex(LPCWSTR name){
			Create(name);
		}
		inline ~Mutex(){
			Close();
		}
		inline bool Create(LPCWSTR name = 0,bool block = 0){
			_Handle = ::CreateMutex(0,block,name);
			return _Handle!=0;
		}
		inline bool Signal(){
			return 0!=::ReleaseMutex(_Handle);
		}
		inline DWORD Wait(uint ms = -1){
			return ::WaitForSingleObject(_Handle,ms);
		}
		inline bool Close(){
			if(_Handle==0) return 0;
			return 0!=CloseHandle(_Handle);
		}
	};
	class JUBASE_API MutexLock : public Mutex{
	public:
		inline MutexLock(LPCWSTR name,uint ms = -1){
			Create(name);
			Wait(ms);
		}
		inline ~MutexLock(){
			Signal();
		}
	};
	template<typename R>DWORD __stdcall _ThreadCall(void* p){
		struct PARAM : public _struct{
			Function<R> OnCall;
		};
		PARAM* pp = (PARAM*)p;
		pp->OnCall();
		delete pp;
		return 1;
	}
	template<typename R,typename P1>DWORD __stdcall _ThreadCall(void* p){
		struct PARAM : public _struct{
			P1 p1;
			Function<R,P1> OnCall;
		};
		PARAM* pp = (PARAM*)p;
		pp->OnCall(pp->p1);
		delete pp;
		return 1;
	}
	template<typename R,typename P1,typename P2>DWORD __stdcall _ThreadCall(void* p){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			Function<R,P1,P2> OnCall;
		};
		PARAM* pp = (PARAM*)p;
		pp->OnCall(pp->p1,pp->p2);
		delete pp;
		return 1;
	}
	template<typename R,typename P1,typename P2,typename P3>DWORD __stdcall _ThreadCall(void* p){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			Function<R,P1,P2,P3> OnCall;
		};
		PARAM* pp = (PARAM*)p;
		pp->OnCall(pp->p1,pp->p2,pp->p3);
		delete pp;
		return 1;
	}
	template<typename R,typename P1,typename P2,typename P3,typename P4>DWORD __stdcall _ThreadCall(void* p){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			Function<R,P1,P2,P3,P4> OnCall;
		};
		PARAM* pp = (PARAM*)p;
		pp->OnCall(pp->p1,pp->p2,pp->p3,pp->p4);
		delete pp;
		return 1;
	}
	template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5>DWORD __stdcall _ThreadCall(void* p){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			P5 p5;
			Function<R,P1,P2,P3,P4,P5> OnCall;
		};
		PARAM* pp = (PARAM*)p;
		pp->OnCall(pp->p1,pp->p2,pp->p3,pp->p4,pp->p5);
		delete pp;
		return 1;
	}
	template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>DWORD __stdcall _ThreadCall(void* p){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			P5 p5;
			P6 p6
			Function<R,P1,P2,P3,P4,P5,P6> OnCall;
		};
		PARAM* pp = (PARAM*)p;
		pp->OnCall(pp->p1,pp->p2,pp->p3,pp->p4,pp->p5,pp->p6);
		delete pp;
		return 1;
	}
	//0个参数
	template<typename R,typename PROC>bool AsynCallStd(PROC proc){
		struct PARAM : public _struct{
			Function<R> OnCall;
		};
		PARAM* p = new PARAM;
		p->OnCall.BindStd(proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R>,&p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//1个参数
	template<typename R,typename P1,typename PROC>bool AsynCallStd(PROC proc,P1 p1){
		struct PARAM : public _struct{
			P1 p1;
			Function<R,P1> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->OnCall.BindStd(proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//2个参数
	template<typename R,typename P1,typename P2,typename PROC>bool AsynCallStd(PROC proc,P1 p1,P2 p2){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			Function<R,P1,P2> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->OnCall.BindStd(proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//3个参数
	template<typename R,typename P1,typename P2,typename P3,typename PROC>bool AsynCallStd(PROC proc,P1 p1,P2 p2,P3 p3){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			Function<R,P1,P2,P3> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->p3 = p3;
		p->OnCall.BindStd(proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//4个参数
	template<typename R,typename P1,typename P2,typename P3,typename P4,typename PROC>bool AsynCallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			Function<R,P1,P2,P3,P4> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->p3 = p3;
		p->p4 = p4;
		p->OnCall.BindStd(proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//5个参数
	template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename PROC>bool AsynCallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			P5 p5;
			Function<R,P1,P2,P3,P4,P5> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->p3 = p3;
		p->p4 = p4;
		p->p5 = p5;
		p->OnCall.BindStd(proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4,P5>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//0个参数
	template<typename R,typename OBJ,typename PROC>bool AsynCall(OBJ* obj,PROC proc){
		struct PARAM : public _struct{
			Function<R> OnCall;
		};
		PARAM* p = new PARAM;
		p->OnCall.Bind(obj,proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//1个参数
	template<typename R,typename P1,typename OBJ,typename PROC>bool AsynCall(OBJ* obj,PROC proc,P1 p1){
		struct PARAM : public _struct{
			P1 p1;
			Function<R,P1> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->OnCall.Bind(obj,proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//2个参数
	template<typename R,typename P1,typename P2,typename OBJ,typename PROC>bool AsynCall(OBJ* obj,PROC proc,P1 p1,P2 p2){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			Function<R,P1,P2> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->OnCall.Bind(obj,proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//3个参数
	template<typename R,typename P1,typename P2,typename P3,typename OBJ,typename PROC>bool AsynCall(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			Function<R,P1,P2,P3> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->p3 = p3;
		p->OnCall.Bind(obj,proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//4个参数
	template<typename R,typename P1,typename P2,typename P3,typename P4,typename OBJ,typename PROC>bool AsynCall(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			Function<R,P1,P2,P3> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->p3 = p3;
		p->p4 = p4;
		p->OnCall.Bind(obj,proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//5个参数
	template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename OBJ,typename PROC>bool AsynCall(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
		struct PARAM : public _struct{
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			P5 p5;
			Function<R,P1,P2,P3,P4,P5> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->p3 = p3;
		p->p4 = p4;
		p->p5 = p5;
		p->OnCall.Bind(obj,proc);
		HANDLE HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4,P5>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//线程的挂起内核计数加 1，此计数为 0 时线程恢复运行，返回值是调用之前的内核计数，函数失败返回-1。
	//线程的挂起内核计数减 1，此计数为 0 时线程恢复运行，返回值是调用之前的内核计数，
	//对于一个内核计数为 0 的线程，计数不再减小，也就是说计数不能为负数，函数失败返回-1。
	//除非调用了 Close 函数，否则，实例不能启动下一个线程。当线程启动之后，实际上可以销毁Thread实例，线程的执行不会受到任何影响。
	//如果是重载函数，需要把所有的模板参数都写出，尤其是函数类型，这是很麻烦的，所以尽量不使用重载函数作为参数，这样，只需要传函数的返回类型作为模板函数的第一个模板参数就可以了。
	//因为线程函数都是异步调用的，所以传递的参数必须是在线程运行期间有效的，比如局部实例的指针不能作为参数传递。
	class JUBASE_API Thread : public HandleType<HANDLE>{
	protected:
		DWORD _Id;
	public:
		//0个参数
		template<typename R,typename PROC>bool CallStd(PROC proc){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				Function<R> OnCall;
			};
			PARAM* p = new PARAM;
			p->OnCall.BindStd(proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//1个参数
		template<typename R,typename P1,typename PROC>bool CallStd(PROC proc,P1 p1){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				Function<R,P1> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->OnCall.BindStd(proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//2个参数
		template<typename R,typename P1,typename P2,typename PROC>bool CallStd(PROC proc,P1 p1,P2 p2){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				P2 p2;
				Function<R,P1,P2> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->p2 = p2;
			p->OnCall.BindStd(proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//3个参数
		template<typename R,typename P1,typename P2,typename P3,typename PROC>bool CallStd(PROC proc,P1 p1,P2 p2,P3 p3){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				P2 p2;
				P3 p3;
				Function<R,P1,P2,P3> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->p2 = p2;
			p->p3 = p3;
			p->OnCall.BindStd(proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//4个参数
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename PROC>bool CallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
				Function<R,P1,P2,P3> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->p2 = p2;
			p->p3 = p3;
			p->p4 = p4;
			p->OnCall.BindStd(proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//5个参数
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename PROC>bool CallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
				P5 p5;
				Function<R,P1,P2,P3,P4,P5> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->p2 = p2;
			p->p3 = p3;
			p->p4 = p4;
			p->p5 = p5;
			p->OnCall.BindStd(proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4,P5>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//0个参数
		template<typename R,typename OBJ,typename PROC>bool Call(OBJ* obj,PROC proc){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				Function<R> OnCall;
			};
			PARAM* p = new PARAM;
			p->OnCall.Bind(obj,proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//1个参数
		template<typename R,typename P1,typename OBJ,typename PROC>bool Call(OBJ* obj,PROC proc,P1 p1){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				Function<R,P1> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->OnCall.Bind(obj,proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//2个参数
		template<typename R,typename P1,typename P2,typename OBJ,typename PROC>bool Call(OBJ* obj,PROC proc,P1 p1,P2 p2){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				P2 p2;
				Function<R,P1,P2> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->p2 = p2;
			p->OnCall.Bind(obj,proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//3个参数
		template<typename R,typename P1,typename P2,typename P3,typename OBJ,typename PROC>bool Call(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				P2 p2;
				P3 p3;
				Function<R,P1,P2,P3> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->p2 = p2;
			p->p3 = p3;
			p->OnCall.Bind(obj,proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//4个参数
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename OBJ,typename PROC>bool Call(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
				Function<R,P1,P2,P3> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->p2 = p2;
			p->p3 = p3;
			p->p4 = p4;
			p->OnCall.Bind(obj,proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//5个参数
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename OBJ,typename PROC>bool Call(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			if(_Handle) return 0;
			struct PARAM : public _struct{
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
				P5 p5;
				Function<R,P1,P2,P3,P4,P5> OnCall;
			};
			PARAM* p = new PARAM;
			p->p1 = p1;
			p->p2 = p2;
			p->p3 = p3;
			p->p4 = p4;
			p->p5 = p5;
			p->OnCall.Bind(obj,proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4,P5>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		Thread():_Id(0){}
		//等待线程结束。
		DWORD Wait(uint ms = -1){
			return ::WaitForSingleObject(_Handle,ms);
		}
		//线程的挂起内核计数加 1，此计数为 0 时线程恢复运行，返回值是调用之前的内核计数。
		int Pause(){
			return ::SuspendThread(_Handle);
		}
		//线程的挂起内核计数减 1，此计数为 0 时线程恢复运行，返回值是调用之前的内核计数，对于一个内核计数为 0 的线程，计数不再减小，也就是说计数不能为负数。
		int Resume(){
			return ::ResumeThread(_Handle);
		}
		//关闭线程句柄，不对线程进行任何操作，但是等待函数会返回。
		bool Close(){
			if(!_Handle) return 0;
			if(!CloseHandle(_Handle)){
				_ASSERT(0);
			}
			_Handle = 0;
			_Id = 0;
			return 1;
		}
		//强制结束线程运行，但是线程的堆栈不被清除。
		bool Stop(DWORD code = 0){
			if(!_Handle) return 0;
			::TerminateThread(_Handle,code);
			::CloseHandle(_Handle);
			_Handle = 0;
			return 1;
		}
		//线程是否处于运行状态。
		inline DWORD GetID(){
			return _Id;
		}
		bool IsRunning(){
			if(!_Handle) return 0;
			DWORD code = -1;
			GetExitCodeThread(_Handle,&code);
			return code==STILL_ACTIVE;
		}
		~Thread(){
			Close();
		}
	};
	//TaskPool, 一个 TaskPool 只能运行一个类型的回调函数，因为这些线程是需要复用的，必须预制参数模式，
	//因为一个已经运行的线程，是无法改变回调函数参数格式的（个数和类型），但是可以改变参数的值。
	//线程池的线程是一直运行或者挂起的，不会退出，所以任务是否已经执行完，需要用户从回调函数是否已经返回
	//来自己判断，比如设置一个事件，但是线程池本身不提供这个功能。
	//Close 函数关闭所有线程，但是会等待所有正在执行的任务完成，可以设置这个等待时间，但是如果某个任务
	//被阻塞，线程将被强制结束，但是这显然会造成内存泄露，所以只有在为了程序退出时更快才应该使用这个方式，
	//运行状态的程序不应该强制结束线程。
	struct JUBASE_API _TaskParam : public _struct{
		HANDLE handle;
		bool exit;
		bool over;
		_TaskParam():handle(0),exit(0),over(0){}
	};
	class JUBASETL_API _TaskPool : public _class{
	protected:
		CriticalSection _cs;
	public:
	};
	template<typename R,typename P1 = NullType,typename P2 = NullType,typename P3 = NullType,typename P4 = NullType,typename P5 = NullType> class JUBASETL_API TaskPool : public _TaskPool{
		struct PARAM : public _TaskParam{
			Function<R,P1,P2,P3,P4,P5> OnCall;
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			P4 p5;
		};
		ObjectList<PARAM> _threadList;
		static DWORD __stdcall _taskPoolProc(void* p){
			PARAM* tp = (PARAM*)p;
			while(true){
				tp->OnCall(tp->p1,tp->p2,tp->p3,tp->p4,tp->p5);
				if(tp->exit) break;
				tp->over = true;
				::SuspendThread(tp->handle);
				if(!tp->over) continue;
				if(tp->exit) break;
			}
			return 0;
		}
	public:
		~TaskPool(){
			Close();
		};
		void Close(DWORD ms = -1){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				tp.exit = true;
				::ResumeThread(tp.handle);
			}
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				if(WAIT_TIMEOUT==::WaitForSingleObject(tp.handle,ms)){
					TerminateThread(tp.handle,0);
				}
				::CloseHandle(tp.handle);
			}
			_threadList.Clear();
		}
		template<typename PROC> bool StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.BindStd(cb);
				th.p1 = p1;
				th.p2 = p2;
				th.p3 = p3;
				th.p4 = p4;
				th.p5 = p5;
				
				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.BindStd(cb);
			tp->p1 = p1;
			tp->p2 = p2;
			tp->p3 = p3;
			tp->p4 = p4;
			tp->p5 = p5;
			tp->handle = ::CreateThread(0,0,&_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
		template<typename OBJ,typename PROC> bool StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.Bind(obj,cb);
				th.p1 = p1;
				th.p2 = p2;
				th.p3 = p3;
				th.p4 = p4;
				th.p5 = p5;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.Bind(obj,cb);
			tp->p1 = p1;
			tp->p2 = p2;
			tp->p3 = p3;
			tp->p4 = p4;
			tp->p5 = p5;
			tp->handle = ::CreateThread(0,0,&TaskPool::_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
	};
	template<typename R,typename P1,typename P2,typename P3,typename P4> class JUBASETL_API TaskPool<R,P1,P2,P3,P4> : public _TaskPool{
		struct PARAM : public _TaskParam{
			Function<R,P1,P2,P3,P4> OnCall;
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
		};
		ObjectList<PARAM> _threadList;
		static DWORD __stdcall _taskPoolProc(void* p){
			PARAM* tp = (PARAM*)p;
			while(true){
				tp->OnCall(tp->p1,tp->p2,tp->p3,tp->p4);
				if(tp->exit) break;
				tp->over = true;
				::SuspendThread(tp->handle);
				if(!tp->over) continue;
				if(tp->exit) break;
			}
			return 0;
		}
	public:
		~TaskPool(){
			Close();
		};
		void Close(DWORD ms = -1){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				tp.exit = true;
				::ResumeThread(tp.handle);
			}
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				if(WAIT_TIMEOUT==::WaitForSingleObject(tp.handle,ms)){
					TerminateThread(tp.handle,0);
				}
				::CloseHandle(tp.handle);
			}
			_threadList.Clear();
		}
		template<typename PROC> bool StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3,P4 p4){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.BindStd(cb);
				th.p1 = p1;
				th.p2 = p2;
				th.p3 = p3;
				th.p4 = p4;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.BindStd(cb);
			tp->p1 = p1;
			tp->p2 = p2;
			tp->p3 = p3;
			tp->p4 = p4;
			tp->handle = ::CreateThread(0,0,&_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
		template<typename OBJ,typename PROC> bool StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3,P4 p4){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.Bind(obj,cb);
				th.p1 = p1;
				th.p2 = p2;
				th.p3 = p3;
				th.p4 = p4;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.Bind(obj,cb);
			tp->p1 = p1;
			tp->p2 = p2;
			tp->p3 = p3;
			tp->p4 = p4;
			tp->handle = ::CreateThread(0,0,&TaskPool::_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
	};
	template<typename R,typename P1,typename P2,typename P3> class JUBASETL_API TaskPool<R,P1,P2,P3> : public _TaskPool{
		struct PARAM : public _TaskParam{
			Function<R,P1,P2,P3> OnCall;
			P1 p1;
			P2 p2;
			P3 p3;
		};
		ObjectList<PARAM> _threadList;
		static DWORD __stdcall _taskPoolProc(void* p){
			PARAM* tp = (PARAM*)p;
			while(true){
				tp->OnCall(tp->p1,tp->p2,tp->p3);
				if(tp->exit) break;
				tp->over = true;
				::SuspendThread(tp->handle);
				if(!tp->over) continue;
				if(tp->exit) break;
			}
			return 0;
		}
	public:
		~TaskPool(){
			Close();
		};
		void Close(DWORD ms = -1){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				tp.exit = true;
				::ResumeThread(tp.handle);
			}
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				if(WAIT_TIMEOUT==::WaitForSingleObject(tp.handle,ms)){
					TerminateThread(tp.handle,0);
				}
				::CloseHandle(tp.handle);
			}
			_threadList.Clear();
		}
		template<typename PROC> bool StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.BindStd(cb);
				th.p1 = p1;
				th.p2 = p2;
				th.p3 = p3;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.BindStd(cb);
			tp->p1 = p1;
			tp->p2 = p2;
			tp->p3 = p3;
			tp->handle = ::CreateThread(0,0,&_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
		template<typename OBJ,typename PROC> bool StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.Bind(obj,cb);
				th.p1 = p1;
				th.p2 = p2;
				th.p3 = p3;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.Bind(obj,cb);
			tp->p1 = p1;
			tp->p2 = p2;
			tp->p3 = p3;
			tp->handle = ::CreateThread(0,0,&TaskPool::_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
	};
	template<typename R,typename P1,typename P2> class JUBASETL_API TaskPool<R,P1,P2> : public _TaskPool{
		struct PARAM : public _TaskParam{
			Function<R,P1,P2> OnCall;
			P1 p1;
			P2 p2;
		};
		ObjectList<PARAM> _threadList;
		static DWORD __stdcall _taskPoolProc(void* p){
			PARAM* tp = (PARAM*)p;
			while(true){
				tp->OnCall(tp->p1,tp->p2);
				if(tp->exit) break;
				tp->over = true;
				::SuspendThread(tp->handle);
				if(!tp->over) continue;
				if(tp->exit) break;
			}
			return 0;
		}
	public:
		~TaskPool(){
			Close();
		};
		void Close(DWORD ms = -1){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				tp.exit = true;
				::ResumeThread(tp.handle);
			}
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				if(WAIT_TIMEOUT==::WaitForSingleObject(tp.handle,ms)){
					TerminateThread(tp.handle,0);
				}
				::CloseHandle(tp.handle);
			}
			_threadList.Clear();
		}
		template<typename PROC> bool StartStdTask(PROC cb,P1 p1,P2 p2){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.BindStd(cb);
				th.p1 = p1;
				th.p2 = p2;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.BindStd(cb);
			tp->p1 = p1;
			tp->p2 = p2;
			tp->handle = ::CreateThread(0,0,&_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
		template<typename OBJ,typename PROC> bool StartTask(OBJ obj,PROC cb,P1 p1,P2 p2){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.Bind(obj,cb);
				th.p1 = p1;
				th.p2 = p2;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.Bind(obj,cb);
			tp->p1 = p1;
			tp->p2 = p2;
			tp->handle = ::CreateThread(0,0,&TaskPool::_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
	};
	template<typename R,typename P1> class JUBASETL_API TaskPool<R,P1> : public _TaskPool{
		struct PARAM : public _TaskParam{
			Function<R,P1> OnCall;
			P1 p1;
		};
		ObjectList<PARAM> _threadList;
		static DWORD __stdcall _taskPoolProc(void* p){
			PARAM* tp = (PARAM*)p;
			while(true){
				tp->OnCall(tp->p1);
				if(tp->exit) break;
				tp->over = true;
				::SuspendThread(tp->handle);
				if(!tp->over) continue;
				if(tp->exit) break;
			}
			return 0;
		}
	public:
		~TaskPool(){
			Close();
		};
		void Close(DWORD ms = -1){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				tp.exit = true;
				::ResumeThread(tp.handle);
			}
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				if(WAIT_TIMEOUT==::WaitForSingleObject(tp.handle,ms)){
					TerminateThread(tp.handle,0);
				}
				::CloseHandle(tp.handle);
			}
			_threadList.Clear();
		}
		template<typename PROC> bool StartStdTask(PROC cb,P1 p1){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.BindStd(cb);
				th.p1 = p1;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.BindStd(cb);
			tp->p1 = p1;
			tp->handle = ::CreateThread(0,0,&_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
		template<typename OBJ,typename PROC> bool StartTask(OBJ obj,PROC cb,P1 p1){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.Bind(obj,cb);
				th.p1 = p1;

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.Bind(obj,cb);
			tp->p1 = p1;
			tp->handle = ::CreateThread(0,0,&TaskPool::_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
	};
	template<typename R> class JUBASETL_API TaskPool<R> : public _TaskPool{
		struct PARAM : public _TaskParam{
			Function<R> OnCall;
		};
		ObjectList<PARAM> _threadList;
		static DWORD __stdcall _taskPoolProc(void* p){
			PARAM* tp = (PARAM*)p;
			while(true){
				tp->OnCall();
				if(tp->exit) break;
				tp->over = true;
				::SuspendThread(tp->handle);
				if(!tp->over) continue;
				if(tp->exit) break;
			}
			return 0;
		}
	public:
		~TaskPool(){
			Close();
		};
		void Close(DWORD ms = -1){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				tp.exit = true;
				::ResumeThread(tp.handle);
			}
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& tp = _threadList[i];
				if(WAIT_TIMEOUT==::WaitForSingleObject(tp.handle,ms)){
					TerminateThread(tp.handle,0);
				}
				::CloseHandle(tp.handle);
			}
			_threadList.Clear();
		}
		template<typename PROC> bool StartStdTask(PROC cb){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.BindStd(cb);

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.BindStd(cb);
			tp->handle = ::CreateThread(0,0,&_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
		template<typename OBJ,typename PROC> bool StartTask(OBJ obj,PROC cb){
			LocalCriticalSection lcs(_cs);
			for(uint i=0;i<_threadList.Count();i++){
				PARAM& th = _threadList[i];
				if(!th.over) continue;
				th.OnCall.Bind(obj,cb);

				th.over = false;
				while(true){
					//if thread not suspend,ResumeThread will return 0;
					DWORD rcount = ::ResumeThread(th.handle);
					if(rcount==-1) return false;
					if(rcount==1) break; 
					Sleep(1);
				}
				return true;
			}
			DWORD id;
			PARAM* tp = _threadList.Add();
			tp->OnCall.Bind(obj,cb);
			tp->handle = ::CreateThread(0,0,&TaskPool::_taskPoolProc,tp,CREATE_SUSPENDED,&id);
			ResumeThread(tp->handle);
			return true;
		}
	};
	//获取当前线程的句柄，与GetCurrentThread不同，这个函数返回的句柄不是伪句柄，可以被其他线程使用。
	JUBASE_API HANDLE DuplicateCurrentThread();
	//返回当前可执行文件的全路径名称.
	JUBASE_API int GetCurrentExeFileName(String& str);
	typedef struct JUBASE_API Guid : public _struct,public GUID{
		//自动生成一个GUID
		bool Create();
		//输出一个标准GUID字串。
		void ToString(String& str);
		//从一个标准GUID字串初始化
		void FromString(LPCWSTR str);
		//输出一个字节字串。
		void ToByteString(String& str);
		//从一个32个字节字符串初始化。
		void FromByteString(LPCWSTR str);
		//取得字节数据，buf至少为16个字节。
		void ToByte(void* buf){memcpy(buf,this,16);}
		//设置字节数据，buf至少为16个字节。
		inline void FromByte(unsigned char* buf){memcpy(this,buf,16);}
		//判断两个GUID是否相同。
		inline bool operator == (Guid& guid){return memcmp(this,&guid,16)==0;}
	}Guid;
}
