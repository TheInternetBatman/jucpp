#pragma once
namespace cs{
	template<typename R,typename T1 = NullType,typename T2 = NullType,typename T3 = NullType,typename T4 = NullType,typename T5 = NullType,typename T6 = NullType> class CONCISETL_API Function : public _StdClassProc
	{
	public:
		//函数对应的静态函数类型.
		typedef R (__stdcall* PROC)(T1,T2,T3,T4,T5,T6);
		//缺省绑定一个空函数,如果函数有返回值，空函数的作用仅仅是调用它不会出错，但返回值是不可控制的。
		Function(){}
		void Empty(){
			uint dsp = GetAlignment<T1>() + GetAlignment<T2>() + GetAlignment<T3>() + GetAlignment<T4>() + GetAlignment<T5>() + GetAlignment<T6>();
			_StdClassProc::Bind<PROC>(0,0,dsp);
		}
		//用一个类的实例和成员函数来初始化函数.
		template<typename C,typename P> void Bind(C* obj,P func)
		{
			typedef R (C::*CP)(T1,T2,T3,T4,T5,T6);
			_StdClassProc::Bind<CP>(obj,func);
		}
		//用一个静态函数来初始化函数.
		template<typename P> void BindStd(P func){
			_StdClassProc::BindStd<PROC>(func);
		}
		//用一个相同类型的函数来初始化函数.
		void operator = (Function<R,T1,T2,T3,T4,T5,T6>& func)
		{
			_StdClassProc::operator = (func);
		}
		//返回静态等效函数的指针.
		inline PROC Procedure()
		{
			return _StdClassProc::Procedure<PROC>();
		}
		//同样功能的自动类型转换.
		operator PROC ()
		{
			return _StdClassProc::Procedure<PROC>();
		}
	};
	template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5> class CONCISETL_API Function<R,T1,T2,T3,T4,T5> : public _StdClassProc
	{
	public:
		//函数对应的静态函数类型.
		typedef R (__stdcall* PROC)(T1,T2,T3,T4,T5);
		//缺省绑定一个空函数,如果函数有返回值，空函数的作用仅仅是调用它不会出错，但返回值是不可控制的。
		Function(){}
		void Empty(){
			uint dsp = GetAlignment<T1>() + GetAlignment<T2>() + GetAlignment<T3>() + GetAlignment<T4>() + GetAlignment<T5>();
			_StdClassProc::Bind<PROC>(0,0,dsp);
		}
		//用一个类的实例和成员函数来初始化函数.
		template<typename C,typename P> void Bind(C* obj,P func)
		{
			typedef R (C::*CP)(T1,T2,T3,T4,T5);
			_StdClassProc::Bind<CP>(obj,func);
		}
		//用一个静态函数来初始化函数.
		template<typename P> void BindStd(P func){
			_StdClassProc::BindStd<PROC>(func);
		}
		//用一个相同类型的函数来初始化函数.
		void operator = (Function<R,T1,T2,T3,T4,T5>& func)
		{
			_StdClassProc::operator = (func);
		}
		//返回静态等效函数的指针.
		inline PROC Procedure()
		{
			return _StdClassProc::Procedure<PROC>();
		}
		//同样功能的自动类型转换.
		operator PROC ()
		{
			return _StdClassProc::Procedure<PROC>();
		}
	};
	template<typename R,typename T1,typename T2,typename T3,typename T4> class CONCISETL_API Function<R,T1,T2,T3,T4> : public _StdClassProc
	{
	public:
		typedef R (__stdcall* PROC)(T1,T2,T3,T4);
		Function(){}
		void Empty(){
			uint dsp = GetAlignment<T1>() + GetAlignment<T2>() + GetAlignment<T3>() + GetAlignment<T4>();
			_StdClassProc::Bind<PROC>(0,0,dsp);
		}
		template<typename C,typename P> void Bind(C* obj,P func)
		{
			typedef R (C::*CP)(T1,T2,T3,T4);
			_StdClassProc::Bind<CP>(obj,func);
		}
		//用一个静态函数来初始化函数.
		template<typename P> void BindStd(P func){
			_StdClassProc::BindStd<PROC>(func);
		}
		void operator = (Function<R,T1,T2,T3,T4>& func)
		{
			_StdClassProc::operator = (func);
		}
		inline PROC Procedure()
		{
			return _StdClassProc::Procedure<PROC>();
		}
		operator PROC ()
		{
			return _StdClassProc::Procedure<PROC>();
		}
	};
	template<typename R,typename T1,typename T2,typename T3> class Function<R,T1,T2,T3> : public _StdClassProc
	{
	public:
		typedef R (__stdcall* PROC)(T1,T2,T3);
		Function(){}
		void Empty(){
			uint dsp = GetAlignment<T1>() + GetAlignment<T2>() + GetAlignment<T3>();
			_StdClassProc::Bind<PROC>(0,0,dsp);
		}
		template<typename C,typename P> void Bind(C* obj,P func)
		{
			typedef R (C::*CP)(T1,T2,T3);
			_StdClassProc::Bind<CP>(obj,func);
		}
		//用一个静态函数来初始化函数.
		template<typename P> void BindStd(P func){
			_StdClassProc::BindStd<PROC>(func);
		}
		void operator = (Function<R,T1,T2,T3>& func)
		{
			_StdClassProc::operator = (func);
		}
		inline PROC Procedure()
		{
			return _StdClassProc::Procedure<PROC>();
		}
		operator PROC ()
		{
			return _StdClassProc::Procedure<PROC>();
		}
	};
	template<typename R,typename T1,typename T2> class CONCISETL_API Function<R,T1,T2> : public _StdClassProc
	{
	public:
		typedef R (__stdcall* PROC)(T1,T2);
		Function(){}
		void Empty(){
			uint dsp = GetAlignment<T1>() + GetAlignment<T2>();
			_StdClassProc::Bind<PROC>(0,0,dsp);
		}
		template<typename C,typename P> void Bind(C* obj,P func)
		{
			typedef R (C::*CP)(T1,T2);
			_StdClassProc::Bind<CP>(obj,func);
		}
		//用一个静态函数来初始化函数.
		template<typename P> void BindStd(P func){
			_StdClassProc::BindStd<PROC>(func);
		}
		template<typename C,typename P> void BindAny(C* obj,P func){
			_StdClassProc::Bind<P>(obj,func);
		}
		//用一个静态函数来初始化函数.
		template<typename P> void BindStdAny(P func){
			_StdClassProc::BindStd<P>(func);
		}
		void operator = (Function<R,T1,T2>& func)
		{
			_StdClassProc::operator = (func);
		}
		inline PROC Procedure()
		{
			return _StdClassProc::Procedure<PROC>();
		}
		operator PROC ()
		{
			return _StdClassProc::Procedure<PROC>();
		}
	};
	template<typename R,typename T1> class CONCISETL_API Function<R,T1> : public _StdClassProc
	{
	public:
		typedef R (__stdcall* PROC)(T1);
		Function(){}
		void Empty(){
			uint dsp = GetAlignment<T1>();
			_StdClassProc::Bind<PROC>(0,0,dsp);
		}
		template<typename C,typename P> void Bind(C* obj,P func){
			typedef R (C::*CP)(T1);
			_StdClassProc::Bind<CP>(obj,func);
		}
		//绑定任何一个函数，有些场合，虽然函数格式正确，但是以为函数所处位置很复杂，不一定符合 CP 的格式，需要使用这个函数，使用这个函数必须保证格式正确，因为完全没有格式检测。
		template<typename C,typename P> void BindAny(C* obj,P func){
			_StdClassProc::Bind<P>(obj,func);
		}
		//用一个静态函数来初始化函数.
		template<typename P> void BindStd(P func){
			_StdClassProc::BindStd<PROC>(func);
		}
		//绑定任何一个函数，有些场合，虽然函数格式正确，但是以为函数所处位置很复杂，不一定符合 PROC 的格式，需要使用这个函数，使用这个函数必须保证格式正确，因为完全没有格式检测。
		template<typename P> void BindStdAny(P func){
			_StdClassProc::BindStd<P>(func);
		}
		void operator = (Function<R,T1>& func){
			_StdClassProc::operator = (func);
		}
		inline PROC Procedure(){
			return _StdClassProc::Procedure<PROC>();
		}
		operator PROC (){
			return _StdClassProc::Procedure<PROC>();
		}
	};
	template<typename R> class CONCISETL_API Function<R> : public _StdClassProc
	{
	public:
		typedef R (__stdcall* PROC)();
		Function(){}
		void Empty(){
			_StdClassProc::Bind<PROC>(0,0,0);
		}
		template<typename C,typename P> void Bind(C* obj,P func)
		{
			typedef R (C::*CP)();
			_StdClassProc::Bind<CP>(obj,func);
		}
		//用一个静态函数来初始化函数.
		template<typename P> void BindStd(P func){
			_StdClassProc::BindStd<PROC>(func);
		}
		void operator = (Function<R>& func)
		{
			_StdClassProc::operator = (func);
		}
		inline PROC Procedure()
		{
			return _StdClassProc::Procedure<PROC>();
		}
		operator PROC ()
		{
			return _StdClassProc::Procedure<PROC>();
		}
	};
	template<typename T1 = NullType,typename T2 = NullType,typename T3 = NullType,typename T4 = NullType,typename T5 = NullType,typename T6 = NullType> class CONCISETL_API Delegate : public _delegate
	{
	public:
		typedef void (__stdcall* STDCALLTYPE)(T1,T2,T3,T4,T5,T6);
		//添加一个类的实例和成员函数调用.
		template<typename R,typename P> uint Add(R* object,P proc,uint index = -1)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2,T3,T4,T5,T6);
			void* p = _CProcedure();
			_InitCProcdure(object,SuperCast<void*,THISCALLTYPE>(proc),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个函数调用.函数必须是void返回类型.
		uint Add(Function<void,T1,T2,T3,T4,T5,T6> func,uint index = -1){
			void* p = _CProcedure();
			if(func.IsNull()) return 0;
			_InitCProcdure(func.GetObject(),func.Procedure(),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个静态函数调用.
		uint Add(STDCALLTYPE func,uint index = -1){
			void* p = _CProcedure();
			_InitStdProcdure(SuperCast<void*>(func),p);
			return _Ptrs.Add(p,index);
		}
		//删除指定索引的函数.
		bool Delete(uint index)
		{
			return _delegate::Delete(index);
		}
		//删除特定的函数,如果函数没有在实例中,返回false.
		template<typename R,typename P> bool Delete(R* object,P proc)
		{
			return Delete(IndexOf(object,proc));
		}
		//删除静态函数.
		bool Delete(STDCALLTYPE func)
		{
			return Delete(IndexOf(func));
		}
		//删除一个调用.
		bool Delete(Function<void,T1,T2,T3,T4,T5,T6> func)
		{
			return Delete(IndexOf(func));
		}
		//返回指定调用的索引,如果调用不在代表中,则返回-1;
		template<typename R,typename P> uint IndexOf(R* object,P proc)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2,T3,T4,T5,T6);
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if((void*)object!=GetObject(i)) continue;
				if(SuperCast<void*,THISCALLTYPE>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(STDCALLTYPE proc)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(SuperCast<void*>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(Function<void,T1,T2,T3,T4,T5,T6> func)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(func.GetObject()!=GetObject(i)) continue;
				if(func.Procedure()!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		//调用代表.添加的所有函数都会被呼叫.
		void Call(T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				((STDCALLTYPE)_Ptrs[i])(t1,t2,t3,t4,t5,t6);
			}
		}
	};
	template<typename T1,typename T2,typename T3,typename T4,typename T5> class CONCISETL_API Delegate<T1,T2,T3,T4,T5> : public _delegate
	{
	public:
		typedef void (__stdcall* STDCALLTYPE)(T1,T2,T3,T4,T5);
		//添加一个类的实例和成员函数调用.
		template<typename R,typename P> uint Add(R* object,P proc,uint index = -1)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2,T3,T4,T5);
			void* p = _CProcedure();
			_InitCProcdure(object,SuperCast<void*,THISCALLTYPE>(proc),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个函数调用.函数必须是void返回类型.
		uint Add(Function<void,T1,T2,T3,T4,T5> func,uint index = -1){
			void* p = _CProcedure();
			if(func.IsNull()) return 0;
			_InitCProcdure(func.GetObject(),func.Procedure(),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个静态函数调用.
		uint Add(STDCALLTYPE func,uint index = -1){
			void* p = _CProcedure();
			_InitStdProcdure(SuperCast<void*>(func),p);
			return _Ptrs.Add(p,index);
		}
		//删除指定索引的函数.
		bool Delete(uint index)
		{
			return _delegate::Delete(index);
		}
		//删除特定的函数,如果函数没有在实例中,返回false.
		template<typename R,typename P> bool Delete(R* object,P proc)
		{
			return Delete(IndexOf(object,proc));
		}
		//删除静态函数.
		bool Delete(STDCALLTYPE func)
		{
			return Delete(IndexOf(func));
		}
		//删除一个调用.
		bool Delete(Function<void,T1,T2,T3,T4,T5> func)
		{
			return Delete(IndexOf(func));
		}
		//返回指定调用的索引,如果调用不在代表中,则返回-1;
		template<typename R,typename P> uint IndexOf(R* object,P proc)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2,T3,T4,T5);
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if((void*)object!=GetObject(i)) continue;
				if(SuperCast<void*,THISCALLTYPE>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(STDCALLTYPE proc)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(SuperCast<void*>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(Function<void,T1,T2,T3,T4,T5> func)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(func.GetObject()!=GetObject(i)) continue;
				if(func.Procedure()!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		//调用代表.添加的所有函数都会被呼叫.
		void Call(T1 t1,T2 t2,T3 t3,T4 t4,T5 t5)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				((STDCALLTYPE)_Ptrs[i])(t1,t2,t3,t4,t5);
			}
		}
	};
	template<typename T1,typename T2,typename T3,typename T4> class CONCISETL_API Delegate<T1,T2,T3,T4> : public _delegate
	{
	public:
		typedef void (__stdcall* STDCALLTYPE)(T1,T2,T3,T4);
		template<typename R,typename P> uint Add(R* object,P proc,uint index = -1)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2,T3,T4);
			void* p = _CProcedure();
			_InitCProcdure(object,SuperCast<void*,THISCALLTYPE>(proc),p);
			return _Ptrs.Add(p,index);
		}
		uint Add(Function<void,T1,T2,T3,T4> func,uint index = -1){
			void* p = _CProcedure();
			if(func.IsNull()) return 0;
			_InitCProcdure(func.GetObject(),func.Procedure(),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个静态函数调用.
		uint Add(STDCALLTYPE func,uint index = -1){
			void* p = _CProcedure();
			_InitStdProcdure(SuperCast<void*>(func),p);
			return _Ptrs.Add(p,index);
		}
		bool Delete(uint index)
		{
			return _delegate::Delete(index);
		}
		template<typename R,typename P> bool Delete(R* object,P proc)
		{
			return Delete(IndexOf(object,proc));
		}
		bool Delete(STDCALLTYPE func)
		{
			return Delete(IndexOf(func));
		}
		bool Delete(Function<void,T1,T2,T3,T4> func)
		{
			return Delete(IndexOf(func));
		}
		template<typename R,typename P> uint IndexOf(R* object,P proc)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2,T3,T4);
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if((void*)object!=GetObject(i)) continue;
				if(SuperCast<void*,THISCALLTYPE>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(STDCALLTYPE proc)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(SuperCast<void*>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(Function<void,T1,T2,T3,T4> func)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(func.GetObject()!=GetObject(i)) continue;
				if(func.Procedure()!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		void Call(T1 t1,T2 t2,T3 t3,T4 t4)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				((STDCALLTYPE)_Ptrs[i])(t1,t2,t3,t4);
			}
		}
	};
	template<typename T1,typename T2,typename T3> class CONCISETL_API Delegate<T1,T2,T3> : public _delegate
	{
	public:
		typedef void (__stdcall* STDCALLTYPE)(T1,T2,T3);
		template<typename R,typename P> uint Add(R* object,P proc,uint index = -1)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2,T3);
			void* p = _CProcedure();
			_InitCProcdure(object,SuperCast<void*,THISCALLTYPE>(proc),p);
			return _Ptrs.Add(p,index);
		}
		uint Add(Function<void,T1,T2,T3> func,uint index = -1){
			void* p = _CProcedure();
			if(func.IsNull()) return 0;
			_InitCProcdure(func.GetObject(),func.Procedure(),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个静态函数调用.
		uint Add(STDCALLTYPE func,uint index = -1){
			void* p = _CProcedure();
			_InitStdProcdure(SuperCast<void*>(func),p);
			return _Ptrs.Add(p,index);
		}
		bool Delete(uint index)
		{
			return _delegate::Delete(index);
		}
		template<typename R,typename P> bool Delete(R* object,P proc)
		{
			return Delete(IndexOf(object,proc));
		}
		bool Delete(STDCALLTYPE func)
		{
			return Delete(IndexOf(func));
		}
		bool Delete(Function<void,T1,T2,T3> func)
		{
			return Delete(IndexOf(func));
		}
		template<typename R,typename P> uint IndexOf(R* object,P proc)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2,T3);
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if((void*)object!=GetObject(i)) continue;
				if(SuperCast<void*,THISCALLTYPE>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(STDCALLTYPE proc){
			for(uint i=0;i<_Ptrs.Count();i++){
				void* p = SuperCast<void*>(proc);
				void* p2 = GetFunction(i);
				if(p!=p2) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(Function<void,T1,T2,T3> func)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(func.GetObject()!=GetObject(i)) continue;
				if(func.Procedure()!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		void Call(T1 t1,T2 t2,T3 t3)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				((STDCALLTYPE)_Ptrs[i])(t1,t2,t3);
			}
		}
	};
	template<typename T1,typename T2> class CONCISETL_API Delegate<T1,T2> : public _delegate
	{
	public:
		typedef void (__stdcall* STDCALLTYPE)(T1,T2);
		template<typename R,typename P> uint Add(R* object,P proc,uint index = -1)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2);
			void* p = _CProcedure();
			_InitCProcdure(object,SuperCast<void*,THISCALLTYPE>(proc),p);
			return _Ptrs.Add(p,index);
		}
		uint Add(Function<void,T1,T2> func,uint index = -1){
			void* p = _CProcedure();
			if(func.IsNull()) return 0;
			_InitCProcdure(func.GetObject(),func.Procedure(),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个静态函数调用.
		uint Add(STDCALLTYPE func,uint index = -1){
			void* p = _CProcedure();
			_InitStdProcdure(SuperCast<void*>(func),p);
			return _Ptrs.Add(p,index);
		}
		template<typename R,typename P> bool Delete(R* object,P proc)
		{
			return Delete(IndexOf(object,proc));
		}
		bool Delete(uint index)
		{
			return _delegate::Delete(index);
		}
		bool Delete(STDCALLTYPE func)
		{
			return Delete(IndexOf(func));
		}
		bool Delete(Function<void,T1,T2> func)
		{
			return Delete(IndexOf(func));
		}
		template<typename R,typename P> uint IndexOf(R* object,P proc)
		{
			typedef void (R::*THISCALLTYPE)(T1,T2);
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if((void*)object!=GetObject(i)) continue;
				if(SuperCast<void*,THISCALLTYPE>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(STDCALLTYPE proc)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(SuperCast<void*>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(Function<void,T1,T2> func)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(func.GetObject()!=GetObject(i)) continue;
				if(func.Procedure()!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		void Call(T1 t1,T2 t2)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				((STDCALLTYPE)_Ptrs[i])(t1,t2);
			}
		}
	};
	template<typename T1> class CONCISETL_API Delegate<T1> : public _delegate
	{
	public:
		typedef void (__stdcall* STDCALLTYPE)(T1);
		template<typename R,typename P> uint Add(R* object,P proc,uint index = -1)
		{
			typedef void (R::*THISCALLTYPE)(T1);
			void* p = _CProcedure();
			_InitCProcdure(object,SuperCast<void*,THISCALLTYPE>(proc),p);
			return _Ptrs.Add(p,index);
		}
		uint Add(Function<void,T1> func,uint index = -1){
			void* p = _CProcedure();
			if(func.IsNull()) return 0;
			_InitCProcdure(func.GetObject(),func.Procedure(),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个静态函数调用.
		uint Add(STDCALLTYPE func,uint index = -1){
			void* p = _CProcedure();
			_InitStdProcdure(SuperCast<void*>(func),p);
			return _Ptrs.Add(p,index);
		}
		bool Delete(uint index)
		{
			return _delegate::Delete(index);
		}
		template<typename R,typename P> bool Delete(R* object,P proc)
		{
			return Delete(IndexOf(object,proc));
		}
		bool Delete(STDCALLTYPE func)
		{
			return Delete(IndexOf(func));
		}
		bool Delete(Function<void,T1> func)
		{
			return Delete(IndexOf(func));
		}
		template<typename R,typename P> uint IndexOf(R* object,P proc)
		{
			typedef void (R::*THISCALLTYPE)(T1);
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if((void*)object!=GetObject(i)) continue;
				if(SuperCast<void*,THISCALLTYPE>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(STDCALLTYPE proc)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(SuperCast<void*>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(Function<void,T1> func)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(func.GetObject()!=GetObject(i)) continue;
				if(func.Procedure()!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		void Call(T1 t1)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				((STDCALLTYPE)_Ptrs[i])(t1);
			}
		}
	};
	template<> class CONCISETL_API Delegate<> : public _delegate
	{
	public:
		typedef void (__stdcall* STDCALLTYPE)();
		template<typename R,typename P> uint Add(R* object,P proc,uint index = -1)
		{
			typedef void (R::*THISCALLTYPE)();
			void* p = _CProcedure();
			_InitCProcdure(object,SuperCast<void*,THISCALLTYPE>(proc),p);
			return _Ptrs.Add(p,index);
		}
		uint Add(Function<void> func,uint index = -1){
			void* p = _CProcedure();
			if(func.IsNull()) return 0;
			_InitCProcdure(func.GetObject(),func.Procedure(),p);
			return _Ptrs.Add(p,index);
		}
		//添加一个静态函数调用.
		uint Add(STDCALLTYPE func,uint index = -1){
			void* p = _CProcedure();
			_InitStdProcdure(SuperCast<void*>(func),p);
			return _Ptrs.Add(p,index);
		}
		//从代表中删除指定索引的调用,如果索引超界返回false.
		bool Delete(uint index)
		{
			return _delegate::Delete(index);
		}
		template<typename R,typename P> bool Delete(R* object,P proc)
		{
			return Delete(IndexOf(object,proc));
		}
		bool Delete(STDCALLTYPE func)
		{
			return _delegate::Delete(IndexOf(func));
		}
		bool Delete(Function<void> func)
		{
			return _delegate::Delete(IndexOf(func));
		}
		template<typename R,typename P> uint IndexOf(R* object,P proc)
		{
			typedef void (R::*THISCALLTYPE)();
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if((void*)object!=GetObject(i)) continue;
				if(SuperCast<void*,THISCALLTYPE>(proc)!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(STDCALLTYPE proc)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(SuperCast<void*>(proc)!=this->GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		uint IndexOf(Function<void> func)
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				if(func.GetObject()!=GetObject(i)) continue;
				if(func.GetFunction()!=GetFunction(i)) continue;
				return i;
			}
			return -1;
		}
		void Call()
		{
			for(uint i=0;i<_Ptrs.Count();i++)
			{
				((STDCALLTYPE)_Ptrs[i])();
			}
		}
	};
}