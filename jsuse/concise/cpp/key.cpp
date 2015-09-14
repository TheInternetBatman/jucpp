#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	int hotKeyCount(int count)
	{
		static int _hotKeyCount = 0;
		_hotKeyCount += count;
		return _hotKeyCount;
	}
	Hotkey::Hotkey(){
		_Identity = 0;
	}
	Hotkey::~Hotkey(){
		UnRegister();
	}
	bool Hotkey::_CallBack(MSG* msg){
		if(msg->message==WM_HOTKEY)
		{
			OnPress((int)msg->wParam);
			return true;
		}
		return false;
	}
	bool Hotkey::Register(WORD id,int key,bool shift,bool ctrl,bool alt,bool win){
		if(_Identity) return 0;
		if(id==0) return 0;
		UINT mod = 0;
		if(shift) mod |= MOD_SHIFT;
		if(alt) mod |= MOD_ALT;
		if(ctrl) mod |= MOD_CONTROL;
		if(win) mod |= MOD_WIN;
		if(::RegisterHotKey(0,id,mod,key)){
			_Identity = id;
			Function<bool,MSG*> func;
			func.Bind(this,&Hotkey::_CallBack);
			MsgLoop::AddMsgHook(func);
			return 1;
		}
		return false;
	}
	bool Hotkey::UnRegister()
	{
		if(_Identity==0) return 0;
		if(!::UnregisterHotKey(0,_Identity)) return 0;
		Function<bool,MSG*> cb;
		cb.Bind(this,&Hotkey::_CallBack);
		MsgLoop::DelMsgHook(cb);
		return 1;
	}
	//struct Accel
	bool Accel::Add(short key,short id,bool ctrl,bool alt,bool shift)
	{
		ACCEL acl;
		acl.cmd = id;
		acl.key = key;
		acl.fVirt = FVIRTKEY;
		if(ctrl) acl.fVirt |= FCONTROL;
		if(alt) acl.fVirt |= FALT;
		if(shift) acl.fVirt |= FSHIFT;
		for(uint i=0;i<_AccelList.Count();i++)
		{
			if((_AccelList[i].fVirt==acl.fVirt)&&(_AccelList[i].key==acl.key)) return false;
		}
		_AccelList.Add(acl);
		return true;
	}
	bool Accel::Add(short key,short id,char flag)
	{
		ACCEL acl;
		acl.cmd = id;
		acl.key = key;
		acl.fVirt = flag;
		for(uint i=0;i<_AccelList.Count();i++)
		{
			if((_AccelList[i].fVirt==acl.fVirt)&&(_AccelList[i].key==acl.key)) return false;
		}
		_AccelList.Add(acl);
		return true;
	}
	bool Accel::Delete(short key,short id,char flag)
	{
		ACCEL acl;
		acl.cmd = id;
		acl.key = key;
		acl.fVirt = flag;
		return _AccelList.DeleteValue(acl);
	}
	bool Accel::Delete(int index)
	{
		return _AccelList.Delete(index)!=0;  
	}
	//class AcceleratorTable
	bool AccelTable::Create(LPACCEL pAcl,int count)
	{
		_Handle = ::CreateAcceleratorTable(pAcl,count);
		return _Handle!=0;
	}
	bool AccelTable::Create(Accel& acl)
	{
		_Handle = ::CreateAcceleratorTable(acl,acl.Count());
		return _Handle!=0;
	}
	bool AccelTable::Start()
	{
		Function<bool,MSG*> func;
		func.Bind(this,&AccelTable::_CallBack);
		return MsgLoop::AddMsgHook(func);
	}
	bool AccelTable::Stop()
	{
		Function<bool,MSG*> cb;
		cb.Bind(this,&AccelTable::_CallBack);
		return MsgLoop::DelMsgHook(cb);
	}
	bool AccelTable::LoadResource(ResID res)
	{
		_Handle = ::LoadAccelerators(res.Instance,res);
		return _Handle!=0;
	}
	bool AccelTable::_CallBack(MSG* msg)
	{
		return ::TranslateAccelerator(msg->hwnd,_Handle,msg)!=0;
	}
}
