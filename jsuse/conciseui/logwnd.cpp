
#include "stdafx.h"
#include "../conciseui.h"

namespace cs{
	typedef struct LINE : public cs::_struct{
		cs::String text;
		int color,width;
		LINE():width(0){}
	}LINE;
	class LogWnd : public cs::ScrollView{
	protected:
		cs::Font	font;
		int maxCount,lineSpace;//,maxWidth;
		cs::BitmapList	bmpList;
		cs::UserMenu	menu;
		cs::Rect margin,selRect;
		int selIndex;
		cs::Brush selBrush;
		cs::ObjectLink<LINE> lines;
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
				LINE* line = lines.Element();
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
		void onMouse(cs::Mouse* m,cs::IWnd*){
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
		void onDoubleDraw(cs::DC* dc,cs::View*){
			cs::Twin16 cs;
			GetClientSize(cs);
			font.Select(*dc);
			dc->SetBkTransparent(true);
			int lineH = font.LogFont()->lfHeight + lineSpace;
			int i = 0;
			selRect.OffsetLocation(-Scr().x,-Scr().y);
			FillRect(*dc,selRect,selBrush);
			selRect.OffsetLocation(Scr().x,Scr().y);
			for(lines.First();;lines.Move(1)){
				LINE* line = lines.Element();
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
				cs::Rect r(x,y,x,lineH+y);
				DrawText(*dc,line->text,line->text.Length(),r,DT_SINGLELINE|DT_CALCRECT);
				line->width = r.Width() + margin.left + margin.right;
				if(line->width>MinSize().x) SetMinSize(line->width,-1);
				DrawText(*dc,line->text,line->text.Length(),r,DT_SINGLELINE);
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
		/*int GetRequireRect(LPCWSTR str,HFONT font){
			HDC hdc = ::GetDC(0);
			cs::Rect r;
			r.SetValue(0,0,0,0);
			HGDIOBJ hf = SelectObject(hdc,font);
			::DrawText(hdc,str,wcslen(str),r,DT_CALCRECT|DT_SINGLELINE);
			SelectObject(hdc,hf);
			::ReleaseDC(0,hdc);
			return r.Width();
		}*/
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
			cs::Twin16 cs;
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
		void onCommand(WORD id,WORD,cs::IWnd*){
			if(id==0x100){
				if(selIndex<0) return;
				lines.First();
				lines.Move(selIndex);
				LINE* line = lines.Element();
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
	class OutputFrame : public cs::Frame{
	protected:
		cs::Label	_labPid;
		cs::ComboBox	_combPid;
		cs::Button	_checkDate,_checkApp;
		bool		_date,_app;
		int			_appIndex;
		StringList	_appList;
		cs::LogWnd _rich;
		cs::UserMenu	_rMenu;
		cs::Event	_waitCreate;

		void _onEraseBk(HDC hdc,cs::View*){
			cs::Twin16 sz;
			if(!this->GetClientSize(sz)) return;
			cs::Rect rect(0,0,sz.x,30),clip;
			::GetClipBox(hdc,clip);
			if(!rect.Intersect(clip)) return;
			HBRUSH bk = (HBRUSH)GetStockObject(DC_BRUSH);
			::SetDCBrushColor(hdc,RGB(236,233,216));
			::FillRect(hdc,rect,bk);
		}
		void _onCreate(cs::IWnd*){
			_rich.Create(_Handle);
			HICON icon = ::ExtractIcon(cs::GetInstance(),L"shell32.dll",76);
			SetIcon(icon);

			_checkDate.Create(_Handle);
			_checkDate.SetCheck(_date);
			_checkApp.Create(_Handle);
			_checkApp.SetCheck(_app);
			_labPid.Create(_Handle);
			_combPid.Create(_Handle);
			_combPid.AddString(L"全部");
			for(uint i=0;i<_appList.Count();i++)
				_combPid.AddString(_appList[i]);
			_combPid.SetCurSelect(_appIndex);
		}
		void _checkFilter(cs::Json* filter,LPCWSTR name,int width = 50,int color = 0,bool checked = true){
			if(!filter->GetProperty(name)){
				cs::Json* fil = filter->GetProperty(name,1);
				fil->SetPropertyBool(L"checked",checked);
				fil->SetPropertyInt32(L"color",color);
				fil->SetPropertyInt32(L"width",width);
			}
		}
		void _readCfg(){
			cs::Config cfg(0);
			cs::Json* root = cfg.Lock();
			cs::Json* ow = root->GetProperty(L"output_wnd",1);
			cs::Json* cbf = ow->GetProperty(_checkDate.Param->Text,1);
			_date = cbf->PropertyBool(L"checked",1);
			cbf = ow->GetProperty(_checkApp.Param->Text,1);
			_app = cbf->PropertyBool(L"checked",0);
			int x = 130;
			_labPid.Param->SetRect(x+20,10,30,20);
			_labPid.Param->Text = L"ID:";
			x += 50;
			_combPid.Param->SetRect(x,5,80,20);
			cs::Json* ids = ow->GetProperty(L"app",1);
			ids->SetToArray();
			if(ids->Count()==0)
				(*ids)[0] = L"jsuser";
			cs::String num;
			for(;!ids->ArrayOverFlow();){
				LPCWSTR app = 0;
				if(ids->GetArrayStr(app)){
					_appList.Add(app);
					ids->ArrayMove(1);
				}else{
					ids->RemoveArrayElm();
				}
			}
			cs::Json* id_index = ow->GetProperty(L"app_index",1);
			_appIndex = 0;
			if(!id_index->GetInt32(_appIndex)){
				*id_index = 0;
			}
			if(_appIndex<0||_appIndex>(int)ids->Count()){
				_appIndex = 0;
			}
		}
		void _onSize(cs::Twin16 cs,cs::IWnd*){
			_rich.SetSize(cs.x,cs.y-30);
		}
		void _onIDSelChanged(WORD id,WORD type,cs::IWnd*){
			if(CBN_SELCHANGE==type){
				cs::Config cfg(0);
				cs::Json* root = cfg.Lock();
				cs::Json* ow = root->GetProperty(L"output_wnd",1);
				cs::Json* id_index = ow->GetProperty(L"app_index",1);
				_appIndex = _combPid.GetCurSelect();
				*id_index = _appIndex;
			}
		}
		void _setCheckState(cs::Button& but){
			cs::Config cfg(0);
			cs::Json* root = cfg.Lock();
			cs::Json* ow = root->GetProperty(L"output_wnd",1);
			cs::Json* fc = ow->GetProperty(but.Param->Text,1);
			fc->SetPropertyBool(L"checked",0!=but.GetCheck());
		}
		void _onCheckChanged(WORD id,WORD type,cs::IWnd* ctrl){
			if(ctrl->Handle()==_checkDate){
				_setCheckState(_checkDate);
				_date = 0!=_checkDate.GetCheck();
			}else if(ctrl->Handle()==_checkApp){
				_setCheckState(_checkApp);
				_app = 0!=_checkApp.GetCheck();
			}
		}
		cs::Thread _thread;
		cs::CriticalSection _cs;
		void _onCreateFrame(bool quit){
			Create(0,1,quit);
			_waitCreate.Signal();
			_waitCreate.Close();
			cs::MsgLoop::Start();
			if(quit) ExitProcess(0);
		}
		void _onPrint(Message* msg,IWnd*){
			Log((LPCWSTR)msg->lParam,0xffffff);
		}
	public:
		OutputFrame(){
			BkColor = 0xff000000;
			Param->Text = L"输出";
			cs::SetWndRememberPos(this,L"output_wnd");
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
		void Log(LPCWSTR inf,int color = 0xffffff,LPCWSTR app = 0){
			int index = _combPid.GetCurSelect();
			if(index>0&&app){
				cs::String str;
				_combPid.GetItemText(index,str);
				if(str!=app) return;
			}
			cs::String str;
			if(_date){
				cs::SystemTime st;
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
		static OutputFrame outputFrame;
		return &outputFrame;
	}
	cs::CriticalSection _outputCs;
	void LogShowWindow(bool exit){
		cs::LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->Show(exit);
	}
	void LogSetFontSize(int size){
		cs::LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->SetFontSize(size);
	}
	void LogHideWindow(){
		cs::LocalCriticalSection lcs(_outputCs);
		::ShowWindow(*GetOutputFrame(),0);
	}
	void Log(LPCWSTR str,int color,LPCWSTR app){
		static cs::String appname = cs::App::GetAppName();
		cs::LocalCriticalSection lcs(_outputCs);
		if((INT_PTR)app==-1) app = appname;
		GetOutputFrame()->Log(str,color,app);
	}
	void Log(LPCSTR str,int color,LPCWSTR app){
		cs::String logstr = str;
		static cs::String appname = cs::App::GetAppName();
		cs::LocalCriticalSection lcs(_outputCs);
		if((INT_PTR)app==-1) app = appname;
		GetOutputFrame()->Log(logstr,color,app);
	}
	void LogSetBkColor(int color){
		cs::LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->SetBkColor(color);
	}
	void LogSetMaxLine(int count){
		cs::LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->SetMaxLine(count);
	}
	void LogClear(){
		cs::LocalCriticalSection lcs(_outputCs);
		GetOutputFrame()->Clear();
	}
}