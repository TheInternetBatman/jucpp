
/**TaskPool 是一个类似于线程池的工具，可以使用它启动线程来执行指定的函数，
线程执行完任务后不会退出，而是等待新人物。每次启动任务的时候，如果有闲置
的线程，就不会启动新线程，而是使用闲置线程来执行任务。*/

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
	//声明一个 TaskPool 对象，和 Thread 不同，TaskPool 需要首先指定函数类型
	//这是因为 TaskPool 事实上在内存里搭建了一个执行的结构，这个结构一旦创建
	//是不能改变的，也就是说，所有它启动的函数必须具有相同的格式。
	ju::TaskPool<int,int,int> tp;
	A a;
	tp.StartTask(&a,&A::func,6,3);
	tp.StartStdTask(&func,6,3);
	ju::MsgLoop::Start();
	//即使不用消息循环来阻止程序退出，TaskPool 对象在销毁时也会等待所有的线
	//程执行完成，如果有正在执行的线程，它会一直等待。所以如果想强制结束所
	//有线程，可以使用 Close 函数，并且指定一个最长等待时间。
	tp.Close(1000);
}
