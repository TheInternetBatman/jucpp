#include "stdafx.h"
#include "../conciseui.h"
#include <commctrl.h>


namespace cs{
	//class Edit
	void EditPtr::ShowToolTip(LPCWSTR tip,LPCWSTR title,int tti)
	{
		EDITBALLOONTIP ebt;
		ebt.cbStruct = sizeof(ebt);
		ebt.ttiIcon = tti;
		ebt.pszText = tip;
		ebt.pszTitle = title;
		Edit_ShowBalloonTip(_Handle,&ebt);
	}
	void EditPtr::HideToolTip()
	{
		Edit_HideBalloonTip(_Handle);
	}
	//class Lable
	//class Button
	bool ButtonPtr::SetImageList(HIMAGELIST hImgl,UINT align,LPRECT margin)
	{
		BUTTON_IMAGELIST bi;
		bi.himl = hImgl;
		bi.uAlign = align;
		if(margin)
			::SetRect(&bi.margin,margin->left,margin->top,margin->right,margin->bottom);
		else
			::SetRect(&bi.margin,1,1,1,1);
		return SendMessage(BCM_SETIMAGELIST,0,&bi)!=0;
	}
	//class MonthCalendar
	//class DateTimePicker
	//class ListBox
	int ListBoxPtr::AddString(LPCWSTR str,int index){
		UINT msgId;
		if(index<0) msgId = LB_ADDSTRING;
		else msgId = LB_INSERTSTRING;
		return (int)SendMessage(msgId,(WPARAM)index,(LPARAM)str);
	}
	int ListBoxPtr::GetText(int index,String& buf){
		int len = GetTextLength(index);
		buf.SetCubage(len);
		int r = (int)SendMessage(LB_GETTEXT,(WPARAM)index,(LPARAM)buf.Handle());
		buf.Realize();
		return r;
	}
	bool ListBoxPtr::SetText(int index,LPCWSTR str){
		int n = GetCount();
		if(index<0||index>=n){
			return 0;
		}
		index = AddString(str,index);
		if(index<0) return 0;
		int i = GetSelItem();
		if(i==(index+1)){
			SetSelItem(index);
		}
		DeleteString(index+1);
		return 1;
	}

