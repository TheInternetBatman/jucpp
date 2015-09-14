#pragma once

#define WM_CTRLUSER			0x3790					//控件的消息转移消息ID
//#define WM_CTRLCOLOR		(WM_CTRLUSER+0x0132)	//控件的颜色消息.
#define WM_GET_IWND			(WM_CTRLUSER+0x01)		//从HWND获取Window*指针.
#define WM_GET_ICTRL		(WM_CTRLUSER+0x02)		//从HWND获取Window*指针.
//#define WM_CTRLMSG			(WM_CTRLUSER+0x03)
//#define WM_USERNOTIFY		(WM_CTRLUSER+0x4)
//#define WM_USERCOLOR		(WM_CTRLUSER+0x5)
namespace cs{

	//窗口函数的返回值类型.
	class CONCISEUI_API Result : public HandleType<INT_PTR>{
	public:
		enum{RESULT_DEFAULT = 0x1,WNDPROC_DISABLE = 0x2};
		INT_PTR		_Flag;	//控制位，用于某些内部应用。
		Result():_Flag(0){};
		inline void operator = (INT_PTR result){_Flag |= RESULT_DEFAULT;_Handle = result;}
		//缺省返回值，此值为true：Wndproc处理完消息，将调用窗口的缺省原内部函数，并且返回它的返回值。
		//此值为false：Wndproc处理完消息，返回用户调用 = 操作设置的值，不调用窗口的内部函数。
		inline bool Default(){return (_Flag&RESULT_DEFAULT)==0;}
		//重新设置为缺省状态。
		inline void SetDefault(){_Flag &= ~RESULT_DEFAULT;}
		//用户自己处理消息，不调用Wndproc的处理代码。Wndproc内部只处理了很少的有限消息，
		//这个函数用户一般不需要调用。
		inline bool _DisableWndproc(){return (_Flag&WNDPROC_DISABLE)!=0;}
		//设置是否禁用Wndproc对消息的处理代码。
		inline void _SetDisableWndproc(bool disable){if(disable) _Flag|=WNDPROC_DISABLE;else _Flag&=~WNDPROC_DISABLE;}
	};
	//Windows 消息结构
	typedef struct CONCISEUI_API Message : public _struct{
		HWND		hWnd;		//消息的窗口句柄.
		UINT		uMsg;		//消息ID
		WPARAM		wParam;		//参数
		LPARAM		lParam;		//参数
		Result		Result;		//返回值.
	}Message;
	//鼠标结构,用于鼠标消息.
	typedef struct CONCISEUI_API Mouse : public _struct{
		Mouse(){}
		//Mouse(Mouse& m){*this = m;}
		//void operator = (Mouse& m){Event = m.Event;Button = m.Button;WheelDelta = m.WheelDelta;X = m.X;Y = m.Y;Msg=m.Msg;}
		UINT Event;			//对应的鼠标消息.
		short Button,		//动作按键,可以是0,比如当Event是WM_MOUSEMOVE时.MK_常数
			WheelDelta;		//当鼠标轮滚动时(Event==WM_MOUSEWHEEL),这个值是120或-120,与滚动的方向有关.
		short X,Y;			//指针的位置.
		Message* Msg;
	}Mouse;
	//非客户去鼠标结构
	typedef struct CONCISEUI_API NcMouse : public _struct{
		UINT Event;		//消息ID
		UINT HitTest;	//HT常数，知识鼠标的状态。
		short X,Y;		//屏幕坐标
		Message* Msg;	//返回值。
	}NcMouse;
	//窗口生成时用到的结构,用于初始化窗口的必要参数.
	typedef struct CONCISEUI_API CreateStruct : public _struct{
		CreateStruct():ClassStyle(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS),Style(WS_VISIBLE),ExStyle(0),Menu(0),lParam(0),Location(200,200),Size(500,300){}
		String ClassName,
			Text;					//窗口的标题,只对顶级窗口有效.
		Twin16 Location,Size;	//窗口位置,大小.
		DWORD ClassStyle,Style,ExStyle;	//窗口的三类风格
		union{
			HMENU		Menu;				//菜单(对于主窗口) 
			WORD		Identity;			//ID(对于子窗口).
		};				
		ResID		Dialog;				//对于对话框
		LPVOID lParam;					//额外参数.
		void SetRect(int left,int top,int width,int height){
			Location.SetValue(left,top);
			Size.SetValue(width,height);
		}
		void AddStyle(DWORD style){
			Style |= style;
		}
		void RemoveStyle(DWORD style){
			Style &= ~style;
		}
		void AddExStyle(DWORD style){
			ExStyle |= style;
		}
		void RemoveExStyle(DWORD style){
			ExStyle &= ~style;
		}
	}CreateStruct;
	//窗口对象的基类，整个窗口系统（自定义窗口类，控件，对话框）全部基于这个类，最好虚继承这个类.
	//Ptr后缀的类都是绑定一个指针对象，大小不会大于指针的大小，所以它只集成函数，不集成数据。
	class CONCISEUI_API WndPtr : public HandleType<HWND>{
	public:
		WndPtr(){}
		WndPtr(HWND hwnd){_Handle = hwnd;}
		//绑定一个窗口。
		void operator = (HWND hwnd){_Handle = hwnd;}
		//返回窗口的区域,对于顶级窗口对应屏幕坐标,对于子窗口,对应父窗口坐标.
		bool GetRect(Rect& r);		
		//返回窗口的客户区尺寸.窗户区的左上角坐标一般是(0,0),但也可能不是,如果有边框的话.但它的具体坐标没有一个API函数可以提供.可以使用Window类的成员来获取.
		bool GetClientSize(Twin16& c);						
		//设置大小，不改变位置.
		inline bool SetSize(int cx,int cy) {return ::SetWindowPos(_Handle,0,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER)!=0;}
		//设置位置，不改变大小
		inline bool SetLocation(int x,int y){return ::SetWindowPos(_Handle,0,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER)!=0;}
		//设置大小和位置。
		inline bool SetDimension(int x,int y,int cx,int cy) {return ::MoveWindow(_Handle,x,y,cx,cy,1)!=0;}
		//移动窗口或调整窗口大小,dx,dy是窗口位置的偏移量,cx,cy是大小的变化量.
		void Offset(int dx, int dy, int cx, int cy);	

