#include "stdafx.h"
#include "../conciseui.h"


namespace cs{
	//interface WndPtr
	bool WndPtr::GetRect(Rect& r){
		if(!::GetWindowRect(_Handle,r)) return false;
		HWND hp = ::GetParent(_Handle);
		if(hp){
			LONG_PTR style = GetWindowParam(GWL_STYLE);
			if(style&WS_CHILD){
				Twin p(0,0);
				::ScreenToClient(hp,p);
				r.OffsetLocation(p.x,p.y);
			}
		}
		return 1;
	}
	bool WndPtr::GetClientSize(Twin16& c){
		RECT cr;
		if(!GetClientRect(_Handle,&cr)) return false;
		c.x = (short)cr.right;
		c.y = (short)cr.bottom;
		return true;
	}
	void WndPtr::Offset(int x, int y, int cx, int cy)//a window's size can't be set to a negativ number.
	{
		Rect mRect;
		if(!GetRect(mRect)) return;
		if(!(cx|cy))//cx=0,cy=0
		{
			if(x|y)//x,y之一不为0;
			{
				::SetWindowPos(_Handle,0,mRect.left+x,mRect.top+y,0,0,SWP_NOZORDER|SWP_NOSIZE);//
			}
		}
		else 
		{
			if(x|y)//x,y之一不为0;
			{
				::SetWindowPos(_Handle,0,mRect.left+x,mRect.top+y,mRect.Width()+cx,mRect.Height()+cy,SWP_NOZORDER);
			}
			else
			{
				::SetWindowPos(_Handle,0,0,0,mRect.Width()+cx,mRect.Height()+cy,SWP_NOZORDER|SWP_NOMOVE);
			}
		}
	}
	void WndPtr::ModifyStyle(DWORD dwRemove,DWORD dwAdd)
	{
		DWORD dwStyle = ::GetWindowLong(_Handle,GWL_STYLE);
		DWORD dwNewStyle = (dwStyle&~dwRemove)|dwAdd;
		if(dwStyle==dwNewStyle) return;
		::SetWindowLong(_Handle,GWL_STYLE,dwNewStyle);
		::SetWindowPos(_Handle,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED|SWP_DRAWFRAME);//
	}
	void WndPtr::ModifyExStyle(DWORD dwRemove,DWORD dwAdd)
	{
		DWORD dwStyle = ::GetWindowLong(_Handle,GWL_EXSTYLE);
		DWORD dwNewStyle = (dwStyle&~dwRemove)|dwAdd;
		if(dwStyle==dwNewStyle) return;
		::SetWindowLong(_Handle,GWL_EXSTYLE,dwNewStyle);
		::SetWindowPos(_Handle,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED|SWP_DRAWFRAME);//
	}
	bool WndPtr::SetWidth(int cx){
		Rect r;
		if(!GetRect(r)) return 0;
		return SetSize(cx,r.Height());
	}
	bool WndPtr::SetHeight(int cy)
	{
		Rect r;
		if(!GetRect(r)) return 0;
		return SetSize(r.Width(),cy);
	}
	bool WndPtr::SetXLocation(int x)
	{
		Rect r;
		if(!GetRect(r)) return 0;
		return SetLocation(x,r.top);
	}
	bool WndPtr::SetYLocation(int y){
		Rect r;
		if(!GetRect(r)) return 0;
		return SetLocation(r.left,y);
	}
	int WndPtr::GetText(String& text){
		uint len = ::GetWindowTextLengthW(_Handle);
		if(len>text.Cubage()) text.SetCubage(len);
		len = ::GetWindowTextW(_Handle,text,text.Cubage()+1);
		text.Realize();
		return len;
	}
	IWnd* GetIWnd(HWND hWnd){
		return (IWnd*)::SendMessage(hWnd,WM_GET_IWND,0,0);
	}
	Font* GetDefaultFont(){
		static Font _font;//__declspec(thread) 
		if(_font.IsNull()){
			_font.Create();
		}
		return &_font;
	}
	//class IWnd
	IWnd::IWnd(void):FontColor(0),_Scr(0,0),_MinSize(0,0),_mLeave(true),_mNcLeave(true),
		_Cursor((HICON)INVALID_HANDLE_VALUE){
		Param.New();
		_DragMinSize.SetValue(0,0);
		_DragMaxSize.SetValue(0xffff,0xffff);
		_QuondamProc = (LONG_PTR)::DefWindowProc;
		BkColor = RGB(236,233,216);
		_WndPointer.Bind(this,&IWnd::_WndProc);
	}
	IWnd::~IWnd()
	{
		::DestroyWindow(_Handle);
	}
	Delegate<Message*,IWnd*>* IWnd::GetMsgHook(UINT msg)
	{
		return GetMsgHook(msg,msg);
	}
	Delegate<Message*,IWnd*>* IWnd::GetMsgHook(UINT msg1,UINT msg2)
	{
		bool exist;
		_UPOINT up;
		up.x = msg1;
		up.y = msg2;
		int i = _MsgList.Add(up,-1,&exist);
		Delegate<Message*,IWnd*>* d;
		if(exist) d = &_ProcList[i];
		else d = _ProcList.Add();
		return d;
	}
	LRESULT IWnd::_WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
		if(!_Handle) _Handle = hWnd;
		Message msg;
		msg.hWnd	= hWnd;
		msg.uMsg	= uMsg;
		msg.wParam	= wParam;
		msg.lParam	= lParam;
		for(uint i=0;i<_MsgList.Count();i++){
			if((uMsg>=_MsgList[i].x)&&(uMsg<=_MsgList[i].y))
				_ProcList[i].Call(&msg,this);
		}
		if(msg.Result._DisableWndproc()) goto return_code;
		if((uMsg>=WM_MOUSEFIRST&&uMsg<=WM_MOUSELAST)){//处理鼠标消息.
			if(_mLeave){
				_mLeave = false;
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.dwHoverTime = 0xFFFFFFFF;
				tme.hwndTrack = hWnd;
				if(!TrackMouseEvent(&tme)) _mLeave = true;
			}
			if((uMsg==WM_MOUSEMOVE)) ::SetCursor(_Cursor);
			Mouse m;
			m.Button = LOWORD(wParam);
			m.WheelDelta = HIWORD(wParam);
			m.X = LOWORD(lParam);
			m.Y = HIWORD(lParam);
			m.Event = uMsg;
			m.Msg = &msg;
			OnMouse.Call(&m,this);
		}else if(uMsg==WM_MOUSELEAVE){
			_mLeave = true;
			Mouse m;
			m.Button = LOWORD(wParam);
			m.WheelDelta = HIWORD(wParam);
			m.X = LOWORD(lParam);
			m.Y = HIWORD(lParam);
			m.Event = uMsg;
			m.Msg = &msg;
			OnMouse.Call(&m,this);
		}else if(((uMsg>=WM_NCMOUSEMOVE)&&(uMsg<=WM_NCMBUTTONDBLCLK))){
			if(_mNcLeave){
				_mNcLeave = false;
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE|TME_NONCLIENT;
				tme.dwHoverTime = 0xFFFFFFFF;
				tme.hwndTrack = hWnd;
				if(!TrackMouseEvent(&tme)) _mNcLeave = true;
			}
			NcMouse nm;
			nm.Event = uMsg;
			nm.HitTest = wParam;
			nm.X = LOWORD(lParam);
			nm.Y = HIWORD(lParam);
			nm.Msg = &msg;
			OnNcMouse.Call(&nm,this);
		}else if((uMsg==WM_NCMOUSELEAVE)){
			_mNcLeave = true;
			NcMouse nm;
			nm.Event = uMsg;
			nm.HitTest = wParam;
			nm.X = LOWORD(lParam);
			nm.Y = HIWORD(lParam);
			nm.Msg = &msg;
			OnNcMouse.Call(&nm,this);
		}else if((uMsg>=WM_CTLCOLORMSGBOX)&&(uMsg<=WM_CTLCOLORSTATIC)){//控件的颜色消息,转给它们自己处理.
			if((HWND)lParam!=hWnd){
				ICtrl* wnd = GetICtrl((HWND)lParam);
				if(wnd&&!wnd->OnColor.IsNull()){
					wnd->OnColor(&msg,wnd);
				}
			}else{//控件自己不会接收到WM_CTLCOLOR类的消息，所以下面的代码一般不会执行到
				return (HRESULT)0;
			}
		}else if(uMsg==WM_COMMAND){
			//控件的 WM_COMMAND 只发给父窗口，但是 WM_COMMAND 消息也用于控件自身的一些
			//事件，比如 Edit 控件的文本变化消息，也是通过 WM_COMMAND 来发给父窗口的，
			//但是通常，这类消息我们希望控件自己来处理。所以，凡是控件消息，除了父窗口
			//之外，还会给自己发一份。
			OnCommand.Call(LOWORD(wParam),HIWORD(wParam),this);
			if((lParam!=0)&&((HWND)lParam!=hWnd)){
				IWnd* wnd = GetIWnd((HWND)lParam);
				if(wnd){
					wnd->OnCommand.Call(LOWORD(wParam),HIWORD(wParam),wnd);
				}
			}
		}else if(uMsg==WM_CREATE){
			OnCreate.Call(this);
		}else if(uMsg==WM_NOTIFY){//与WM_COMMAND一样,这两个消息往往是控件自己的消息,并不需要主窗口处理.
			if(((LPNMHDR)lParam)->hwndFrom!=hWnd){
				ICtrl* wnd = GetICtrl(((LPNMHDR)lParam)->hwndFrom);
				if(wnd&&!wnd->OnNotify.IsNull()){
					wnd->OnNotify(&msg,wnd);
				}
			}
		}else if(uMsg==WM_GET_IWND){//响应GetWindowByHandle函数.
			msg.Result = (UINT_PTR)this;
		}else if(uMsg==WM_NCDESTROY){
			_Handle = 0;
		}else if(uMsg==WM_MOVE){//这几乎是唯一取得客户区位置的方式.(NC类的消息了也有）
			_ClientLocation.SetValue(LOWORD(lParam),HIWORD(lParam));
		}else if(uMsg==WM_GETMINMAXINFO){
			LPMINMAXINFO pmi = (LPMINMAXINFO)lParam;

			Rect wr;
			Twin16 cs;
			Twin msz;

			GetRect(wr);
			GetClientSize(cs);

			msz = _DragMinSize;
			msz.x += wr.Width() - cs.x;
			msz.y += wr.Height() - cs.y;
			if(msz.x>pmi->ptMinTrackSize.x) pmi->ptMinTrackSize.x = msz.x;
			if(msz.y>pmi->ptMinTrackSize.y) pmi->ptMinTrackSize.y = msz.y;

			msz = _DragMaxSize;
			msz.x += wr.Width() - cs.x;
			msz.y += wr.Height() - cs.y;
			if(msz.x<pmi->ptMaxTrackSize.x) pmi->ptMaxTrackSize.x = msz.x;
			if(msz.y<pmi->ptMaxTrackSize.y) pmi->ptMaxTrackSize.y = msz.y;
		}else if(uMsg==WM_SIZE){
			if(msg.wParam!=SIZE_MINIMIZED){
				Twin16 cs = *((Twin16*)&msg.lParam);
				OnSize.Call(cs,this);
			}
		}else if(uMsg==WM_CLOSE){
			bool cancel = false;
			OnClose.Call(cancel,this);
			if(cancel)
				msg.Result = 0;
		}
return_code:
		if(msg.Result.Default())
			return ((WNDPROC)_QuondamProc)(hWnd,uMsg,wParam,lParam);
		else
			return msg.Result.Handle();
	}
	bool IWnd::SetFont(HFONT font,bool reDraw){
		if(font==0){
			font = *GetDefaultFont();
		}
		return SendMessage(WM_SETFONT,(WPARAM)font,reDraw)!=0;
	}
	//Class View
	View** GetAfxWindow()
	{
		static __declspec(thread) View* pView = 0;
		return &pView;
	}
	LRESULT __stdcall WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		View* createWnd = *GetAfxWindow();//因为消息会出现嵌套，所以，可能出现这个函数的递归调用，
		if(createWnd==0) //检测createWnd是否被正确的赋值，除非错误的没有设置createWnd的值，正确的调用，这种情况实际上不会出现。
			return ::DefWindowProc(hWnd,msg,wParam,lParam);
		*GetAfxWindow() = 0;//第一时间把实例的窗口句柄赋值。
		createWnd->Attach(hWnd);//改变窗口的消息函数。
		return createWnd->_WndPointer(hWnd,msg,wParam,lParam);//不遗漏消息，实际上，在重新设置了窗口的消息函数之后，后面的消息将不会调用这个函数。
	}
	ATOM RegisterWindowClass(String& className,UINT style){
		WNDCLASSEXW wcex;
		wcex.lpszClassName	= className;
		wcex.cbSize			= sizeof(WNDCLASSEX); 
		wcex.style			= style;
		wcex.lpfnWndProc	= (WNDPROC)&WindowProcedure;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;//DLGWINDOWEXTRA;
		wcex.hInstance	    = (HINSTANCE)::GetModuleHandle(0);
		wcex.hIcon			= NULL;
		wcex.hCursor		= NULL;
		wcex.hbrBackground	= (HBRUSH)0;
		wcex.lpszMenuName	= NULL;
		wcex.hIconSm		= NULL;
		return RegisterClassExW(&wcex);
	}

	View::View(){
		_Cursor = ::LoadCursor(0,IDC_ARROW);
		Param->Style |= WS_CHILD;
		GetMsgHook(WM_PAINT)->Add(this,&View::_DrawProc);
		GetMsgHook(WM_ERASEBKGND)->Add(this,&View::_EraseBkgndProc);
		GetMsgHook(WM_DROPFILES)->Add(this,&View::_DropFilesProc);
	}
	View::~View()
	{
		/*GetMsgHook(WM_PAINT)->Delete(this,&View::_DrawProc);
		GetMsgHook(WM_ERASEBKGND)->Delete(this,&View::_EraseBkgndProc);
		GetMsgHook(WM_SIZE)->Delete(this,&View::_CtrlSizeProc);
		GetMsgHook(WM_DROPFILES)->Delete(this,&View::_DropFilesProc);*/
	}
	INT_PTR View::Create(HWND hParent)
	{
		if(_Handle) return 0;
		String bn;
		if(Param->ClassName.IsNull()){
			Param->ClassName = L"Form.View.";
			bn.Format(L"%X",(INT_PTR)&bn);//在DLL和EXE中，因为有各自的窗口函数，必须区分开，
			//否则后者注册失败后，将会使用前一个窗口类和窗口函数，就会发生混乱。
			Param->ClassName += bn;
			Param->ClassName += L".";
			bn.Format(L"%X",Param->ClassStyle);
			Param->ClassName += bn;
			RegisterWindowClass(Param->ClassName,Param->ClassStyle);
		}
		View** pv = GetAfxWindow();
		*pv = this;
		_Handle = ::CreateWindowEx(Param->ExStyle,Param->ClassName,Param->Text,Param->Style,Param->Location.x,Param->Location.y,Param->Size.x,Param->Size.y,hParent,Param->Menu,::GetModuleHandle(0),this/*Param->lParam*/);
		if(_Handle==0){
			Warning(L"View.Create");
			return 0;
		}
		return (INT_PTR)_Handle;
	}
	bool View::Attach(HWND handle)
	{
		if(IsWindow(handle)==0){
			Warning(L"要绑定的句柄不是一个窗口句柄.");
			return 0;
		}
		_QuondamProc = (LONG_PTR)::DefWindowProc;
		_Handle = handle;
		this->SetWindowParam(GWL_WNDPROC,(LONG_PTR)_WndPointer.Procedure());
		return 1;
	}
	HWND View::Detach()
	{
		if(_Handle==0) return 0;
		SetWindowParam(GWL_WNDPROC,_QuondamProc);
		HWND hWnd = _Handle;
		_Handle = 0;
		return hWnd;
	}
	void View::SetCursor(HCURSOR cursor)
	{
		_Cursor = cursor;
		::SetCursor(_Cursor);
	}
	void View::SetCursor(ResID resource)
	{
		_Cursor = ::LoadCursor(resource.Instance,resource);
		::SetCursor(_Cursor);
	}
	void View::_DoubleDraw(DC* hdc,IWnd*){
		Rect rect;
		::GetClientRect(_Handle,rect);
		Bitmap bmp;
		bmp.CreateCompatibleBitmap(rect.Width(),rect.Height());
		DC dc;
		dc.CreateCompatible(0);
		bmp.Select(dc);
		if((BkColor&0xff000000)==0){
			HBRUSH bk = (HBRUSH)GetStockObject(DC_BRUSH);
			::SetDCBrushColor(dc,BkColor);
			::FillRect(dc,rect,bk);
		}
		/*if(BkImage&&(BK_IMAGE&BkMode)){
			Twin sz = BkImage->Size();
			if(sz.Value!=0){
				int iLeft = _Scr.x/sz.x;
				int iRight = (rect.Width()+_Scr.x)/sz.x + 1;
				int jTop = _Scr.y/sz.y;
				int jBottom = (rect.Height()+_Scr.y)/sz.y + 1;
				for(int i=iLeft;i<iRight;i++){
					for(int j=jTop;j<jBottom;j++){
						BkImage->DrawAlpha(dc,BkAlpha,0!=(BkMode&BK_IMAGE_ALPHA),i*sz.x-_Scr.x,j*sz.y-_Scr.y);
					}
				}
			}
		}*/
		OnDoubleDraw.Call(&dc,this);
		::BitBlt(hdc->Handle(),rect.left,rect.top,rect.right,rect.bottom,dc,0,0,SRCCOPY);
		bmp.Select(dc);
	}
	void View::_DrawProc(Message* msg,IWnd*){
		PAINTSTRUCT ps;
		::BeginPaint(_Handle,&ps);

		DC dc;
		dc = ps.hdc;
		if(OnDoubleDraw.Count()){
			_DoubleDraw(&dc,this);
		}else{
			OnDraw.Call(&dc,this);
		}

		::EndPaint(_Handle,&ps);
		msg->Result = 0;
	}
	void View::_EraseBkgndProc(Message* msg,IWnd*){
		if(OnDoubleDraw.Count()) return;
		if(OnEraseBkgnd.Count()){
			OnEraseBkgnd.Call((HDC)msg->wParam,this);
		}else{
			Rect rect;
			::GetClipBox((HDC)msg->wParam,rect);
			if((BkColor&0xff000000)==0){
				HBRUSH bk = (HBRUSH)GetStockObject(DC_BRUSH);
				::SetDCBrushColor((HDC)msg->wParam,BkColor);
				::FillRect((HDC)msg->wParam,rect,bk);
			}
			/*if(BkImage&&((BK_IMAGE|BK_IMAGE_ALPHA)&BkMode)){
				Twin sz = BkImage->Size();
				if(sz.Value!=0){
					int iLeft = (rect.left+_Scr.x)/sz.x;
					int iRight = (rect.right+_Scr.x)/sz.x + 1;
					int jTop = (rect.top+_Scr.y)/sz.y;
					int jBottom = (rect.bottom+_Scr.y)/sz.y + 1;
					for(int i=iLeft;i<iRight;i++){
						for(int j=jTop;j<jBottom;j++){
							BkImage->DrawAlpha((HDC)msg->wParam,BkAlpha,0!=(BkMode&BK_IMAGE_ALPHA),i*sz.x-_Scr.x,j*sz.y-_Scr.y);
						}
					}
				}
			}*/
		}
		msg->Result = 1;
	}
	void View::_DropFilesProc(Message* msg,IWnd*){
		if(OnDropFiles.Count()==0) return;
		HDROP hd = (HDROP)msg->wParam;
		int count = ::DragQueryFile(hd,-1,0,0);
		wchar_t fn[260];
		StringMemList files;
		for(int i=0;i<count;i++)
		{
			::DragQueryFileW(hd,i,fn,260);
			files.Add(fn);
		}
		::DragFinish(hd);			
		OnDropFiles.Call(&files,this);
	}
	//Class ScrollView
	ScrollView::ScrollView(void):_vsPage(0),_hsPage(0),_vsLine(1),_hsLine(1),_ResponseKeyBoard(1),_vsMax(0), _hsMax(0),_MouseWheelScroll(WM_VSCROLL)
	{
		GetMsgHook(WM_KEYDOWN)->Add(this,&ScrollView::_KeyDownProc);
		GetMsgHook(WM_SIZE)->Add(this,&ScrollView::_SizeProc);
		GetMsgHook(WM_MOUSEWHEEL)->Add(this,&ScrollView::_MouseWheelProc);
		GetMsgHook(WM_VSCROLL)->Add(this,&ScrollView::_VScrollProc);
		GetMsgHook(WM_HSCROLL)->Add(this,&ScrollView::_HScrollProc);
	}
	ScrollView::~ScrollView()
	{
		/*GetMsgHook(WM_KEYDOWN)->Delete(this,&ScrollView::_KeyDownProc);
		GetMsgHook(WM_SIZE)->Delete(this,&ScrollView::_SizeProc);
		GetMsgHook(WM_MOUSEWHEEL)->Delete(this,&ScrollView::_MouseWheelProc);
		GetMsgHook(WM_VSCROLL)->Delete(this,&ScrollView::_VScrollProc);
		GetMsgHook(WM_HSCROLL)->Delete(this,&ScrollView::_HScrollProc);*/
	}
	void ScrollView::_MouseWheelProc(Message* msg,IWnd*)
	{
		if(_MouseWheelScroll==0) return;
		if((short)HIWORD(msg->wParam)>0) ::SendMessage(_Handle,_MouseWheelScroll,0,0);
		else ::SendMessage(_Handle,_MouseWheelScroll,1,0);
	}
	void ScrollView::ScrollLine(int xLine,int yLine)
	{
		if(xLine>0)
		{
			for(int i=0;i<xLine;i++)
				SendMessage(WM_HSCROLL,SB_LINERIGHT,0);
		}
		else if(xLine<0)
		{
			for(int i=0;i<-xLine;i++)
				SendMessage(WM_HSCROLL,SB_LINELEFT,0);
		}
		if(yLine>0)
		{
			for(int i=0;i<yLine;i++)
				SendMessage(WM_VSCROLL,SB_LINEDOWN,0);
		}
		else if(yLine<0)
		{
			for(int i=0;i<-yLine;i++)
				SendMessage(WM_VSCROLL,SB_LINEUP,0);
		}
	}
	void ScrollView::ScrollPage(int xPage,int yPage)
	{
		if(xPage>0)
		{
			for(int i=0;i<xPage;i++)
				SendMessage(WM_HSCROLL,SB_PAGERIGHT,0);
		}
		else if(xPage<0)
		{
			for(int i=0;i<-xPage;i++)
				SendMessage(WM_HSCROLL,SB_PAGELEFT,0);
		}
		if(yPage>0)
		{
			for(int i=0;i<yPage;i++)
				SendMessage(WM_VSCROLL,SB_PAGEDOWN,0);
		}
		else if(yPage<0)
		{
			for(int i=0;i<-yPage;i++)
				SendMessage(WM_VSCROLL,SB_PAGEUP,0);
		}
	}
	void ScrollView::ScrollEnd(int xEnd,int yEnd)
	{
		if(xEnd>0)
		{
			SendMessage(WM_HSCROLL,SB_RIGHT,0);
		}
		else if(xEnd<0)
		{
			SendMessage(WM_HSCROLL,SB_LEFT,0);
		}
		if(yEnd>0)
		{
			SendMessage(WM_VSCROLL,SB_BOTTOM,0);
		}
		else if(yEnd<0)
		{
			SendMessage(WM_VSCROLL,SB_TOP,0);
		}
	}
	void ScrollView::_KeyDownProc(Message* msg,IWnd*)
	{
		WORD vKey = (WORD)msg->wParam;
		if(_ResponseKeyBoard==0)
			return;
		switch(vKey)
		{
		case VK_PRIOR:
			SendMessage(WM_VSCROLL,SB_PAGEUP,0);
			break;
		case VK_NEXT:
			SendMessage(WM_VSCROLL,SB_PAGEDOWN,0);
			break;
		case VK_END:
			SendMessage(WM_VSCROLL,SB_BOTTOM,0);
			break;
		case VK_HOME:
			SendMessage(WM_VSCROLL,SB_TOP,0);
			break;
		case VK_LEFT:
			SendMessage(WM_HSCROLL,SB_LINEUP,0);
			break;
		case VK_UP:
			SendMessage(WM_VSCROLL,SB_LINEUP,0);
			break;
		case VK_RIGHT:
			SendMessage(WM_HSCROLL,SB_LINEDOWN,0);
			break;
		case VK_DOWN:
			SendMessage(WM_VSCROLL,SB_LINEDOWN,0);
			break;
		}
	}
	void ScrollView::_SizeProc(Message* msg,IWnd*)
	{
		Twin16 _Client;
		if(msg) _Client = *((Twin16*)&msg->lParam);
		else GetClientSize(_Client);
		SCROLLINFO si;
		_hsLine = _Client.x/10;
		if(_hsLine==0) _hsLine++;
		_vsLine = _Client.y/10;
		if(_vsLine==0) _vsLine++;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMin = 0;
		LONG_PTR style = GetWindowParam(GWL_STYLE);
		int cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
		int cxHScroll = GetSystemMetrics(SM_CXHSCROLL);
		int cx = _Client.x + ((style&WS_VSCROLL)?cxVScroll:0);
		int cy = _Client.y + ((style&WS_HSCROLL)?cxHScroll:0);
		bool _bHScroll = cx<_MinSize.x;
		bool _bVScroll = cy<_MinSize.y;
		if(_bHScroll)
		{
			if(cy<(_MinSize.y+cxHScroll)) _bVScroll = 1;
		}
		if(_bVScroll)
		{
			if(cx<(cxVScroll+_MinSize.x)) _bHScroll = 1;
		}
		if(_bHScroll)//滚动条大小为17
		{
			_hsMax = _MinSize.x - _Client.x + 1;
			if(_hsMax<0)
				_hsMax = 0;
			if(_Scr.x>_hsMax)
			{
				ScrollWindow(_Handle,_Scr.x-_hsMax,0,NULL,NULL);
				_Scr.x = _hsMax;
			}
			si.nPage = _Client.x;
			si.nMax = _MinSize.x;
			si.nPos = _Scr.x;
			_hsPage = si.nPage - _hsLine;
			SetScrollInfo(_Handle,0,&si,TRUE);
			ShowScrollBar(_Handle,0,1);
		}
		else
		{
			ShowScrollBar(_Handle,0,0);
			if(_Scr.x)
			{
				ScrollWindow(_Handle,_Scr.x,0,NULL,NULL);
				Invalidate();
				_Scr.x = 0;
			}
			_hsMax = 0;
		}
		if(_bVScroll)//滚动条大小为17
		{
			_vsMax = _MinSize.y - _Client.y + 1;
			if(_vsMax<0)
				_vsMax = 0;
			if(_Scr.y>_vsMax)
			{
				ScrollWindow(_Handle,0,_Scr.y-_vsMax,NULL,NULL);//sometime it is necessary and other time it is must not;
				_Scr.y = _vsMax;
			}
			si.nPage = _Client.y;
			si.nMax = _MinSize.y;
			si.nPos = _Scr.y;
			_vsPage = si.nPage - _vsLine;
			SetScrollInfo(_Handle,1,&si,TRUE);
			ShowScrollBar(_Handle,1,1);
		}
		else
		{
			ShowScrollBar(_Handle,1,0);
			if(_Scr.y)
			{
				ScrollWindow(_Handle,0,_Scr.y,NULL,NULL);
				Invalidate();
				_Scr.y = 0;
			}
			_vsMax = 0;
		}
	}
	void ScrollView::_HScrollProc(Message* msg,IWnd*)
	{
		if(msg->lParam) return;
		else ::SendMessage((HWND)msg->lParam,WM_HSCROLL,msg->wParam,0);
		UINT nSBCode = LOWORD(msg->wParam);
		UINT nPos = HIWORD(msg->wParam);
		int p = _Scr.x;
		switch(nSBCode)
		{
		case 0:
			_Scr.x -= _hsLine;
			if(_Scr.x<0)
				_Scr.x = 0;
			break;
		case 1:
			_Scr.x += _hsLine;
			if(_Scr.x>_hsMax)
				_Scr.x = _hsMax;
			break;
		case 2:
			_Scr.x -= _hsPage;
			if(_Scr.x<0)
				_Scr.x = 0;
			break;
		case 3:
			_Scr.x += _hsPage;
			if(_Scr.x>_hsMax)
				_Scr.x = _hsMax;
			break;
		case SB_THUMBTRACK:
			{
				SCROLLINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cbSize = sizeof(si);
				si.fMask = SIF_TRACKPOS;
				if(GetScrollInfo(_Handle,SB_HORZ,&si))
					_Scr.x = si.nTrackPos;
				else
					_Scr.x = nPos;
				break;
			}
		case 6:
			_Scr.x = 0;
			break;
		case 7:
			_Scr.x = _hsMax;
			break;
		}
		int scCX = p - _Scr.x;
		if(scCX)
		{
			SetScrollPos(_Handle,0,_Scr.x,1);//this auto redraw the invalidate client;
			ScrollWindow(_Handle,scCX,0,NULL,NULL);
			OnScroll.Call(scCX,0,this);
		}
	}
	void ScrollView::_VScrollProc(Message* msg,IWnd*)
	{
		if(msg->lParam) return;
		else ::SendMessage((HWND)msg->lParam,WM_VSCROLL,msg->wParam,0);
		UINT nSBCode = LOWORD(msg->wParam);
		UINT nPos = HIWORD(msg->wParam);
		int p = _Scr.y;
		switch(nSBCode)
		{
		case SB_LINEUP:
			_Scr.y -= _vsLine;
			if(_Scr.y<0)
				_Scr.y = 0;
			break;
		case SB_LINEDOWN:
			_Scr.y += _vsLine;
			if(_Scr.y>_vsMax)
				_Scr.y = _vsMax;
			break;
		case SB_PAGEUP:
			_Scr.y -= _vsPage;
			if(_Scr.y<0)
				_Scr.y = 0;
			break;
		case SB_PAGEDOWN:
			_Scr.y += _vsPage;
			if(_Scr.y>_vsMax)
				_Scr.y = _vsMax;
			break;
			/*case SB_THUMBPOSITION://释放拖动.
			break;*/
		case SB_THUMBTRACK://拖动
			{
				SCROLLINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cbSize = sizeof(si);
				si.fMask = SIF_TRACKPOS;
				if(GetScrollInfo(_Handle,SB_VERT,&si))
					_Scr.y = si.nTrackPos;
				else
					_Scr.y = nPos;
				break;
			}
		case SB_TOP:
			_Scr.y = 0;
			break;
		case SB_BOTTOM:
			_Scr.y = _vsMax;
			break;
		}
		int scCY = p - _Scr.y;
		if(scCY)
		{
			SetScrollPos(_Handle,1,_Scr.y,1);
			ScrollWindow(_Handle,0,scCY,NULL,NULL);
			OnScroll.Call(0,scCY,this);
		}
	}
	void ScrollView::SetMouseWheelScroll(int vScroll)
	{
		switch(vScroll)
		{
		case 0:
			_MouseWheelScroll = WM_HSCROLL;
			break;
		case 1:
			_MouseWheelScroll = WM_VSCROLL;
			break;
		case 2:
			_MouseWheelScroll = 0;
			break;
		}
	}
	void ScrollView::SetMinSize(int cx,int cy)
	{
		if(cx<0) cx = _MinSize.x;
		if(cy<0) cy = _MinSize.y;
		if((cx==_MinSize.x)&&(cy==_MinSize.y)) return;
		_MinSize.x = cx;
		_MinSize.y = cy;
		if(_Handle){
			_SizeProc(0,this);
			_SizeProc(0,this);
		}
	}
	void ScrollView::SetMinSize(Twin size)
	{
		SetMinSize(size.x,size.y);
	}
	void ScrollView::SetResponseKeyBoard(bool response)
	{
		_ResponseKeyBoard = response;
	}
	void ScrollView::SetMinScroll(int hsLine, int vsLine)
	{
		if((hsLine==_hsLine)&&(vsLine==_vsLine)) return;
		if(hsLine>0) _hsLine = hsLine;
		else
		{
			if((vsLine<=0)||(vsLine==_vsLine)) return;
		}
		if(vsLine>0) _vsLine = vsLine;
		else
		{
			if((hsLine<=0)||(hsLine==_hsLine)) return;
		}
		_SizeProc(0,this);
	}
	void ScrollView::TransDimension(Twin& pt)
	{
		pt.Offset(-_Scr.x,-_Scr.y);
	}
	void ScrollView::TransDimension(Rect& rect)
	{
		rect.OffsetLocation(-_Scr.x,-_Scr.y);
	}
	bool ScrollView::Invalidate(const RECT* clip,bool erase)
	{
		if(clip==NULL)
			return ::InvalidateRect(_Handle,NULL,erase)!=0;
		else
		{
			Rect rc;
			rc = clip;
			rc.OffsetLocation(-_Scr.x,-_Scr.y);
			return ::InvalidateRect(_Handle,rc,erase)!=0;
		}
	}
	bool ScrollView::Invalidate(const HRGN clip,bool erase)
	{
		if(clip==0) return ::InvalidateRgn(_Handle,0,erase)!=0;
		else
		{
			Rgn rgn;
			rgn = clip;
			rgn.Offset(-_Scr.x,-_Scr.y);
			return ::InvalidateRgn(_Handle,rgn,erase)!=0;
		}
	}
	void ScrollView::Scroll(int dx,int dy)
	{
		if(dy)
		{
			dy += _Scr.y;
			if(dy<0)
				dy = 0;
			if(dy>_vsMax)
				dy = _vsMax;
			SetScrollPos(_Handle,1,dy,1);
			dy <<= 16;
			dy += SB_THUMBTRACK;
			::SendMessage(_Handle,WM_VSCROLL,dy,0);
		}
		if(dx)
		{
			dx += _Scr.x;
			if(dx<0)
				dx = 0;
			if(dx>_hsMax)
				dx = _hsMax;
			SetScrollPos(_Handle,0,dx,1);
			dx <<= 16;
			dx += SB_THUMBTRACK;
			::SendMessage(_Handle,WM_HSCROLL,dx,0);
		}
	}
	//Class Frame
	Frame::Frame(){
		Param->Style = WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS;
		Param->ExStyle |= WS_EX_CONTROLPARENT;//启用子窗口的双缓冲//这个风格回让窗口系统按钮失去鼠标滑过效果
	}
	Frame::~Frame(){
		//GetMsgHook(WM_DESTROY)->Delete(this,&Frame::_DestroyProc);
	}
	INT_PTR Frame::Create(HWND hParent,int nCmdShow,bool quit){
		if(quit){
			if(GetMsgHook(WM_DESTROY)->IndexOf(this,&Frame::_DestroyProc)==-1)
				GetMsgHook(WM_DESTROY)->Add(this,&Frame::_DestroyProc);
		}else{
			if(GetMsgHook(WM_DESTROY)->IndexOf(this,&Frame::_DestroyProc)!=-1)
				GetMsgHook(WM_DESTROY)->Delete(this,&Frame::_DestroyProc);
		}
		INT_PTR b = ScrollView::Create(hParent);
		if(b){//对于主窗口,生成时的WS_STYLE是无效的,总是被移除.
			::ShowWindow(_Handle,nCmdShow);//SW常数不支持位组合,只是一个枚举.而WS常数支持位组合,而且大体上是可以和SW对应的.
			::UpdateWindow(_Handle);
		}
		return b;
	}
	void Frame::_DestroyProc(Message* msg,IWnd*){
		//PostQuitMessage必须在启动线程里调用才有效，ExitProcess可以在任何线程调用，但是它并不结束正在运行的线程。
		//也就是说ExitProcess后面的代码实际上是不会被运行的。
		::PostQuitMessage(0);
	}
	void Frame::Quit(int code){
		if(Close())
			PostQuitMessage(code);
	}
	//Window Function
	void SetWndIcon(HWND h,ResID id)
	{
		Icon icon;
		icon.LoadIcon(id,16,16);
		SendMessage(h,WM_SETICON,0,(LPARAM)icon.Detach());
		icon.LoadIcon(id,32,32);
		SendMessage(h,WM_SETICON,1,(LPARAM)icon.Detach());
	}

	void WndDragger::SetWnd(IWnd* wnd){
		if(_wnd){
			_wnd->OnMouse.Delete(this,&WndDragger::onMouse);
		}
		if(wnd){
			wnd->OnMouse.Add(this,&WndDragger::onMouse);
		}
		_wnd = wnd;
	}

	void WndDragger::onMouse(Mouse* m,IWnd*){
		if(m->Event==WM_LBUTTONDOWN){
			::SetCapture(_wnd->Handle());
			_drag = 1;
			_x = m->X;
			_y = m->Y;
		}else if(m->Event==WM_LBUTTONUP){
			_drag = 0;
			::ReleaseCapture();
		}else if(m->Event==WM_MOUSEMOVE){
			if(!_drag) return;
			int dx = m->X - _x;
			int dy = m->Y - _y;
			if(dx==0&&dy==0) return;

			int rst = DRAG_X|DRAG_Y;
			if(!OnDrag.IsNull()&&!OnDrag.IsEmpty()){
				rst = OnDrag(dx,dy,this);
			}
			if((rst&DRAG_X)&&(rst&DRAG_Y)){
				_wnd->Offset(dx,dy,0,0);
			}else if(rst&DRAG_X){
				if(dx==0) return;
				_wnd->Offset(dx,0,0,0);
			}else if(rst&DRAG_Y){
				if(dy==0) return;
				_wnd->Offset(0,dy,0,0);
			}
		}
	}
}