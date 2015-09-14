#include "stdafx.h"
#include "../conciseui.h"

namespace cs{
	ICtrl* CreateCtrlObject(LPCWSTR className){
		if(WcsLength(className)==0){
			CONASSERT(L"className is null");
			return 0;
		}else if(WcsEqualNoCase(className,L"Static")){
			return (ICtrl*)new Label;
		}else if(WcsEqualNoCase(className,L"Button")){
			return new Button;
		}else if(WcsEqualNoCase(className,L"Edit")){
			return new Edit;
		}else if(WcsEqualNoCase(className,L"SysDateTimePick32")){
			return new DateTimePicker;
		}else if(WcsEqualNoCase(className,L"SysIPAddress32")){
			return new IpAddress;
		}else if(WcsEqualNoCase(className,L"SysLink")){
			return new Link;
		}else if(WcsEqualNoCase(className,L"SysHeader32")){
			return new Header;
		}else if(WcsEqualNoCase(className,L"msctls_updown32")){
			return new UpDown;
		}else if(WcsEqualNoCase(className,L"msctls_trackbar32")){
			return new TrackBar;
		}else if(WcsEqualNoCase(className,L"msctls_statusbar32")){
			return new StatusBar;
		}else if(WcsEqualNoCase(className,L"SysListView32")){
			return new ListView;
		}else if(WcsEqualNoCase(className,L"msctls_hotkey32")){
			return new HotKey;
		}else if(WcsEqualNoCase(className,L"SysTabControl32")){
			return new TabPage;
		}else if(WcsEqualNoCase(className,L"msctls_progress32")){
			return new ProgressBar;
		}else if(WcsEqualNoCase(className,L"ToolbarWindow32")){
			return new ToolBar;
		}else if(WcsEqualNoCase(className,L"SysTreeView32")){
			return new TreeView;
		}else if(WcsEqualNoCase(className,L"ComboBoxEx32")){
			return new ComboBoxEx;
		}else if(WcsEqualNoCase(className,L"ComboBox")){
			return new ComboBox;
		}else if(WcsEqualNoCase(className,L"ListBox")){
			return (ICtrl*)new ListBox;
		}else if(WcsEqualNoCase(className,L"SysMonthCal32")){
			return new MonthCalendar;
		}
		_ASSERT_EXPR(0,L"ICtrl.CreateCtrlObject not find class name");
		return new ICtrl;
	}

