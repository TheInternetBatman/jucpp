#pragma once
//控件的很多风格实际上很少用到,而且还有很多风格已经废弃,常用的风格并不是很多.对窗口和控件的风格,不必完全了解.
namespace cs{
	//这个函数初始化要使用的控件,默认参数初始化所有控件.
	CONCISEUI_API bool InitControl60(DWORD icc = ICC_WIN95_CLASSES);
	//ImageList管理图标或图像,它主要用于其它控件使用的图像源.
	class CONCISEUI_API  ImageList : public HandleType<HIMAGELIST>{
	public:
		~ImageList();
		//绑定一个HIMAGELIST句柄.
		void operator = (HIMAGELIST hImgl);
		//这个函数返回零可能表示没有成功销毁前一个句柄(但hImgl成功绑定),或hImgl为NULL.
		bool Attach(HIMAGELIST hImgl);
		//解除绑定的句柄.
		HIMAGELIST Detach();

		//生成一个ImageList,cx,cy是图像的大小,一个ImageList只能存储相同大小和色深的图像.ilc是图像的类型,initial初始大小,grow增量.
		bool Create(int cx,int cy,UINT ilc = ILC_COLORDDB,int initial = 16,int grow = 16);
		//从图像资源加载.这个函数是创建一个全新的ImageList,原来的HIMAGELIST实例被删除.cx是生成的IMAGELIST图像宽度,根据这个宽度和加载的
		//图像宽度来确定生成多少个图像,不足一个图像的部分会被舍弃.mask是透明色,res是资源ID,hInst = 0表示从BMP文件加载(不表示从当前实例加载),
		//grow是增量.返回图像个数.
		int LoadResource(int cx,COLORREF mask,ResID res,int grow = 16);
		//销毁HIMAGELIST.
		bool Destroy();

		//替换相应索引的图标,HICON资源要自己处理.
		bool Replace(HICON icon,int index);
		//添加一个图标,HICON资源要自己处理.图标的色深和ImageList的色深要一致,否则背景不透明.
		bool AddIcon(HICON icon);
		//把位图添加到ImageList中,返回添加的数量,这个数量是位图的宽度可以分割成的ImageList中图像的数量,不够一个的部分舍去.
		int AddBitmap(HBITMAP bmp,HBITMAP mask = 0);
		//删除相应索引的图像.
		bool Delete(int index);
		//清空图像
		bool Clear();
		//显示图像.
		bool Draw(HDC hdc,int index,int x,int y,UINT style = ILD_NORMAL);
		//拷贝相应索引的图像.
		bool Copy(int iDst,int iSrc);
		//交换相应索引的图像.
		bool Swap(int iDst,int iSrc);
		//创建一个副本.
		HIMAGELIST Duplicate();
		//返回图像数量.
		int Count();
		//返回图像大小.
		Twin GetImageSize();
		//返回相应索引的图像.
		HICON GetIcon(int index);
		//返回背景色.
		COLORREF GetBkColor();
		//设置背景色.
		COLORREF SetBkColor(COLORREF bk);
		//设置图像大小,这个操作会清空图像.
		bool SetImageSize(int cx,int cy);
		//这个函数设置图像的容量,如果数量大于原来的数量,必须自己用AddIcon函数填补空位.
		bool SetCount(int n);
	};
	//控件的基类，和View不同，控件有自己的窗口类，事实上直到控件生成完成，我们都接收不到消息。
	//OnCreate是个伪消息回调，它是直到Create完成之后才调用的，而不是响应WM_CREATE消息。
	class CONCISEUI_API  ICtrl : public IWnd{
	protected:
		//截取控件消息的回调函数.
		void _OnICtrl(Message* msg,IWnd*);
		//处理控件的背景的默认函数。
		void _CtrlColor(Message*,ICtrl*);
		//控件颜色响应函数 ，如果重新绑定一个函数来控制控件的颜色，默认的_ControlColor函数将不会被调用。
	public:
		Function<void,Message*,ICtrl*> OnNotify;
		Function<void,Message*,ICtrl*> OnColor;
		//构造函数。
		ICtrl();
		~ICtrl();
		//根据 Param 参数生成控件，并且重新设置消息函数。
		INT_PTR Create(HWND parent);
		//绑定一个已经生成的窗口，并且重新设置消息函数。
		bool Attach(HWND ctrl);
		//解除绑定的窗口，释放消息函数为原来的值。
		HWND Detach();
	};
	CONCISEUI_API ICtrl* GetICtrl(HWND hWnd);
	//下面的控件虽然从ICtrl和Ptr继承，且没有增加占用空间的成员，但是不能把一个 new ICtrl 对象的指针
	//转换为下面控件的指针，它们的大小是不同的；如果这个iCtrl指针不是  new ICtrl 产生的，而是比如 new ListBox
	//则可以转换为 ListBox指针。