		//取得文本字串的长度，不包括结尾 0 字符。
		inline int TextLength(){return ::GetWindowTextLength(_Handle);}
		//取得窗口文本，text不必初始化。
		int GetText(String& text);
		//设置窗口文本。
		inline bool SetText(LPCWSTR text){return ::SetWindowTextW(_Handle,text)!=0;}
		//给窗口设置计时器,成功后窗口会定时发送WM_TIMER消息,ms是间隔时间(毫秒),id是计时器的标识,如果这个id是之前设置的一个timer，则替换原来的。对于无窗口绑定的计时器，且不想替换已有的timer，应该给id传0值，由系统来分配一个id，函数返回这个id。
		inline UINT_PTR SetTimer(UINT ms,UINT_PTR id){return ::SetTimer(_Handle,id,ms,NULL);}
		//删除一个timer。id是设置时返回的id。
		inline bool KillTimer(UINT_PTR id){return ::KillTimer(_Handle,id)!=0;}
		//要求窗口重绘指定的区域,erase是否重绘背景,也就是是否引发WM_ERASEBKGND消息.
		inline bool Invalidate(const RECT* clip = 0,bool erase = 1){if(_Handle==0) return 0;return ::InvalidateRect(_Handle,clip,erase)!=0;}
		//要求窗口重绘指定的区域,erase是否重绘背景,也就是是否引发WM_ERASEBKGND消息.
		inline bool Invalidate(const HRGN hRgn,bool erase = 1){if(_Handle==0) return 0;return ::InvalidateRgn(_Handle,hRgn,erase)!=0;}
		//返回窗口参数。
		inline LONG_PTR GetWindowParam(int gwl){return ::GetWindowLongPtr(_Handle,gwl);}
		//设置窗口相关参数.
		inline LONG_PTR SetWindowParam(int gwl,LONG_PTR value){return ::SetWindowLongPtr(_Handle,gwl,(LONG)value);}
		//窗口的父窗口句柄.
		inline HWND Parent(){return ::GetParent(_Handle);}
		//消息函数,有更自由的参数类型,这个函数本身使用的参数类型是非常多的.
		template<typename T,typename R> inline LRESULT SendMessage(UINT ttm,T wParam,R lParam){return ::SendMessage(_Handle,ttm,(WPARAM)wParam,(LPARAM)lParam);}
		//Enable 或者 Disable 一个窗口。
		inline bool Enable(bool enable = 1){return ::EnableWindow(_Handle,enable)!=0;}//return value is pre state of disabled.
		//向窗口发送WM_CLOSE消息，如果关闭没有被取消返回true，反之返回false。
		bool Close(){return 0!=SendMessage(WM_CLOSE,0,0);}
		//更改窗口的风格。
		void ModifyStyle(DWORD dwRemove,DWORD dwAdd);
		//更改窗口的扩展风格。
		void ModifyExStyle(DWORD dwRemove,DWORD dwAdd);
		//设置窗口的宽度。
		bool SetWidth(int cx);
		//设置窗口的高度。
		bool SetHeight(int cy);
		//设置窗口的水平位置。
		bool SetXLocation(int x);
		//设置窗口的垂直位置。
		bool SetYLocation(int y);
	};
	//UI一般只有一个线程，同一个窗口的所有绘图都是在同一个线程完成的，很多时候，并不需要
	//每个窗口有一个独立的 Font 对象，所以使用一个线程变量来声明字体，改变这个字体，所有
	//使用它的窗口都可以统一调整。
	CONCISEUI_API Font* GetDefaultFont();
	//截获窗口消息的静态函数类型.
	typedef void (__stdcall*MSGPROC)(Message*); 
	//Class IWnd 类提供基本的窗口结构，不要实例化这个类。
	interface CONCISEUI_API IWnd : virtual public WndPtr{//public HandleType<HWND>//
	public:
		//因为有一个外部函数要调用这个函数,在一般情况下这个函数不必在外部调用,所以用"_"来标识.
		LRESULT _WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		Function<LRESULT,HWND,UINT,WPARAM,LPARAM> _WndPointer;	//窗口函数.
	private:
		typedef struct _UPOINT : _struct{
			UINT x,y;
			bool operator == (_UPOINT up){
				return (x==up.x)&&(y==up.y);
			}
		}_UPOINT;
		Diction<_UPOINT> _MsgList;
		ObjectList<Delegate<Message*,IWnd*> > _ProcList;
		bool _mLeave,_mNcLeave;					//鼠标是否离开.

