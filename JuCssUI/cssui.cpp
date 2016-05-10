
#include "cssui.h"

namespace ju {

	void DivParam::SetVertical() {
		BitRemove(Style, (int)HORIZONTAL);
	}
	void DivParam::SetHorizontal() {
		Style |= HORIZONTAL;
	}
	void DivParam::Reset() {
		::ZeroMemory(this, sizeof(*this));
		Weight = 1;
	}
	void DivParam::SetDimen(int length, float weight) {
		Length = length;
		Weight = weight;
	}
	void DivParam::SetMargin(int left, int top, int right, int bottom) {
		Margin.SetValue(left, top, right, bottom);
	}
	void DivParam::SetParam(int length, float weight, int left, int top, int right, int bottom, bool hor) {
		if(hor) Style |= HORIZONTAL;
		else BitRemove(Style, (int)HORIZONTAL);
		Length = length;
		Weight = weight;
		Margin.SetValue(left, top, right, bottom);
	}
	DivParam::DivParam() {
		::ZeroMemory(this, sizeof(*this));
		Weight = 1;
	}
	//
	inline bool weightIsZero(float weight) {
		return ((weight<0.0001f) && (weight>-0.0001f));
	}
	//
	Div::Div() {
	}
	Div::~Div() {
	}
	void Div::SetParam(int length, float weight, int left, int top, int right, int bottom, bool hor) {
		Param.SetParam(length, weight, left, top, right, bottom, hor);
	}
	Twin Div::_calcMinSize(Div* lay, bool parentHor) {
		Twin sz(0, 0);
		if(parentHor) {
			sz.x = lay->Param.Length;
		} else {
			sz.y = lay->Param.Length;
		}
		Twin ms(0, 0);
		for(uint i = 0; i<lay->_childs.Count(); i++) {
			Div& child = lay->_childs[i];
			Twin childSz = _calcMinSize(&child, lay->Param.IsHorizontal());
			ms.x += childSz.x;
			ms.y += childSz.y;
		}
		if(sz.x<ms.x) sz.x = ms.x;
		if(sz.y<ms.y) sz.y = ms.y;

		sz.x += lay->Param.Margin.left + lay->Param.Margin.right;
		sz.y += lay->Param.Margin.top + lay->Param.Margin.bottom;
		return sz;
	}
	void Div::_setRect(int left, int top, int width, int height) {
		_location.x = (short)left;
		_location.y = (short)top;
		_size.x = (short)width;
		_size.y = (short)height;
		if(Param.Style&DivParam::HORIZONTAL)
			_setChildHor(left, top, width, height);
		else
			_setChildVer(left, top, width, height);
	}
	bool Div::PtInDiv(int x, int y) {
		Rect rect(_location.x, _location.y, _location.x + _size.x, _location.y + _size.y);
		return rect.PtInRect(x, y);
	}
	void Div::ToJson(Json* js) {
		js->SetPropertyStr(L"name", Name);
		js->SetPropertyInt32(L"style", Param.Style);
		js->SetPropertyDouble(L"weight", Param.Weight);
		js->SetPropertyInt32(L"length", Param.Length);
		js->SetPropertyInt32(L"margin_left", Param.Margin.left);
		js->SetPropertyInt32(L"margin_top", Param.Margin.top);
		js->SetPropertyInt32(L"margin_right", Param.Margin.right);
		js->SetPropertyInt32(L"margin_bottom", Param.Margin.bottom);
		if(_childs.Count()>0) {
			Json* childs = js->GetProperty(L"childs", 1);
			for(uint i = 0; i<_childs.Count(); i++) {
				_childs[i].ToJson(childs->AddArrayElm());
			}
		}
	}
	bool Div::FromJson(Json* js) {
		Clear();
		DivParam param;
		if(!js->GetPropertyInt32(L"style", param.Style)) return false;
		double v = 1.0;
		if(!js->GetPropertyDouble(L"weight", v)) return false;
		param.Weight = (float)v;
		if(!js->GetPropertyInt32(L"length", param.Length)) return false;
		if(!js->GetPropertyInt32(L"margin_left", param.Margin.left)) return false;
		if(!js->GetPropertyInt32(L"margin_top", param.Margin.top)) return false;
		if(!js->GetPropertyInt32(L"margin_right", param.Margin.right)) return false;
		if(!js->GetPropertyInt32(L"margin_bottom", param.Margin.bottom)) return false;
		Param = param;
		Name = js->PropertyStr(L"name");
		Json* childs = js->GetProperty(L"childs");
		if(childs) {
			for(uint i = 0; i<childs->Count(); i++) {
				Json* child = childs->GetArrayElm(i);
				if(child == 0) break;
				Div* newLay = new Div();
				if(!newLay->FromJson(child)) {
					delete newLay;
					continue;
				}
				AddIn(newLay);
			}
		}
		return true;
	}
	void Div::Reset() {
		Param.Reset();
		Name = L"";
		_childs.Clear();
	}
	int Div::_setChildHor(int left, int top, int width, int height) {
		float sumWeight = 0;
		int sumDim = 0;
		for(uint i = 0; i<_childs.Count(); i++) {
			DivParam& lp = _childs[i].Param;
			sumWeight += lp.Weight;
			sumDim += lp.Length + lp.Margin.left + lp.Margin.right;
		}
		int rest = width - sumDim;
		float x = (float)left;
		for(uint i = 0; i<_childs.Count(); i++) {
			Div& lay = _childs[i];
			DivParam& lp = lay.Param;
			float rationW;
			if(weightIsZero(sumWeight))
				rationW = (float)lp.Length;
			else
				rationW = lp.Length + rest*lp.Weight / sumWeight;
			x += lp.Margin.left;
			lay._setRect((int)x, top + lp.Margin.top, (int)(x + rationW) - (int)x, height - lp.Margin.top - lp.Margin.bottom);
			x += rationW + lp.Margin.right;
		}
		return sumDim;
	}
	int Div::_setChildVer(int left, int top, int width, int height) {
		float sumWeight = 0;
		int sumDim = 0;
		for(uint i = 0; i<_childs.Count(); i++) {
			DivParam& lp = _childs[i].Param;
			sumWeight += lp.Weight;
			sumDim += lp.Length + lp.Margin.top + lp.Margin.bottom;
		}
		int rest = height - sumDim;
		float y = (float)top;
		for(uint i = 0; i<_childs.Count(); i++) {
			Div& lay = _childs[i];
			DivParam& lp = lay.Param;
			float rationH;
			if(weightIsZero(sumWeight))
				rationH = (float)lp.Length;
			else
				rationH = lp.Length + rest*lp.Weight / sumWeight;
			y += lp.Margin.top;
			lay._setRect(left + lp.Margin.left, (int)y, width - lp.Margin.left - lp.Margin.right, (int)(y + rationH) - (int)y/*消除舍1导致的一个像素的间距*/);
			y += rationH + lp.Margin.bottom;
		}
		return sumDim;
	}
	void Div::_createControlTree(HWND parent) {
		for(uint i = 0; i<_childs.Count(); i++) {
			_childs[i]._createControlTree(parent);
		}
	}
	void Div::Refresh(HWND parent) {
		if(parent) _createControlTree(parent);
		if(_parent) {
			_parent->_setRect(_parent->_location.x, _parent->_location.y, _parent->_size.x, _parent->_size.y);
		} else {
			_setRect(_location.x, _location.y, _size.x, _size.y);
		}
	}
	bool Div::Save(LPCWSTR file, DWORD code) {
		Json json;
		ToJson(&json);
		return json.SaveToFile(file, code);
	}
	bool Div::Load(LPCWSTR file, DWORD code) {
		Json json;
		if(!json.LoadFromFile(file, code)) return false;
		Param.Reset();
		Name = L"";
		_childs.Clear();
		return FromJson(&json);
	}
	bool Div::Load(ResID res, ResID rt, DWORD code) {
		Param.Reset();
		Name = L"";
		_childs.Clear();
		DWORD size = 0;
		void* pm = GetResource(res, rt, &size);
		String str;
		str.FromMultiByte((LPCSTR)pm, size, code);
		Json json;
		if(!json.Parse(str)) return false;
		return FromJson(&json);
	}
	int Div::Move(int pix) {
		if(!_parent) return 0;
		uint index = _parent->IndexOf(this);
		if(index == 0 || index == (_parent->Count() - 1)) {
			//最前面和最后面的 Div 无法调整位置。
			return 0;
		}

		Div* preLay = _parent->Element(index - 1);
		Div* nextLay = _parent->Element(index + 1);
		int dx1 = preLay->Param.Length + pix;
		int dx2 = nextLay->Param.Length - pix;
		int dx = Minimum(dx1, dx2);
		if(dx<0) pix += dx;

		if(pix == 0) return 0;
		if(pix<0 && preLay->Param.Length <= 0) return 0;
		if(pix>0 && nextLay->Param.Length <= 0) return 0;

		preLay->Param.Length += pix;
		nextLay->Param.Length -= pix;
		_parent->_setRect(_parent->_location.x, _parent->_location.y, _parent->_size.x, _parent->_size.y);
		return pix;
	}
	Div* Div::GetElementByName(LPCWSTR name) {
		if(Name == name) return this;
		for(uint i = 0; i<_childs.Count(); i++) {
			Div* lay = _childs[i].GetElementByName(name);
			if(lay) return lay;
		}
		return 0;
	}
	void TopDiv::_onSize(Message* msg, IWnd*) {
		if(msg->wParam == SIZE_MINIMIZED) return;
		Twin16 cs = *((Twin16*)&msg->lParam);
		// Scroll 版本的
		int w = Maximum(_container->MinSize().x, (int)cs.x);
		int h = Maximum(_container->MinSize().y, (int)cs.y);
		_setRect(Param.Margin.left - _container->Scr().x,
			Param.Margin.top - _container->Scr().y,
			w - Param.Margin.left - Param.Margin.right,
			h - Param.Margin.top - Param.Margin.bottom);
		_container->Invalidate();
	}
	void TopDiv::Reset() {
		Div::Reset();
		Name = L"Root";
		Param.Weight = 0;
		Param.Margin.SetValue(10, 10, 10, 10);
	}
	TopDiv::TopDiv() :_container(0) {
		Name = L"Root";
		Param.Margin.SetValue(10, 10, 10, 10);
		Param.Weight = 0;
	}
	void TopDiv::Refresh(HWND parent) {
		Twin16 cs;
		if(!_container->GetClientSize(cs)) return;
		int w = Maximum(_container->MinSize().x, (int)cs.x);
		int h = Maximum(_container->MinSize().y, (int)cs.y);
		_location.SetValue(Param.Margin.left - _container->Scr().x,
			Param.Margin.top - _container->Scr().y);
		_size.SetValue(w - Param.Margin.left - Param.Margin.right,
			h - Param.Margin.top - Param.Margin.bottom);
		Div::Refresh(*_container);
	}
	void TopDiv::SetContainer(IWnd* view) {
		if(_container == view) return;
		if(_container != NULL) {
			_container->GetMsgHook(WM_SIZE)->Delete(this, &TopDiv::_onSize);
		}
		_container = view;
		if(!view) return;
		view->GetMsgHook(WM_SIZE)->Add(this, &TopDiv::_onSize);
		Refresh();
	}
	void DivBinder::_onSize(Message* msg, IWnd*) {
		if(msg->wParam == SIZE_MINIMIZED) return;
		Twin16 cs = *((Twin16*)&msg->lParam);
		// Scroll 版本的
		int w = Maximum(_container->MinSize().x, (int)cs.x);
		int h = Maximum(_container->MinSize().y, (int)cs.y);
		Lay._setRect(Lay.Param.Margin.left - _container->Scr().x,
			Lay.Param.Margin.top - _container->Scr().y,
			w - Lay.Param.Margin.left - Lay.Param.Margin.right,
			h - Lay.Param.Margin.top - Lay.Param.Margin.bottom);
		_container->Invalidate();
	}
	void DivBinder::Reset() {
		Lay.Reset();
		Lay.Name = L"Root";
		Lay.Param.Weight = 0;
		Lay.Param.Margin.SetValue(10, 10, 10, 10);
		if(_container != NULL) {
			_container->GetMsgHook(WM_SIZE)->Delete(this, &DivBinder::_onSize);
		}
		_container = NULL;
	}
	DivBinder::DivBinder() :_container(0), _containerIsScrollView(0) {
		Lay.Name = L"Root";
		Lay.Param.Margin.SetValue(10, 10, 10, 10);
		Lay.Param.Weight = 0;
	}
	void DivBinder::Refresh(HWND parent) {
		Twin16 cs;
		if(!_container->GetClientSize(cs)) return;
		int w = Maximum(_container->MinSize().x, (int)cs.x);
		int h = Maximum(_container->MinSize().y, (int)cs.y);
		Lay._location.SetValue(Lay.Param.Margin.left - _container->Scr().x,
			Lay.Param.Margin.top - _container->Scr().y);
		Lay._size.SetValue(w - Lay.Param.Margin.left - Lay.Param.Margin.right,
			h - Lay.Param.Margin.top - Lay.Param.Margin.bottom);
		Lay.Refresh(*_container);
		if(_containerIsScrollView) {
			Twin ms = GetMinSize();
			((ScrollView*)_container)->SetMinSize(ms.x, ms.y);
		}
	}
	void DivBinder::Bind(IWnd* wnd, bool isScrollView) {
		_containerIsScrollView = isScrollView;
		if(_container == wnd) return;
		if(_container != NULL) {
			_container->GetMsgHook(WM_SIZE)->Delete(this, &DivBinder::_onSize);
		}
		_container = wnd;
		if(!wnd) return;
		wnd->GetMsgHook(WM_SIZE)->Add(this, &DivBinder::_onSize);
		Refresh();
	}
}