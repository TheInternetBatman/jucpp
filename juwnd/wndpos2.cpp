#include "stdafx.h"
#include "baseui.h"


namespace base{
	WndPosRestore::WndPosRestore(){}
	WndPosRestore::~WndPosRestore(){}
	void WndPosRestore::_OnCreate(Message* msg){
		int index = -1;
		for(uint i=0;i<wndList.Count();i++)
		{
			if(wndList[i]->Handle()==msg->hWnd)
			{
				index = i;
				break;
			}
		}
		if(index==-1) return;
		RegisterTree rt(0);
		if(!rt.Lock()){
			return;
		}
		Xml* pst = rt.GetXml()->GetNode(L"wndpos");
		if(pst==0) return;
		pst = pst->GetNode(wndName[index]);
		if(pst==0) return;
		int p[4];
		if(!pst->GetPropertyInt(L"left",*p)) return;
		if(!pst->GetPropertyInt(L"top",*(p+1))) return;
		if(!pst->GetPropertyInt(L"right",*(p+2))) return;
		if(!pst->GetPropertyInt(L"bottom",*(p+3))) return;
		WndPtr wnd = msg->hWnd;
		wnd.SetDimension(p[0],p[1],p[2]-p[0],p[3]-p[1]);
	}
	void WndPosRestore::_OnDestroy(Message* msg)
	{
		int index = -1;
		for(uint i=0;i<wndList.Count();i++)
		{
			if(wndList[i]->Handle()==msg->hWnd)
			{
				index = i;
				break;
			}
		}
		if(index==-1) return;
		AutoStruct<WINDOWPLACEMENT,1> wp;
		GetWindowPlacement(msg->hWnd,&wp);
		RegisterTree rt(0);
		if(!rt.Lock()){
			return;
		}
		Xml* pst = rt.GetXml()->GetNode(L"wndpos",1);
		pst = pst->GetNode(wndName[index],1);
		if(pst==0) return;
		pst->SetPropertyInt(L"left",wp.rcNormalPosition.left);
		pst->SetPropertyInt(L"top",wp.rcNormalPosition.top);
		pst->SetPropertyInt(L"right",wp.rcNormalPosition.right);
		pst->SetPropertyInt(L"bottom",wp.rcNormalPosition.bottom);
	}
	bool WndPosRestore::AddWnd(IWnd* wnd,LPCWSTR name)
	{
		if(wnd==0) return 0;
		wnd->GetMsgHook(WM_DESTROY)->Add(this,&WndPosRestore::_OnDestroy);
		wnd->GetMsgHook(WM_CREATE)->Add(this,&WndPosRestore::_OnCreate);
		//Dialog no WM_CREATE Message
		wnd->GetMsgHook(WM_INITDIALOG)->Add(this,&WndPosRestore::_OnCreate);
		wndList.Add(wnd);
		wndName.Add(name);
		return 1;
	}
	bool WndPosRestore::RemoveWnd(IWnd* wnd)
	{
		int index = wndList.IndexOf(wnd);
		if(index==-1) return 0;
		wndList.Delete(index);
		wndName.Delete(index);
		wnd->GetMsgHook(WM_DESTROY)->Delete(this,&WndPosRestore::_OnDestroy);
		wnd->GetMsgHook(WM_CREATE)->Delete(this,&WndPosRestore::_OnCreate);
		wnd->GetMsgHook(WM_INITDIALOG)->Delete(this,&WndPosRestore::_OnCreate);
		return 1;
	}

	void WndPosRestore::OnCreate(HWND hwnd){
		Message msg;
		msg.hWnd = hwnd;
		_OnCreate(&msg);
	}
	void WndPosRestore::OnDestroy(HWND hwnd){
		Message msg;
		msg.hWnd = hwnd;
		_OnDestroy(&msg);
	}
	inline WndPosRestore* getAppWndPosRestore(){
		static WndPosRestore wpr;
		return &wpr;
	}
	void WPRRegisterWnd(IWnd* wnd,LPCWSTR name){
		getAppWndPosRestore()->AddWnd(wnd,name);
	}
	void WPRUnRegisterWnd(IWnd* wnd){
		getAppWndPosRestore()->RemoveWnd(wnd);
	}
}