	protected:
		Twin		_DragMinSize;		//允许拖拽时的最小尺寸。
		Twin		_DragMaxSize;		//允许拖拽时的最小尺寸。
		Twin		_MinSize;			//没有滚动条的最小大小.
		Twin		_Scr;				//偏移量
		Twin16		_ClientLocation;	//Client Location;
		LONG_PTR	_QuondamProc;		//窗口原来的窗口函数.对于一个用户自己定义的窗口,这个参数无关紧要,一般用DefWndProc来代替,对于一个Windows内部的窗口,比如Text控件,这个参数必须正确传递.
		HCURSOR		_Cursor;			//鼠标指针形状

	public:
		IWnd(void);
		~IWnd();
		Color					FontColor;	//控件的前景色(字体颜色).
		Color					BkColor;	//控件的背景色.设为-1表示背景透明(这种情况下重绘字体时,往往会形成重叠显示字符).
		Object<CreateStruct>	Param;		//这个成员用于设置窗口生成时的参数,它有自己的默认参数,只需要设置想改变的参数.
		Delegate<WORD,WORD,IWnd*>	OnCommand;	//因为这个事件会被频繁的使用,所以集成它.
		Delegate<IWnd*>				OnCreate;	//WM_CREATE消息，但是在生成控件的时候，这消息其实是生成完成后模拟调用了一次。
		Delegate<Mouse*,IWnd*>		OnMouse;	//MouseEvent处理所有鼠标消息.		
		Delegate<NcMouse*,IWnd*>	OnNcMouse;	//OnNcMouse处理非客户区的（除WM_NCMOUSELEAVE）所有鼠标消息.实际上所有消息都是WM_NCHITTEST经过DefWindowProc计算后产生的。
		Delegate<bool&,IWnd*>		OnClose;	//截获WM_CLOSE消息，bool值默认为false，如果置为true，则窗口关闭动作取消。窗口并不会在关闭的时候主动发送WM_CLOSE消息, 这和WM_DESTROY是不同的, WM_CLOSE消息是外部主动发出的, 窗口接收到这个消息的默认行为是销毁, 包括子窗口也是如此.
		Delegate<Twin16,IWnd*>		OnSize;		//WM_SIZE消息，参数是客户区的大小。
		//用户自定义数据，这个成员的作用是可以在不重新继承新类的情况下给这个组件添加成员。
		IntegerList<void*>	UserDataList;	//保存用户自定义数据指针.
		//滚动偏移
		inline Twin Scr(){return _Scr;}
		//出现滚动条的最小尺寸
		inline Twin MinSize(){return _MinSize;}
		//客户区在窗口坐标中的位置.;
		inline Twin16 GetClientPos() const {return _ClientLocation;}
		//返回对应消息的代表指针,可以从这个指针添加或删除相应的函数.
		Delegate<Message*,IWnd*>* GetMsgHook(UINT msg);
		//返回对应消息的代表指针,可以从这个指针添加或删除相应的函数.
		Delegate<Message*,IWnd*>* GetMsgHook(UINT msg1,UINT msg2);
		//设置控件使用的字体,font=0表示使用一个concise库内置的字体对象，整个concise库只有一个缺省字体对象，所以多线程的界面（这很少使用）极端情况下会同时使用这个字体.
		bool SetFont(HFONT font = 0,bool reDraw = 0);
		HFONT GetFont(){return (HFONT)SendMessage(WM_GETFONT,0,0);}
		//生成一个窗口，调用这个函数必须设置Param的ClassName参数，这个参数决定窗口的类型，如果是自定义窗口，必须自己注册这个类。否则函数会失败。
		virtual INT_PTR Create(HWND parent){return 0;};
		//设置拖放时的最小尺寸
		void SetMinDragSize(int cx,int cy){_DragMinSize.SetValue(cx,cy);}
		//设置拖放时的最大尺寸
		void SetMaxDragSize(int cx,int cy){_DragMaxSize.SetValue(cx,cy);}
	};
	CONCISEUI_API IWnd* GetIWnd(HWND hWnd);
	//Class View
	class CONCISEUI_API View : public IWnd{//public WndPtr//,
	protected:
		void _DropFilesProc(Message* msg,IWnd*);	
		void _DrawProc(Message* msg,IWnd*);
		void _DoubleDraw(DC* dc,IWnd*);
		void _EraseBkgndProc(Message* msg,IWnd*);
		//void _CtrlSizeProc(Message* msg);
	public:
		View();
		~View();
		//当前窗口没有销毁之前,不能生成第2个窗口.
		INT_PTR Create(HWND hParent);

