#include "stdafx.h"
#include "../conciseui.h"

#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")

namespace cs{
	bool Init_Ctrl60(DWORD icc)
	{
		INITCOMMONCONTROLSEX iccex; 
		iccex.dwICC = icc;
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		return InitCommonControlsEx(&iccex)!=0;
	}
	//class ImageList
	ImageList::~ImageList()
	{
		::ImageList_Destroy(_Handle);
	}
	void ImageList::operator = (HIMAGELIST hImgl)
	{
		Attach(hImgl);
	}
	bool ImageList::Attach(HIMAGELIST hImgl)
	{
		if(hImgl==0) return 0;
		::ImageList_Destroy(_Handle);
		_Handle = hImgl;
		return true;
	}
	bool ImageList::Create(int cx,int cy,UINT ilt,int initial,int grow)
	{
		HIMAGELIST handle = ::ImageList_Create(cx,cy,ilt,initial,grow);
		if(handle==0) return 0;
		if(_Handle)::ImageList_Destroy(_Handle);
		_Handle = handle;
		return true;
	}
	HIMAGELIST ImageList::Detach()
	{
		HIMAGELIST hImgl = _Handle;
		_Handle = 0;
		return hImgl;
	}
	bool ImageList::Destroy()
	{
		if(_Handle==0) return 0;
		HIMAGELIST himgl = _Handle;
		_Handle = 0;
		return ::ImageList_Destroy(himgl)==1;
	}
	int ImageList::AddBitmap(HBITMAP bmp,HBITMAP mask)
	{
		int count = ImageList_Add(_Handle,bmp,mask);
		if(count<0) return 0;
		return ImageList_GetImageCount(_Handle) - count;
	}
	bool ImageList::AddIcon(HICON icon)
	{
		return ::ImageList_ReplaceIcon(_Handle,-1,icon)!=-1;
	}
	bool ImageList::Replace(HICON icon,int index)
	{
		if((index<0)||(index>=ImageList_GetImageCount(_Handle))) return false;
		return ::ImageList_ReplaceIcon(_Handle,index,icon)!=-1;
	}
	int ImageList::LoadResource(int cx,COLORREF mask,ResID res,int grow)
	{
		UINT lr = LR_CREATEDIBSECTION|LR_DEFAULTSIZE;
		if(res.Instance==0) lr |= LR_LOADFROMFILE;
		HIMAGELIST handle = ::ImageList_LoadImageW(res.Instance,res,cx,grow,mask,IMAGE_BITMAP,lr);
		if(handle==0) return 0;
		if(_Handle) ::ImageList_Destroy(_Handle);
		_Handle = handle;
		return ImageList_GetImageCount(_Handle);
	}
	bool ImageList::Draw(HDC hdc,int index,int x,int y,UINT style)
	{
		return ::ImageList_Draw(_Handle,index,hdc,x,y,style)!=0;
	}
	bool ImageList::Copy(int iDst,int iSrc)
	{
		return ImageList_Copy(_Handle,iDst,_Handle,iSrc,ILCF_MOVE)!=0;
	}
	bool ImageList::Swap(int iDst,int iSrc)
	{
		return ImageList_Copy(_Handle,iDst,_Handle,iSrc,ILCF_SWAP)!=0;
	}
	HIMAGELIST ImageList::Duplicate()
	{
		return ::ImageList_Duplicate(_Handle);
	}
	COLORREF ImageList::GetBkColor()
	{
		return ::ImageList_GetBkColor(_Handle);
	}
	int ImageList::Count()
	{
		return ::ImageList_GetImageCount(_Handle);
	}
	Twin ImageList::GetImageSize()
	{
		Twin size(0,0);
		::ImageList_GetIconSize(_Handle,&size.x,&size.y);
		return size;
	}
	HICON ImageList::GetIcon(int index)
	{
		return ::ImageList_GetIcon(_Handle,index,0);
	}
	bool ImageList::Delete(int index)
	{
		if(index<0) return 0;
		return ::ImageList_Remove(_Handle,index)!=0;
	}//is index = -1,then remove all
	bool ImageList::Clear()
	{
		return ImageList_Remove(_Handle,-1)!=0;
	}
	COLORREF ImageList::SetBkColor(COLORREF bk)
	{
		return ::ImageList_SetBkColor(_Handle,bk);
	}
	bool ImageList::SetImageSize(int cx,int cy)
	{
		return ::ImageList_SetIconSize(_Handle,cx,cy)!=0;
	}//this clear the imagelist.
	bool ImageList::SetCount(int count)
	{
		return ::ImageList_SetImageCount(_Handle,count)!=0;
	}
	//class ICtrl
	ICtrl::ICtrl(){
		Param->AddStyle(WS_TABSTOP);
		Param->Location.SetValue(10,10);
		Param->Size.SetValue(100,100);
		BkColor = RGB(236,233,216);
		GetMsgHook(WM_GET_ICTRL)->Add(this,&ICtrl::_OnICtrl);
		OnColor.Bind(this,&ICtrl::_CtrlColor);
	}
	ICtrl::~ICtrl(){
	}
	void ICtrl::_OnICtrl(Message* msg,IWnd*){
		msg->Result = (UINT_PTR)this;
	}
	void ICtrl::_CtrlColor(Message* msg,ICtrl*){
		::SetTextColor((HDC)msg->wParam,FontColor);
		::SetBkMode((HDC)msg->wParam,TRANSPARENT);
		HBRUSH bk = (HBRUSH)GetStockObject(DC_BRUSH);
		SetDCBrushColor((HDC)msg->wParam,BkColor);
		msg->Result = (INT_PTR)bk;
	}
	bool ICtrl::Attach(HWND hCtrl)
	{
		if(_Handle) return 0;
		_QuondamProc = ::SetWindowLongPtr(hCtrl,GWLP_WNDPROC,(LONG)(LONG_PTR)_WndPointer.Procedure());
		if(_QuondamProc==0) return 0;
		_Handle = hCtrl;
		return 1;
	}
	HWND ICtrl::Detach()
	{
		if(_Handle==0) return 0;
		HWND hwnd = _Handle;
		_Handle = 0;
		::SetWindowLongPtr(hwnd,GWLP_WNDPROC,_QuondamProc);
		return hwnd;
	}
	INT_PTR ICtrl::Create(HWND hParent)
	{
		if(_Handle) return 0;
		Param->Style |= WS_CHILD;
		_Handle = ::CreateWindowEx(Param->ExStyle,Param->ClassName,Param->Text,Param->Style,Param->Location.x,Param->Location.y,Param->Size.x,Param->Size.y,hParent,Param->Menu,GetModuleHandle(0),Param->lParam);
		if(_Handle==0){
			Warning(L"ICtrl.Create");
			return 0;
		}
		OnCreate.Call(this);
		_QuondamProc = ::SetWindowLongPtr(_Handle,GWLP_WNDPROC,(LONG)(LONG_PTR)_WndPointer.Procedure());
		SetFont(0);
		return (INT_PTR)_Handle;
	}
	ICtrl* GetICtrl(HWND hWnd){
		return (ICtrl*)::SendMessage(hWnd,WM_GET_ICTRL,0,0);
	}
	//class Edit
	Edit::Edit(){
		Param->ClassName = L"edit";
		Param->ExStyle = WS_EX_CLIENTEDGE;
		Param->Style |= ES_AUTOHSCROLL;
		BkColor = 0xffffff;
		OnColor.Bind(this,&Edit::_CtrlColor);
		OnCommand.Add(this,&Edit::_Command);
	}
	void Edit::_Command(WORD id,WORD code,IWnd* wnd){
		if(code==EN_CHANGE&&!OnChange.IsNull()){
			OnChange((Edit*)wnd);
		}
	}
	void Edit::_CtrlColor(Message* msg,ICtrl*){
		::SetTextColor((HDC)msg->wParam,FontColor);
		::SetBkColor((HDC)msg->wParam,BkColor);
		HBRUSH bk = (HBRUSH)GetStockObject(DC_BRUSH);
		::SetDCBrushColor((HDC)msg->wParam,BkColor);
		msg->Result = (INT_PTR)bk;
	}
	void Edit::SetNumerical(bool numerical){
		Delegate<Message*,IWnd*>* wmChar = GetMsgHook(WM_CHAR);
		if(numerical){
			if(wmChar->IndexOf(this,&Edit::_OnNumberChar)==-1)
				wmChar->Add(this,&Edit::_OnNumberChar);
		}else{
			wmChar->Delete(this,&Edit::_OnNumberChar);
		}
	}
	//获取删除选中项后的文本
	int getNewText(Edit* edit,String& ns){
		String str(256,1);
		edit->GetText(str);
		Twin16 sel = edit->GetSelect();
		ns.CopyFrom(str,sel.x);
		ns.CopyFrom(str+sel.y);
		return sel.x;
	}
	void Edit::_OnNumberChar(Message* msg,IWnd*){
		wchar_t ch = msg->wParam;
		if(ch>'9'||ch<'0'){
			if(ch=='-'){
				String str;
				int pos = getNewText(this,str);
				if(pos!=0||str.Find('-')!=-1){
					msg->Result = 0;
					return;
				}
			}else if(ch=='.'){
				String str;
				int pos = getNewText(this,str);
				if(str.Find('.')!=-1){
					msg->Result = 0;
					return;
				}
			}else{
				if(ch!=8){
					msg->Result = 0;
					return;
				}
			}
		}
	}
	Label::Label(){
		Param->ClassName = L"Static";
	}
	//class Button
	Button::Button(){
		Param->ClassName = L"Button";
	}
	GroupBox::GroupBox(){
		Param->ClassName = L"Button";
		Param->Style |= BS_GROUPBOX;
	}
	CheckBox::CheckBox(){
		Param->ClassName = L"Button";
		Param->Style |= BS_AUTOCHECKBOX;
	}
	Radio::Radio(){
		Param->ClassName = L"Button";
		Param->Style |= BS_AUTORADIOBUTTON;
	}
	//class MonthCalendar
	MonthCalendar::MonthCalendar()
	{
		Param->ClassName = L"SysMonthCal32";
	}
	//class DateTimePicker
	DateTimePicker::DateTimePicker()
	{
		Param->ClassName = L"SysDateTimePick32";
	}
	//class ListBox
	ListBox::ListBox()
	{
		Param->Style = WS_HSCROLL|WS_VSCROLL|WS_VISIBLE|LBS_HASSTRINGS;
		Param->ExStyle = WS_EX_CLIENTEDGE;
		Param->ClassName = L"ListBox";
		BkColor = 0xffffff;
	}
	//class ComboBox
	ComboBox::ComboBox(){
		Param->ClassName = L"ComboBox";
		Param->Style = WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN|CBS_AUTOHSCROLL|CBS_HASSTRINGS;
	}
	//class ComboBoxEx
	ComboBoxEx::ComboBoxEx()
	{
		Param->ClassName = L"ComboBoxEx32";
		Param->Style |= CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_VISIBLE;
	}
	//class TreeView
	TreeView::TreeView()
	{
		Param->ClassName = L"SysTreeView32";
		Param->Style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_INFOTIP|TVS_LINESATROOT;
		Param->ExStyle |= WS_EX_CLIENTEDGE;
		BkColor = 0xffffff;
		//this->OnNotify.Bind(this,&TreeView::_OnNotify);
	}
	ToolBar::ToolBar()
	{
		Param->ClassName = L"ToolbarWindow32";
		Param->Style |= TBSTYLE_TRANSPARENT|WS_VISIBLE|TBSTYLE_TOOLTIPS|TBSTYLE_FLAT;
		//|TBSTYLE_LIST：文字在右，TBSTYLE_FLAT：文字在下。如果不要文字，应该设为 NULL，而不是空串，空串仍然会把按钮撑大，甚至NULL也会把文字在右时的按钮撑大一点。
	}
	INT_PTR ToolBar::Create(HWND hParent){
		if(!ICtrl::Create(hParent)) return 0;
		SendMessage(TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
		SetExStyle(TBSTYLE_EX_HIDECLIPPEDBUTTONS,1);
		/*if(IWnd* wp = GetWindowByHandle(hParent)){
			wp->GetMsgHook(WM_SIZE)->Add(this,&ToolBar::_OnParentSize);
		}*/
		return (INT_PTR)_Handle;
	}
	/*void ToolBar::_OnParentSize(Message*)
	{
		//SendMessage(TB_AUTOSIZE,0,0);
	}*/
	//class Progress Bar
	ProgressBar::ProgressBar()
	{
		Param->ClassName = L"msctls_progress32";
	}
	//class ToolTip
	ToolTip::ToolTip(void)
	{
		Param->ClassName = L"tooltips_class32";
		Param->ExStyle = WS_EX_TOPMOST;
		Param->Style = WS_POPUP|TTS_NOPREFIX|TTS_ALWAYSTIP;//|TTS::BALLOON;
	}

	INT_PTR ToolTip::Create(HWND hParent)
	{
		if(!ICtrl::Create(hParent)) return 0;
		::SetWindowPos(_Handle, HWND_TOPMOST,0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		return (INT_PTR)_Handle;
	}
	//class Tab_Ctrl
	TabPage::TabPage()
	{
		Param->ClassName = L"SysTabControl32";
		//OnNotify.Bind(this,&TabPage::_OnNotify);
	}
	//class HotKey
	HotKey::HotKey()
	{
		this->Param->Size.SetValue(146,18);
		Param->ClassName = L"msctls_hotkey32";
	}
	ListView::ListView()
	{
		Param->ClassName = L"SysListView32";
		Param->ExStyle = WS_EX_CLIENTEDGE;
	}
	DWORD ListView::SetExStyle(DWORD exStyle,bool add)
	{
		//这个消息的规则如下，如果 wParam 为 0，则直接设置 lParam 指定的风格；如果 wParam 包含和某个风格，而
		//lParam 不包含这个风格，则这个风格被移除；如果 wParam 不包含某个风格，而 lParam 包含这个风格，这个风
		//格无效；wParam 和 lParam 都包含的风格被添加。
		if(add)
			return SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,exStyle,exStyle);
		else
			return SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,exStyle,0);
	}
	Rebar::Rebar(){
		Param->ClassName = REBARCLASSNAME;
		Param->ExStyle |= WS_EX_TOOLWINDOW;
		Param->Style |= RBS_VARHEIGHT|CCS_NODIVIDER;
	}
	//class Status Bar
	StatusBar::StatusBar()
	{
		Param->ClassName = L"msctls_statusbar32";
	}
	TrackBar::TrackBar()
	{
		Param->ClassName = L"msctls_trackbar32";
	}
	//class UpDown Control
	UpDown::UpDown()
	{
		Param->ClassName = L"msctls_updown32";
	}
	INT_PTR UpDown::Create(HWND parent){
		return ICtrl::Create(parent);
	}
	INT_PTR UpDown::Create(HWND buddy,HWND parent,int nUpper,int nLower,int nPos)
	{
		if(parent==0)
			parent = GetParent(buddy);
		if(parent==0) return 0;
		DWORD style = GetWindowLong(buddy,GWL_STYLE);
		style |= ES_NUMBER;
		SetWindowLong(buddy,GWL_STYLE,style);
		_Handle = CreateUpDownControl(WS_VISIBLE|WS_CHILD|UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS,0,0,0,0,parent,Param->Identity,GetInstance(),buddy,nUpper,nLower,nPos);
		if(_Handle==0){
			Warning(L"UpDown.Create");
			return 0;
		}
		OnCreate.Call(this);
		_QuondamProc = ::SetWindowLongPtr(_Handle,GWLP_WNDPROC,(LONG)(LONG_PTR)_WndPointer.Procedure());
		return (INT_PTR)_Handle;
	}
	//class Header
	Header::Header(){
		Param->ClassName = WC_HEADER;//L"SysHeader32"
		Param->AddStyle(WS_BORDER | HDS_BUTTONS | HDS_HORZ);
	}
	Link::Link(){
		Param->ClassName = WC_LINK;
	}
	IpAddress::IpAddress(){
		Param->ClassName = WC_IPADDRESS;
	}
}