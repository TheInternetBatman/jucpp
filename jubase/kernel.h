#pragma once
#include "function.h"
#define threadvar __declspec(thread) //�̱߳��ش洢����
namespace ju{
	//ʹ�� MessageBox ��ʾһ��ϵͳ�� GetLastError,WSAGetLastError ��������ֵ�������ı��Ի���
	JUBASE_API int SystemMessageBox(HWND hWnd = 0,LPCWSTR title = L"��Ϣ",DWORD style = MB_ICONINFORMATION,DWORD code = GetLastError());
	//ȡ��ϵͳ�� GetLastError,WSAGetLastError �Ⱥ������ı�����. code �����Ϻ������صĴ���.
	JUBASE_API int GetSystemMessage(String& str,DWORD code = GetLastError());
	/*����һ��������Event���ڵ���Wait����֮ǰ����Signal��֪ͨ״̬��Ȼ��ı�
	����̵߳ȴ����Event��ʱ��Signalʱ�����̵߳���Wait���Ⱥ�˳��֪ͨ*/
	class JUBASE_API Event : public HandleType<HANDLE>{
	public:
		Event();
		~Event();
		HANDLE Attach(HANDLE ent){return HandleType<HANDLE>::Attach(ent);}
		HANDLE Detach(){return HandleType<HANDLE>::Detach();}
		//autosinaled������Wait�������ú�Event ״̬�Ǳ���Ϊδ֪ͨ״̬��autoblock=1������ά��֪ͨ״̬��autoblock=0����signaled����ʼ״̬��
		bool Create(bool autoblock = 1,bool signal = 1,LPCWSTR name = 0);
		//��һ�����ڵ�Event��
		bool Open(LPCWSTR name);
		//���� Event ״̬��signaled = 1��Wait�����᷵�ء�
		void Signal(bool signaled = 1);
		//��������̡߳�
		DWORD Wait(int ms = -1);
		//�رն���
		void Close();
	};
	/*CriticalSection�����ڶ��̵߳���ʱ��Lock��Unlock�����м�Ĵ���ͬʱֻ��
	һ���߳���ִ�С����ǣ�ע�⣺Lock���������������̵߳Ķ�ε��ã����̶߳��
	����Lockֻ������ CRITICAL_SECTION LockCount��Ա��ֵ��
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
		//��ʼ�������룬���ú�������߳�û�е���Unlock֮ǰ�������̶߳�Lock�ĵ��ö������
		inline void Lock(){
			EnterCriticalSection(&_Handle);
		}
		//���������������߳������һ�������������
		inline void Unlock(){
			LeaveCriticalSection(&_Handle);
		}
		inline bool TryLock(){
			return 0!=TryEnterCriticalSection(&_Handle);
		}
	};
	//ʹ��LocalCriticalSection������������Lock��Unlock�������Զ��������������Ĵ��롣
	//����ֻ�ں�������Ϊ�ֲ�����ʹ�á�
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
	//0������
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
	//1������
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
	//2������
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
	//3������
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
	//4������
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
	//5������
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
	//0������
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
	//1������
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
	//2������
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
	//3������
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
	//4������
	template<typename R,typename P1,typename P2,typename P3,typename P4,typename OBJ,typename PROC>bool AsynCall(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
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
		p->OnCall.Bind(obj,proc);
		HANDLE _Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4>,p,0,0);
		if(!_Handle){
			delete p;
			return 0;
		}
		CloseHandle(_Handle);
		return 1;
	}
	//5������
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
	//�̵߳Ĺ����ں˼����� 1���˼���Ϊ 0 ʱ�ָ̻߳����У�����ֵ�ǵ���֮ǰ���ں˼���������ʧ�ܷ���-1��
	//�̵߳Ĺ����ں˼����� 1���˼���Ϊ 0 ʱ�ָ̻߳����У�����ֵ�ǵ���֮ǰ���ں˼�����
	//����һ���ں˼���Ϊ 0 ���̣߳��������ټ�С��Ҳ����˵��������Ϊ����������ʧ�ܷ���-1��
	//���ǵ����� Close ����������ʵ������������һ���̡߳����߳�����֮��ʵ���Ͽ�������Threadʵ�����̵߳�ִ�в����ܵ��κ�Ӱ�졣
	//��������غ�������Ҫ�����е�ģ�������д���������Ǻ������ͣ����Ǻ��鷳�ģ����Ծ�����ʹ�����غ�����Ϊ������������ֻ��Ҫ�������ķ���������Ϊģ�庯���ĵ�һ��ģ������Ϳ����ˡ�
	//��Ϊ�̺߳��������첽���õģ����Դ��ݵĲ������������߳������ڼ���Ч�ģ�����ֲ�ʵ����ָ�벻����Ϊ�������ݡ�
	class JUBASE_API Thread : public HandleType<HANDLE>{
	protected:
		DWORD _Id;
	public:
		//0������
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
		//1������
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
		//2������
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
		//3������
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
		//4������
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename PROC>bool CallStd(PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
			if(_Handle) return 0;
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
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//5������
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
		//0������
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
		//1������
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
		//2������
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
		//3������
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
		//4������
		template<typename R,typename P1,typename P2,typename P3,typename P4,typename OBJ,typename PROC>bool Call(OBJ* obj,PROC proc,P1 p1,P2 p2,P3 p3,P4 p4){
			if(_Handle) return 0;
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
			p->OnCall.Bind(obj,proc);
			_Handle = ::CreateThread(0,0,&_ThreadCall<R,P1,P2,P3,P4>,p,0,&_Id);
			if(!_Handle){
				delete p;
				return 0;
			}
			return 1;
		}
		//5������
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
		//�ȴ��߳̽�����
		DWORD Wait(uint ms = -1){
			return ::WaitForSingleObject(_Handle,ms);
		}
		//�̵߳Ĺ����ں˼����� 1���˼���Ϊ 0 ʱ�ָ̻߳����У�����ֵ�ǵ���֮ǰ���ں˼�����
		int Pause(){
			return ::SuspendThread(_Handle);
		}
		//�̵߳Ĺ����ں˼����� 1���˼���Ϊ 0 ʱ�ָ̻߳����У�����ֵ�ǵ���֮ǰ���ں˼���������һ���ں˼���Ϊ 0 ���̣߳��������ټ�С��Ҳ����˵��������Ϊ������
		int Resume(){
			return ::ResumeThread(_Handle);
		}
		//�ر��߳̾���������߳̽����κβ��������ǵȴ������᷵�ء�
		bool Close(){
			if(!_Handle) return 0;
			if(!CloseHandle(_Handle)){
				_ASSERT(0);
			}
			_Handle = 0;
			_Id = 0;
			return 1;
		}
		//ǿ�ƽ����߳����У������̵߳Ķ�ջ���������
		bool Stop(DWORD code = 0){
			if(!_Handle) return 0;
			::TerminateThread(_Handle,code);
			::CloseHandle(_Handle);
			_Handle = 0;
			return 1;
		}
		//�߳��Ƿ�������״̬��
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
	//TaskPool, һ�� TaskPool ֻ������һ�����͵Ļص���������Ϊ��Щ�߳�����Ҫ���õģ�����Ԥ�Ʋ���ģʽ��
	//��Ϊһ���Ѿ����е��̣߳����޷��ı�ص�����������ʽ�ģ����������ͣ������ǿ��Ըı������ֵ��
	//�̳߳ص��߳���һֱ���л��߹���ģ������˳������������Ƿ��Ѿ�ִ���꣬��Ҫ�û��ӻص������Ƿ��Ѿ�����
	//���Լ��жϣ���������һ���¼��������̳߳ر����ṩ������ܡ�
	//Close �����ر������̣߳����ǻ�ȴ���������ִ�е�������ɣ�������������ȴ�ʱ�䣬�������ĳ������
	//���������߳̽���ǿ�ƽ�������������Ȼ������ڴ�й¶������ֻ����Ϊ�˳����˳�ʱ�����Ӧ��ʹ�������ʽ��
	//����״̬�ĳ���Ӧ��ǿ�ƽ����̡߳�
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
	//��ȡ��ǰ�̵߳ľ������GetCurrentThread��ͬ������������صľ������α��������Ա������߳�ʹ�á�
	JUBASE_API HANDLE DuplicateCurrentThread();
	//���ص�ǰ��ִ���ļ���ȫ·������.
	JUBASE_API int GetCurrentExeFileName(String& str);
	typedef struct JUBASE_API Guid : public _struct,public GUID{
		//�Զ�����һ��GUID
		bool Create();
		//���һ����׼GUID�ִ���
		void ToString(String& str);
		//��һ����׼GUID�ִ���ʼ��
		void FromString(LPCWSTR str);
		//���һ���ֽ��ִ���
		void ToByteString(String& str);
		//��һ��32���ֽ��ַ�����ʼ����
		void FromByteString(LPCWSTR str);
		//ȡ���ֽ����ݣ�buf����Ϊ16���ֽڡ�
		void ToByte(void* buf){memcpy(buf,this,16);}
		//�����ֽ����ݣ�buf����Ϊ16���ֽڡ�
		inline void FromByte(unsigned char* buf){memcpy(this,buf,16);}
		//�ж�����GUID�Ƿ���ͬ��
		inline bool operator == (Guid& guid){return memcmp(this,&guid,16)==0;}
	}Guid;
}
