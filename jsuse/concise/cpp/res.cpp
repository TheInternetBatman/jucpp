#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	//class ResName
	void ResName::operator = (ResID name)
	{
		if(IsHandle(_Handle)) MemoryFree((void*)_Handle);
		if(IsHandle(name.Handle))
		{
			int len = WcsLength(name.Handle) + 1;
			_Handle = (LPCWSTR)MemoryAlloc(len*2);
			memcpy((void*)_Handle,name.Handle,len*2);
		}
		else _Handle = name;
	}
	ResName::~ResName()
	{
		if(IsHandle(_Handle)) MemoryFree((void*)_Handle);
	}
	void* GetResource(ResID name,ResID type,DWORD* size){
		HMODULE module = (HMODULE)name.Instance;
		HRSRC hr = ::FindResource(module,name,type);
		if(size) *size = SizeofResource(module,hr);
		HGLOBAL hg = ::LoadResource(module,hr);
		return ::LockResource(hg);
	}
	//class Menu
	Menu::Menu()
	{
	}
	Menu::~Menu()
	{
		if(_Handle)
			::DestroyMenu(_Handle);
	}
	bool Menu::Create(bool popup)
	{
		if(_Handle) if(!::DestroyMenu(_Handle)) return 0;
		if(popup) _Handle = ::CreatePopupMenu();
		else _Handle = ::CreateMenu();
		return 1;
	}
	void Menu::Destroy()
	{
		if(_Handle) ::DestroyMenu(_Handle);
		_Handle = 0;
	}
	void Menu::Attach(HMENU menu)
	{
		if(_Handle) ::DestroyMenu(_Handle);//某些时候菜单是不能被销毁的.
		_Handle = menu;
	}
	bool Menu::Popup(LPARAM lParam,HWND hOwner,UINT tpm)
	{
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(_Handle,tpm,LOWORD(lParam),HIWORD(lParam),0,hOwner,0)!=0;
	}
	bool Menu::Popup(HWND hOwner,UINT tpm)
	{
		POINT pt; 
		::GetCursorPos(&pt);
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(_Handle,tpm,pt.x,pt.y,0,hOwner,0)!=0;
	}
	bool Menu::PopupSubMenu(UINT uPos,HWND hOwner,UINT tpm)
	{
		POINT pt; 
		::GetCursorPos(&pt);
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(::GetSubMenu(_Handle,uPos),tpm,pt.x,pt.y,0,hOwner,0)!=0;
	}
	bool Menu::Popup(short x,short y,HWND hOwner,UINT tpm)
	{
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(_Handle,tpm,x,y,0,hOwner,0)!=0;
	}
	bool Menu::PopSubMenu(UINT uPos,LPARAM lParam,HWND hOwner,UINT tpm)
	{
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(::GetSubMenu(_Handle,uPos),tpm,LOWORD(lParam),HIWORD(lParam),0,hOwner,0)!=0;
	}
	bool Menu::PopSubMenu(UINT uPos,short x,short y,HWND hOwner,UINT tpm)
	{
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(::GetSubMenu(_Handle,uPos),tpm,x,y,0,hOwner,0)!=0;
	}
	bool Menu::AddItem(UINT uid,LPCWSTR text,UINT id,UINT IndexFlag)
	{
		AutoStruct(MENUITEMINFO,mii);
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_TYPE|MIIM_STATE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)text;
		mii.wID = id;
		//::GetMenuItemCount(_Handle);
		return ::InsertMenuItem(_Handle,uid,IndexFlag,&mii)!=0; 
	}
	HMENU Menu::AddSubMenu(UINT uid,LPCWSTR text,HMENU sub,UINT id,UINT IndexFlag)
	{
		AutoStruct(MENUITEMINFO,mii);
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_DATA|MIIM_SUBMENU|MIIM_TYPE|MIIM_STATE;
		mii.fType = MFT_STRING;
		if(id)
			mii.fMask |= MIIM_ID;
		mii.dwTypeData = (LPWSTR)text;
		mii.wID = id;
		HMENU h = 0;
		if(sub==0)
		{
			h = ::CreatePopupMenu();
			sub = h;
		}
		mii.hSubMenu = sub;
		if(::InsertMenuItemW(_Handle,uid,IndexFlag,&mii)==0)
		{
			::DestroyMenu(h);
			return 0;
		}
		return sub;
	}
	bool Menu::AddSplit(UINT uid,UINT IndexFlag)
	{
		AutoStruct(MENUITEMINFO,mii);
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_SEPARATOR;
		return ::InsertMenuItemW(_Handle,uid,IndexFlag,&mii)!=0; 
	}
	bool Menu::DeleteItem(UINT uid,UINT IndexFlag)
	{
		return ::DeleteMenu(_Handle,uid,IndexFlag)!=0;
	}
	bool Menu::RemoveItem(UINT uid,UINT IndexFlag)
	{
		return ::RemoveMenu(_Handle,uid,IndexFlag)!=0;
	}
	bool Menu::Load(ResID rMenu)
	{
		HMENU hMenu = ::LoadMenuW(rMenu.Instance,rMenu);
		Attach(hMenu);
		return hMenu!=0;
	}
	bool Menu::BindFromWindow(HWND hWnd)
	{
		HMENU hMenu = ::GetMenu(hWnd);
		Attach(hMenu);
		return hMenu!=0;
	}
	bool Menu::BindToWindow(HWND hWnd)
	{
		HMENU h = ::GetMenu(hWnd);
		if(!::SetMenu(hWnd,_Handle)) return 0;
		::DestroyMenu(h);
		return 1;
	}
	bool Menu::SelectWindow(HWND hWnd)
	{
		HMENU h = ::GetMenu(hWnd);
		if(!::SetMenu(hWnd,_Handle)) return 0;
		_Handle = h;
		return 1;
	}
	HMENU Menu::GetSubMenu(UINT uid)
	{
		AutoStruct(MENUITEMINFO,mii);
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_SUBMENU;
		::GetMenuItemInfo(_Handle,uid,MF_BYPOSITION,&mii);
		return mii.hSubMenu;
	}
	int Menu::GetItemCount()
	{
		return ::GetMenuItemCount(_Handle);
	}
	bool Menu::SetItemCheck(UINT uid,bool check,UINT IndexFlag)
	{
		UINT state = IndexFlag;
		if(check)
			state |= MF_CHECKED;
		return ::CheckMenuItem(_Handle,uid,state)!=-1;
	}
	bool Menu::GetItemCheck(UINT uid,UINT IndexFlag)
	{
		return (::GetMenuState(_Handle,uid,IndexFlag)&MF_CHECKED)==MF_CHECKED;
	}
	bool Menu::SetItemDisable(UINT uid,bool disable,UINT IndexFlag)
	{
		UINT state = IndexFlag;
		if(disable) state |= MF_DISABLED|MF_GRAYED;
		return ::EnableMenuItem(_Handle,uid,state)!=-1;
	}
	bool Menu::GetItemDisable(UINT uid,UINT IndexFlag)
	{
		return (::GetMenuState(_Handle,uid,IndexFlag)&MF_DISABLED)==MF_DISABLED;
	}
	bool Menu::SetItemState(UINT uid,UINT state,UINT IndexFlag)
	{
		AutoStruct(MENUITEMINFO,mii);
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_STATE;
		mii.fState = state;
		return ::SetMenuItemInfo(_Handle,uid,IndexFlag,&mii)!=0;
	}
	UINT Menu::GetItemState(UINT uid,UINT IndexFlag)
	{
		return ::GetMenuState(_Handle,uid,IndexFlag);
	} 
	bool Menu::SetItemText(UINT uid,LPCWSTR text,UINT IndexFlag)
	{
		AutoStruct(MENUITEMINFO,mii);
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = (LPWSTR)text;
		return ::SetMenuItemInfo(_Handle,uid,IndexFlag,&mii)!=0;
	}
	int Menu::GetItemText(UINT pos,String& text,UINT IndexFlag)
	{
		int len = ::GetMenuString(_Handle,pos,0,0,IndexFlag)+1;
		if(len<=1)//if menu string is a null string return value still is 1.
			return 0;
		if(text.Cubage()<(uint)len)
			text.SetCubage(len);
		return ::GetMenuStringW(_Handle,pos,text.Handle(),len,IndexFlag);
	}
	bool Menu::SetItemID(UINT uid,DWORD id,UINT IndexFlag)
	{
		AutoStruct(MENUITEMINFO,mii);
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_ID;
		mii.wID = id;
		return ::SetMenuItemInfo(_Handle,uid,IndexFlag,&mii)!=0;
	}
	UINT Menu::GetItemID(int index)
	{
		return ::GetMenuItemID(_Handle,index);
	}
}
