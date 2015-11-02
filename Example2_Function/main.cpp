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