#pragma once

namespace cs{
	/*对话框最多可以预先放置255个控件,更多的控件必须在初始化对话框的时候手动生成.

	对话框仍然是一个窗口，实际上对话框是常规窗口的一种包装，主要改变了两个地方：
	1. 生成函数不调用CreateWindow或CreateWindowEx，而是重新包装好的函数：
	DialogBoxParam（模态对话框）和CreateDialogParam（非模态对话框），而且不需要
	注册类名，其实对话框就是一个特定的窗口类，和其它内置控件一样预先注册好了。
	2. 同样窗口函数也进行了包装，对话框的窗口函数虽然形式上和普通窗口是一样的，
	而且大多消息也是未经处理就发给这个函数，但是对话框不发送很多消息，比如WM_CREATE，
	而是用WM_INITDIALOG代替。而且对话框并没有一个DefDialogProc函数供缺省返回，
	对话框窗口函数返回值一般是FALSE，那么对话框会按内部的缺省方式处理，反之返回
	TRUE，那么内部就忽略这个消息，不做任何处理。

	有些特定消息需要返回其他值，比如WM_CTRLCOLORDLG这消息，返回一个画刷句柄，那么
	这个画刷将用来绘制对话框的背景，而且对话框就不再发送WM_ERASEBKGROUND消息。

	IDlg 和 IWnd 走的消息流程完全相同，但是IDlg最终的消息返回时不调用DefWindowProc，
	而是调用_defDlgProc一个直接返回 0 的函数。

	模态对话框的关闭按钮必须在WM_COMMAND（OnCommand）里的IDCANCEL分支里明确调用Close
	函数，否则不会自动关闭。*/
	class CONCISEUI_API IDlg : virtual public IWnd{
	private:
		HRESULT _defDlgProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
		Function<HRESULT,HWND,UINT,WPARAM,LPARAM> _defDlgFunc;
	protected:
		void _MsgHookProc(Message* msg,IWnd*);
	public:
		Delegate<IDlg*>	OnCreate;	//WM_CREATE消息，但是在生成控件的时候，这消息其实是生成完成后模拟调用了一次。
		IDlg();
		HWND	GetNextGroupItem(HWND ctrl,bool pre = 0){return ::GetNextDlgGroupItem(_Handle,ctrl,pre);}
		HWND	GetNextTabItem(HWND ctrl,bool pre = 0){return ::GetNextDlgTabItem(_Handle,ctrl,pre);}
		int		GetCtrlID(HWND ctrl){return GetDlgCtrlID(ctrl);}
		HWND	GetCtrlHandle(int id){return GetDlgItem(_Handle,id);}
		int		GetCtrlInt(int id,bool sign = 1,int* bSuccess = 0){return ::GetDlgItemInt(_Handle,id,bSuccess,sign);}
		int		GetCtrlText(int id,String& str);
		bool	SetCtrlInt(int id,int value,bool sign = 0){return SetDlgItemInt(_Handle,id,value,sign)!=0;}
		bool	SetCtrlText(int id,const wchar_t* text){return SetDlgItemText(_Handle,id,text)!=0;}
	};
	//模态对话框，模态对话框有自己的消息循环。它运行期间，主消息循环接收不到消息。
	class CONCISEUI_API ModelDialog : public IDlg
	{
	public:
		ModelDialog();
		//生成一个模态对话框,直到对话框销毁之前,这个函数不会返回.正常情况下的返回值是调用EndDialog设置的值.如果因为hParent
		//参数不可用,返回值为0,其它错误返回-1.即使对话框的属性为不可见,这个函数也会显示对话框.
		INT_PTR Create(HWND hParent = 0);
		//关闭对话框，code指定退出代码.
		bool Close(int code = 0);
	};
	//非模态对话框.
	class CONCISEUI_API NormalDialog : public IDlg
	{
	public:
		NormalDialog();
		//生成一个非模态对话框,返回对话框的句柄.
		INT_PTR Create(HWND hParent = 0);
		//关闭对话框.
		bool Close();
	};
};