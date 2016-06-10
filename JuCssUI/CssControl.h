#pragma once
namespace ju {
	struct BorderParam : public _struct {
		int Width,Style,Radius;
		Color Color;
		BorderParam() {
			::ZeroMemory(this, sizeof(this));
		}
	};
	struct Border : public _struct {
		BorderParam Left, Top, Right, Bottom;
	};
	struct TextParam {
		String Text,FontName;
		int Style,FontStyle,FontSize;
		TextParam():Style(0),FontStyle(0),FontSize(12){
		}
	};
	class CssControl : public _class {
	protected:
		Color color,background;
		Border border;
		Rect rect,padding;
		TextParam text;

		void onDraw(ju::DC* dc, ju::View*);
	public:
		Delegate<Mouse*, IWnd*>		OnMouse;	//MouseEvent处理所有鼠标消息.		
		Delegate<Twin16, IWnd*>		OnSize;		//WM_SIZE消息，参数是客户区的大小。
		Delegate<IWnd*>				OnClick;

		CssControl();
		void Resize(int cx,int cy);
		void Redraw(ju::DC* dc, ju::View*);
	};
}