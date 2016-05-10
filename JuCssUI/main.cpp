#include "../juwnd.h"
#include "cssui.h"
#pragma usexpstyle

class Main : public ju::Frame {
protected:
	ju::Json layout;
	ju::Json defcss;
	ju::View body;

	void onDraw(ju::DC* dc, ju::View*) {
		font.Select(*dc);
		dc->SetBkTransparent(true);
		dc->SetTextColor(0xffffff);
		ju::String str = L"年8月8日 -&为	了实现 阴影	效果，这两天google了不少，终于从 SysShadow 中找到了 ... 的话，TxDraw函数的HDC来源也是一个比较麻烦的事：用GDI的HDC，则 ...";
		ju::Rect rc;
		body.GetRect(rc);
		DRAWTEXTPARAMS dtp;
		::ZeroMemory(&dtp, sizeof(dtp));
		dtp.cbSize = sizeof(dtp);
		dtp.iTabLength = 4;
		DWORD err = ::DrawTextEx(*dc, str, str.Length(), rc, DT_NOCLIP|DT_WORDBREAK|DT_NOPREFIX | DT_EXPANDTABS | DT_EDITCONTROL, &dtp);
		INT nFit;
		INT nDx[1000];
		nDx[0] = 80;
		nDx[1] = 160;
		nDx[2] = 240;
		nDx[3] = 320;
		SIZE size;
		DWORD ext;
		//DWORD ext = ::GetTextExtentExPoint(*dc, str, str.Length(), 1000, &nFit, nDx, &size);
		//ju::Print(L"size:%d X %d,%d,%d,%d,%d,%d",size.cx,size.cy, nFit, nDx[0], nDx[1], nDx[2], nDx[3]);
		ext = ::GetTabbedTextExtent(*dc, str, str.Length(), 4, nDx);
		ju::Print(L"%dX%d,%d,%d,%d,%d",HIWORD(ext),LOWORD(ext), nDx[0], nDx[1], nDx[2], nDx[3]);
		font.Select(*dc);
	}
	void onSize(ju::Twin16 cs,ju::IWnd*) {
		body.SetDimension(0, 0, cs.x, cs.y);
	}
	void onCreate(ju::IWnd*) {
		//initDefCss();
		//layout.LoadFromFile(L"main.js");
		body.Create(_Handle);
		body.BkColor = 0x000000;
	}
	void initDefCss() {
		defcss.SetPropertyInt32(L"width", -1);
		defcss.SetPropertyInt32(L"height", 0);
	}
	ju::Font font;
public:
	Main() {
		Param->Text = "JuCssUI";
		font.SetSize(20);
		font.Create();

		OnSize.Add(this, &Main::onSize);
		OnCreate.Add(this, &Main::onCreate);
		body.OnDraw.Add(this, &Main::onDraw);
	}
};
WINMAIN{
	Main main;
	main.Create();
	ju::MsgLoop::Start();
}
