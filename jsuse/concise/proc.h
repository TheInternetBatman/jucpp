namespace cs{
	//下划线开头的函数和类, 一般是库的开发人员使用, 库的使用者不必使用这些对象.
	CONCISE_API void* _CProcedure(void* p = 0);
	CONCISE_API void _InitCProcdure(void* obj,void* func,void* pointer,int n = 0);
	//这个函数实际上是 _InitCProcdure obj = NULL 的特殊情形。
	CONCISE_API void _InitStdProcdure(void* func,void* pointer,int n = 0);
	//_StdClassProc可以看作函数指针类型,但是可以处理成员函数,并且不会进行过分严格的类型检查,事实上,它不进行任何安全
	//检查,你可以把任何类型的函数传递给它.迫于C++的语法,没有一个简单可行的方式进行类型检查.
	class CONCISE_API _StdClassProc : public HandleType<void*>{
	public:
		_StdClassProc(){};
		_StdClassProc(_StdClassProc& scp){
			*this = scp;
		}
		~_StdClassProc();
		//判断一个成员函数是否已经绑定这个实例,对于静态函数,obj参数传NULL值.
		template<typename T> bool HasBind(void* obj,T func){
			if(obj!=GetObject()) return false;
			return SuperCast<void*,T>(func)==GetFunction();
		}
		//返回指针对应的实例指针.
		void* GetObject();
		//返回指针对应的函数指针.
		void* GetFunction();
		//判断两个实例是否对应相同的调用(本质上就是判断GetObject()和GetFunction()返回的值是否相同).
		bool operator == (_StdClassProc& scp);
		//把一个调用赋值给另一个调用.
		void operator = (_StdClassProc& scp);
		//返回调用指针,可以用于__stdcall类型的回调函数.
		template<typename P> inline P Procedure(){
			if(_Handle==0)//不可恢复错误
				CONASSERT(L"调用一个空的函数对象,程序将崩溃");
			return (P)(INT_PTR)_Handle;
		}
		//是否是一个空函数（可以被调用，但是没有绑到任何函数），IsNull 返回 true，对象不可调用。
		bool IsEmpty();
		//释放对象的资源，IsNull返回 true 。
		void Release();
		//绑定的是一个静态函数（__stdcall）
		bool IsStdcall();
		//绑定的是一个类的成员函数。（__thiscall）
		bool IsThiscall();
		//绑定一个成员函数,obj是实例指针,func是类的成员函数.
		template<typename T>void Bind(void* obj,T func,int n = 0){
			if(_Handle==0) _Handle = _CProcedure();
			if(!_Handle) return;
			_InitCProcdure(obj,SuperCast<void*>(func),_Handle,n);
		}
		//绑定一个静态函数,func是函数.
		template<typename T>void BindStd(T func,int n = 0){
			if(_Handle==0) _Handle = _CProcedure();
			_InitStdProcdure(SuperCast<void*>(func),_Handle,n);
		}
	};
	//_delegate是Delegate的基类,因为Delegate并没有增加成员变量,所以大小和_delegate是相同的,如果要声明一个列表(ObjectList)
	//而其成员是不同的Delegate,可是声明为_delegate类型,而在使用的时候再转换为相应类型的Delegate,而不用担内存和析构的问题.
	class CONCISE_API _delegate : public _class
	{
	public:
		//从代表中删除指定索引的调用,如果索引超界返回false.
		bool Delete(uint index);
		//返回指定索引的调用对应的对象指针.索引超界返回0
		void* GetObject(uint index);
		//返回指定索引对应的函数指针.索引超界返回0
		void* GetFunction(uint index);
		//赋值
		void operator = (_delegate& dt);
		//返回绑定的调用数量.
		inline uint Count(){return _Ptrs.Count();}
		~_delegate();
	protected:
		IntegerList<void*> _Ptrs;
	};
	/**	{
	//	"type":"class",
	//	"name":"APIHook",
	//	"text":"APIHook 用来设置当前进程的 API 函数拦截功能，也就是当前进程对拦截的 API 函数的呼叫全部指向我们设置的函数（hook），这个函数是一个参数和返回值和原 API 都相同的函数。",
	//	"remark":[
	//		"APIHook 不能用于拦截其它程序对指定 API 的调用"
	//	]**/
	class CONCISE_API APIHook : public _class{
	protected:
		uchar* _orgFunc;
	public:
		APIHook();
		~APIHook();
		/**{
		//	"type":"function",
		//	"name":"SetHook(void* api,void* hook,int pSize)"
		//	"text":"设置一个 API 的拦截。",
		//	"param":[
		//		{
		//			"type":"void*",
		//			"name":"api",
		//			"text":"要拦截的 api，需要用 GetProcAddress 来获取，不能直接设置函数名，因为函数名实际上是跳转地址，不是 API 的真正地址"
		//		},
		//		{
		//			"type":"void*",
		//			"name":"hook",
		//			"text":"拦截后，指向的函数地址，也就是我们自己的函数。"
		//		},
		//		{
		//			"type":"void*",
		//			"name":"pSize",
		//			"text":"API 函数的参数长度，这个值是所有参数的大小总和。注意：即使是 bool、uchar 这样的参数，实际上编译器也是按 4 字节对齐的，也就是任何参数的大小都是 4 的倍数。"
		//		}
		//	],
		//	"return":{
		//		"type":"bool",
		//		"text":"返回值表示设置是否成功"
		//	}
		}**/
		bool SetHook(void* api,void* hook,int pSize);
		/**,{
		//	"type":"function",
		//	"name":"UnHook()",
		//	"text":"还原拦截的 API 。",
		//	"return":{
		//		"type":"bool",
		//		"text":"返回值表示函数是否成功"
		//	}
		}**/
		bool UnHook();
		/**,{
		//	"type":"function",
		//	"name":"OrgFunc()",
		//	"text":"获取原来的 API ，调用这个函数相当于绕过拦截，往往我们自己的函数里需要调用这个函数。",
		//	"return":{
		//		"type":"void*",
		//		"text":"这个值是一个函数地址，如果没有拦截任何函数，这个值返回 NULL 。"
		//	}
		}**/
		inline const void* OrgFunc(){
			return _orgFunc;
		}
	};
	/**}**/
	/*@function GetThis(T,var)
	@text 获取静态(__stdcall)函数被 Function 回调的时候，初始化 Function 时设置的 object 指针。
	这个函数本质上一个宏，它嵌入一段汇编代码在函数的开头。<span class="red_bold">注意：
	这个宏必须在函数的开头使用</span>。因为它实际上只有2行，声明变量 var 和一行嵌入式汇编 mov var,edx 。
	慎用这个机制，必须确保 edx 寄存器在之前不被使用，而不同的编译器编译的方式是不同的。
	@text 给静态函数增加 this 指针有什么用？很多时候，在设计一个类的时候，往往它的回调函数参数里没有自己，
	否则，就需要给每一个回调函数都额外增加一个参数来指示呼叫实例。而在某些场合，回调函数又是一个静态函数，
	而非它自己的成员函数，如果这个静态函数被多个实例回调，我们就需要使用这个机制知道哪个实例呼叫了回调函数。
	@param T
	@text 想要声明的对象类型。
	@param var -- T
	@text 声明的变量名称
	@return 无
	@code 
	class A{
	public:
	int call(int i){
	return i+3;
	}
	};
	int __stdcall cbfunc(int i){
	GetThis(A,pa);
	return 2*pa->call(i);
	}
	int main(){
	//声明一个函数对象
	Function<int,int> func;
	//声明一个 A 对象。
	A a;
	//把 a 的地址作为 this 指针传递给静态函数 cbfunc。
	func.BindStd(&cbfunc,&a);
	//呼叫函数，cbfunc将会被回调，而 pa 将会是 a 的地址。
	int n = func(2);
	//n 应该返回 10 .
	return n;
	}
	*/
#define GetThis(T,var) T* var;__asm mov var,edx
	//一个不会被使用的空类型,实际当中你永远不会用到它,这正是它的作用.
	CONCISE_API void* GetNullProc(uint n);
	class CONCISE_API NullType{private:NullType(){}};
	template<typename T> struct CONCISETL_API is_reference{
		enum{value = false};
	};
	template<typename T> struct CONCISETL_API is_reference<T&>{
		enum{value = true};
	};
	template<typename T> uint GetAlignment(){
		if(is_reference<T>::value) return 4;
		else{
			size_t dsp = sizeof(T);
			if(dsp%4){
				dsp /= 4;
				dsp += 1;
				dsp *= 4;
			}
			return (uint)dsp;
		}
	}
	//Function类提供函数指针的作用,但是有严格的类型检查,并且,可以把类的成员函数用作回调(__stdcall)函数.与Delegate的区别是,
	//Function只能设置一个函数,而Delegate可以调用多个函数,后者也不能回调.
	//Function的一个重要特性，就是当函数已经执行到 Function 实例指向的函数代码时，实例本身是可以安全删除的，因为代码再也不
	//会回到实例本身了，实例实际上提供的是代码的跳转功能，一旦已经跳转，这部分内存就不再需要了。
	//空函数，一个Function对象是可以不经初始化（Bind一个函数）而调用的，但是这种情况不能使用它的返回值，Function本身只完成跳转功能，
	//空函数不进行任何跳转相当于一个无返回值的空函数调用，所以它这种情况下不等价于一个对应类型的函数。
}