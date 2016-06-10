#pragma once
#include "CssControl.h"
namespace ju {
	//CssLayoutParam
	struct JUWND_API CssLayoutParam : public _struct {
		enum {
			VERTICAL = 0,		//垂直布局: 子元素垂直方向排列, 水平自动.
			HORIZONTAL = 1		//水平布局: 子元素水平方向排列, 垂直自动.
		};
		Rect		Margin;
		float		Weight;
		int			Style;
		int			Length;

		void SetVertical();
		void SetHorizontal();
		inline bool IsHorizontal() { return (Style&HORIZONTAL) == HORIZONTAL; }
		inline bool IsVertical() { return (Style&HORIZONTAL) == 0; }
		void Reset();
		void SetDimen(int length, float weight);
		void SetMargin(int left, int top, int right, int bottom);
		void SetParam(int length, float weight = 1, int left = 0, int top = 0, int right = 0, int bottom = 0, bool hor = false);
		CssLayoutParam();
	};
	/**布局系统规则如下：
	1. 顶级布局总是充满窗口，但是留出边界，无视weight和length属性。因为顶级CssLayout只能有一个。
	2. 一个布局的多个子布局按这个布局是Horizontal或者Vertical属性来水平或竖直顺序排列。
	3. 子布局的大小按如下规则：水平布局，所有子布局的高是一样的；垂直布局，所有子布局的
	宽是一样的；都是加上边界宽度后充满父布局。水平布局时子布局的宽度分配和垂直布局子布局的
	高度分配，都遵循先分配每个子布局指定长度和边界值，剩余尺寸按权重（weight）分配给每个子
	布局的原则。注意：当剩余尺寸为负数的情况，将按权重缩小每个子布局的尺寸。
	4. 对于子布局权重总和为 0 的情况（小于0.00001），子布局尺寸按指定长度分配，而无视父布局的尺寸，所以这种情况
	可能导致子布局超出父布局的情形。

	当前布局尺寸分配仅仅取决于同级的布局的参数（Param）和父布局的尺寸，而不会受它的子布局的
	影响，也就是子布局的任何参数不会影响父布局的尺寸。

	例如：父布局实际分的的长度为110，有2个子布局，子布局的长度都是10，权重分别为1.0和2.0，
	那么，110 - （10 + 10） = 90，这个90就是按权重分配的可调尺寸，按比例，权重1.0的布局，分
	得30，权重2.0的布局分的60，他们的实际尺寸分别为10+30和10+60。如果他们的边界值都不是 0 ，
	那么还要从90里面减去边界值，剩下的尺寸才去分配。*/
	//CssLayout Object use for View layout,but pure CssLayout object not bind a View object.
	class JUWND_API CssLayout : public TPTree<CssLayout> {
		friend class CssLayoutBinder;
	protected:
		Twin16	_size;
		Twin16	_location;
		WndPtr	_wnd;		//这个参数是绑定的窗口。

		Twin _calcMinSize(CssLayout* lay, bool parentHor);
		//set realize dimension of this object,and his child will be resize
		void _setRect(int left, int top, int width, int height);
		//return fix size
		int _setChildHor(int left, int top, int width, int height);
		//return fix size
		int _setChildVer(int left, int top, int width, int height);
		//create all control in this object or his child.
		void _createControlTree(HWND parent);
	public:
		String			Name;
		CssLayoutParam		Param;

		CssLayout();
		~CssLayout();
		//set a created window as bind window,this will destroy inner control of _createWnd.
		void SetControl(HWND ctrl);
		//get bind window handle.
		HWND GetCtrlHandle() { return _wnd; }
		//get realize location of object
		Twin16 GetLocation() { return _location; }
		//get realize size of object
		Twin16 GetSize() { return _size; }
		//a point whether in this layout.
		bool PtInCssLayout(int x, int y);
		//clear all child and set object to default state
		void Reset();
		//convert to a json object.
		void ToJson(Json* json);
		//from a xml to create layout
		bool FromJson(Json* json);
		//when Param is changed or SetControl is called,call this function to refresh it and if parent != 0,will create control.
		virtual void Refresh(HWND parent);
		//save to a file normaly a .layout file
		bool Save(LPCWSTR file, DWORD code = CP_UTF8);
		//load from a .layout file.
		bool Load(LPCWSTR file, DWORD code = CP_UTF8);
		//load from resource .
		bool Load(ResID res, ResID rt = L"RT_LAYOUT", DWORD code = CP_UTF8);
		//return a layout that name is required.
		CssLayout* GetElementByName(LPCWSTR name);
		//移动当前 CssLayout，这个移动的本质是调整它的上一个和下一个同级 CssLayout 的 Length 属性，所以如果一个 CssLayout 没有同级的上一个或下一个 CssLayout，是无法调整位置的。
		//调整位置的幅度，是上一个或下一个同级 CssLayout 的 Length 属性不小于 0 。
		//返回实际调整的位置幅度。
		int Move(int pix);
		//快速设置 CssLayoutParam 参数
		void SetParam(int length, float weight = 1, int left = 0, int top = 0, int right = 0, int bottom = 0, bool hor = false);
	};
	//TopCssLayout 会接收绑定窗口的 WM_SIZE 消息，从而对子 CssLayout 的布局进行调整，TopCssLayout 是顶级 CssLayout ，它不能是其它 CssLayout 的子 CssLayout。
	class JUWND_API TopCssLayout : public CssLayout {
	protected:
		IWnd* _container;
		void _onSize(Message* msg, IWnd*);
	public:
		TopCssLayout();
		//获取布局所需的最新尺寸。
		Twin GetMinSize() {
			Param.Length = 0;
			return _calcMinSize(this, Param.IsHorizontal());
		}
		//this can be set before view is created.if view is a dlg set isdlg = 1 else set isdlg to 0
		void SetContainer(IWnd* view);
		//create all control and calc layout size,allways set parent param is 0,it is not use,this only to make it a override virtual function.
		//call this function in initialize,because no wm_size message was send,so every layout size has not calc.
		virtual void Refresh(HWND parent = 0);
		//clear all child and set object to default state
		void Reset();
	};
	class JUWND_API CssLayoutBinder {
	protected:
		IWnd*	_container;
		bool	_containerIsScrollView;
		void _onSize(Message* msg, IWnd*);
	public:
		CssLayout Lay;
		CssLayoutBinder();
		//获取布局所需的最小尺寸。
		Twin GetMinSize() {
			Lay.Param.Length = 0;
			return Lay._calcMinSize(&Lay, Lay.Param.IsHorizontal());
		}
		//this can be set before view is created.if view is a dlg set isdlg = 1 else set isdlg to 0
		void Bind(IWnd* view, bool isScrollView = false);
		//create all control and calc layout size,allways set parent param is 0,it is not use,this only to make it a override virtual function.
		//call this function in initialize,because no wm_size message was send,so every layout size has not calc.
		//这个函数通常不需要调用，当依附的主窗口尺寸变化时，CssLayout 会自动计算大小。
		//但是 CssLayout 不会自动生成控件，不会自动设置主窗口的最小滚动尺寸（当主窗口是ScrollView的时候），而且
		//只有WM_SIZE消息被发送时，子CssLayout的大小才会更新。所以如果CssLayout树发生变化时，包括参数变化，子CssLayout添加删除，
		//设置了新空间，需要调用Refresh来刷新变化。通常，如果仅仅是增删和更改了子CssLayout，而没有设置自动生成控件，
		//和不需要调整主窗口的最小滚动尺寸，可以不调用 Refresh。
		virtual void Refresh(HWND parent = 0);
		//clear all child and set object to default state
		void Reset();
	};
}