	//class Label
	class CONCISEUI_API  Label : public ICtrl,public LabelPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Label();
	};
	//Button Control
	/*只有XP风格的按钮才可以设置图像列表控件,如果图像列表控件只有一个图像,则显示这个图像.如果有多于1个图像则应该有
	5个图像,分别在正常,有焦点,鼠标停留,DISABLE状态和按下状态下显示

	BS_GROUPBOX风格的Button因为背景透明（不绘制背景），父窗口不能使用WS_CLIPCHILDREN风格。

	*/
	class CONCISEUI_API  Button : public ICtrl,public ButtonPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Button();
	};
	class CONCISEUI_API  GroupBox : public ICtrl,public ButtonPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		GroupBox();
	};
	class CONCISEUI_API  CheckBox : public ICtrl,public ButtonPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		CheckBox();
	};
	class CONCISEUI_API  Radio : public ICtrl,public ButtonPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Radio();
	};
	//Edit Control
	class CONCISEUI_API  Edit : public ICtrl,public EditPtr{
		void _CtrlColor(Message*,ICtrl*);
		void _Command(WORD id,WORD type,IWnd*);
		void _OnNumberChar(Message* msg,IWnd*);
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Edit();
		Function<void,Edit*> OnChange;
		void SetNumerical(bool numerical);
	};
	//DateTimePicker Control
	class CONCISEUI_API  DateTimePicker : public ICtrl,public DateTimePickerPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		DateTimePicker();

		//Function<void,SystemTime*> OnChanged;
	protected:
	};
	//Calendar Control
	class CONCISEUI_API  MonthCalendar : public ICtrl,public MonthCalendarPtr
	{
	public:
		MonthCalendar();
		void operator = (HWND hWnd){Attach(hWnd);}
	};
	//ListBox Control 有3种风格分别是单选(默认),连续多选(LBS_EXTENDEDSEL,可以用SHIFT来选中一个区域),任意多选(LBS_MULTIPLESEL,可以点击来选取或取消任意一项)
	class CONCISEUI_API  ListBox : public ICtrl,public ListBoxPtr
	{
	public:
		ListBox();
		void operator = (HWND hWnd){Attach(hWnd);}
	protected:
	};
	//ComboBox Control 本身包含一个ListBox控件,所以这两个控件的很多函数是很相似的.
	class CONCISEUI_API  ComboBox : public ICtrl,public ComboBoxPtr
	{
	public:
		ComboBox();
		void operator = (HWND hWnd){Attach(hWnd);}
	};
	//CoboBoxEx Control 可以显示图标.ComboBoxEx有一个额外的风格是CBES_EX_CASESENSITIVE,会自动列出成员.
	class CONCISEUI_API  ComboBoxEx : public ICtrl,public ComboBoxExPtr//ComboBoxEx不支持搜索字串有关的函数.
	{
	public:
		ComboBoxEx();
		void operator = (HWND hWnd){Attach(hWnd);}
	};
	//TreeView Control
	/*TreeView控件就是所谓的树形显示控件,它的元素以树形分布,这是一种应用非常广泛的结构,比如文件系统.TreeView控件可以绑定
	两个图像列表控件,一个用于普通状态下,一个用于选中状态下.*/
	class CONCISEUI_API  TreeView : public ICtrl,public TreeViewPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		/*Function<void,LPNMTREEVIEW> OnBeginDrag;	//开始拖曳
		Function<void,LPNMTREEVIEW> OnBeginRDrag;	//开始右键拖曳
		Function<bool,LPNMTVDISPINFO> OnBeginEdit;	//开始编辑标签,返回1,则取消编辑标签.
		Function<bool,LPNMTVDISPINFO> OnEndEdit;	//完成编辑标签,返回1,则取消编辑文本.
		Function<void,LPNMTREEVIEW> OnDeleteItem;	//删除元素.
		Function<void,LPNMTREEVIEW> OnExpanded;		//展开或折叠了一个子项.
		Function<bool,LPNMTREEVIEW> OnExpanding;	//正要展开或折叠一个子项,返回1取消动作.
		Function<void,LPNMTREEVIEW> OnSelChanged;	//当前选择项变化.
		Function<bool,LPNMTREEVIEW> OnSelChanging;	//正要改变当前选择项,返回1阻止动作.
		Function<UINT,LPNMTVKEYDOWN> OnChar;		//有按键动作,可以执行搜索操作,详见MSDN说明.
		*/
		TreeView();
	};
	//ToolBar Control
	//ToolBar控件需要 SendMessage(TB_AUTOSIZE,0,0)，来刷新控件，这个消息会
	//调整按钮和自身尺寸，并且设置宽度和父窗口客户区相同。
	class CONCISEUI_API  ToolBar : public ICtrl,public ToolBarPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ToolBar();
		//生成控件.
		INT_PTR Create(HWND hParent);
	private:
		//void _OnParentSize(Message*);
	};
	//ProgressBar Control
	class CONCISEUI_API  ProgressBar : public ICtrl,public ProgressBarPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ProgressBar();
	};
	class CONCISEUI_API Rebar : public ICtrl,public RebarPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Rebar();
	};
	//ToolTip Control 这个控件必须在启用6.0版本控件时才可用.也就是说,必须要有MANIFEST.
	class CONCISEUI_API  ToolTip : public ICtrl,public ToolTipPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ToolTip(void);
		INT_PTR Create(HWND hParent);
	};

	//TabPage控件本身并不提供对控件的操作,它只提供一个框架,然后点击页按钮会引发相应的消息事件.在这些事件中处理控件的行为,使它表现的类似于处在不同的页面.
	class CONCISEUI_API  TabPage : public ICtrl,public TabPagePtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		TabPage();
	};
	//表示组合键的一个结构.
	//热键控件
	class CONCISEUI_API  HotKey : public ICtrl,public HotKeyPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		HotKey();
	};
	class CONCISEUI_API  ListView : public ICtrl,public ListViewPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ListView();
		//ListView 的扩展风格必须生成之后调用这个函数来设置，在生成窗口时的风格和扩展风格中设置都不起作用。add表示添加还是移除exStyle指定的风格。
		DWORD SetExStyle(DWORD exStyle,bool add = 1);
	};
	class CONCISEUI_API  StatusBar : public ICtrl,public StatusBarPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		StatusBar();
	};
	class CONCISEUI_API  TrackBar : public ICtrl,public TrackBarPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		TrackBar();
	};
	//UpDown 控件会向父窗口发送滚动条消息
	class CONCISEUI_API  UpDown : public ICtrl,public UpDownPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		UpDown();
		//直接生成一个 UDS_SETBUDDYINT UDS_ALIGNRIGHT UDS_ARROWKEYS 风格的 UpDown 控件，nUpeer：上限；nLower：下限；nPos：当前位置。
		INT_PTR Create(HWND buddy,HWND parent,int nUpper,int nLower,int nPos);
		INT_PTR Create(HWND parent);
	};
	class CONCISEUI_API Header : public ICtrl,public HeaderPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Header();
	};
	class CONCISEUI_API Link : public ICtrl,public LinkPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Link();
	};
	class CONCISEUI_API IpAddress : public ICtrl,public IpAddressPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		IpAddress();
	};
}