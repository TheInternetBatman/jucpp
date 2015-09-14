#pragma once
#include "function.h"
#define threadvar __declspec(thread) //线程本地存储变量
namespace concise{
	//使用 MessageBox 显示一个系统对 GetLastError,WSAGetLastError 函数返回值的描述文本对话框。
	CONCISE_API int SystemMessageBox(HWND hWnd = 0,LPCWSTR title = L"信息",DWORD style = MB_ICONINFORMATION,DWORD code = GetLastError());
	//取得系统对 GetLastError,WSAGetLastError 等函数的文本描述. code 是以上函数返回的代码.
	CONCISE_API int GetSystemMessage(String& str,DWORD code = GetLastError());
	/*对于一个阻塞的Event，在调用Wait函数之前调用Signal，通知状态仍然会改变
	多个线程等待这个Event的时候，Signal时，按线程调用Wait的先后顺序通知*/
	class CONCISE_API Event : public HandleType<HANDLE>{
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
	class CONCISE_API CriticalSection : public _class{
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
	class CONCISE_API LocalCriticalSection : public _class{
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
	//0个参数
	template<typename R,typename PROC>bool AsynCallStd(PROC proc){
		struct PARAM : public _struct{
			Function<R> OnCall;
		};
		PARAM* p = new PARAM;
		p->OnCall.Bind(proc);
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
		p->OnCall.Bind(proc);
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
		p->OnCall.Bind(proc);
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
		p->OnCall.Bind(proc);
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
			Function<R,P1,P2,P3> OnCall;
		};
		PARAM* p = new PARAM;
		p->p1 = p1;
		p->p2 = p2;
		p->p3 = p3;
		p->p4 = p4;
		p->OnCall.Bind(proc);
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
		p->OnCall.Bind(proc);
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
	class CONCISE_API Thread : public HandleType<HANDLE>{
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
	//#define THREAD_STATUS_NULL		0	//线程为空，或者正要退出
#define THREAD_STATUS_SUSPEND	1	//线程就绪，可以运行
#define THREAD_STATUS_RUNNING	2	//运行状态

#define THREAD_FLAG_SUSPEND		0
#define THREAD_FLAG_CONTINUE	1
#define THREAD_FLAG_EXIT		2	//结束线程
	struct _StaticThreadParam : public _struct{
		DWORD Id;		//线程ID
		LONG Status;	//THREAD_STATUS_NULL,THREAD_STATUS_READY,THREAD_STATUS_RUN
		LONG Flag;		//
		HANDLE Handle;	//线程句柄
		Event* exit;
		LPTHREAD_START_ROUTINE routine;
		_StaticThreadParam():Status(THREAD_STATUS_SUSPEND),Flag(THREAD_FLAG_SUSPEND){}
	};
	template<typename PARAM> bool checkState(PARAM pp){
		//此时Flag有3种可能：Suspend，Continue，Exit；如果状态为Exit，退出循环
		if(THREAD_FLAG_EXIT==::InterlockedCompareExchange(&pp->Flag,THREAD_FLAG_EXIT,THREAD_FLAG_EXIT)){
			return true;
		}
		//此时状态只可能为Continue或者Suspend，如果状态为Suspend，则执行挂起线程，否则继续。
		if(THREAD_FLAG_SUSPEND==::InterlockedCompareExchange(&pp->Flag,THREAD_FLAG_SUSPEND,THREAD_FLAG_CONTINUE)){
			//把状态置为Suspend。
			::InterlockedCompareExchange(&pp->Status,THREAD_STATUS_SUSPEND,pp->Status);
			//如果此时调用了Start，状态会置为Running，如果是Running不挂起线程。
			if(THREAD_STATUS_RUNNING!=::InterlockedCompareExchange(&pp->Status,pp->Status,THREAD_STATUS_RUNNING)){
				::SuspendThread(pp->Handle);
			}
		}
		//如果从Suspend恢复，需要检测是否设置了退出标志。
		if(THREAD_FLAG_EXIT==::InterlockedCompareExchange(&pp->Flag,THREAD_FLAG_EXIT,THREAD_FLAG_EXIT)){
			return true;
		}
		return false;
	}
	template<typename R>DWORD __stdcall _StaticThreadCall(void* p){
		struct PARAM : public _StaticThreadParam{
			Function<R> OnCall;
		};
		PARAM* pp = (PARAM*)p;
		while(1){
			pp->OnCall();
			pp->exit->Signal();
			if(checkState(pp)) break;
		}
		//delete pp;
		return 1;
	}
	template<typename R,typename P1>DWORD __stdcall _StaticThreadCall(void* p){
		struct PARAM : public _StaticThreadParam{
			Function<R,P1> OnCall;
			P1 p1;
		};
		PARAM* pp = (PARAM*)p;
		while(1){
			pp->OnCall(pp->p1);
			pp->exit->Signal();
			if(checkState(pp)) break;
		}
		return 1;
	}
	template<typename R,typename P1,typename P2>DWORD __stdcall _StaticThreadCall(void* p){
		struct PARAM : public _StaticThreadParam{
			Function<R,P1,P2> OnCall;
			P1 p1;
			P2 p2;
		};
		PARAM* pp = (PARAM*)p;
		while(1){
			pp->OnCall(pp->p1,pp->p2);
			pp->exit->Signal();
			if(checkState(pp)) break;
		}
		//delete pp;
		return 1;
	}
	template<typename R,typename P1,typename P2,typename P3>DWORD __stdcall _StaticThreadCall(void* p){
		struct PARAM : public _StaticThreadParam{
			Function<R,P1,P2,P3> OnCall;
			P1 p1;
			P2 p2;
			P3 p3;
		};
		PARAM* pp = (PARAM*)p;
		while(1){
			pp->OnCall(pp->p1,pp->p2,pp->p3);
			pp->exit->Signal();
			if(checkState(pp)) break;
		}
		//delete pp;
		return 1;
	}
	template<typename R,typename P1,typename P2,typename P3,typename P4>DWORD __stdcall _StaticThreadCall(void* p){
		struct PARAM : public _StaticThreadParam{
			Function<R,P1,P2,P3,P4> OnCall;
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
		};
		PARAM* pp = (PARAM*)p;
		while(1){
			pp->OnCall(pp->p1,pp->p2,pp->p3,pp->p4);
			pp->exit->Signal();
			if(checkState(pp)) break;
		}
		//delete pp;
		return 1;
	}
	template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5>DWORD __stdcall _StaticThreadCall(void* p){
		struct PARAM : public _StaticThreadParam{
			Function<R,P1,P2,P3,P4,P5> OnCall;
			P1 p1;
			P2 p2;
			P3 p3;
			P4 p4;
			P5 p5;
		};
		PARAM* pp = (PARAM*)p;
		while(1){
			pp->OnCall(pp->p1,pp->p2,pp->p3,pp->p4,pp->p5);
			pp->exit->Signal();
			if(checkState(pp)) break;
		}
		//delete pp;
		return 1;
	}
	class CONCISE_API _StaticThread : public HandleType<_StaticThreadParam*> {
	protected:
		CriticalSection _Cs;
		Event		_evExit;
	public:
		_StaticThread();
		//等待线程结束，除非主动结束线程（调用SetClose），否则线程不会随线程函数返回而结束。
		DWORD Wait(uint ms = -1);
		//设置线程退出标志，如果任务正在运行，在任务结束后，线程退出，如果任务已经结束，线程马上退出。
		void SetClose();
		//如果任务已经结束，退出线程，并且关闭句柄，如果任务没有结束，等待任务结束，再退出线程，关闭句柄。
		//等待线程退出，如果指定的时间内线程没有退出，强制结束线程
		void Close(uint timeOut = -1);
		//挂起任务。
		bool Pause();
		//启动线程函数，如果此时没有设置回调参数，或者调用了Close函数而没有重新设置回调参数，
		//或者回调函数正在执行，会返回0，wait参数指定当线程已经创建的情况下，如果新的呼叫没有启动，
		//是否等待前面的呼叫结束，从而成功启动.
		//如果已经设置了SetClose，这个函数返回 0。
		//成功返回一个Event指针，这个Event在指定的函数执行完后会被Signed。
		Event* Start(bool wait = 0);
		//回调函数正在被执行
		bool IsRunning();
		//这个函数应该在回调函数里被呼叫，否则会失败，用来指示立即开始下一次回调，无需调用Start函数。
		inline void SetContinue(bool cont = true);
		//强制结束线程运行，但是线程的堆栈不被清除。
		bool Stop(DWORD code = 0);
		//返回线程的ID，如果线程没有初始化，返回 -1 。
		DWORD GetID();
		~_StaticThread();
	};
	/*StaticThread和Thread的用法几乎相同，但是它在回调的函数返回后并不退出线程，而是等待下一次
	Start调用，重新执行指定的任务（回调函数）。它唯一的作用就是在需要频繁启动退出线程的场合，
	减少CPU做这种创建退出线程的工作。

	多次StaticThread的SetCall和SetCallStd调用的函数必须是同一个类型，否则，只有第一次成功，
	第一次调用确定对象回调函数的格式，要改变格式，需要先Close对象。

	StaticThread只有2中状态，Suspend和Running，可以通过IsRunning函数取得它的状态。如果在运行状
	态下调用 SetCall 和 SetCallStd 函数，有两种情况，1，回调已经被呼叫，此时新设置
	的参数直到下次呼叫才起作用，2.回调还未被呼叫，此时执行新设置的参数（执行了Start后，回调不一定
	已经被执行）*/
	class CONCISETL_API StaticThread : public _StaticThread{
	public:
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename PROC> void SetCallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3,P4,P5> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
				P5 p5;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 5;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1,P2,P3,P4,P5>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=5){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
			((PARAM*)_Handle)->p4 = p4;
			((PARAM*)_Handle)->p5 = p5;
		}
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename OBJ,typename PROC> void SetCall(OBJ obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3,P4,P5> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
				P5 p5;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 5;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1,P2,P3,P4,P5>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=5){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
			((PARAM*)_Handle)->p4 = p4;
			((PARAM*)_Handle)->p5 = p5;
		}
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename PROC> void SetCallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3,P4> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
			};

			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 4;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1,P2,P3,P4>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=4){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
			((PARAM*)_Handle)->p4 = p4;
		}
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename OBJ,typename PROC> void SetCall(OBJ obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3,P4> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
			};

			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 4;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1,P2,P3,P4>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=4){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
			((PARAM*)_Handle)->p4 = p4;
		}
		template<typename R,typename P1,typename P2,typename P3,typename PROC> void SetCallStd(PROC proc,P1 p1,P2 p2,P3 p3){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
			};

			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 3;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1,P2,P3>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=3){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
		}
		template<typename R,typename P1,typename P2,typename P3,typename OBJ,typename PROC> void SetCall(OBJ obj,PROC proc,P1 p1,P2 p2,P3 p3){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
			};

			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 3;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1,P2,P3>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=3){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
		}
		template<typename R,typename P1,typename P2,typename PROC> void SetCallStd(PROC proc,P1 p1,P2 p2){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2> OnCall;
				P1 p1;
				P2 p2;
			};

			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 2;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1,P2>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=2){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
		}
		template<typename R,typename P1,typename P2,typename OBJ,typename PROC> void SetCall(OBJ obj,PROC proc,P1 p1,P2 p2){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2> OnCall;
				P1 p1;
				P2 p2;
			};

			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 2;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1,P2>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=2){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
		}
		template<typename R,typename P1,typename PROC> void SetCallStd(PROC proc,P1 p1){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1> OnCall;
				P1 p1;
			};

			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
		}
		template<typename R,typename P1,typename OBJ,typename PROC> void SetCall(OBJ obj,PROC proc,P1 p1){	
			struct PARAM : public _StaticThreadParam{
				Function<R,P1> OnCall;
				P1 p1;
			};

			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->ParamCount = 1;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R,P1>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			if(_Handle->ParamCount!=1){
				_ASSERT(0);
				return;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
		}
		template<typename R,typename PROC> void SetCallStd(PROC proc){	
			struct PARAM : public _StaticThreadParam{
				Function<R> OnCall;
			};
			LocalCriticalSection lcs(_Cs);
			static bool firstcall = 1;
			if(firstcall){
				firstcall = 0;
				_Handle = new PARAM;
				_Handle->ParamCount = 0;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			/*if(_Handle->ParamCount!=0){
			_ASSERT(0);
			return;
			}*/
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
		}
		template<typename R,typename OBJ,typename PROC> void SetCall(OBJ obj,PROC proc){	
			struct PARAM : public _StaticThreadParam{
				Function<R> OnCall;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->Handle = ::CreateThread(0,0,&_StaticThreadCall<R>,_Handle,CREATE_SUSPENDED,&_Handle->Id);
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
		}
	};
	template<typename R,typename P1 = NullType,typename P2 = NullType,typename P3 = NullType,typename P4 = NullType,typename P5 = NullType> class CONCISETL_API StaticThread2 : public _StaticThread{
	public:
		template<typename PROC> void SetCallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3,P4,P5> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
				P5 p5;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1,P2,P3,P4,P5>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
			((PARAM*)_Handle)->p4 = p4;
			((PARAM*)_Handle)->p5 = p5;
		}
		template<typename OBJ,typename PROC> void SetCall(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3,P4,P5> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
				P5 p5;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1,P2,P3,P4,P5>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
			((PARAM*)_Handle)->p4 = p4;
			((PARAM*)_Handle)->p5 = p5;
		}
	};
	template<typename R,typename P1,typename P2,typename P3,typename P4> class CONCISETL_API StaticThread2<R,P1,P2,P3,P4> : public _StaticThread{
	public:
		template<typename PROC> void SetCallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3,P4> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1,P2,P3,P4>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
			((PARAM*)_Handle)->p4 = p4;
		}
		template<typename OBJ,typename PROC> void SetCall(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3,P4> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
				P4 p4;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1,P2,P3,P4>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
			((PARAM*)_Handle)->p4 = p4;
		}
	};
	template<typename R,typename P1,typename P2,typename P3> class CONCISETL_API StaticThread2<R,P1,P2,P3> : public _StaticThread{
	public:
		template<typename PROC> void SetCallStd(PROC proc,P1 p1,P2 p2,P3 p3){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1,P2,P3>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
		}
		template<typename OBJ,typename PROC> void SetCall(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2,P3> OnCall;
				P1 p1;
				P2 p2;
				P3 p3;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1,P2,P3>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
			((PARAM*)_Handle)->p3 = p3;
		}
	};
	template<typename R,typename P1,typename P2> class CONCISETL_API StaticThread2<R,P1,P2> : public _StaticThread{
	public:
		template<typename PROC> void SetCallStd(PROC proc,P1 p1,P2 p2){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2> OnCall;
				P1 p1;
				P2 p2;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1,P2>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
		}
		template<typename OBJ,typename PROC> void SetCall(OBJ* obj,PROC proc,P1 p1,P2 p2){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1,P2> OnCall;
				P1 p1;
				P2 p2;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1,P2>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
			((PARAM*)_Handle)->p2 = p2;
		}
	};
	template<typename R,typename P1> class CONCISETL_API StaticThread2<R,P1> : public _StaticThread{
	public:
		template<typename PROC> void SetCallStd(PROC proc,P1 p1){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1> OnCall;
				P1 p1;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
			((PARAM*)_Handle)->p1 = p1;
		}
		template<typename OBJ,typename PROC> void SetCall(OBJ* obj,PROC proc,P1 p1){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1> OnCall;
				P1 p1;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R,P1>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
			((PARAM*)_Handle)->p1 = p1;
		}
	};
	template<typename R> class CONCISETL_API StaticThread2<R> : public _StaticThread{
	public:
		template<typename PROC> void SetCallStd(PROC proc){
			struct PARAM : public _StaticThreadParam{
				Function<R> OnCall;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.BindStd(proc);
		}
		template<typename OBJ,typename PROC> void SetCall(OBJ* obj,PROC proc){
			struct PARAM : public _StaticThreadParam{
				Function<R,P1> OnCall;
			};
			LocalCriticalSection lcs(_Cs);
			if(_Handle==0){
				_Handle = new PARAM;
				_Handle->routine = &_StaticThreadCall<R>;
			}
			_Handle->exit = &_evExit;
			((PARAM*)_Handle)->OnCall.Bind(obj,proc);
		}
	};
	class CONCISETL_API _TaskPool : public _class{
	protected:
		CriticalSection		_cs;
		uint				_cleanPeroid;		//0.1S为单位
		uint				_maxThreadCount;
		Thread				_cleanThread;
		bool				_exitCleanThread;
		ObjectList<StaticThread>	_threadList;

		void _onClean();
		StaticThread* _getThread();
	public:
		_TaskPool();
		~_TaskPool();
		//启动清理线程
		void StartCleanThread(uint period);
		//设置最大的允许线程数
		void SetMaxThread(int maxCount);
		//返回当前运行的线程数
		int GetTaskCount();
		//返回当前空闲的线程数
		int GetIdleCount();
		//返回激活的线程总数，空闲线程数+运行线程数
		int GetThreadCount();
		//关闭这个线程池对象，这个函数会等待所有线程退出才会返回。
		//如果指定的时间内，线程没有结束，强制结束线程。
		void Close(uint timeOut = -1);
	};
	/* TaskPool 在销毁时会等待所有的线程退出，可以利用这一点在使用它的时候，要保证TaskPool先于它
	呼叫的实例销毁（放在成员列表的最下面），否则。可能线程仍然在执行，那些对象销毁后就会使
	程序出错，这在退出多线程程序时经常发生，也是所有多线程程序都需要解决的一个问题——程序
	退出时对象的销毁顺序问题。一般是先调用退出线程的函数，TaskPool等待，退出结束后，开始销毁其它对象。

	如果程序结束时不需要保存任何数据，可以使用ExitProcess迅速结束进程。
	*/
	class CONCISETL_API TaskPool : public _TaskPool{
	public:
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename PROC> Event* StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCallStd<R>(cb,p1,p2,p3,p4,p5);
			return pst->Start();
		}
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCall<R>(obj,cb,p1,p2,p3,p4,p5);
			return pst->Start();
		}
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename PROC> Event* StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3,P4 p4){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCallStd<R>(cb,p1,p2,p3,p4);
			return pst->Start();
		}
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3,P4 p4){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCall<R>(obj,cb,p1,p2,p3,p4);
			return pst->Start();
		}
		template<typename R,typename P1,typename P2,typename P3,typename PROC> Event* StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCallStd<R>(cb,p1,p2,p3);
			return pst->Start();
		}
		template<typename R,typename P1,typename P2,typename P3,typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCall<R>(obj,cb,p1,p2,p3);
			return pst->Start();
		}
		template<typename R,typename P1,typename P2,typename PROC> Event* StartStdTask(PROC cb,P1 p1,P2 p2){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCallStd<R>(cb,p1,p2);
			return pst->Start();
		}
		template<typename R,typename P1,typename P2,typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1,P2 p2){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCall<R>(obj,cb,p1,p2);
			return pst->Start();
		}
		template<typename R,typename P1,typename PROC> Event* StartStdTask(PROC cb,P1 p1){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCallStd<R>(cb,p1);
			return pst->Start();
		}
		template<typename R,typename P1,typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCall<R>(obj,cb,p1);
			return pst->Start();
		}
		template<typename R,typename PROC> Event* StartStdTask(PROC cb){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCallStd<R>(cb);
			return pst->Start();
		}
		template<typename R,typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread* pst = _getThread();
			pst->SetCall<R>(obj,cb);
			return pst->Start();
		}
	};
	class CONCISETL_API _TaskPool2 : public _class{
	protected:
		CriticalSection		_cs;
		uint				_cleanPeroid;		//0.1S为单位
		uint				_maxThreadCount;
		Thread				_cleanThread;
		bool				_exitCleanThread;
		ObjectList<_StaticThread>	_threadList;

		void _onClean();
		_StaticThread* _getThread();
	public:
		_TaskPool2();
		~_TaskPool2();
		//启动清理线程
		void StartCleanThread(uint period);
		//设置最大的允许线程数
		void SetMaxThread(int maxCount);
		//返回当前运行的线程数
		int GetTaskCount();
		//返回当前空闲的线程数
		int GetIdleCount();
		//返回激活的线程总数，空闲线程数+运行线程数
		int GetThreadCount();
		//关闭这个线程池对象，这个函数会等待所有线程退出才会返回。
		//如果指定的时间内，线程没有结束，强制结束线程。
		void Close(uint timeOut = -1);
	};
	template<typename R,typename P1 = NullType,typename P2 = NullType,typename P3 = NullType,typename P4 = NullType,typename P5 = NullType> class CONCISETL_API TaskPool2 : public _TaskPool2{
	public:
		template<typename PROC> Event* StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1,P2,P3,P4,P5>* pst = (StaticThread2<R,P1,P2,P3,P4,P5>*)_getThread();
			pst->SetCallStd(cb,p1,p2,p3,p4,p5);
			return pst->Start();
		}
		template<typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1,P2,P3,P4,P5>* pst = (StaticThread2<R,P1,P2,P3,P4,P5>*)_getThread();
			pst->SetCall(obj,cb,p1,p2,p3,p4,p5);
			return pst->Start();
		}
	};
	template<typename R,typename P1,typename P2,typename P3,typename P4> class CONCISETL_API TaskPool2<R,P1,P2,P3,P4> : public _TaskPool2{
	public:
		template<typename PROC> Event* StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3,P4 p4){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1,P2,P3,P4>* pst = (StaticThread2<R,P1,P2,P3,P4>*)_getThread();
			pst->SetCallStd(cb,p1,p2,p3,p4);
			return pst->Start();
		}
		template<typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3,P4 p4){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1,P2,P3,P4>* pst = (StaticThread2<R,P1,P2,P3,P4>*)_getThread();
			pst->SetCall(obj,cb,p1,p2,p3,p4);
			return pst->Start();
		}
	};
	template<typename R,typename P1,typename P2,typename P3> class CONCISETL_API TaskPool2<R,P1,P2,P3> : public _TaskPool2{
	public:
		template<typename PROC> Event* StartStdTask(PROC cb,P1 p1,P2 p2,P3 p3){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1,P2,P3>* pst = (StaticThread2<R,P1,P2,P3>*)_getThread();
			pst->SetCallStd(cb,p1,p2,p3);
			return pst->Start();
		}
		template<typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1,P2 p2,P3 p3){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1,P2,P3>* pst = (StaticThread2<R,P1,P2,P3>*)_getThread();
			pst->SetCall(obj,cb,p1,p2,p3);
			return pst->Start();
		}
	};
	template<typename R,typename P1,typename P2> class CONCISETL_API TaskPool2<R,P1,P2> : public _TaskPool2{
	public:
		template<typename PROC> Event* StartStdTask(PROC cb,P1 p1,P2 p2){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1,P2>* pst = (StaticThread2<R,P1,P2>*)_getThread();
			pst->SetCallStd(cb,p1,p2);
			return pst->Start();
		}
		template<typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1,P2 p2){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1,P2>* pst = (StaticThread2<R,P1,P2>*)_getThread();
			pst->SetCall(obj,cb,p1,p2);
			return pst->Start();
		}
	};
	template<typename R,typename P1> class CONCISETL_API TaskPool2<R,P1> : public _TaskPool2{
	public:
		template<typename PROC> Event* StartStdTask(PROC cb,P1 p1){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1>* pst = (StaticThread2<R,P1>*)_getThread();
			pst->SetCallStd(cb,p1);
			return pst->Start();
		}
		template<typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb,P1 p1){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R,P1>* pst = (StaticThread2<R,P1>*)_getThread();
			pst->SetCall(obj,cb,p1);
			return pst->Start();
		}
	};
	template<typename R> class CONCISETL_API TaskPool2<R> : public _TaskPool2{
	public:
		template<typename PROC> Event* StartStdTask(PROC cb){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R>* pst = (StaticThread2<R>*)_getThread();
			pst->SetCallStd(cb);
			return pst->Start();
		}
		template<typename OBJ,typename PROC> Event* StartTask(OBJ obj,PROC cb){
			LocalCriticalSection lcs(_cs);
			if(_threadList.Count()>=_maxThreadCount) return 0;
			StaticThread2<R>* pst = (StaticThread2<R>*)_getThread();
			pst->SetCall(obj,cb);
			return pst->Start();
		}
	};
	//返回这个进程的一个唯一TaskPool对象，一个进程实际上并不需要很多个TaskPool对象，使用一个唯一的TaskPool对象启动线程，效率更高。
	CONCISE_API TaskPool* ProcessTaskPool(bool release = false);	
	//获取当前线程的句柄，与GetCurrentThread不同，这个函数返回的句柄不是伪句柄，可以被其他线程使用。
	CONCISE_API HANDLE DuplicateCurrentThread();
	//返回当前可执行文件的全路径名称.
	CONCISE_API int GetCurrentExeFileName(String& str);
	typedef struct CONCISE_API Guid : public _struct,public GUID{
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
		void ToByte(unsigned char* buf){memcpy(buf,this,16);}
		//设置字节数据，buf至少为16个字节。
		inline void FromByte(unsigned char* buf){memcpy(this,buf,16);}
		//判断两个GUID是否相同。
		inline bool operator == (Guid& guid){return memcmp(this,&guid,16)==0;}
	}Guid;
}
