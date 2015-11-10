/**
APIHook 有异常强大的拦截 Winddows API 的功能，而且使用非常简单。
系统 API 拦截还有一个重要问题就是如何呼叫原来的 API 的问题，APIHook
完美的解决了这个问题。
*/
#include "../juwnd.h"
#pragma usexpstyle
//声明一个 MessageBox 函数类型，因为我们需要使用这个类型的函数指针。
typedef int (__stdcall * MSGBOX)(HWND,LPCWSTR,LPCWSTR,int);
//声明在这里，为的是全局使用 hook 这个变量更方便。
ju::APIHook hook;
//声明一个我们自己的函数，它必须和要拦截的 API 具有完全相同的格式，
//包括参数和返回值，以及必须是 __stdcall，或者是一个类的成员函数。
int __stdcall alert(HWND hWnd,LPCWSTR msg,LPCWSTR title,int option){
	//显示参数
	ju::LogfI(L"alert--hWnd:%d,msg:%s,title:%s,option:%d",hWnd,msg,title,option);
	//获取系统原来的 API 指针，此时如果呼叫 API 函数名，还会被指向这里。
	MSGBOX org = (MSGBOX)hook.OrgFunc();
	ju::LocalString str;
	str.Format(L"alert: %s",msg);
	//呼叫系统原来的函数
	return org(hWnd,str,title,option);
}
//下面是类的成员函数版
class A{
public:
	int alert(HWND hWnd,LPCWSTR msg,LPCWSTR title,int option){
		//显示参数
		ju::LogfI(L"A::alert--hWnd:%d,msg:%s,title:%s,option:%d",hWnd,msg,title,option);
		//获取系统原来的 API 指针，此时如果呼叫 API 函数名，还会被指向这里。
		MSGBOX org = (MSGBOX)hook.OrgFunc();
		ju::LocalString str;
		str.Format(L"A::alert: %s",msg);
		//呼叫系统原来的函数
		return org(hWnd,str,title,option);
	}
};
WINMAIN{
	ju::LogShowWindow();

	//第三个参数是 API 函数参数的大小总和，因为只是传递了一个函数指针，内部无法
	//判定函数的格式。16 的计算方式是：MessageBox 函数有 4 个参数，每个的大小都
	//是 4 字节，需要注意的是，所有 API 几乎都是 4 字节参数，尽管被声明为 char 
	//之类的，如果大于 4 字节，就是 8 字节、12 字节等等。
	if(hook.SetHook(&MessageBox,&alert,16)){
		::MessageBox(0,L"Hello",L"OK",MB_ICONINFORMATION);
	}else{
		ju::LogfE(L"绑定函数失败，函数名称是否正确？");
	}
	//恢复系统 API，如果不这么做，后续的绑定就会出问题，会丢失系统 API 的地址
	hook.UnHook();

	A a;
	//为了绑定类的成员函数，声明一个 Function 对象，一般来说 Function 对象如果
	//仍然可能被呼叫，是不能释放的，但是这里的 func 只起一个参数传递的作用，只要
	//hook没有被销毁，调用就会一切正常。
	ju::Function<int,HWND,LPCWSTR,LPCWSTR,int> func;
	//绑定 thiscall 函数。
	func.Bind(&a,&A::alert);
	//把函数作为 hook 拦截 API。
	if(hook.SetHook(&MessageBox,func)){
		::MessageBox(0,L"Hello",L"OK",MB_ICONINFORMATION);
	}else{
		ju::LogfE(L"绑定函数失败，函数名称是否正确？");
	}
	ju::MsgLoop::Start();
	//这里没有调用 UnHook，是因为这个拦截只在当前进程有效，对系统没有任何影响
	//如果要全局拦截 API，必须要把代码写在一个 dll 动态链接库里。
	//当然，当前进程一般是不需要拦截系统 API 的，因为代码都是自己写的。但是也
	//不一定，我曾经在一个项目里使用了 WebBrowser 控件，又想看它的网络请求，就
	//使用了进程级的 API 拦截 socket 函数 send、recv 之类。
}
