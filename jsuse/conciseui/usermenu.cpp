#include "stdafx.h"
#include "../conciseui.h"

namespace cs{
	///////////////////////
	UserMenu::UserMenu():_parent(0),ImageList(0){
		;
	}
	UserMenu::~UserMenu(){
		if(_Handle)
			::DestroyMenu(_Handle);
	}
	bool UserMenu::Create(bool popup){
		if(_Handle) if(!::DestroyMenu(_Handle)) return 0;
		if(popup) _Handle = ::CreatePopupMenu();
		else _Handle = ::CreateMenu();
		return 1;
	}
	void UserMenu::Destroy(){
		if(_Handle) ::DestroyMenu(_Handle);
		_Handle = 0;
	}
	void UserMenu::Attach(HMENU menu){
		if(_Handle) ::DestroyMenu(_Handle);//某些时候菜单是不能被销毁的.
		_Handle = menu;
	}
	bool UserMenu::Popup(LPARAM lParam,HWND hOwner,UINT tpm){
		if(_parent)
			hOwner = _parent->Handle();
		if(hOwner==0) return 0;
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(_Handle,tpm,LOWORD(lParam),HIWORD(lParam),0,hOwner,0)!=0;
	}
	bool UserMenu::Popup(HWND hOwner,UINT tpm){
		if(_parent)
			hOwner = _parent->Handle();
		if(hOwner==0) return 0;
		POINT pt; 
		::GetCursorPos(&pt);
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(_Handle,tpm,pt.x,pt.y,0,hOwner,0)!=0;
	}
	bool UserMenu::PopupSubMenu(UINT uPos,HWND hOwner,UINT tpm){
		if(_parent)
			hOwner = _parent->Handle();
		if(hOwner==0) return 0;
		POINT pt; 
		::GetCursorPos(&pt);
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(::GetSubMenu(_Handle,uPos),tpm,pt.x,pt.y,0,hOwner,0)!=0;
	}
	bool UserMenu::Popup(short x,short y,HWND hOwner,UINT tpm){
		if(_parent)
			hOwner = _parent->Handle();
		if(hOwner==0) return 0;
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(_Handle,tpm,x,y,0,hOwner,0)!=0;
	}
	bool UserMenu::PopSubMenu(UINT uPos,LPARAM lParam,HWND hOwner,UINT tpm){
		if(_parent)
			hOwner = _parent->Handle();
		if(hOwner==0) return 0;
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(::GetSubMenu(_Handle,uPos),tpm,LOWORD(lParam),HIWORD(lParam),0,hOwner,0)!=0;
	}
	bool UserMenu::PopSubMenu(UINT uPos,short x,short y,HWND hOwner,UINT tpm){
		if(_parent)
			hOwner = _parent->Handle();
		if(hOwner==0) return 0;
		::SetForegroundWindow(hOwner);
		return ::TrackPopupMenu(::GetSubMenu(_Handle,uPos),tpm,x,y,0,hOwner,0)!=0;
	}
	bool UserMenu::AddItem(UINT uPos,LPCWSTR text,UINT msgId,HBITMAP bmp,bool byPos){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_DATA|MIIM_ID|MIIM_FTYPE|MIIM_STRING;
		mii.fType = MFT_STRING|MFT_OWNERDRAW;
		mii.dwTypeData = (LPWSTR)text;
		mii.wID = msgId;
		mii.dwItemData = (ULONG_PTR)bmp;

		return ::InsertMenuItem(_Handle,uPos,byPos,&mii)!=0; 
	}
	void setMenuOwner(HMENU hMenu){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_TYPE;
		int count = ::GetMenuItemCount(hMenu);
		for(int i=0;i<count;i++){
			GetMenuItemInfo(hMenu,i,1,&mii);
			mii.fType |= MFT_OWNERDRAW;
			SetMenuItemInfo(hMenu,i,1,&mii);
			HMENU sub = GetSubMenu(hMenu,i);
			if(sub) setMenuOwner(sub);
		}
	}
	void UserMenu::ToOwnerStyle(){
		setMenuOwner(_Handle);
	}

