# Hello Jsuse

这是一个最基本的 jsuse 库的工程框架，它创建一个 Windows 窗口程序。


```CPP

//引用 juwnd.h，因为我们要创建一个窗口程序
#include "../juwnd.h"

//集成一个主窗口类，ju::Frame 是主窗口基类，它是一个独立窗口，
//区别于附着于主窗口的子窗口控件。
class Main : public ju::Frame{
protected:
	void onCreate(ju::IWnd* wnd){
		// 在这里填写创建子控件等代码，此时窗口已经创建完成;
	}
public:
	Main(){
		//此时窗口还没创建，初始化主窗口参数。

		// 设置窗口文本（标题）
		Param->Text = L"Hello jsuse!";
		//添加 WM_CREATE 消息回调函数，这是一个常用消息，所以设置了一个 OnCreate 代理成员（Delegate）
		//Delegate 支持添加多个回调函数，它支持添加类的成员函数或者静态函数作为回调。
		OnCreate.Add(this,&Main::onCreate);
	}
};

//WINMAIN 就是 WinMain 函数的宏，它就是通常的入口函数
WINMAIN{
	Main m;
	//创建并显示主窗口
	m.Create();
	//启动消息循环，这是每个窗口程序的标准流程。
	return ju::MsgLoop::Start();
};```

工程配置如下

1. 创建工程：
![](https://github.com/pgmsoul/store/blob/master/image/hello.jpg)

2. 配置工程的运行库链接为静态：
![](https://github.com/pgmsoul/store/blob/master/image/setlink.jpg)
![](https://github.com/pgmsoul/store/blob/master/image/setlinkr.jpg)

3. 程序运行结果：
![](https://github.com/pgmsoul/store/blob/master/image/example1.jpg)


