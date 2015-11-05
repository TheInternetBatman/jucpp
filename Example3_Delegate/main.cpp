
#include "../juwnd.h"
#pragma usexpstyle

class A{
public:
	void func(ju::String& str,int i){
		ju::LogfI(L"call A::func(%s,%d)",str.Handle(),i);
	}
};
void __stdcall stdfunc(ju::String& str,int i){
	ju::LogfI(L"call stdfunc(%s,%d)",str.Handle(),i);
}
WINMAIN{
	ju::LogShowWindow();

	//声明 Delegate 对象，它又两个参数，分别是 ju::String& 和 int 类型，返回值是 void
	//所有 Delegate 的回调函数都必须是返回 void，因为 Delegate 会绑定多个回调函数，
	//所以无法设置返回值。
	ju::Delegate<ju::String&,int> dg;

	ju::String str = L"hello";
	//添加两个回调函数对这个 Delegate 对象。
	A a;
	dg.Add(&a,&A::func);
	dg.Add(&stdfunc);
	//执行 Delegate，上面添加的两个函数被执行。
	dg.Call(str,3);

	ju::MsgLoop::Start();
};