	void LayoutParam::SetVertical(){
		BitRemove(Style,(int)HORIZONTAL);
	}
	void LayoutParam::SetHorizontal(){
		Style |= HORIZONTAL;
	}
	void LayoutParam::Reset(){
		::ZeroMemory(this,sizeof(*this));
		Weight = 1;
	}
	void LayoutParam::SetDimen(int length,float weight){
		Length = length;
		Weight = weight;
	}
	void LayoutParam::SetMargin(int left,int top,int right,int bottom){
		Margin.SetValue(left,top,right,bottom);
	}
	LayoutParam::LayoutParam(){
		::ZeroMemory(this,sizeof(*this));
		Weight = 1;
	}
//
	inline bool weightIsZero(float weight){
		return ((weight<0.0001f)&&(weight>-0.0001f));
	}
//
	Layout::Layout(){
		_createWnd = 0;
	}
	Layout::~Layout(){
		if(_createWnd){
			_createWnd->Close();
			delete _createWnd;
		}
	}
	Twin Layout::_calcMinSize(Layout* lay,bool parentHor){
		Twin sz(0,0);
		if(parentHor){
			sz.x = lay->Param.Length;
		}else{
			sz.y = lay->Param.Length;
		}
		Twin ms(0,0);
		for(uint i=0;i<lay->_childs.Count();i++){
			Layout& child = lay->_childs[i];
			Twin childSz = _calcMinSize(&child,lay->Param.IsHorizontal());
			ms.x += childSz.x;
			ms.y += childSz.y;
		}
		if(sz.x<ms.x) sz.x = ms.x;
		if(sz.y<ms.y) sz.y = ms.y;

		sz.x += lay->Param.Margin.left + lay->Param.Margin.right;
		sz.y += lay->Param.Margin.top + lay->Param.Margin.bottom;
		return sz;
	}
	void Layout::_setRect(int left,int top,int width,int height){
		_location.x = (short)left;
		_location.y = (short)top;
		_size.x = (short)width;
		_size.y = (short)height;
		if(_wnd){
			_wnd.SetLocation(left,top);
			_wnd.SetSize(width,height);
		}
		if(Param.Style&LayoutParam::HORIZONTAL)
			_setChildHor(left,top,width,height);
		else
			_setChildVer(left,top,width,height);
	}
	void Layout::SetControl(HWND ctrl){
		if(_createWnd){
			_createWnd->Close();
			delete _createWnd;
			_createWnd = 0;
		}
		_wnd = ctrl;
	}
	void Layout::SetControl(LPCWSTR className,WORD id,LPCWSTR text,DWORD style,DWORD exstyle){
		if(WcsLength(className)!=0){
			if(_createWnd==0){
				_createWnd = CreateCtrlObject(className);
			}else{
				_createWnd->Close();
				_wnd = 0;
			}
			_createWnd->Param->ClassName = className;
			_createWnd->Param->Text = text;
			_createWnd->Param->Identity = id;
			if(style!=-1) _createWnd->Param->Style = style;
			if(exstyle!=-1) _createWnd->Param->ExStyle = exstyle;
		}else if(_createWnd){
			_createWnd->Close();
			delete _createWnd;
			_createWnd = 0;
			_wnd = 0;
		}
	}
	bool Layout::PtInLayout(int x,int y){
		Rect rect(_location.x,_location.y,_location.x+_size.x,_location.y+_size.y);
		return rect.PtInRect(x,y);
	}
	void Layout::ToJson(Json* js){
		js->SetPropertyStr(L"name",Name);
		js->SetPropertyInt32(L"style",Param.Style);
		js->SetPropertyDouble(L"weight",Param.Weight);
		js->SetPropertyInt32(L"length",Param.Length);
		js->SetPropertyInt32(L"margin_left",Param.Margin.left);
		js->SetPropertyInt32(L"margin_top",Param.Margin.top);
		js->SetPropertyInt32(L"margin_right",Param.Margin.right);
		js->SetPropertyInt32(L"margin_bottom",Param.Margin.bottom);
		if(_createWnd){
			js->SetPropertyStr(L"ctrl_class",_createWnd->Param->ClassName);
			js->SetPropertyStr(L"ctrl_text",_createWnd->Param->Text);
			js->SetPropertyInt32(L"ctrl_style",_createWnd->Param->Style);
			js->SetPropertyInt32(L"ctrl_exstyle",_createWnd->Param->ExStyle);
			js->SetPropertyInt32(L"ctrl_id",_createWnd->Param->Identity);
		}
		if(_childs.Count()>0){
			Json* childs = js->GetProperty(L"childs",1);
			for(uint i=0;i<_childs.Count();i++){
				_childs[i].ToJson(childs->AddArrayElm());
			}
		}
	}
	bool Layout::FromJson(Json* js){
		Clear();
		LayoutParam param;
		if(!js->GetPropertyInt32(L"style",param.Style)) return false;
		double v = 1.0;
		if(!js->GetPropertyDouble(L"weight",v)) return false;
		param.Weight = (float)v;
		if(!js->GetPropertyInt32(L"length",param.Length)) return false;
		if(!js->GetPropertyInt32(L"margin_left",param.Margin.left)) return false;
		if(!js->GetPropertyInt32(L"margin_top",param.Margin.top)) return false;
		if(!js->GetPropertyInt32(L"margin_right",param.Margin.right)) return false;
		if(!js->GetPropertyInt32(L"margin_bottom",param.Margin.bottom)) return false;
		LPCWSTR ctrlClass = js->PropertyStr(L"ctrl_class");
		if(WcsLength(ctrlClass)!=0){
			if(_createWnd){
				_createWnd->Close();
			}else if(!_createWnd){
				_createWnd = CreateCtrlObject(ctrlClass);
			}
			_createWnd->Param->ClassName = ctrlClass;
			_createWnd->Param->Text = js->PropertyStr(L"ctrl_text");
			js->GetPropertyInt32(L"ctrl_style",(int&)_createWnd->Param->Style);
			js->GetPropertyInt32(L"ctrl_exstyle",(int&)_createWnd->Param->ExStyle);
			int id = 0;
			if(js->GetPropertyInt32(L"ctrl_id",id))
				_createWnd->Param->Identity = id;
		}else{
			if(_createWnd){
				_createWnd->Close();
				delete _createWnd;
				_createWnd = 0;
			}
		}
		_wnd = 0;
		Param = param;
		Name = js->PropertyStr(L"name");
		Json* childs = js->GetProperty(L"childs");
		if(childs){
			for(childs->ArrayFirst();;childs->ArrayMove(1)){
				Json* child = childs->GetArrayElm();
				if(child==0) break;
				Layout* newLay = new Layout();
				if(!newLay->FromJson(child)){
					delete newLay;
					continue;
				}
				AddIn(newLay);
			}
		}
		return true;
	}
	void Layout::Reset(){
		Param.Reset();
		if(_createWnd){
			_createWnd->Close();
			delete _createWnd;
			_createWnd = 0;
		}
		Name = L"";
		_childs.Clear();
	}
	int Layout::_setChildHor(int left,int top,int width,int height){
		float sumWeight = 0;
		int sumDim = 0;
		for(uint i=0;i<_childs.Count();i++){
			LayoutParam& lp = _childs[i].Param;
			sumWeight += lp.Weight;
			sumDim += lp.Length + lp.Margin.left + lp.Margin.right;
		}
		int rest = width - sumDim;
		float x = (float)left;
		for(uint i=0;i<_childs.Count();i++){
			Layout& lay = _childs[i];
			LayoutParam& lp = lay.Param;
			float rationW;
			if(weightIsZero(sumWeight))
				rationW = (float)lp.Length;
			else
				rationW = lp.Length + rest*lp.Weight/sumWeight;
			x += lp.Margin.left;
			lay._setRect((int)x,top+lp.Margin.top,(int)(x+rationW)-(int)x,height-lp.Margin.top-lp.Margin.bottom);
			x += rationW + lp.Margin.right;
		}
		return sumDim;
	}
	int Layout::_setChildVer(int left,int top,int width,int height){
		float sumWeight = 0;
		int sumDim = 0;
		for(uint i=0;i<_childs.Count();i++){
			LayoutParam& lp = _childs[i].Param;
			sumWeight += lp.Weight;
			sumDim += lp.Length + lp.Margin.top + lp.Margin.bottom;
		}
		int rest = height - sumDim;
		float y = (float)top;
		for(uint i=0;i<_childs.Count();i++){
			Layout& lay = _childs[i];
			LayoutParam& lp = lay.Param;
			float rationH;
			if(weightIsZero(sumWeight))
				rationH = (float)lp.Length;
			else
				rationH = lp.Length + rest*lp.Weight/sumWeight;
			y += lp.Margin.top;
			lay._setRect(left+lp.Margin.left,(int)y,width-lp.Margin.left-lp.Margin.right,(int)(y+rationH)-(int)y/*消除舍1导致的一个像素的间距*/);
			y += rationH + lp.Margin.bottom;
		}
		return sumDim;
	}
	void Layout::_createControlTree(HWND parent){
		if(_createWnd!=0&&_wnd==0){
			_createWnd->Create(parent);
			_wnd = _createWnd->Handle();
		}
		for(uint i=0;i<_childs.Count();i++){
			_childs[i]._createControlTree(parent);
		}
	}
	void Layout::Refresh(HWND parent){
		if(parent) _createControlTree(parent);
		if(_parent){
			_parent->_setRect(_parent->_location.x,_parent->_location.y,_parent->_size.x,_parent->_size.y);
		}else{
			_setRect(_location.x,_location.y,_size.x,_size.y);
		}
	}
	bool Layout::Save(LPCWSTR file,DWORD code){
		Json json;
		ToJson(&json);
		return json.SaveToFile(file,code);
	}
	bool Layout::Load(LPCWSTR file,DWORD code){
		Json json;
		if(!json.LoadFromFile(file,code)) return false;
		Param.Reset();
		Name = L"";
		_childs.Clear();
		return FromJson(&json);
	}
	bool Layout::Load(ResID res,ResID rt,DWORD code){
		Param.Reset();
		Name = L"";
		_childs.Clear();
		DWORD size = 0;
		void* pm = GetResource(res,rt,&size);
		String str;
		str.FromMultiByte((LPCSTR)pm,size,code);
		Json json;
		if(!json.Parse(str)) return false;
		return FromJson(&json);
	}
	int Layout::Move(int pix){
		if(!_parent) return 0;
		uint index = _parent->IndexOf(this);
		if(index==0||index==(_parent->Count()-1)){
			//最前面和最后面的 Layout 无法调整位置。
			return 0;
		}

		Layout* preLay = _parent->Element(index-1);
		Layout* nextLay = _parent->Element(index+1);
		int dx1 = preLay->Param.Length + pix;
		int dx2 = nextLay->Param.Length - pix;
		int dx = Minimum(dx1,dx2);
		if(dx<0) pix += dx;

		if(pix==0) return 0;
		if(pix<0&&preLay->Param.Length<=0) return 0;
		if(pix>0&&nextLay->Param.Length<=0) return 0;

		preLay->Param.Length += pix;
		nextLay->Param.Length -= pix;
		_parent->_setRect(_parent->_location.x,_parent->_location.y,_parent->_size.x,_parent->_size.y);
		return pix;
	}
	Layout* Layout::GetElementByName(LPCWSTR name){
		if(Name==name) return this;
		for(uint i=0;i<_childs.Count();i++){
			Layout* lay = _childs[i].GetElementByName(name);
			if(lay) return lay;
		}
		return 0;
	}
	void TopLayout::_onSize(Message* msg,IWnd*){
		if(msg->wParam==SIZE_MINIMIZED) return;
		Twin16 cs = *((Twin16*)&msg->lParam);
		// Scroll 版本的
		int w = Maximum(_container->MinSize().x,(int)cs.x);
		int h = Maximum(_container->MinSize().y,(int)cs.y);
		_setRect(Param.Margin.left-_container->Scr().x,
			Param.Margin.top-_container->Scr().y,
			w-Param.Margin.left-Param.Margin.right,
			h-Param.Margin.top-Param.Margin.bottom);
		_container->Invalidate();
	}
	void TopLayout::Reset(){
		Layout::Reset();
		Name = L"Root";
		Param.Weight = 0;
		Param.Margin.SetValue(10,10,10,10);
	}
	TopLayout::TopLayout():_container(0){
		Name = L"Root";
		Param.Margin.SetValue(10,10,10,10);
		Param.Weight = 0;
	}
	void TopLayout::Refresh(HWND parent){
		Twin16 cs;
		if(!_container->GetClientSize(cs)) return;
		int w = Maximum(_container->MinSize().x,(int)cs.x);
		int h = Maximum(_container->MinSize().y,(int)cs.y);
		_location.SetValue(Param.Margin.left-_container->Scr().x,
			Param.Margin.top-_container->Scr().y);
		_size.SetValue(w-Param.Margin.left-Param.Margin.right,
			h-Param.Margin.top-Param.Margin.bottom);
		Layout::Refresh(*_container);
	}
	void TopLayout::SetContainer(IWnd* view){
		if(_container==view) return;
		if(_container!=NULL){
			_container->GetMsgHook(WM_SIZE)->Delete(this,&TopLayout::_onSize);
		}
		_container = view;
		if(!view) return;
		view->GetMsgHook(WM_SIZE)->Add(this,&TopLayout::_onSize);
		Refresh();
	}
	/*DragLayout::DragLayout():_dragLay(0){
	}
	void DragLayout::SetContainer(IWnd* view){
		if(_container==view) return;
		if(_container!=NULL){
			_container->GetMsgHook(WM_SIZE)->Delete(this,&DragLayout::_onSize);
			_container->OnMouse.Delete(this,&DragLayout::_onMouse);
		}
		_container = view;
		if(!view) return;
		view->GetMsgHook(WM_SIZE)->Add(this,&DragLayout::_onSize);
		view->OnMouse.Add(this,&DragLayout::_onMouse);
		Refresh();
	}
	bool ptInLayoutAndCanDrag(int x,int y,Layout* lay){
		if(!lay->Param.CanDrag()) return false;
		Twin16 lc = lay->GetLocation();
		Twin16 sz = lay->GetSize();
		Rect rect(lc.x,lc.y,lc.x+sz.x,lc.y+sz.y);
		return rect.PtInRect(x,y);
	}
	Layout* getDragLayout(int x,int y,Layout* lay){
		int count = lay->Count() - 1;
		for(int i=1;i<count;i++){
			Layout* child = lay->Element(i);
			if(ptInLayoutAndCanDrag(x,y,child)) return child;
		}
		count++;
		for(int i=0;i<count;i++){
			Layout* child = lay->Element(i);
			child = getDragLayout(x,y,child);
			if(child) return child;
		}
		return NULL;
	}
	void DragLayout::_calcPixelPerWeight(Layout* parent){
		float sumWeight = 0;
		int sumDim = 0;
		for(uint i=0;i<parent->Count();i++){
			LayoutParam& lp = parent->Element(i)->Param;
			sumWeight += lp.Weight;
			sumDim += lp.Length + lp.Margin.left + lp.Margin.right;
		}
		int rest = parent->GetSize().x - sumDim;
		_pixPerWeight = rest/sumWeight;
	}
	void DragLayout::_onMouse(Mouse* m){
		if(m->Event==WM_LBUTTONDOWN){
			_dragLay = getDragLayout(m->X,m->Y,this);
			if(_dragLay==0) return;
			int index = _dragLay->GetParent()->IndexOf(_dragLay);
			_preLay = _dragLay->GetParent()->Element(index-1);
			_nextLay = _dragLay->GetParent()->Element(index+1);
			Twin16 lc = _dragLay->GetLocation();
			_x = m->X - lc.x;
			_y = m->Y - lc.y;
			_calcPixelPerWeight(_dragLay->GetParent());
			::SetCapture(*_container);
			if(!OnDrag.IsNull()) OnDrag(_dragLay);
		}else if(m->Event==WM_LBUTTONUP){
			if(_dragLay==0) return;
			_dragLay = 0;
			::ReleaseCapture();
		}else if(m->Event==WM_MOUSEMOVE){
			if(_dragLay==0){
				return;
			}
			Layout* parent = _dragLay->GetParent();
			int dl;
			if(parent->Param.IsHorizontal()){
				int x = m->X - _dragLay->GetLocation().x;
				dl = x - _x;
			}else{
				int y = m->Y - _dragLay->GetLocation().y;
				dl = y - _y;
				//PrintD(L"dl:%d,y:%d,_y:%d,lc.y:%d",dl,y,_y,_dragLay->GetLocation().y);
			}
			//if(dl==0) return;
			//required changed weight.
			float dw = dl/_pixPerWeight;
			float npw = _preLay->Param.Weight + dw;
			float nnw = _nextLay->Param.Weight - dw;
			if(npw<0||nnw<0) return;
			_preLay->Param.Weight = npw;
			_nextLay->Param.Weight = nnw;
			parent->Refresh(*_container);
			if(!OnDrag.IsNull()) OnDrag(_dragLay);
		}
	}*/
}