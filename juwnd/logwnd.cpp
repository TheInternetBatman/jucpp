
#include "stdafx.h"
#include "../juwnd.h"
#include <stdio.h>

namespace ju{
	typedef struct LINE : public _struct{
		String text;
		int color,width;
		LINE():width(0){}
	}LINE;
	class LogWnd : public ScrollView{
	protected:
		Font	font;
		int maxCount,lineSpace;//,maxWidth;
		BitmapList	bmpList;
		UserMenu	menu;
		Rect margin,selRect;
		int selIndex;
		Brush selBrush;
		ObjectLink<LINE> lines;
		void setSelectRect(int clickY){
			if(lines.Count()<=0) return;
			int fontH = font.LogFont()->lfHeight;
			int lineH = fontH + lineSpace;
			clickY -= margin.top;
			int y = clickY + Scr().y;
			selIndex = y/lineH;
			if(selIndex>=(int)lines.Count()||lineH*selIndex+fontH<y){
				selRect.SetValue(0,0,0,0);
				selIndex = -1;
			}else{
				y = selIndex*lineH + margin.top;
				lines.First();
				lines.Move(selIndex);
				LINE* line = lines.GetElement();
				selRect.SetValue(margin.left,y,line->width-margin.right,fontH+y);
			}
			Invalidate();
		}
		void resetSelectRect(int decrease){
			if(selIndex==-1){
				selRect.SetValue(0,0,0,0);
			}else{
				int fontH = font.LogFont()->lfHeight;
				int lineH = fontH + lineSpace;
				selRect.OffsetLocation(0,-decrease*lineH);
			}
		}
		void onMouse(Mouse* m,IWnd*){
			if(m->Event==WM_RBUTTONDOWN||m->Event==WM_LBUTTONDOWN){
				setSelectRect(m->Y);
				SetFocus(_Handle);
			}else if(m->Event==WM_RBUTTONUP){
				if(menu.IsNull()){
					menu.SetOwnerWnd(this);
					menu.ImageList = &bmpList;
					Bitmap bmp;
					HICON hcopy = ExtractIcon(GetInstance(),L"shell32.dll",134);
					bmp.FromIcon(hcopy);
					::DestroyIcon(hcopy);
					bmp.Resize(24,24);
					HBITMAP copy = bmp.Detach();
					bmpList.Add(copy);

					menu.Param.ItemHeight = 34;
					menu.Param.LeftMargin = 44;
					menu.Create(1);
					menu.AddItem(0,L"Copy",0x100,copy);
				}
				menu.Popup();
			}
		}
		void onDoubleDraw(DC* dc,View*){
			Twin16 cs;
			GetClientSize(cs);
			font.Select(*dc);
			dc->SetBkTransparent(true);
			int lineH = font.LogFont()->lfHeight + lineSpace;
			int i = 0;
			selRect.OffsetLocation(-Scr().x,-Scr().y);
			FillRect(*dc,selRect,selBrush);
			selRect.OffsetLocation(Scr().x,Scr().y);
			DRAWTEXTPARAMS dtp;
			::ZeroMemory(&dtp,sizeof(dtp));
			dtp.cbSize = sizeof(dtp);
			dtp.iTabLength = 4;
			for(lines.First();;lines.Move(1)){
				LINE* line = lines.GetElement();
				if(!line) break;
				int y = margin.top + i*lineH - Scr().y;
				if(y<-lineH){
					i++;
					continue;
				}else if(y>cs.y){
					break;
				}
				dc->SetTextColor(line->color);
				int x = margin.left - Scr().x;
				Rect r(x,y,x,lineH+y);
				DrawTextEx(*dc,line->text,line->text.Length(),r,DT_SINGLELINE|DT_CALCRECT|DT_EXPANDTABS,&dtp);
				line->width = r.Width() + margin.left + margin.right;
				if(line->width>MinSize().x) SetMinSize(line->width,-1);
				DrawTextEx(*dc,line->text,line->text.Length(),r,DT_SINGLELINE|DT_EXPANDTABS,&dtp);
				i++;
			}
			font.Select(*dc);
		}
		void addLine(LPCWSTR text,uint pos,uint len,int color){
			LINE* line;
			if(lines.Count()==maxCount){
				line = lines.PushBack();
			}else{
				line = lines.Add();
			}
			line->text = L"";
			line->text.CopyFrom(text+pos,len);
			line->color = color;
		}
	public:
		LogWnd():maxCount(1000),lineSpace(5),selIndex(-1){
			margin.SetValue(10,10,10,10);
			selRect.SetValue(0,0,0,0);
			selBrush.Create(RGB(51,153,255));
			OnDoubleDraw.Add(this,&LogWnd::onDoubleDraw);
			OnMouse.Add(this,&LogWnd::onMouse);
			OnCommand.Add(this,&LogWnd::onCommand);
			BkColor = 0x0;
			font.SetSize(16);
			font.Create();
		}
		void AddLine(LPCWSTR text,int color){
			uint len = WcsLength(text);
			uint pos = 0,inc = lines.Count(),newCount = 0;
			for(uint i=0;i<len;i++){
				if(text[i]=='\r'){
					addLine(text,pos,i-pos,color);
					pos = i+1;newCount++;
					if(text[pos]=='\n'){
						i++;
						pos++;
					}
				}else if(text[i]=='\n'){
					addLine(text,pos,i-pos,color);
					pos = i+1;newCount++;
				}
			}
			addLine(text,pos,-1,color);newCount++;
			inc = lines.Count() - inc;
			newCount -= inc;
			if(newCount!=0&&selIndex!=-1){
				selIndex -= newCount;
				if(selIndex<0) selIndex = -1;
				resetSelectRect(newCount);
			}

			int lineH = font.LogFont()->lfHeight + lineSpace;
			int h = lines.Count()*lineH + margin.top + margin.bottom - lineSpace;
			SetMinSize(-1,h);
		}
		void UpdateDisplay(){
			if(!_Handle) return;
			Twin16 cs;
			GetClientSize(cs);
			int pos1 = cs.y+Scr().y;
			int pos2 = MinSize().y-(font.LogFont()->lfHeight+lineSpace)*3;
			if(pos1>=pos2){
				ScrollEnd(0,1);
			}
			Invalidate();
		}
		void Clear(){
			lines.Clear();
			SetMinSize(0,0);
			selRect.SetValue(0,0,0,0);
			Invalidate();
		}
		void SetMaxLine(int count){
			if(count<1) return;
			if(maxCount==count) return;
			maxCount = count;
			int h;
			while(lines.Count()>(uint)maxCount){
				lines.Delete(LINK_FIRST);
			}
			int lineH = font.LogFont()->lfHeight + lineSpace;
			h = lines.Count()*lineH + margin.top + margin.bottom - lineSpace;
			SetMinSize(-1,h);
		}
		void SetFontSize(int size){
			if(font.LogFont()->lfHeight==size) return;
			font.SetSize(size);
			font.Create();
			int lineH = font.LogFont()->lfHeight + lineSpace;
			int h = lines.Count()*lineH + margin.top + margin.bottom - lineSpace;
			SetMinSize(0,h);
			UpdateDisplay();
		}
		void onCommand(WORD id,WORD,IWnd*){
			if(id==0x100){
				if(selIndex<0) return;
				lines.First();
				lines.Move(selIndex);
				LINE* line = lines.GetElement();
				if(!line) return;

				OpenClipboard(_Handle);
				BOOL b = EmptyClipboard();
				HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, line->text.Length()*2+2);
				LPWSTR buffer = (wchar_t*)GlobalLock(clipbuffer);
				memcpy(buffer,line->text.Handle(),line->text.Length()*2+2);
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_UNICODETEXT,clipbuffer);
				CloseClipboard();
			}
		}
	};
	
	void logf(Memory<wchar_t>& buf,LPCWSTR fms,va_list vaList){
		int bufSize = _vscwprintf(fms,vaList);
		if(bufSize==-1){
			return;
		}
		buf.SetLength(bufSize+1);
		int n = vswprintf_s(buf,bufSize+1,fms,vaList);
	}
	void logf(Memory<char>& buf,LPCSTR fms,va_list vaList){
		int bufSize = _vscprintf(fms,vaList);
		if(bufSize==-1){
			return;
		}
		buf.SetLength(bufSize+1);
		int n = vsprintf_s(buf,bufSize+1,fms,vaList);
	}
	LogView::LogView():_handle(0){}
	LogView::~LogView(){
		if(_handle){
			delete (LogWnd*)_handle;
		}
	}
	HWND LogView::Create(HWND parent){
		LogWnd* lw = new LogWnd();
		if(!lw->Create(parent)){
			delete lw;
		}
		_handle = lw;
		return *lw;
	}
	void LogView::Destroy(){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		::DestroyWindow(*lw);
		delete lw;
		_handle = NULL;
	}
	void LogView::SetBkColor(int bkColor){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		lw->BkColor = bkColor;
		lw->Invalidate();
	}
	void LogView::SetMaxLine(int count){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		lw->SetMaxLine(count);
	}
	void LogView::SetFontSize(int size){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		lw->SetFontSize(size);
	}
	void LogView::Log(LPCWSTR str,int color){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		lw->AddLine(str,color);
		lw->UpdateDisplay();
	}
	void LogView::Log(LPCSTR str,int color){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		String s = str;
		lw->AddLine(s,color);
		lw->UpdateDisplay();
	}
	void LogView::Logf(int color,LPCWSTR fms,...){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		Memory<wchar_t> buf;
		va_list vaList;
		va_start(vaList,fms);
		logf(buf,fms,vaList);
		va_end(vaList);
		Log(buf,color);
	}
	void LogView::Logf(int color,LPCSTR fms,...){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		Memory<char> buf;
		va_list vaList;
		va_start(vaList,fms);
		logf(buf,fms,vaList);
		va_end(vaList);
		Log(buf,color);
	}
	void LogView::Logf(LPCWSTR fms,...){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		Memory<wchar_t> buf;
		va_list vaList;
		va_start(vaList,fms);
		logf(buf,fms,vaList);
		va_end(vaList);
		Log(buf);
	}
	void LogView::Logf(LPCSTR fms,...){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		Memory<char> buf;
		va_list vaList;
		va_start(vaList,fms);
		logf(buf,fms,vaList);
		va_end(vaList);
		Log(buf);
	}
	void LogView::Clear(){
		if(_handle==NULL) return;
		LogWnd* lw = (LogWnd*)_handle;
		lw->Clear();
	}

	class OutputFrame : public Frame{
	protected:
		Font	_font;
		Label	_labPid;
		ComboBox	_combPid;
		Button	_checkDate,_checkApp,_checkTop,_butClear;
		bool		_date,_app,_top;
		int			_appIndex;
		StringList	_appList;
		LogWnd _rich;
		UserMenu	_rMenu;
		Event	_waitCreate;
		int			_setColor;
		String	_setApp;

		void _onEraseBk(HDC hdc,View*){
			Twin16 sz;
			if(!this->GetClientSize(sz)) return;
			Rect rect(0,0,sz.x,30),clip;
			::GetClipBox(hdc,clip);
			if(!rect.Intersect(clip)) return;
			HBRUSH bk = (HBRUSH)GetStockObject(DC_BRUSH);
			::SetDCBrushColor(hdc,RGB(236,233,216));
			::FillRect(hdc,rect,bk);
		}
		void _onCreate(IWnd*){
			_rich.Create(_Handle);
			HICON icon = ::ExtractIcon(GetInstance(),L"shell32.dll",76);
			SetIcon(icon);

			_checkDate.Create(_Handle);
			_checkDate.SetFont(_font);
			_checkDate.SetCheck(_date);
			_checkApp.Create(_Handle);
			_checkApp.SetFont(_font);
			_checkApp.SetCheck(_app);
			_checkTop.Create(_Handle);
			_checkTop.SetFont(_font);
			_checkTop.SetCheck(_top);
			if(_top)
				SetWindowPos(_Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			_butClear.Create(_Handle);
			_butClear.SetFont(_font);
			_labPid.Create(_Handle);
			_labPid.SetFont(_font);
			_combPid.Create(_Handle);
			_combPid.SetFont(_font);
			_combPid.AddString(L"全部");
			for(uint i=0;i<_appList.Count();i++)
				_combPid.AddString(_appList[i]);
			_combPid.SetCurSelect(_appIndex);
		}
		void _checkFilter(Json* filter,LPCWSTR name,int width = 50,int color = 0,bool checked = true){
			if(!filter->GetProperty(name)){
				Json* fil = filter->GetProperty(name,1);
				fil->SetPropertyBool(L"checked",checked);
				fil->SetPropertyInt32(L"color",color);
				fil->SetPropertyInt32(L"width",width);
			}
		}
		void _readCfg(){
			Config cfg(0);
			Json* root = cfg.Lock();
			Json* ow = root->GetProperty(L"output_wnd",1);
			Json* cbf = ow->GetProperty(_checkDate.Param->Text,1);
			_date = cbf->PropertyBool(L"checked",1);
			cbf = ow->GetProperty(_checkApp.Param->Text,1);
			_app = cbf->PropertyBool(L"checked",0);
			cbf = ow->GetProperty(_checkTop.Param->Text,1);
			_top = cbf->PropertyBool(L"checked",0);
			int x = 230;
			_labPid.Param->SetRect(x+20,10,30,20);
			_labPid.Param->Text = L"ID:";
			x += 50;
			_combPid.Param->SetRect(x,5,80,20);
			Json* ids = ow->GetProperty(L"app",1);
			ids->SetToArray();
			if(ids->Count()==0)
				(*ids)[0] = L"app name";
			String num;
			for(uint i=0;i<ids->Count();i++){
				LPCWSTR app = 0;
				if(ids->GetArrayStr(app,i)){
					_appList.Add(app);
				}else{
					ids->RemoveArrayElm(i);
					i--;
				}
			}
			Json* id_index = ow->GetProperty(L"app_index",1);
			_appIndex = 0;
			if(!id_index->GetInt32(_appIndex)){
				*id_index = 0;
			}
			if(_appIndex<0||_appIndex>(int)ids->Count()){
				_appIndex = 0;
			}
		}
		void _onSize(Twin16 cs,IWnd*){
			_rich.SetSize(cs.x,cs.y-30);
		}
		void _onIDSelChanged(WORD id,WORD type,IWnd*){
			if(CBN_SELCHANGE==type){
				Config cfg(0);
				Json* root = cfg.Lock();
				Json* ow = root->GetProperty(L"output_wnd",1);
				Json* id_index = ow->GetProperty(L"app_index",1);
				_appIndex = _combPid.GetCurSelect();
				*id_index = _appIndex;
			}
		}
		void _setCheckState(Button& but){
			Config cfg(0);
			Json* root = cfg.Lock();
			Json* ow = root->GetProperty(L"output_wnd",1);
			Json* fc = ow->GetProperty(but.Param->Text,1);
			fc->SetPropertyBool(L"checked",0!=but.GetCheck());
		}
		void _onCheckChanged(WORD id,WORD type,IWnd* ctrl){
			if(ctrl->Handle()==_checkDate){
				_setCheckState(_checkDate);
				_date = 0!=_checkDate.GetCheck();
			}else if(ctrl->Handle()==_checkApp){
				_setCheckState(_checkApp);
				_app = 0!=_checkApp.GetCheck();
			}else if(ctrl->Handle()==_checkTop){
				_setCheckState(_checkTop);
				_top = 0!=_checkTop.GetCheck();
				if(_top)
					SetWindowPos(_Handle,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
				else
					SetWindowPos(_Handle,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}else if(ctrl->Handle()==_butClear){
				Clear();
			}
		}
		Thread _thread;
		CriticalSection _cs;
		void _onCreateFrame(bool quit){
			Create(0,1,quit);
			_waitCreate.Signal();
			_waitCreate.Close();
			MsgLoop::Start();
			if(quit) ExitProcess(0);
		}
		void _onPrint(Message* msg,IWnd*){
			Log((LPCWSTR)msg->lParam,0xffffff);
		}
	public:
		OutputFrame(){
			_setColor = 0xffffff;
			_font.SetSize(12);
			_font.Create();
			BkColor = 0xff000000;
			Param->Text = L"输出";
			SetWndRememberPos(this,L"output_wnd");
			_rich.Param->Location.SetValue(0,30);
			OnCreate.Add(this,&OutputFrame::_onCreate);
			OnEraseBkgnd.Add(this,&OutputFrame::_onEraseBk);
			OnSize.Add(this,&OutputFrame::_onSize);
			_combPid.OnCommand.Add(this,&OutputFrame::_onIDSelChanged);
			GetMsgHook(WM_LOGPRINTTEXT)->Add(this,&OutputFrame::_onPrint);

			_checkDate.OnCommand.Add(this,&OutputFrame::_onCheckChanged);
			_checkDate.Param->Text = L"时间";
			_checkDate.Param->SetRect(10,0,50,30);
			_checkDate.Param->AddStyle(BS_AUTOCHECKBOX);

			_checkApp.OnCommand.Add(this,&OutputFrame::_onCheckChanged);
			_checkApp.Param->Text = L"名称";
			_checkApp.Param->SetRect(60,0,50,30);
			_checkApp.Param->AddStyle(BS_AUTOCHECKBOX);

			_checkTop.OnCommand.Add(this,&OutputFrame::_onCheckChanged);
			_checkTop.Param->Text = L"置顶";
			_checkTop.Param->SetRect(110,0,50,30);
			_checkTop.Param->AddStyle(BS_AUTOCHECKBOX);

			_butClear.OnCommand.Add(this,&OutputFrame::_onCheckChanged);
			_butClear.Param->Text = L"清除";
			_butClear.Param->SetRect(160,3,50,25);

			_readCfg();
		}
		~OutputFrame(){
			;
		}
		void Show(bool quit = true){
			if(_Handle){
				::ShowWindow(_Handle,1);
			}else{
				_thread.Close();
				_waitCreate.Create(1,0);
				_thread.Call<void>(this,&OutputFrame::_onCreateFrame,quit);
				_waitCreate.Wait();
			}
		}
		void SetFontSize(int size){
			_rich.SetFontSize(size);
		};
		void SetBkColor(int color){
			_rich.BkColor = color;
			_rich.Invalidate();
		}
		void SetMaxLine(int count){
			_rich.SetMaxLine(count);
		}
		void Clear(){
			_rich.Clear();
		}
		void Set(int color,LPCWSTR app = 0){
			_setColor = color;
			if(app) _setApp = app;
			if(_setApp.IsNull()) _setApp.Release();
		}
		void Log(LPCWSTR inf){
			Log(inf,_setColor,_setApp);
		}
		void Log(LPCWSTR inf,int color,LPCWSTR app = 0){
			int index = _combPid.GetCurSelect();
			if(index>0&&app){
				String str;
				_combPid.GetItemText(index,str);
				if(str!=app) return;
			}
			String str;
			if(_date){
				SystemTime st;
				st.GetLocalTime();
				if(app&&_app)
					str.Format(L"%02d:%02d:%02d.%03d (%s) %s",st.Hour,st.Minute,st.Second,st.Milliseconds,app,inf);
				else
					str.Format(L"%02d:%02d:%02d.%03d %s",st.Hour,st.Minute,st.Second,st.Milliseconds,inf);
			}else{
				if(app&&_app)
					str.Format(L"(%s) %s",app,inf);
				else
					str.Format(L"%s",inf);
			}
			_rich.AddLine(str,color);
			_rich.UpdateDisplay();
		}
	};

	OutputFrame* GetOutputFrame(){
		static OutputFrame* outputFrame = 0;
		if(outputFrame==0){
			MutexLock ml(L"{A1A1C078-4BDD-4593-8CE7-3B7258A3EF9C}");
			if(outputFrame==0){
				outputFrame = NewStd<OutputFrame>();
			}
		}
		return outputFrame;
	}
	CriticalSection _outputCs;
	void LogShowWindow(bool exit){
		LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->Show(exit);
	}
	void LogSetFontSize(int size){
		LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->SetFontSize(size);
	}
	void LogHideWindow(){
		LocalCriticalSection lcs(_outputCs);
		::ShowWindow(*GetOutputFrame(),0);
	}
	void LogCloseWindow(){
		LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->Close();
	}
	void LogSet(int color,LPCWSTR app){
		LocalCriticalSection lcs(_outputCs);
		if((INT_PTR)app==-1) app = App::GetAppName();
		GetOutputFrame()->Set(color,app);
	}
	void Log(LPCWSTR str){
		LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->Log(str);
	}
	void Log(LPCSTR str){
		String logstr = str;
		Log(logstr);
	}
	void Log(LPCWSTR str,int color,LPCWSTR app){
		LocalCriticalSection lcs(_outputCs);
		if((INT_PTR)app==-1) app = App::GetAppName();
		GetOutputFrame()->Log(str,color,app);
	}
	void Log(LPCSTR str,int color,LPCWSTR app){
		String logstr = str;
		Log(logstr,color,app);
	}
	void logf(int color,LPCWSTR fms,va_list vaList){
		int bufSize = _vscwprintf(fms,vaList);
		if(bufSize==-1){
			return;
		}
		Memory<wchar_t> buf;
		buf.SetLength(bufSize+1);
		int n = vswprintf_s(buf,bufSize+1,fms,vaList);
		Log(buf,color);
	}
	void logf(int color,LPCSTR fms,va_list vaList){
		int bufSize = _vscprintf(fms,vaList);
		if(bufSize==-1){
			return;
		}
		Memory<char> buf;
		buf.SetLength(bufSize+1);
		int n = vsprintf_s(buf,bufSize+1,fms,vaList);
		Log(buf,color);
	}
	void Logf(int color,LPCWSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(color,fms,vaList);
		va_end(vaList);
	}
	void Logf(int color,LPCSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(color,fms,vaList);
		va_end(vaList);
	}
	void LogfD(LPCWSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(0xffffff,fms,vaList);
		va_end(vaList);
	}
	void LogfD(LPCSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(0xffffff,fms,vaList);
		va_end(vaList);
	}
	void LogfI(LPCWSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(0xff00,fms,vaList);
		va_end(vaList);
	}
	void LogfI(LPCSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(0xff00,fms,vaList);
		va_end(vaList);
	}
	void LogfE(LPCWSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(0xff,fms,vaList);
		va_end(vaList);
	}
	void LogfE(LPCSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(0xff,fms,vaList);
		va_end(vaList);
	}
	void LogfW(LPCSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(0xffff,fms,vaList);
		va_end(vaList);
	}
	void LogfW(LPCWSTR fms,...){
		va_list vaList;
		va_start(vaList,fms);
		logf(0xffff,fms,vaList);
		va_end(vaList);
	}
	void Logf(LPCWSTR fms,...){
		Memory<wchar_t> buf;
		va_list vaList;
		va_start(vaList,fms);
		int bufSize = _vscwprintf(fms,vaList);
		if(bufSize==-1){
			va_end(vaList);
			return;
		}
		buf.SetLength(bufSize+1);
		int n = vswprintf_s(buf,bufSize+1,fms,vaList);
		va_end(vaList);
		buf[bufSize] = 0;
		Log(buf);
	}
	void Logf(LPCSTR fms,...){
		Memory<char> buf;
		va_list vaList;
		va_start(vaList,fms);
		int bufSize = _vscprintf(fms,vaList);
		if(bufSize==-1){
			va_end(vaList);
			return;
		}
		buf.SetLength(bufSize+1);
		int n = vsprintf_s(buf,bufSize+1,fms,vaList);
		va_end(vaList);
		buf[bufSize] = 0;
		Log(buf);
	}
	void LogSetBkColor(int color){
		LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->SetBkColor(color);
	}
	void LogSetMaxLine(int count){
		LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->SetMaxLine(count);
	}
	void LogClear(){
		LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->Clear();
	}
}