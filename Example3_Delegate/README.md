# Delegate 示例

Function 可以很好的用于回调函数，但是某些场合，我们需要添加自己的回调，但是不破坏其它人设置的回调，Delegate 就是满足这种需求的一个类，Delegate 可以添加多个回调函数，每个回调都是相同的格式，可以是类的成员函数，也可以是 __stdcall 静态函数。当 Delegate 被执行的时候，它会依次调用添加的所有回调函数。

显然 Delegate 返回值是无意义的，因为绑定了多个回调函数，它们的返回值无法取舍，干脆全部舍去。当然，事实上你根本无法把有返回值的函数添加进 Delegate。

```CPP

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

	//声明 Delegate 对象，它有两个参数，分别是 ju::String& 和 int 类型，返回值是 void
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
```
# Delegate 类函数参考：

1. uint Add(R* object,P proc,uint index = -1) 把一个类的成员函数添加到回调列表，object 是类实例指针，proc 是成员函数，index 是添加位置，用于调整呼叫先后顺序，默认值 -1 表示添加到最后。

2. uint Add(Function<void> func,uint index = -1) 把一个 Function 对象添加到回调列表，index 是添加位置，用于调整呼叫先后顺序，默认值 -1 表示添加到最后。

3. uint Add(STDCALLTYPE func,uint index = -1) 把一个静态函数添加到回调列表，index 是添加位置，用于调整呼叫先后顺序，默认值 -1 表示添加到最后。

4. bool Delete(uint index) 删除已经添加的指定索引的回调函数，索引超限的话，返回 false.

5. bool Delete(R* object,P proc) 从回调列表删除特定的函数,如果函数没有在实例中,返回false.

6. bool Delete(STDCALLTYPE func) 从回调列表删除静态函数,如果函数没有在实例中,返回false.

7. bool Delete(Function<void> func) 从回调列表删除一个 Function 对象，如果对象不在列表中，返回 false。

8. uint IndexOf(R* object,P proc) 返回指定调用的索引,如果调用不在 Delegate 中,则返回-1。

9. uint IndexOf(STDCALLTYPE proc) 返回指定调用的索引,如果调用不在 Delegate 中,则返回-1。

10. uint IndexOf(Function<void> func) 返回指定调用的索引,如果调用不在 Delegate 中,则返回-1。

11. void Call() 调用 Delegate .添加的所有函数都会被呼叫

12, bool Delete(uint index) 从 Delegate 中删除指定索引的调用,如果索引超界返回false;

13. void* GetObject(uint index) 返回指定索引的调用对应的对象指针.索引超界返回0;

14. void* GetFunction(uint index) 返回指定索引对应的函数指针.索引超界返回0;

15. void operator = (_delegate& dt) Delegate 的赋值操作;

16. uint Count() 返回绑定的回调数量。