		bool Attach(HWND handle);
		HWND Detach();
		//设置窗体指针。
		void SetCursor(HCURSOR cursor);
		//取得当前鼠标指针。
		HCURSOR GetCursor(){return _Cursor;}
		//设置窗体指针，参数为资源id和资源所在地实例，0表示系统资源。
		void SetCursor(ResID resurce);

		Delegate<DC*,View*>	OnDoubleDraw;//一旦在这个调用中加入函数，OnDraw就不会被呼叫。这个函数会自动滚动窗口，也就是说绘图不必自己考虑窗口的滚动问题。
		Delegate<DC*,View*>	OnDraw;//绘图消息在程序中实际上是异步于应用程序线程的.		
		Delegate<HDC,View*>	OnEraseBkgnd;	//也可以在这里面作图,它在OnDraw之前被调用.但它们的机制是不同的
		Delegate<StringMemList*,View*>	OnDropFiles;//响应拖曳消息.
	};
	//Class ScrollView
	class CONCISEUI_API ScrollView : public View{
	public:
		ScrollView(void);
		~ScrollView();
		//设置最小的客户区尺寸,当客户区小于这个尺寸,滚动条出现.
		void SetMinSize(Twin size);	
		//设置最小的客户区尺寸,当客户区小于这个尺寸,滚动条出现.如果cx或cy为负值，则相应的大小不变。
		void SetMinSize(int cx,int cy);				
		//设置最小的滚动单位.
		void SetMinScroll(int hsline, int vsline);		
		//0-hScroll,2-NoScroll,设置鼠标滚轮滚动窗口的方向.
		void SetMouseWheelScroll(int vScroll = 1);		
		//设置是否响应键盘方向键而滚动窗口.
		void SetResponseKeyBoard(bool response = 1);	
		//滚动窗口，dx：水平滚动距离，dy：垂直滚动距离。
		void Scroll(int dx,int dy);
		//滚动指定的行数，xLine 是水平行数，yLine 是垂直行数，负数反方向滚动。
		void ScrollLine(int xLine,int yLine);
		//滚动指定的页数，xPage 是水平页数，yPage 是垂直页数，负数反方向滚动。
		void ScrollPage(int xPage,int yPage);
		//滚动到顶点，xEnd = 1 滚动到最右边，xEnd = -1 滚动到最左边，yEnd = 1 滚动到最下边，yEnd = -1 滚动到最上边。
		void ScrollEnd(int xEnd,int yEnd);
		//重绘窗口.如果客户区有滚动,这个函数会自己处理,所以不必对区域进行平移.
		bool Invalidate(const RECT* clip = NULL,bool erase = 1);
		//重绘窗口.如果客户区有滚动,这个函数会自己处理,所以不必对区域进行平移.
		bool Invalidate(const HRGN clip,bool erase = 1);
		//调整一个点,根据滚动偏移量.
		void TransDimension(Twin& pt);			
		//调整一个矩形,根据滚动偏移量.
		void TransDimension(Rect& rect);			

