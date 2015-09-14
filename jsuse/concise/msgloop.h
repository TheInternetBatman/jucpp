//消息循环
#pragma once
namespace cs{
	//MsgLoop对象是线程的消息泵，一个线程只能实例化一个实例。试图实例化同一线程下的第二个 MsgLoop 实例，将会导致程序退出。
	class CONCISE_API MsgLoop : public _class{
	protected:
		HWND	_Dlg;	//对话框句柄。
		HACCEL	_Accel;	//加速键表。
		//这个集合是截获消息的函数集合,如果某个函数返回TRUE,则下面的函数不会再被调用,直接进入下一个消息.
		ObjectList<Function<bool,MSG*> > _MsgHook;
	public:
		MsgLoop();
		//设置当前线程中对话框的句柄，以使它能够响应 Tab 键。
		void _SetDialog(HWND hDlg);
		//设置当前线程的加速键表。
		void _SetAccelerator(HACCEL hacl);
		//开始线程的主消息循环.
		int _Start();
		//添加当前线程的消息(uMsg)截取钩子,index优先位置。返回值是一个函数调用指针,可以绑定想要接收消息的函数.
		Function<bool,MSG*>* _AddMsgHook(int index = -1);
		//移除线程已经绑定的钩子，添加和移除函数必须被成对的调用.
		bool _DelMsgHook(Function<bool,MSG*> func);
		~MsgLoop();
		//设置当前线程中对话框的句柄，以使它能够响应 Tab 键。
		static int Start();
		//设置当前线程中对话框的句柄，以使它能够响应 Tab 键。
		static bool SetDialog(HWND hDlg);
		//设置当前线程的加速键表。
		static bool SetAccelerator(HACCEL hacl);
		//添加当前线程的消息(uMsg)截取钩子,index优先位置。
		static bool AddMsgHook(Function<bool,MSG*> func,int index = -1);
		//移除线程已经绑定的钩子，添加和移除函数必须被成对的调用.
		static bool DelMsgHook(Function<bool,MSG*> func);
	};
	//在没有当前线程的 MsgLoop 对象指针时，使用以下函数。

	//返回当前实例的句柄,常用于加载当前模块的资源.
	CONCISE_API inline HINSTANCE GetInstance(){return (HINSTANCE)::GetModuleHandle(0);}
}
