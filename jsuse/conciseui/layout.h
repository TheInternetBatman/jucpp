#pragma once
namespace cs{
	//LayoutParam
	struct CONCISEUI_API LayoutParam : public _struct{
		enum{
			VERTICAL = 0,		//垂直布局: 子元素垂直方向排列, 水平自动.
			HORIZONTAL = 1		//水平布局: 子元素水平方向排列, 垂直自动.
		};
		Rect		Margin;
		float		Weight;	
		int			Style;
		int			Length;

		void SetVertical();
		void SetHorizontal();
		inline bool IsHorizontal(){return (Style&HORIZONTAL)==HORIZONTAL;}
		inline bool IsVertical(){return (Style&HORIZONTAL)==0;}
		void Reset();
		void SetDimen(int length,float weight);
		void SetMargin(int left,int top,int right,int bottom);
		LayoutParam();
	};
	/**布局系统规则如下：
	1. 顶级布局总是充满窗口，但是留出边界，无视weight和length属性。因为顶级Layout只能有一个。
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
	//Layout Object use for View layout,but pure Layout object not bind a View object.
	class CONCISEUI_API Layout : public TPTree<Layout>{
	protected:
		Twin16	_size;
		Twin16	_location;
		WndPtr	_wnd;		//这个参数是绑定的窗口。
		ICtrl*	_createWnd; //自己生成控件

		Twin _calcMinSize(Layout* lay,bool parentHor);
		//set realize dimension of this object,and his child will be resize
		void _setRect(int left,int top,int width,int height);
		//return fix size
		int _setChildHor(int left,int top,int width,int height);
		//return fix size
		int _setChildVer(int left,int top,int width,int height);
		//create all control in this object or his child.
		void _createControlTree(HWND parent);
	public:
		String			Name;
		LayoutParam		Param;

		Layout();
		~Layout();
		//set _createWnd CreateParam value,if _createWnd is null,return null.
		ICtrl* Layout::GetControl(){return _createWnd;}
		//this will make layout create control (when Refresh call) and delete it when destroy himself.-1 is to make it default value.
		//设置内部生成控件, 这函数会解除绑定的控件, 或者销毁已经生成的控件, 新控件需要调用refresh来生成.
		void SetControl(LPCWSTR className,WORD id,LPCWSTR text,DWORD style = -1,DWORD exstyle = -1);
		//set a created window as bind window,this will destroy inner control of _createWnd.
		void SetControl(HWND ctrl);
		//get bind window handle.
		HWND GetCtrlHandle(){return _wnd;}
		//get realize location of object
		Twin16 GetLocation(){return _location;}
		//get realize size of object
		Twin16 GetSize(){return _size;}
		//a point whether in this layout.
		bool PtInLayout(int x,int y);
		//clear all child and set object to default state
		void Reset();
		//convert to a json object.
		void ToJson(Json* json);
		//from a xml to create layout
		bool FromJson(Json* json);
		//when Param is changed or SetControl is called,call this function to refresh it and if parent != 0,will create control.
		virtual void Refresh(HWND parent);
		//save to a file normaly a .layout file
		bool Save(LPCWSTR file,DWORD code = CP_UTF8);
		//load from a .layout file.
		bool Load(LPCWSTR file,DWORD code = CP_UTF8);
		//load from resource .
		bool Load(ResID res,ResID rt = L"RT_LAYOUT",DWORD code = CP_UTF8);
		//return a layout that name is required.
		Layout* GetElementByName(LPCWSTR name);
		//移动当前 Layout，这个移动的本质是调整它的上一个和下一个同级 Layout 的 Length 属性，所以如果一个 Layout 没有同级的上一个或下一个 Layout，是无法调整位置的。
		//调整位置的幅度，是上一个或下一个同级 Layout 的 Length 属性不小于 0 。
		//返回实际调整的位置幅度。
		int Move(int pix);
	};
	//TopLayout 会接收绑定窗口的 WM_SIZE 消息，从而对子 Layout 的布局进行调整，TopLayout 是顶级 Layout ，它不能是其它 Layout 的子 Layout。
	class CONCISEUI_API TopLayout : public Layout{
	protected:
		IWnd* _container;
		void _onSize(Message* msg,IWnd*);
	public:
		TopLayout();
		//获取布局所需的最新尺寸。
		Twin GetMinSize(){
			Param.Length = 0;
			return _calcMinSize(this,Param.IsHorizontal());
		}
		//this can be set before view is created.if view is a dlg set isdlg = 1 else set isdlg to 0
		void SetContainer(IWnd* view);
		//create all control and calc layout size,allways set parent param is 0,it is not use,this only to make it a override virtual function.
		//call this function in initialize,because no wm_size message was send,so every layout size has not calc.
		virtual void Refresh(HWND parent = 0);
		//clear all child and set object to default state
		void Reset();
	};
}
