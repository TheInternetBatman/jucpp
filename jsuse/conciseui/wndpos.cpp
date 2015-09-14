#include "stdafx.h"
#include "../conciseui.h"


namespace cs{
	WndPosRestore::WndPosRestore(){}
	WndPosRestore::~WndPosRestore(){}
	void WndPosRestore::_OnCreate(Message* msg,IWnd*){
		int index = -1;
		for(uint i=0;i<_wndList.Count();i++){
			if(_wndList[i]->Handle()==msg->hWnd){
				index = i;
				break;
			}
		}
		if(index==-1) return;
		Config rt(0);
		Json* root = rt.Lock();
		if(!root) return;
		Json* pst = root->GetProperty(L"wndpos");
		if(pst==0) return;
		pst = pst->GetProperty(_wndName[index]);
		if(pst==0) return;
		int p[4];
		if(!pst->GetPropertyInt32(L"left",*p)) return;
		if(!pst->GetPropertyInt32(L"top",*(p+1))) return;
		if(!pst->GetPropertyInt32(L"right",*(p+2))) return;
		if(!pst->GetPropertyInt32(L"bottom",*(p+3))) return;
		WndPtr wnd = msg->hWnd;
		Rect rect;
		wnd.GetRect(rect);
		if(_flag&WPR_FLAG_LEFT){
			rect.OffsetLocation(p[0]-rect.left,0);
		}
		if(_flag&WPR_FLAG_TOP){
			rect.OffsetLocation(0,p[1]-rect.top);
		}
		if(_flag&WPR_FLAG_WIDTH){
			rect.right = p[2] - p[0] + rect.left;
		}
		if(_flag&WPR_FLAG_HEIGHT){
			rect.bottom = p[3] - p[1] + rect.top;
		}
		wnd.SetDimension(rect.left,rect.top,rect.Width(),rect.Height());
	}
	void WndPosRestore::_OnDestroy(Message* msg,IWnd*){
		int index = -1;
		for(uint i=0;i<_wndList.Count();i++){
			if(_wndList[i]->Handle()==msg->hWnd){
				index = i;
				break;
			}
		}
		if(index==-1) return;
		AutoStruct(WINDOWPLACEMENT,wp);
		GetWindowPlacement(msg->hWnd,&wp);
		Config rt(0);
		Json* root = rt.Lock();
		if(!root) return;
		Json* pst = root->GetProperty(L"wndpos",true);
		if(pst==0) return;
		pst = pst->GetProperty(_wndName[index],true);
		if(pst==0) return;
		pst->SetPropertyInt32(L"left",wp.rcNormalPosition.left);
		pst->SetPropertyInt32(L"top",wp.rcNormalPosition.top);
		pst->SetPropertyInt32(L"right",wp.rcNormalPosition.right);
		pst->SetPropertyInt32(L"bottom",wp.rcNormalPosition.bottom);
	}
	bool WndPosRestore::AddWnd(IWnd* wnd,LPCWSTR name){
		if(wnd==0) return 0;
		wnd->GetMsgHook(WM_DESTROY)->Add(this,&WndPosRestore::_OnDestroy);
		wnd->GetMsgHook(WM_CREATE)->Add(this,&WndPosRestore::_OnCreate);
		//Dialog no WM_CREATE Message
		wnd->GetMsgHook(WM_INITDIALOG)->Add(this,&WndPosRestore::_OnCreate);
		_wndList.Add(wnd);
		_wndName.Add(name);
		return 1;
	}
	bool WndPosRestore::RemoveWnd(IWnd* wnd){
		int index = _wndList.IndexOf(wnd);
		if(index==-1) return 0;
		_wndList.Delete(index);
		_wndName.Delete(index);
		wnd->GetMsgHook(WM_DESTROY)->Delete(this,&WndPosRestore::_OnDestroy);
		wnd->GetMsgHook(WM_CREATE)->Delete(this,&WndPosRestore::_OnCreate);
		wnd->GetMsgHook(WM_INITDIALOG)->Delete(this,&WndPosRestore::_OnCreate);
		return 1;
	}

	void WndPosRestore::OnCreate(HWND hwnd){
		Message msg;
		msg.hWnd = hwnd;
		_OnCreate(&msg,GetIWnd(hwnd));
	}
	void WndPosRestore::OnDestroy(HWND hwnd){
		Message msg;
		msg.hWnd = hwnd;
		_OnDestroy(&msg,GetIWnd(hwnd));
	}
	inline WndPosRestore* getAppWndPosRestore(){
		static WndPosRestore wpr;
		return &wpr;
	}
	void SetWndRememberPos(IWnd* wnd,LPCWSTR name,DWORD flag){
		getAppWndPosRestore()->SetFlag(flag);
		getAppWndPosRestore()->AddWnd(wnd,name);
	}
	void WPRUnRegisterWnd(IWnd* wnd,DWORD flag){
		getAppWndPosRestore()->RemoveWnd(wnd);
	}
}