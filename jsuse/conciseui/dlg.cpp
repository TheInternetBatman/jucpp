#include "stdafx.h"
#include "../conciseui.h"
#include <commctrl.h>


namespace cs{
	IDlg::IDlg(){
		GetMsgHook(WM_INITDIALOG)->Add(this,&IDlg::_MsgHookProc);
		GetMsgHook(WM_ACTIVATE)->Add(this,&IDlg::_MsgHookProc);
		GetMsgHook(WM_CLOSE)->Add(this,&IDlg::_MsgHookProc);
		_defDlgFunc.Bind(this,&IDlg::_defDlgProc);
		_QuondamProc = (LONG_PTR)_defDlgFunc.Procedure();
	}
	HRESULT IDlg::_defDlgProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam){
		return (HRESULT)FALSE;
	}
	void IDlg::_MsgHookProc(Message* msg,IWnd*){
		if(msg->uMsg==WM_CLOSE){
			msg->Result._SetDisableWndproc(1);//禁用内部调用 OnClose 。
		}else if(msg->uMsg==WM_INITDIALOG){
			OnCreate.Call(this);
		}else if(msg->uMsg==WM_ACTIVATE){
			if(LOWORD(msg->wParam)==WA_INACTIVE)
				MsgLoop::SetDialog(0);
			else
				MsgLoop::SetDialog(_Handle);
		}
	}
	int IDlg::GetCtrlText(int id,String& str){
		HWND ctrl = GetDlgItem(_Handle,id);
		if(ctrl==0) return 0;
		int i = GetWindowTextLength(ctrl);
		if((int)str.Cubage()<i) str.SetCubage(i);
		i = GetWindowText(ctrl,str.Handle(),str.Cubage()+1);
		str.Realize();
		return str.Length();
	}
	//class ModelDialog
	ModelDialog::ModelDialog(){
	}
	INT_PTR ModelDialog::Create(HWND hParent){
		if(_Handle) return 0;
		if(hParent==0) hParent = GetDesktopWindow();
		return ::DialogBoxParam(Param->Dialog.Instance,Param->Dialog,hParent,(DLGPROC)_WndPointer.Procedure(),(LPARAM)Param->lParam);
	}
	bool ModelDialog::Close(int code){
		bool cancel = 0;
		OnClose.Call(cancel,this);
		if(cancel) return 0;
		return 0!=::EndDialog(_Handle,code);
	}
	NormalDialog::NormalDialog(){
		GetMsgHook(WM_ACTIVATE)->Add(this,&NormalDialog::_MsgHookProc);
	}
	INT_PTR NormalDialog::Create(HWND hParent)
	{
		if(_Handle) return 0;
		if(hParent==0) hParent = GetDesktopWindow();
		return (INT_PTR)::CreateDialogParam(Param->Dialog.Instance,Param->Dialog,hParent,(DLGPROC)_WndPointer.Procedure(),(LPARAM)Param->lParam);
	}
	bool NormalDialog::Close(){
		bool cancel = 0;
		OnClose.Call(cancel,this);
		if(cancel) return 0;
		return 0!=::DestroyWindow(_Handle);
	}
};