		Delegate<int,int,ScrollView*> OnScroll;//窗口滚动时调用.参数为偏移量,实际上,两个参数总有一个是0.

	private:
		void _MouseWheelProc(Message* msg,IWnd*);
		void _VScrollProc(Message* msg,IWnd*);
		void _HScrollProc(Message* msg,IWnd*);
		void _KeyDownProc(Message* msg,IWnd*);
		void _SizeProc(Message* msg,IWnd*);

		int _vsPage;
		int _hsPage;
		int _vsLine;
		int _hsLine;
		int _vsMax;
		int _hsMax;
		DWORD _MouseWheelScroll;//发送的WM_VSCROLL或WM_HSCROLL消息.
		bool _ResponseKeyBoard;//指示响应键盘，有垂直条，有水平条
	};
	//Class Frame
	//Frame对象点击关闭按钮会自动退出应用程序，但是这必须在启动线程里才有效，本质上，它
	//内部会调用PostQuitMessage，所以只会退出它所在的线程。
	class CONCISEUI_API Frame : public ScrollView{
	public:
		Frame();
		~Frame();
		//生成一个没有WS_CHILD风格的主窗口，虽然它实际上也可以是一个窗口的子窗口，但是它并不随着主窗口销毁
		//而销毁。quit指定是否退出时程序结束。
		INT_PTR Create(HWND hParent = 0,int nCmdShow = 1,bool quit = 1);
		//退出消息循环，窗口关闭，当前线程结束, 如果是唯一线程, 应用程序也会退出．
		void Quit(int code = 0);
		//big=0,for set small icon,big=1,for set big icon.
		inline HICON SetIcon(HICON icon,bool big=0){return (HICON)SendMessage(WM_SETICON,big,icon);}
		//type=0,for small icon,type=1,for large icon,type=2,for system assign icon for application.
		inline HICON GetIcon(DWORD type=ICON_SMALL){return (HICON)SendMessage(WM_GETICON,type,0);};
	private:
		void _DestroyProc(Message* msg,IWnd*);
	};
	//设置主窗口的图标，含大图标和小图标。
	CONCISEUI_API void SetWndIcon(HWND h,ResID id);
	/* WndDragger 用来拖动一个 IWnd 的位置
	*/
	class WndDragger : public _class{
	protected:
		int _x,_y;
		bool _drag;
		IWnd* _wnd;
		void onMouse(Mouse* m,IWnd*);
	public:
		//用于 OnDrag 的返回值
		enum{
			DRAG_NONE = 0,	//不响应拖动
			DRAG_X = 1,		//只响应 X 方向的拖动
			DRAG_Y = 2		//只响应 Y 方向的拖动
		};
		Function<int,int,int,WndDragger*> OnDrag;	//拖动回调函数，返回值指定拖动方式。两个参数分别是 x 和 y 方向的移动距离。
		WndDragger():_drag(0),_wnd(0){}
		//设置要拖动的窗口，一个 Dragger 只能用于一个窗口的拖动，设置了新窗口后，原窗口就不在响应这个 dragger。解除绑定的窗口，参数直接传 0 。
		void SetWnd(IWnd* wnd);
		IWnd* GetWnd(){return _wnd;}
	};
}
/*		HandleType<HWND>
			|			
			|          
		WndPtr		
	   /   |   \
	  /v   |v   \v
	IWnd IButton \
   /  |    |      \
  /   |    |     IDlg
View ICtrl | 
        \  |
         \ |
		  Button

窗口系统内部机制

窗口系统实际上内部结构非常简单，一个窗口的创建流程如下：

  1. 调用 Create 函数，这个函数核心的代码在 View 对象里面，任何非系统类的
窗口（比如 Edit、Button 等窗口类是系统实现的），最终都会调用 View 的这个
Create 函数。

  2. Create 函数首先查看 Parma->ClassName 参数，如果此参数为空，Create 会
首先调用 RegisterClassEx 注册一个窗口类，这就是一个缺省的窗口类，你可以
在 Param->ClassStyle 参数里设置窗口类的风格。其它窗口类参数一般为空，比
如菜单、图标、背景色等，因为这些参数，应该是窗口的，而不是窗口类的，但是
微软把它设计到了窗口类当中。
    如果 Param->ClassName 参数不为空，表示用户使用了一个现成的或者自己注册
的窗口类，那么 Create 将不会调用 RegisterClassEx 函数。窗口类的名称是由 
ClassStyle 参数决定的，因为只有这个参数是可变的，也就是说，对于不改变风格
的窗口类，将不会创建相同的窗口类，因为 RegisterClassEx 会返回已存在的窗口类。

  3. 所有窗口类的 lpfnWndProc 参数指向同一个静态窗口回调函数 WindowProcedure，
也就是任何自定义窗口第一个回调都是调用这个函数。
    因为 CreateWindowEx 函数总是在同一个线程里调用窗口回调函数，所以 View 的
Create 函数使用了这样一个设计——在一个全局的线程变量里预先设置自己的指针，
当 WindowProcedure （第一次）被回调的时候（实际上它只会被呼叫一次），把窗口
的回调函数指向 View 自己的回调函数，这个 View 的回调函数是 _WndPointer 成员。
_WndPointer 并不是一个函数，而是一个 Function 对象，所以你可以在自己的继承类
里面设置 _WndPointer 指向自己的回调函数，当然，一般无需这么做，默认 _WndPointer
指向 View 的父类 IWnd 的成员函数 _WndProc，_WndProc 对常用消息进行了包装，
IWnd以及任何它的子类的消息事件（On开头的事件）都是由 _WndProc 触发的。
*/