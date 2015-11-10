#include "../juwnd.h"
#pragma usexpstyle
class A{
public:
	int func(int a,int b){
		ju::LogfI(L"新线程里执行 A::func(%d,%d); 返回 %d",a,b,a+b);
		return a+b;
	}
};
int __stdcall func(int a,int b){
	ju::LogfI(L"新线程里执行 func(%d,%d); 返回 %d",a,b,a-b);
	return a-b;
}
WINMAIN{
	ju::LogShowWindow();

	//声明一个 Thread 对象
	ju::Thread th;

	A a;
	//在新线程执行类的成员函数，并且传递参数，Thread支持最多 10 个任意类型参数的函数
	th.Call<int>(&a,&A::func,3,2);//函数返回值必须作为模版参数传递，不能省略
	//一个 Thread 对象一旦被执行，就包含了一个线程的句柄，Close 函数释放这个句柄，使它就像新的一样，
	//这不会对正在执行的线程有任何影响。如果不执行 Close 函数，后面的 Call 或者 CallStd 不会成功
	th.Close();
	//在新线程执行静态函数(__stdcall)，并且传递参数，Thread支持最多 10 个任意类型参数的函数
	th.CallStd<int>(&func,5,3);//函数返回值必须作为模版参数传递，不能省略
	//Wait 函数等待正在执行的线程返回，或者 Close 被呼叫，查看Wait的返回值来区分是哪种情况。
	th.Wait();
	ju::MsgLoop::Start();
}