	HMENU UserMenu::AddSubMenu(UINT uPos,LPCWSTR text,HMENU sub,HBITMAP image,bool byPos){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_DATA|MIIM_SUBMENU|MIIM_FTYPE|MIIM_STRING|MIIM_ID;
		mii.fType = MFT_STRING|MFT_OWNERDRAW;
		mii.dwTypeData = (LPWSTR)text;
		mii.dwItemData = (ULONG_PTR)image;
		static int subMenuId = 0x10000;
		mii.wID = subMenuId++;
		HMENU h = 0;
		if(sub==0){
			sub = h = ::CreatePopupMenu();
		}
		mii.hSubMenu = sub;
		if(::InsertMenuItem(_Handle,uPos,byPos,&mii)==0){
			if(h) ::DestroyMenu(h);
			return 0;
		}
		return sub;
	}
	bool UserMenu::AddSplit(UINT uid,bool byPos){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_SEPARATOR;
		return ::InsertMenuItemW(_Handle,uid,byPos,&mii)!=0; 
	}
	bool UserMenu::DeleteItem(UINT uid,bool byPos){
		return ::DeleteMenu(_Handle,uid,byPos)!=0;
	}
	bool UserMenu::RemoveItem(UINT uid,bool byPos){
		return ::RemoveMenu(_Handle,uid,byPos)!=0;
	}
	bool UserMenu::Load(ResID rMenu){
		HMENU hMenu = ::LoadMenuW(rMenu.Instance,rMenu);
		Attach(hMenu);
		return hMenu!=0;
	}
	bool UserMenu::BindFromWindow(HWND hWnd){
		HMENU hMenu = ::GetMenu(hWnd);
		Attach(hMenu);
		return hMenu!=0;
	}
	bool UserMenu::BindToWindow(HWND hWnd){
		HMENU h = ::GetMenu(hWnd);
		if(!::SetMenu(hWnd,_Handle)) return 0;
		::DestroyMenu(h);
		return 1;
	}
	bool UserMenu::SelectWindow(HWND hWnd){
		HMENU h = ::GetMenu(hWnd);
		if(!::SetMenu(hWnd,_Handle)) return 0;
		_Handle = h;
		return 1;
	}
	HMENU UserMenu::GetSubMenu(UINT uid){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_SUBMENU;
		::GetMenuItemInfo(_Handle,uid,MF_BYPOSITION,&mii);
		return mii.hSubMenu;
	}
	int UserMenu::GetItemCount(){
		return ::GetMenuItemCount(_Handle);
	}
	bool UserMenu::SetItemCheck(UINT uid,bool check,bool byPos){
		UINT state = byPos?MF_BYPOSITION:MF_BYCOMMAND;
		if(check)
			state |= MF_CHECKED;
		else
			state |= MF_UNCHECKED;
		return ::CheckMenuItem(_Handle,uid,state)!=-1;
	}
	bool UserMenu::GetItemCheck(UINT uid,bool byPos){
		return (::GetMenuState(_Handle,uid,byPos?MF_BYPOSITION:MF_BYCOMMAND)&MF_CHECKED)==MF_CHECKED;
	}
	bool UserMenu::SetItemDisable(UINT uid,bool disable,bool byPos){
		UINT state = byPos?MF_BYPOSITION:MF_BYCOMMAND;
		if(disable) state |= MF_DISABLED|MF_GRAYED;
		return ::EnableMenuItem(_Handle,uid,state)!=-1;
	}
	bool UserMenu::GetItemDisable(UINT uid,bool byPos){
		return (::GetMenuState(_Handle,uid,byPos)&MF_DISABLED)==MF_DISABLED;
	}
	bool UserMenu::SetItemState(UINT uid,UINT state,bool byPos){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_STATE;
		mii.fState = state;
		return ::SetMenuItemInfo(_Handle,uid,byPos,&mii)!=0;
	}
	UINT UserMenu::GetItemState(UINT uid,bool byPos){
		return ::GetMenuState(_Handle,uid,byPos?MF_BYPOSITION:MF_BYCOMMAND);
	} 
	bool UserMenu::SetItemText(UINT uid,LPCWSTR text,bool byPos){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = (LPWSTR)text;
		return ::SetMenuItemInfo(_Handle,uid,byPos,&mii)!=0;
	}
	int UserMenu::GetItemText(UINT pos,String& text,bool byPos){
		uint flags = byPos?MF_BYPOSITION:MF_BYCOMMAND;
		int len = ::GetMenuString(_Handle,pos,0,0,flags);
		if(!len) return 0;
		if(text.Cubage()<(uint)len)
			text.SetCubage(len);
		len = ::GetMenuString(_Handle,pos,text.Handle(),len+1,flags);
		text.Realize();
		return len;
	}
	bool UserMenu::SetItemType(UINT uPos,UINT mft,bool byPos){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_TYPE;
		mii.fType = mft;
		return ::SetMenuItemInfo(_Handle,uPos,byPos,&mii)!=0;
	}
	UINT UserMenu::GetItemType(UINT uPos,bool byPos){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_TYPE;
		::GetMenuItemInfo(_Handle,uPos,byPos,&mii);
		return mii.fType;
	}
	bool UserMenu::SetItemID(UINT uid,DWORD id,bool byPos){
		AutoStruct2(MENUITEMINFO,mii);
		mii.fMask = MIIM_ID;
		mii.wID = id;
		return ::SetMenuItemInfo(_Handle,uid,byPos,&mii)!=0;
	}
	UINT UserMenu::GetItemID(int index){
		return ::GetMenuItemID(_Handle,index);
	}
	void UserMenu::HookMeasureItem(Message* msg,IWnd*){
		LPMEASUREITEMSTRUCT mi = (LPMEASUREITEMSTRUCT)msg->lParam;
		if(mi->CtlType!=ODT_MENU) return;
		if(!OnMeasureItem.IsNull()) OnMeasureItem(mi);
		else{
			//测试此消息是否是这个菜单发出
			AutoStruct2(MENUITEMINFO,mii);
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_TYPE;
			bool b = GetItemInfo(mi->itemID,&mii,false);
			if(!b) return;

			mi->itemHeight = Param.ItemHeight;
			String text;
			this->GetItemText(mi->itemID,text,false);
			text.Realize();
			SIZE sz = CalcTextSize(0,text,Param.Font);
			mi->itemWidth = sz.cx + Param.LeftMargin;
			if(Param._TextCY==0&&sz.cy!=0){
				Param._TextCY = (Param.ItemHeight-sz.cy)/2;
			}
		}
	}
#define HAS_STYLE(v,s) (v&s)==s
	void UserMenu::HookDrawItem(Message* msg,IWnd*){
		LPDRAWITEMSTRUCT di = (LPDRAWITEMSTRUCT)msg->lParam;
		if(di->CtlType!=ODT_MENU) return;
		if(!OnDrawItem.IsNull()) OnDrawItem(di);
		else{
			//测试此消息是否是这个菜单发出
			AutoStruct2(MENUITEMINFO,mii);
			mii.fMask = MIIM_TYPE;
			bool b = GetItemInfo(di->itemID,&mii,false);
			if(!b) return;

			int color = Param.TextColor;
			Rect& rc = *(Rect*)&di->rcItem;
			if(HAS_STYLE(di->itemState,ODS_HOTLIGHT)){
			}
			if(HAS_STYLE(di->itemState,ODS_DISABLED)){
				color = RGB(200,200,200);
			}else if(HAS_STYLE(di->itemState,ODS_SELECTED)){
				GdiSelector gsb(di->hDC,Param.Brush);
				::SetDCBrushColor(di->hDC,Param.SelectBkColor);
				GdiSelector gsp(di->hDC,Param.Pen);
				::SetDCPenColor(di->hDC,Param.SelectBorderColor);
				Rectangle(di->hDC,rc.left,rc.top,rc.right,rc.bottom);
				color = RGB(255,0,0);
			}else{
				GdiSelector gsb(di->hDC,Param.Brush);
				::SetDCBrushColor(di->hDC,Param.DefaultBkColor);
				gsb.Close();
				::FillRect(di->hDC,rc,Param.Brush);
			}
			if(ImageList){
				HBITMAP icon = HAS_STYLE(di->itemState,ODS_CHECKED)?Param.BmpCheck:(HBITMAP)di->itemData;
				ImageList->Draw(icon,di->hDC,255,1,rc.left+Param.ImageX,rc.top+Param.ImageY);
			}
			SetTextColor(di->hDC,color);
			String text;
			GetItemText(di->itemID,text,false);
			text.Realize();
			SetBkMode(di->hDC,TRANSPARENT);
			GdiSelector gs(di->hDC,Param.Font);
			TextOut(di->hDC,rc.left+Param.LeftMargin,rc.top+Param._TextCY,text,text.Length());
		}
	}
	void UserMenu::SetOwnerWnd(IWnd* parent){
		if(_parent){
			_parent->GetMsgHook(WM_MEASUREITEM)->Delete(this,&UserMenu::HookMeasureItem);
			_parent->GetMsgHook(WM_DRAWITEM)->Delete(this,&UserMenu::HookDrawItem);
		}
		if(parent){
			parent->GetMsgHook(WM_MEASUREITEM)->Add(this,&UserMenu::HookMeasureItem);
			parent->GetMsgHook(WM_DRAWITEM)->Add(this,&UserMenu::HookDrawItem);
		}
		_parent = parent;
	}
	bool UserMenu::SetItemInfo(UINT uid,LPMENUITEMINFO mii,bool byPos){
		return ::SetMenuItemInfo(_Handle,uid,byPos,mii)!=0;
	}
	bool UserMenu::GetItemInfo(UINT uid,LPMENUITEMINFO mii,bool byPos){
		return ::GetMenuItemInfo(_Handle,uid,byPos,mii)!=0;
	}
	USERMENUPARAM::USERMENUPARAM(){
		BmpCheck = 0;
		ImageX = 10;
		ImageY = 5;
		LeftMargin = 36;
		_TextCY = 0;
		ItemHeight = 26;
		TextColor = 0;
		SelectBorderColor = RGB(0,200,200);
		SelectBkColor = RGB(185,213,238);
		DefaultBkColor = GetSysColor(COLOR_MENU);
		Brush = (HBRUSH)GetStockObject(DC_BRUSH);
		Pen = (HPEN)GetStockObject(DC_PEN);
	}
}