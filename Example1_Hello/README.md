# Hello Jsuse

这是一个最基本的 jsuse 库的工程框架，它创建一个 Windows 窗口程序。


```CPP
#include "../juwnd.h"

class Main : public ju::Frame{
protected:
	void onCreate(ju::IWnd* wnd){
		;
	}
public:
	Main(){
		Param->Text = L"Hello jsuse!";
		OnCreate.Add(this,&Main::onCreate);
	}
};
WINMAIN{
	Main m;
	m.Create();
	return ju::MsgLoop::Start();
};
```

工程配置如下

![](https://github.com/pgmsoul/store/blob/master/hello.png)
![](https://github.com/pgmsoul/store/blob/master/setlink.png)
![](https://github.com/pgmsoul/store/blob/master/setlinkr.png)
