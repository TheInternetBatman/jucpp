#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	//这个函数有3个用途：
	// 1. 存储线程的唯一 MsgLoop 对象的指针（局部静态变量），供后面调用，此时 msg 参数传线程唯一 MsgLoop 对象的指针，实际上它只在 MsgLoop 的构造函数中调用一次。
	// 2. 取得线程唯一 MsgLoop 对象的指针，此时 msg 参数传默认值 0 。
	// 3. 重新设置线程唯一 MsgLoop 对象的指针为 0 ，参数 msg 必须传设置时的值，如果不是程序将会退出。它实际上只在 MsgLoop 对象析构的时候调用一次。
	__declspec(thread) static MsgLoop* cMsgLoop = 0;
	MsgLoop* getThreadMsgLoop(bool del = false){
		if(del){
			if(cMsgLoop){
				delete cMsgLoop;
				cMsgLoop = 0;
			}
			return 0;
		}
		if(cMsgLoop==0) cMsgLoop = new MsgLoop;
		return cMsgLoop;
	}
	MsgLoop::MsgLoop():_Dlg(0),_Accel(0){
	}
	void MsgLoop::_SetDialog(HWND hDlg){
		_Dlg = hDlg;
	}
	void MsgLoop::_SetAccelerator(HACCEL hacl){
		_Accel = hacl;
	}
	int MsgLoop::_Start(){
		MSG msg;
		while(::GetMessage(&msg,0,0,0)){
			if(_Dlg){
				if(IsDialogMessage(_Dlg,&msg)) continue;
			}
			if(_Accel){
				if(TranslateAccelerator(msg.hwnd,_Accel,&msg)) continue;
			}
			uint i = 0;
			for(;i<_MsgHook.Count();i++){
				if(!_MsgHook[i].IsNull()) {
					if(_MsgHook[i].Procedure()(&msg)) break;
				}
			}
			if(i==_MsgHook.Count()){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		getThreadMsgLoop(true);
		return (int)msg.wParam;
	}
	Function<bool,MSG*>* MsgLoop::_AddMsgHook(int index){
		return _MsgHook.Add(index);
	}
	bool MsgLoop::_DelMsgHook(Function<bool,MSG*> func){
		for(uint i=0;i<_MsgHook.Count();i++){
			if(func==_MsgHook[i]){
				_MsgHook.Delete(i);
				return 1;
			}
		}
		return 0;
	}
	MsgLoop::~MsgLoop(){
	}
	int MsgLoop::Start(){
		MsgLoop* ml = getThreadMsgLoop();
		return ml->_Start();
	}
	//设置当前线程中对话框的句柄，以使它能够响应 Tab 键。
	bool MsgLoop::SetDialog(HWND hDlg){
		MsgLoop* ml = getThreadMsgLoop();
		if(ml==0) return 0;
		ml->_SetDialog(hDlg);
		return 1;
	}
	//设置当前线程的加速键表。
	bool MsgLoop::SetAccelerator(HACCEL hacl){
		MsgLoop* ml = getThreadMsgLoop();
		if(ml==0) return 0;
		ml->_SetAccelerator(hacl);
		return 1;
	}
	//添加当前线程的消息(uMsg)截取钩子,index优先位置。返回值是一个函数调用指针,可以绑定想要接收消息的函数.
	bool MsgLoop::AddMsgHook(Function<bool,MSG*> func,int index){
		MsgLoop* ml = getThreadMsgLoop();
		if(ml==0) return 0;
		*(ml->_AddMsgHook(index)) = func;
		return 1;
	}
	//移除线程已经绑定的钩子，添加和移除函数必须被成对的调用.
	bool MsgLoop::DelMsgHook(Function<bool,MSG*> func){
		MsgLoop* ml = getThreadMsgLoop();
		if(ml==0) return 0;
		return ml->_DelMsgHook(func);
	}
}