	//class ComboBox
	bool ComboBoxPtr::GetItemText(int index,String& str){
		str = L"";
		int len = (int)SendMessage(CB_GETLBTEXTLEN,index,0);
		if(len==CB_ERR) return 0;
		if((int)str.Cubage()<len) str.SetCubage(len);
		if(SendMessage(CB_GETLBTEXT,(WPARAM)index,(LPARAM)str.Handle())==-1) 
			return 0;
		str.Realize();
		return 1;
	}
	//class ComboBoxEx
	int ComboBoxExPtr::AddItem(LPCWSTR str,int image,int indent,int index,int selImage,LPARAM lParam)
	{
		AutoStruct(COMBOBOXEXITEM,cbi);
		cbi.iOverlay = 0;
		cbi.cchTextMax = 0;
		cbi.mask = CBEIF_INDENT|CBEIF_TEXT;
		if(image!=-1)
		{
			cbi.mask |= CBEIF_IMAGE|CBEIF_SELECTEDIMAGE;
			cbi.iImage = image;
			if(selImage!=-1)
				cbi.iSelectedImage = selImage;
			else
				cbi.iSelectedImage = image;
		}
		cbi.iIndent = indent;
		cbi.iItem = index;
		cbi.pszText = (LPWSTR)str;
		cbi.lParam = lParam;
		return (int)SendMessage(CBEM_INSERTITEM,0,(LPARAM)&cbi);
	}
	bool ComboBoxExPtr::SetText(int index,LPCWSTR text)
	{
		AutoStruct(COMBOBOXEXITEM,cbi);
		cbi.mask = CBEIF_TEXT;
		cbi.pszText = (LPWSTR)text;
		cbi.iItem = index;
		return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)&cbi)!=0;
	}
	bool ComboBoxExPtr::GetText(int index,String& text)
	{
		AutoStruct(COMBOBOXEXITEM,cbi);
		cbi.mask = CBEIF_TEXT;
		cbi.pszText = text;
		cbi.cchTextMax = text.Cubage() + 1;
		cbi.iItem = index;
		if(SendMessage(CBEM_GETITEM,0,(LPARAM)&cbi)==0) return 0;
		if(cbi.pszText!=text.Handle()) text = cbi.pszText;
		text.Realize();
		return 1;
	}
	int ComboBoxExPtr::GetIndent(int index)
	{
		AutoStruct(COMBOBOXEXITEM,cbi);
		cbi.mask = CBEIF_INDENT;
		cbi.iItem = index;
		if(SendMessage(CBEM_GETITEM,0,(LPARAM)&cbi)==0) return 0;
		return cbi.iIndent;
	}
	bool ComboBoxExPtr::SetIndent(int index,int indent)
	{
		AutoStruct(COMBOBOXEXITEM,cbi);
		cbi.mask = CBEIF_INDENT;
		cbi.iIndent = indent;
		return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)&cbi)!=0;
	}
	int ComboBoxExPtr::GetImage(int index,int flag)
	{
		AutoStruct(COMBOBOXEXITEM,cbi);
		if((index<0)||(index>2))
			return -1;
		cbi.mask = 2 << flag;
		cbi.iItem = index;
		if(SendMessage(CBEM_GETITEM,0,(LPARAM)&cbi)==0)
			return -1;
		switch(flag)
		{
		case 0:
			return cbi.iImage;
		case 1:
			return cbi.iSelectedImage;
		case 2:
			return cbi.iOverlay;
		}
		return -1;
	}
	bool ComboBoxExPtr::SetImage(int index,int image,int flag)
	{
		AutoStruct(COMBOBOXEXITEM,cbi);
		if((index<0)||(index>2))
			return false;
		cbi.mask = 2 << flag;
		cbi.iItem = index;
		switch(flag)
		{
		case 0:
			cbi.iImage = image;
			break;
		case 1:
			cbi.iSelectedImage = image;
			break;
		case 2:
			cbi.iOverlay = image;
			break;
		}
		return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)&cbi)!=0;
	}
	//class TreeView
	bool TreeViewPtr::GetItemData(HTREEITEM hti,LPARAM* pParam)
	{
		AutoStruct(TVITEMEX,tvi);
		tvi.mask = TVIF_PARAM;
		tvi.hItem = hti;
		if(!TreeView_GetItem(_Handle,&tvi)) return 0;
		*pParam = tvi.lParam;
		return 1;
	}
	bool TreeViewPtr::SetItemData(HTREEITEM hti,LPARAM data)
	{
		AutoStruct(TVITEMEX,tvi);
		tvi.mask = TVIF_PARAM;
		tvi.hItem = hti;
		tvi.lParam = data;
		return TreeView_SetItem(_Handle,&tvi)!=0;
	}
	HTREEITEM TreeViewPtr::AddItem(HTREEITEM parent,LPCWSTR text,int imgIndex,int selIndex,HTREEITEM insAfter)
	{
		TVINSERTSTRUCT ti;
		::ZeroMemory(&ti,sizeof(ti));
		ti.hInsertAfter = insAfter;
		ti.hParent = parent;
		ti.itemex.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT;
		ti.itemex.iImage = imgIndex;
		ti.itemex.iSelectedImage = selIndex;
		ti.itemex.pszText = (LPWSTR)text;
		return TreeView_InsertItem(_Handle,&ti);
	}
	int TreeViewPtr::GetItemText(HTREEITEM hti,String& str)
	{
		AutoStruct(TVITEMEX,tvi);
		tvi.mask = TVIF_TEXT;
		tvi.hItem = hti;
		tvi.pszText = str.Handle();
		tvi.cchTextMax = str.Cubage() + 1;
		if(!TreeView_GetItem(_Handle,&tvi))
			return 0;
		str.Realize();
		return str.Length();
	}
	bool TreeViewPtr::SetItemText(HTREEITEM hti,LPCWSTR str)
	{
		AutoStruct(TVITEMEX,tvi);
		tvi.mask = TVIF_TEXT;
		tvi.hItem = hti;
		tvi.pszText = (LPWSTR)str;
		return TreeView_SetItem(_Handle,&tvi)!=0;
	}
	int TreeViewPtr::GetImageIndex(HTREEITEM hti,UINT flag)
	{
		AutoStruct(TVITEMEX,tvi);
		if(flag)
		{
			tvi.mask = TVIF_SELECTEDIMAGE;
			tvi.iSelectedImage = -1;
		}
		else
		{
			tvi.mask = TVIF_IMAGE;
			tvi.iImage = -1;
		}
		tvi.hItem = hti;
		TreeView_GetItem(_Handle,&tvi);
		if(flag) return tvi.iSelectedImage;
		else return tvi.iImage;
	}
	bool TreeViewPtr::SetImageIndex(HTREEITEM hti,int image,int selImage)
	{
		AutoStruct(TVITEMEX,tvi);
		if(selImage>=0)
		{
			tvi.mask |= TVIF_SELECTEDIMAGE;
			tvi.iSelectedImage = selImage;
		}
		if(image>=0)
		{
			tvi.mask |= TVIF_IMAGE;
			tvi.iImage = image;
		}
		tvi.hItem = hti;
		return TreeView_SetItem(_Handle,&tvi)!=0;
	}
	UINT ToolBarPtr::SetExStyle(UINT exStyle,DWORD opt)
	{
		if(opt==0)
		{
			return (UINT)SendMessage(TB_SETEXTENDEDSTYLE,0,exStyle);
		}
		UINT exs = (UINT)SendMessage(TB_GETEXTENDEDSTYLE,0,0);
		exs |= exStyle;
		if(opt==1) exs -= exStyle;
		return (UINT)SendMessage(TB_SETEXTENDEDSTYLE,0,exs);
	}
	UINT ToolBarPtr::SetStyle(UINT style,DWORD opt)
	{
		if(opt==2)
		{
			return (UINT)SendMessage(TB_SETSTYLE,0,style);
		}
		UINT exs = (UINT)SendMessage(TB_GETSTYLE,0,0);
		exs |= style;
		if(opt==1) exs -= style;
		return (UINT)SendMessage(TB_SETSTYLE,0,exs);
	}
	bool ToolBarPtr::AddSplit(int index)
	{
		AutoStruct(TBBUTTON,tb);
		tb.fsStyle = TBSTYLE_SEP;
		tb.idCommand = 0x10000;
		return (int)SendMessage(TB_INSERTBUTTON,index,(LPARAM)&tb)!=0;
	}
	bool ToolBarPtr::AddButton(WORD cmdID,int image,LPCWSTR text,BYTE style,BYTE state,int index)
	{
		AutoStruct(TBBUTTON,tb);
		tb.iBitmap = image;
		tb.fsState = state;
		tb.fsStyle = style;
		tb.iString = (INT_PTR)text;
		tb.idCommand = cmdID;
		tb.dwData = 0;
		return (int)SendMessage(TB_INSERTBUTTON,index,(LPARAM)&tb)!=0;
	}
	bool ToolBarPtr::SetButtonState(int id,int state,DWORD option)
	{
		int ts;
		switch(option)
		{
		case 0:
			break;
		case 1:
			state |= GetButtonState(id);
			break;
		case 2:
			ts = state;
			state |= GetButtonState(id);
			state -= ts;
			break;
		}
		return (int)SendMessage(TB_SETSTATE,id,state)!=0;
	}
	HIMAGELIST ToolBarPtr::SetImageList(int it,HIMAGELIST hImgl)
	{
		if(it==0)
			return (HIMAGELIST)SendMessage(TB_SETIMAGELIST,0,(LPARAM)hImgl);
		else if(it==1)
			return (HIMAGELIST)SendMessage(TB_SETDISABLEDIMAGELIST,0,(LPARAM)hImgl);
		else if(it==2)
			return (HIMAGELIST)SendMessage(TB_SETHOTIMAGELIST,0,(LPARAM)hImgl);
		return 0;
	}
	HIMAGELIST ToolBarPtr::GetImageList(int type)
	{
		UINT msg;
		switch(type)
		{
		case 0:
			msg = TB_GETIMAGELIST;
			break;
		case 1:
			msg = TB_GETDISABLEDIMAGELIST;
			break;
		case 2:
			msg = TB_GETHOTIMAGELIST;
			break;
		default:
			return 0;
		}
		return (HIMAGELIST)SendMessage(msg,0,0);
	}
	bool ToolBarPtr::LoadBitmap(int it,int cx,COLORREF mask,ResID res,bool delpre)
	{
		UINT lr = LR_DEFAULTCOLOR|LR_DEFAULTSIZE|LR_CREATEDIBSECTION;
		if(res.Instance==0)
			lr |= LR_LOADFROMFILE;
		HIMAGELIST hImgl = ::ImageList_LoadImage(res.Instance,res,cx,16,mask,IMAGE_BITMAP,lr),preImgl;
		if(hImgl==0)
			return 0;
		UINT msg,get;
		switch(it)
		{
		case 0:
			msg = TB_SETIMAGELIST;
			get = TB_GETIMAGELIST;
			break;
		case 1:
			msg = TB_SETDISABLEDIMAGELIST;
			get = TB_GETDISABLEDIMAGELIST;
			break;
		case 2:
			msg = TB_SETHOTIMAGELIST;
			get = TB_GETHOTIMAGELIST;
			break;
		}
		preImgl = (HIMAGELIST)SendMessage(msg,0,(LPARAM)hImgl);
		if((preImgl)&&delpre)
			::ImageList_Destroy(preImgl);
		if(hImgl!=(HIMAGELIST)SendMessage(get,0,0))
		{
			::ImageList_Destroy(hImgl);
			return 0;
		}
		return 1;
	}
	INT_PTR ToolBarPtr::GetButtonInfo(WORD id,DWORD tbif)
	{
		AutoStruct(TBBUTTONINFO,bi);
		bi.cbSize = sizeof(bi);
		bi.dwMask = tbif;
		if(TBIF_TEXT&tbif)
		{
			static String buffer;
			if(tbif&TBIF_BYINDEX)
			{
				bi.dwMask = TBIF_COMMAND|TBIF_BYINDEX;
				if(SendMessage(TB_GETBUTTONINFO,id,&bi)==-1)
					return -1;
				id = (WORD)bi.idCommand;
			}
			uint len = (uint)SendMessage(TB_GETBUTTONTEXT,id,0);
			if(buffer.Cubage()<len)
				buffer.SetCubage(len);
			SendMessage(TB_GETBUTTONTEXT,id,buffer.Handle());
			return (INT_PTR)buffer.Handle();
		}
		if(SendMessage(TB_GETBUTTONINFO,id,&bi)==-1)
			return -1;
		tbif |= TBIF_BYINDEX;
		tbif -= TBIF_BYINDEX;
		switch(tbif)
		{
		case TBIF_COMMAND:
			return bi.idCommand;
		case TBIF_IMAGE:
			return bi.iImage;
		case TBIF_LPARAM:
			return bi.lParam;
		case TBIF_SIZE:
			return bi.cx;
		case TBIF_STATE:
			return bi.fsState;
		case TBIF_STYLE:
			return bi.fsStyle;
		}
		return -1;
	}
	bool ToolBarPtr::SetButtonInfo(WORD id,DWORD tbif,INT_PTR value)
	{
		AutoStruct(TBBUTTONINFO,bi);
		bi.cbSize = sizeof(bi);
		bi.dwMask = tbif;
		tbif |= TBIF_BYINDEX;
		tbif -= TBIF_BYINDEX;
		switch(tbif)
		{
		case TBIF_COMMAND:
			bi.idCommand = (int)value;
			break;
		case TBIF_IMAGE:
			bi.iImage = (int)value;
			break;
		case TBIF_LPARAM:
			bi.lParam = value;
			break;
		case TBIF_SIZE:
			bi.cx = (WORD)value;
			break;
		case TBIF_STATE:
			bi.fsState = (BYTE)value;
			break;
		case TBIF_STYLE:
			bi.fsStyle = (BYTE)value;
			break;
		case TBIF_TEXT:
			bi.pszText = (LPWSTR)value;
			break;
		default:
			return 0;
		}
		return (int)SendMessage(TB_SETBUTTONINFO,id,&bi)!=0;
	}
	bool ToolBarPtr::GetButtonRect(int index,LPRECT lpRect,bool byIndex)
	{
		if(byIndex)
			return (int)SendMessage(TB_GETITEMRECT,index,(LPARAM)lpRect)!=0;
		else
			return (int)SendMessage(TB_GETRECT,index,(LPARAM)lpRect)!=0;
	}
	//class Progress Bar
	//class ToolTip
	bool ToolTipPtr::AddTip(HWND owner,LPCWSTR text,UINT id,LPRECT pr,UINT ttf)
	{
		AutoStruct(TOOLINFO,ti);
		ti.cbSize = sizeof(ti);
		ti.uId = id;
		ti.hwnd = owner;
		ti.hinst = 0;
		if(pr==0)
			::GetClientRect(owner,&ti.rect);
		else
			ti.rect = *pr;
		ti.uFlags = ttf|TTF_SUBCLASS;
		ti.lpszText = (LPWSTR)text;
		return 0!=this->SendMessage(TTM_ADDTOOL,0,&ti);
	}
	void ToolTipPtr::DelTip(HWND owner,UINT uID)
	{
		AutoStruct(TOOLINFO,ti);
		ti.cbSize = sizeof(ti);
		ti.uId = uID;
		ti.hwnd = owner;
		this->SendMessage(TTM_DELTOOL,0,&ti);
	}
	bool ToolTipPtr::SetTipRect(LPRECT pr,HWND owner,UINT id)
	{
		AutoStruct(TOOLINFO,ti);
		ti.cbSize = sizeof(ti);
		ti.hwnd = owner;
		ti.uId = id;
		GetTipInfo(&ti);
		ti.rect = *pr;
		return SetTipInfo(&ti);
	}
	bool ToolTipPtr::SetTipText(LPCWSTR text,HWND owner,UINT uID)
	{
		AutoStruct(TOOLINFO,ti);
		ti.cbSize = sizeof(ti);
		ti.hwnd = owner;
		ti.uId = uID;
		GetTipInfo(&ti);
		ti.lpszText = (LPWSTR)text;
		return SetTipInfo(&ti);
	}
	bool ToolTipPtr::SetTipFlag(UINT ttf,HWND owner,UINT id)
	{
		AutoStruct(TOOLINFO,ti);
		ti.cbSize = sizeof(ti);
		ti.hwnd = owner;
		ti.uId = id;
		GetTipInfo(&ti);
		ti.uFlags = ttf|TTF_SUBCLASS;
		return SetTipInfo(&ti);
	}
	//class TabControl
	bool TabPagePtr::Add(int index,LPWSTR title,int image,LPARAM param)
	{
		TCITEM ti;
		ti.mask = TCIF_IMAGE|TCIF_PARAM|TCIF_TEXT;
		ti.iImage = image;
		ti.lParam = param;
		ti.pszText = title;
		return SendMessage(TCM_INSERTITEM,index,&ti)!=-1;
	}
	//class HotKey
	void HotKeyPtr::SetHotKey(unsigned char comb,unsigned char vk)
	{
		HotkeyParam hk;
		hk.VirtualKey = comb;
		hk.CombineKey = vk;
		SendMessage(HKM_SETHOTKEY,hk.wParam,0);
	}
	HotkeyParam HotKeyPtr::GetHotKey()
	{
		HotkeyParam hk;
		hk.wParam = (WPARAM)SendMessage(HKM_GETHOTKEY,0,0);
		return hk;
	}
	int ListViewPtr::AddColumn(LPCWSTR title,int width,int index)
	{
		AutoStruct(LVCOLUMN,lvc);
		lvc.mask = LVCF_TEXT|LVCF_WIDTH;
		lvc.pszText = (LPWSTR)title;
		lvc.cx = width;
		int count = GetColumnCount();
		if((uint)index>(uint)count) index = count;
		return (int)this->SendMessage(LVM_INSERTCOLUMN,index,&lvc);
	}
	int ListViewPtr::AddItem(LPCWSTR text,int index){
		LVITEM lvi;
		::ZeroMemory(&lvi,sizeof(lvi));
		lvi.mask = LVIF_TEXT;
		lvi.pszText = (LPWSTR)text;
		if(index<0) index = 0xfffffff;
		lvi.iItem = index;
		return (int)this->SendMessage(LVM_INSERTITEM,0,&lvi);
	}
	bool ListViewPtr::SetItemData(int index, INT_PTR data){
		LVITEM lvi;
		::ZeroMemory(&lvi,sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.lParam = data;
		if(index<0) index = 0xfffffff;
		lvi.iItem = index;
		return 0!=this->SendMessage(LVM_SETITEM,0,&lvi);
	}
	bool ListViewPtr::SetItemText(LPCWSTR text, int index,int column)
	{
		AutoStruct(LVITEM,lvi);
		lvi.mask = LVIF_TEXT;
		lvi.pszText = (LPWSTR)text;
		lvi.iItem = index;
		lvi.iSubItem = column;
		return 0!=this->SendMessage(LVM_SETITEMTEXT,index,&lvi);
	}
	int ListViewPtr::GetItemText(String& str,int index,int column)
	{
		AutoStruct(LVITEM,lvi);
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = column;
		str.SetCubage(255);
		str[0] = 0;
		lvi.pszText = str.Handle();
		lvi.cchTextMax = 256;
		int n = (int)this->SendMessage(LVM_GETITEMTEXT,index,&lvi);
		str.Realize();
		return n;
	}
	int ListViewPtr::GetColumnCount()
	{
		HWND header = (HWND)this->SendMessage(LVM_GETHEADER,0,0);
		if(!header) return 0;
		return (int)::SendMessage(header,HDM_GETITEMCOUNT,0,0);
	}
	int HeaderPtr::AdjustSize(HWND parent,int x,int y,int width){
		RECT rcParent; 
		HDLAYOUT hdl; 
		WINDOWPOS wp; 
		GetClientRect(parent,&rcParent); 

		hdl.prc = &rcParent; 
		hdl.pwpos = &wp;
		if(!SendMessage(HDM_LAYOUT, 0, (LPARAM) &hdl))
			return 0; 
		if(width<0) width = wp.cx;
		SetWindowPos(_Handle,wp.hwndInsertAfter, wp.x+x, wp.y+y,width, wp.cy, wp.flags | SWP_SHOWWINDOW); 
		return wp.cy;
	}
	int HeaderPtr::AddItem(LPCWSTR text,int width,int index){
		AutoStruct(HDITEM,hdi); 
		hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH; 
		hdi.pszText = (LPWSTR)text; 
		hdi.cxy = width; 
		hdi.fmt = HDF_LEFT | HDF_STRING; 
		if(index<0)
			index = 10000;

		return SendMessage(HDM_INSERTITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool HeaderPtr::SetItem(int index,LPCWSTR text,int width){
		AutoStruct(HDITEM,hdi); 
		hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH; 
		hdi.pszText = (LPWSTR)text; 
		hdi.cxy = width; 
		hdi.fmt = HDF_LEFT | HDF_STRING; 

		return 0!=SendMessage(HDM_SETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	int HeaderPtr::GetItem(int index,String& text){
		text.SetCubage(256);
		AutoStruct(HDITEM,hdi); 
		hdi.mask = HDI_TEXT | HDI_WIDTH; 
		hdi.pszText = text.Handle();
		hdi.cchTextMax = 256;
		if(!SendMessage(HDM_GETITEM,(WPARAM)index,(LPARAM)&hdi))
			return 0; 
		text.CopyFrom(hdi.pszText);
		text.Realize();
		return hdi.cxy;
	}
	bool HeaderPtr::SetItemWidth(int index,int width){
		AutoStruct(HDITEM,hdi); 
		hdi.mask = HDI_WIDTH; 
		hdi.cxy = width; 
		return 0!=SendMessage(HDM_SETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool HeaderPtr::SetItemText(int index,LPCWSTR title){
		AutoStruct(HDITEM,hdi); 
		hdi.mask = HDI_TEXT; 
		hdi.pszText = (LPWSTR)title; 
		return 0!=SendMessage(HDM_SETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool HeaderPtr::SetItem(int index,HDITEM* hdi){
		return 0!=SendMessage(HDM_SETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool HeaderPtr::GetItem(int index,HDITEM* hdi){
		return 0!=SendMessage(HDM_GETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
}