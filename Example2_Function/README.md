# Function 示例
```CPP
/* 工程设置：
	“配置属性”-->“连接器”-->“常规”-->“附加库目录” 添加 “../lib”
	“配置属性”-->“C/C++”-->“代码生成”-->“运行库” 选择 “多线程调试(/MTd)”，
	Release 版本选“多线程(/MT)”
*/
//引用 juwnd.h，因为使用了 Log 窗口
#include "../juwnd.h"
#pragma usexpstyle	//启用 XP 控件风格，否则是非常难看的 Win98 样式

class A{
public:
	int func2(LPCWSTR str,int n){
		ju::LogfI(L"A::func2 参数 1: %s,参数 2: %d",str,n);
		return ju::WcsLength(str);
	}
};
int __stdcall stdfunc(LPCWSTR str,int n){
	ju::LogfI(L"stdfunc 参数 1: %s,参数 2: %d",str,n);
	return ju::WcsLength(str);
}
//WINMAIN 就是 WinMain 函数的宏，它就是通常的入口函数d
WINMAIN{
	//打开输出窗口，Log组件具有更多的样式控制，但是和控制台不同的是，它不能输入
	ju::LogShowWindow(true);

	A a;
	ju::Function<int,LPCWSTR,int> func2;

	int r = a.func2(L"类直接调用",3);
	ju::LogfI(L"函数返回: %d",r);

	func2.Bind(&a,&A::func2);
	r = func2(L"绑定回调函数调用",6);
	ju::LogfI(L"函数返回: %d",r);

	func2.BindStd(&stdfunc);
	r = func2(L"绑定静态回调函数调用",9);
	ju::LogfI(L"函数返回: %d",r);
	//启动消息循环，这是每个窗口程序的标准流程。
	return ju::MsgLoop::Start();
};
```
# Function 有什么用？

函数回调在多线程和事件驱动的应用中普遍使用，C++的类成员函数回调非常难以使用，因为C++的类成员函数指针对类的类型是控制死的，不同的类成员函数，尽管函数格式相同（参数和返回值相同），但是并不能统一的作为回调函数，这一点和静态回调函数是不同的。Windows API 的回调函数全部是 __stdcall 类型的。

但是Function类，只需要指定函数的格式（参数和返回值），就可以绑定任何类的成员或者一个静态函数。而且它是安全的，类型和返回值不同的函数是无法绑定到 Function 实例上的，在编译阶段就会报错。

# Function 类函数参考：

1. IsNull()	这个函数是重写它的父类同名函数，IsNull 返回 true 的时候不能调用这个 Function 实例，因为它没有被有效初始化。
2. Empty() 这个函数解绑它绑定的函数，但是呼叫这个实例并不报错，只是不执行任何函数。可以对一个未初始化的函数调用 Empty() 从而使它可以安全的被调用，Empty() 实际上会初始化实例，但是不绑定任何函数，IsNull() 返回 false。
3. Procedure() 显式的返回一个函数指针，直接调用 Function 的实例实际上是操作符返回和 Procedure 函数相同的指针。
4. Bind(obj,func) 绑定一个类的成员函数，需要传类的指针和类成员函数的指针两个参数。
5. BindStd(func) 绑定一个静态回调函数(__stdcall)。
6. operator = () 你可以直接使用等于号来对两个 Function 实例之间赋值，但是必须是同样格式（参数和返回值相同）的 Function 实例。
7. HasBind() 实例是否已经绑定了一个函数。
8. GetObject() 返回绑定的函数对象，如果绑定了一个静态函数，或者 Empty 状态，返回 NULL。
9. GetFunction() 返回绑定的函数指针的 32 位值，如果是静态函数，它就是绑定的函数本身，如果函数为 Empty 状态，返回 NULL。
10. operator == () 比较两个 Function 实例是否绑定了同一个对象。
11. IsEmpty() 函数是否是 Empty 状态，函数调用了 Empty 之后就是 Empty 状态，这种状态可以调用，但是不回调任何函数。
12. Release() 释放 Function 对象，每个 Function 实例占用 10 字节的内存，Release 之后，实例不可再调用，IsNull() 返回 true。
13. IsStdcall() 是否绑定了一个静态函数。
14. IsThiscall() 是否绑定了一个类的成员函数。
