#pragma once
//如果不需要相应控件的事件，只是调用控件的功能函数，可以使用这个接口。

namespace cs{
	/*PBS_NORMAL = 1,PBS_HOT = 2,PBS_PRESSED = 3,PBS_DISABLED = 4,PBS_DEFAULTED = 5,PBS_STYLUSHOT = 6*/
	interface CONCISEUI_API ButtonPtr : virtual public WndPtr{
	public:
		ButtonPtr(){}
		ButtonPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//返回按钮显示文字所需的尺寸.
		bool GetIdealSize(LPSIZE ps){return SendMessage(BCM_GETIDEALSIZE,0,ps)!=0;}
		//获取按钮的选中状态,它返回3个值:0(未选中),1(选中),2(不确定).
		int GetCheck(){return (int)(INT_PTR)SendMessage(BM_GETCHECK,0,0)!=0;}
		//设置按键的选中状态.//BST_UNCHECKED(0),BST_CHECKED(1),BST_INDETERMINATE(2);
		void SetCheck(int state = BST_CHECKED){SendMessage(BM_SETCHECK,(WPARAM)state,0);}
		//设置一个ImageList作为按钮显示的图像列表.
		bool SetImageList(HIMAGELIST hImgl,UINT align = BUTTON_IMAGELIST_ALIGN_LEFT,LPRECT margin = 0);
		//返回按键的图像文字显示参数.
		bool GetImageList(BUTTON_IMAGELIST& bi){return SendMessage(BCM_GETIMAGELIST,0,&bi)!=0;}
		//设置文本的位置参数.
		bool SetTextMargin(int left,int top,int right,int bottom){return SendMessage(BCM_SETTEXTMARGIN,0,&left)!=0;}
		//获取文本的位置参数.
		bool GetTextMargin(LPRECT margin){return SendMessage(BCM_GETTEXTMARGIN,0,margin)!=0;}
		//模拟按下按钮动作.
		void Click(){SendMessage(BM_CLICK,0,0);}
	};
	interface CONCISEUI_API ListBoxPtr : virtual public WndPtr{
	public:
		ListBoxPtr(){}
		ListBoxPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//添加一个字串.str是要添加的字串,index是索引位置.返回字串在列表中的索引.
		inline int AddString(LPCWSTR str,int index = -1);
		//删除一个字串.index是要删除的字串的索引.
		inline bool DeleteString(int index)					{return (int)SendMessage(LB_DELETESTRING,(WPARAM)index,0)!=0;}
		//查找一个字串,begin是开始查找的索引,返回字串的索引或-1.这个查找只要部分匹配就可以,而且不分大小写.
		inline int FindString(LPCWSTR str,int begin = -1)	{return (int)SendMessage(LB_FINDSTRING,(WPARAM)begin,(LPARAM)str);}
		//精确查找,不但要求整个字串匹配,还要区分大小写.
		inline int FindExactString(LPCWSTR str,int begin = -1)	{return (int)SendMessage(LB_FINDSTRINGEXACT,(WPARAM)begin,(LPARAM)str);}
		//返回元素数量.
		inline int GetCount()								{return (int)SendMessage(LB_GETCOUNT,0,0);}
		//判断项是否已经被选中.
		inline bool ItemIsSelect(int index)					{return (int)SendMessage(LB_GETSEL,(WPARAM)index,(LPARAM)0)!=0;}
		//返回选中项的索引,或-1.仅用于单列表.
		inline int GetSelItem()								{return (int)SendMessage(LB_GETCURSEL,0,0);}//this is only use for single select list box,for multisel it return value is the focused item(but single select is always select item);
		//设置列表的相应索引项选中,如果index = -1则没有项被选中.仅用于单选列表.
		inline void SetSelItem(int index)					{SendMessage(LB_SETCURSEL,(WPARAM)index,(LPARAM)0);}//for single select list box, must one item is selected.
		//设置特定字串为选中状态.
		inline int SetSelItem(LPCWSTR str,int start = -1)	{return (int)SendMessage(LB_SELECTSTRING,(WPARAM)start,(LPARAM)str);}
		//获取多选时的选中项索引,sel要设置足够的大小.返回选中项的总数.这个函数只能用于多选列表,对于单选列表,总是返回-1.
		inline int GetSelItems(Memory<int>* sel)		{return (int)SendMessage(LB_GETSELITEMS,(WPARAM)sel->Length(),(LPARAM)sel->Handle());}//this is only use for multy select style,for single select style it is always return -1;
		//设置多选列表的选中范围,只能用于多选列表.如果last小于first,则这一范围的选中项会被清除.有两点需要注意,这个函数不能
		//让first和last相同,否则函数失败.范围选择允许超界.如果想只选中一项可以采用交集的办法,先选2项,再清除一项.即使对于LBS_EXTENDEDSEL风格
		//的列表,也可以和这个方法选取任意的项,而不一定连续.
		inline int SetSelItems(uint first,uint last)		{return (int)SendMessage(LB_SELITEMRANGEEX,(WPARAM)first,(LPARAM)last);}//if last is less than first,the range is removed from select.
		//获取选中项的总数.
		inline int GetSelCount()							{return (int)SendMessage(LB_GETSELCOUNT,0,0);}//this is only use for multy select style,for single select style it is always return -1;
		//取得相应索引项的文本,可以预先用GetTextLength取得文本所需空间大小,text的空间必须要大于返回值加1.返回取得文本的长度或-1.
		int GetText(int index,String& buf);
		//设置设置指定项的文本，Window并没有提供LB_SETTEXT消息，这个函数内部通过删除和添加字串完成这个功能。
		bool SetText(int index,LPCWSTR str);
		//取得相应索引项的文本长度,不包括结尾NULL字符.
		inline int GetTextLength(int index)					{return (int)SendMessage(LB_GETTEXTLEN,(WPARAM)index,0);}//返回文本长度.
		//获取特定位置的项的索引值.
		inline int GetIndexByPosition(Twin16 pos)	{return (int)SendMessage(LB_ITEMFROMPOINT,0,(LPARAM)pos.Value);}
		//清空所有项.
		inline void Clear()									{SendMessage(LB_RESETCONTENT,0,0);}
		//设置多列列表控件的宽度.
		inline void SetColumnWidth(int width)				{SendMessage(LB_SETCOLUMNWIDTH,(WPARAM)width,0);}
		//设置元素的绘制高度,调用这个函数后要重绘控件..
		inline bool SetItemHeight(int height)				{return (SendMessage(LB_SETITEMHEIGHT,0,(LPARAM)height)!=LB_ERR);}
		//获取顶部项(的索引).
		inline int GetTopItem()								{return (int)SendMessage(LB_GETTOPINDEX,0,0);}
		//设置顶部项.
		inline bool SetTopItem(int index)					{return (SendMessage(LB_SETTOPINDEX,(WPARAM)index,0)!=-1);}
		//ListBox不会自动设置水平滚动条.
		inline void SetHScrollWidth(int width)				{SendMessage(LB_SETHORIZONTALEXTENT,(WPARAM)width,0);}
		//把普通ListBox转换成DragListBox。
		inline bool MakeDrglist()							{return 0!=::MakeDragList(_Handle);}
	};
	interface CONCISEUI_API LabelPtr : virtual public WndPtr
	{
	public:
		LabelPtr(){}
		LabelPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	//Button Control
	/*只有XP风格的按钮才可以设置图像列表控件,如果图像列表控件只有一个图像,则显示这个图像.如果有多于1个图像则应该有
	5个图像,分别在正常,有焦点,鼠标停留,DISABLE状态和按下状态下显示*/
	//Edit Control
	interface CONCISEUI_API EditPtr : virtual public WndPtr
	{
	public:
		EditPtr(){}
		EditPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}

		//返回控件用于存储字串的空间,使用LocalLock函数和LocalUnlock函数来获取和释放内存句柄.不要向这段内存写入数据,控件是自己管理这段内存的.
		HLOCAL LockTextBuffer(){return (HLOCAL)SendMessage(EM_GETHANDLE,0,0);}
		//这个函数设置控件的字串缓存,它会清空UNDO操作和MODIFY标记.hMem可以是自己分配的内存(LocalLock),或者是前面的函数返回的内存.
		void SetTextBuffer(HLOCAL hMem){SendMessage(EM_SETHANDLE,hMem,0);}
		//设置控件是否有只读属性(ES_READONLY风格).
		bool SetReadOnly(bool readonly){return SendMessage(EM_SETREADONLY,readonly,0)!=0;}
		//滚动到光标位置.
		void ScrollCaret(){SendMessage(EM_SCROLLCARET,0,0);}
		//返回控件包含的文本行数.
		int GetLineCount(){return (int)SendMessage(EM_GETLINECOUNT,0,0);}
		//用已知文本替换选中文本.
		void ReplaceSelText(LPCWSTR str){SendMessage(EM_REPLACESEL,1,str);}
		//返回已经选择文本的起始和结束位置.
		Twin16 GetSelect(){return (Twin16)SendMessage(EM_GETSEL,0,0);}
		//设置选择文本的起始和结束位置.
		void SetSelect(Twin16 sel){SendMessage(EM_SETSEL,sel.x,sel.y);}
		//显示一个汽泡提示信息.
		void ShowToolTip(LPCWSTR tip,LPCWSTR title = 0,int tti = TTI_NONE);
		//隐藏显示的汽泡提示.
		void HideToolTip();
		//UNDO操作
		bool UnDo(){return SendMessage(EM_UNDO,0,0)!=0;}
	};
	//DateTimePicker Control
	interface CONCISEUI_API DateTimePickerPtr : virtual public WndPtr
	{
	public:
		DateTimePickerPtr(){}
		DateTimePickerPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}

		HWND GetMonthCalControl()			{return DateTime_GetMonthCal(_Handle);}
		void SetFormat(LPCWSTR format)		{DateTime_SetFormat(_Handle,format);}

		COLORREF GetMonthCalColor(int mcsc = MCSC_BACKGROUND)	{return (int)(INT_PTR)DateTime_GetMonthCalColor(_Handle,mcsc);}
		COLORREF SetMonthCalColor(COLORREF color,int mcsc = MCSC_BACKGROUND)	{return (int)(INT_PTR)DateTime_SetMonthCalColor(_Handle,mcsc,color);}

		HFONT GetMonthCalFont()				{return (HFONT)DateTime_GetMonthCalFont(_Handle);}
		void SetMonthCalFont(HFONT font,bool redraw = 0)		{DateTime_SetMonthCalFont(_Handle,font,redraw);}

		DWORD GetRange(LPSYSTEMTIME pst)	{return DateTime_GetRange(_Handle,pst);}//pst是一个含两个SystemTime元素数组的指针.
		bool SetRange(LPSYSTEMTIME pst,DWORD flag = GDTR_MIN|GDTR_MAX)			{return DateTime_SetRange(_Handle,flag,pst)!=0;}

		DWORD GetTime(LPSYSTEMTIME pst)		{return DateTime_GetSystemtime(_Handle,pst);}
		bool SetTime(LPSYSTEMTIME pst,DWORD flag = GDT_VALID)	{return DateTime_SetSystemtime(_Handle,flag,pst)!=0;}
	};
	//Calendar Control
	interface CONCISEUI_API MonthCalendarPtr : virtual public WndPtr
	{
	public:
		MonthCalendarPtr(){}
		MonthCalendarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	//ComboBox Control 本身包含一个ListBox控件,所以这两个控件的很多函数是很相似的.
	interface CONCISEUI_API ComboBoxPtr : virtual public WndPtr
	{
	public:
		ComboBoxPtr(){}
		ComboBoxPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//添加一个字串.返回添加的位置.
		inline int AddString(LPCWSTR str,int index = -1)		{return (int)SendMessage(CB_INSERTSTRING,(WPARAM)index,(LPARAM)str);}//retrun index of insert item.
		//删除一个字串(通过索引).
		inline bool DeleteString(int index)						{return SendMessage(CB_DELETESTRING,(WPARAM)index,0)!=-1;}
		//清空字串.
		inline void Clear()										{SendMessage(CB_RESETCONTENT,0,0);}
		//设置特定项的文本.
		inline bool SetItemText(int index,LPCWSTR str)			{return SendMessage(CB_SETITEMDATA,(WPARAM)index,(LPARAM)str)!=-1;}
		//返回特定项的文本.
		bool GetItemText(int index,String& str);
		//获取元素的总数量.
		inline int GetCount()									{return (int)SendMessage(CB_GETCOUNT,0,0);}
		//返回当前选择项的索引.
		inline int GetCurSelect()								{return (int)SendMessage(CB_GETCURSEL,0,0);}
		//设置特定索引项为选中状态.
		inline int SetCurSelect(int index)						{return (int)SendMessage(CB_SETCURSEL,(WPARAM)index,0);}
		//查找一个字串.
		inline int FindString(LPCWSTR str,int start = -1)		{return (int)SendMessage(CB_FINDSTRING,(WPARAM)start,(LPARAM)str);}
		//精确查找一个字串.
		inline int FindExactString(LPCWSTR str,int start = -1)	{return (int)SendMessage(CB_FINDSTRINGEXACT,(WPARAM)start,(LPARAM)str);}
		//设置选定项(通过字串).
		inline int SelectString(LPCWSTR str,int start = -1)		{return (int)SendMessage(CB_SELECTSTRING,(WPARAM)start,(LPARAM)str);}
		//展开下拉框.
		inline void ShowDropDown(bool show = 1)					{SendMessage(CB_SHOWDROPDOWN,(WPARAM)show,0);}
		//使特定项可见.
		inline bool EnsureVisible(int index)					{return (SendMessage(CB_SETTOPINDEX,(WPARAM)index,0)!=-1);}
		//设置用户可以键入的最大字符个数,默认值是30000.
		inline void SetEditLimit(int count)						{SendMessage(CB_LIMITTEXT,(WPARAM)count,0);}//count = 0 for no limit;
	};
	//CoboBoxEx Control 可以显示图标.ComboBoxEx有一个额外的风格是CBES_EX_CASESENSITIVE,会自动列出成员.
	interface CONCISEUI_API ComboBoxExPtr : virtual public WndPtr//ComboBoxEx不支持搜索字串有关的函数.
	{
	public:
		ComboBoxExPtr(){}
		ComboBoxExPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//设置控件的图像列表控件
		inline HIMAGELIST SetImageList(HIMAGELIST hImgl)	{return (HIMAGELIST)SendMessage(CBEM_SETIMAGELIST,0,(LPARAM)hImgl);}
		//获取控件的图像静静控件.
		inline HIMAGELIST GetImageList()					{return (HIMAGELIST)SendMessage(CBEM_GETIMAGELIST,0,0);}
		//添加一个项,str:文本,image:图像在图像列表控件中的索引,indent:图像和文字的右移宽度,1个indent大概为10个pix,
		//index:添加的位置,selImage:被选中时的图像索引,lParam:附加的参数.一个问题是当image和selImage不同时会出现显示问题.
		int AddItem(LPCWSTR str,int image = -1,int indent = 0,int index = -1,int selImage = -1,LPARAM lParam = 0);
		//删除一项.
		inline bool DeleteItem(int index)					{return (SendMessage(CBEM_DELETEITEM,(WPARAM)index,0)==-1);}
		//清空.
		inline void Clear()									{SendMessage(CB_RESETCONTENT,0,0);}
		//设置额外风格,这些风格只能用这个函数来设置,而不能在生成参数中设置.
		inline DWORD SetExStyle(DWORD cbs)					{return (DWORD)SendMessage(CBEM_SETEXSTYLE,0,(LPARAM)cbs);}
		//返回额外风格.
		inline DWORD GetExStyle()							{return (DWORD)SendMessage(CBEM_GETEXSTYLE,0,0);}
		//设置项参数.
		inline bool SetItem(PCOMBOBOXEXITEM pcbi)			{return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)pcbi)!=0;}
		//获取项参数.
		inline bool GetItem(PCOMBOBOXEXITEM pcbi)			{return (int)SendMessage(CBEM_GETITEM,0,(LPARAM)pcbi)!=0;}
		//没有办法知道要取得字串的长度,所以给text设置一个较大的Capacity,否则可能只取得字串的部分,这种情况下函数仍返回1.
		bool GetText(int index,String& text);
		//当index = -1时,设置的是编辑框的文本.
		bool SetText(int index,LPCWSTR text);
		//获取项的图标索引.
		int GetImage(int index,int flag = 0);// 0 for image,1 for selImage,2 for overlay.
		//设置项的图标索引.
		bool SetImage(int index,int image,int flag = 0);
		//设置平移.
		bool SetIndent(int index,int indent);
		//返回平移量
		int GetIndent(int index);
		//返回元素数量.
		inline int GetCount()								{return (int)SendMessage(CB_GETCOUNT,0,0);}
		//返回选中项的索引.
		inline int GetCurSelect()							{return (int)SendMessage(CB_GETCURSEL,0,0);}
		//设置选中项.
		inline int SetCurSelect(int index)					{return (int)SendMessage(CB_SETCURSEL,(WPARAM)index,0);}
		//设置可见项的数量.
		inline bool SetVisibleCount(int count)				{return (int)SendMessage(CB_SETMINVISIBLE,(WPARAM)count,0)!=0;}
		//获取选中项的数量.
		inline int GetVisibleCount()						{return ComboBox_GetMinVisible(_Handle);}
		//设置编辑控件的最大字符数.
		inline void SetEditLimit(int count)					{SendMessage(CB_LIMITTEXT,(WPARAM)count,0);}//count = 0 for no limit;
		//提前设置存储空间.
		inline bool ExpendStorage(int count,int strStorage)	{return SendMessage(CB_INITSTORAGE,(WPARAM)count,(LPARAM)strStorage)!=CB_ERRSPACE;}
	};
	//TreeView Control
	/*TreeView控件就是所谓的树形显示控件,它的元素以树形分布,这是一种应用非常广泛的结构,比如文件系统.TreeView控件可以绑定
	两个图像列表控件,一个用于普通状态下,一个用于选中状态下.*/
	interface CONCISEUI_API TreeViewPtr : virtual public WndPtr
	{
	public:
		TreeViewPtr(){}
		TreeViewPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//添加新项,parent:添加的位置,可以是TVI_ROOT.text:显示的文本,imgIndex:图标索引,selIndex:选中状态下的图标索引,insAfter:同级子项的相对位置.
		HTREEITEM AddItem(HTREEITEM parent,LPCWSTR text,int imgIndex,int selIndex,HTREEITEM tvi = 0);
		//删除子项.
		bool DeleteItem(HTREEITEM hItem){return TreeView_DeleteItem(_Handle,hItem)!=0;}
		//清除所有的项.
		bool Clear(){return TreeView_DeleteAllItems(_Handle)!=0;}
		//必须设置str足够的容量,返回值是字串的长度.
		int GetItemText(HTREEITEM hti,String& str);
		//设置子项的文本.
		bool SetItemText(HTREEITEM hti,LPCWSTR str);
		//取得子项的图标索引,flag可以是TVSIL_STATE和TVSIL_NORMAL之一.
		int GetImageIndex(HTREEITEM hti,UINT flag = TVSIL_NORMAL);
		//设置子项的图标索引,-1表示不设置这个索引.
		bool SetImageIndex(HTREEITEM hti,int image,int selImage = -1);
		//取得控件使用的图像列表控件的句柄,flag有两个可选值:TVSIL_NORMAL和TVSIL_STATE.
		HIMAGELIST GetImageList(UINT flag = TVSIL_NORMAL)					{return TreeView_GetImageList(_Handle,flag);}
		//设置控件使用的图像列表控件,flag有两个可选值,TVSIL_NORMAL和TVSIL_STATE.
		HIMAGELIST SetImageList(HIMAGELIST hImgl,UINT flag = TVSIL_NORMAL)	{return (HIMAGELIST)SendMessage(TVM_SETIMAGELIST,flag,(HIMAGELIST)hImgl);}//设置ImageList,可以选择删除前一个ImageList或返回其句柄.hImgl为0则解除绑定.
		//设置控件的背景色,这个函数重载了Control类的同名函数,不要用那个函数来设置背景色.-1使用系统的缺省色.
		COLORREF SetBkgndColor(COLORREF color)	{return TreeView_SetBkColor(_Handle,color);}
		//获取控件的背景色.
		COLORREF GetBkgndColor()				{return TreeView_GetBkColor(_Handle);}
		//设置文本的颜色,这个函数同样重载了Control类的同名函数.因为那个函数对这个控件不可用.color = -1表示使用系统的缺省颜色.
		COLORREF SetFontColor(COLORREF color)	{return TreeView_SetTextColor(_Handle,color);}
		//获取文本颜色.
		COLORREF GetFontColor()					{return TreeView_GetTextColor(_Handle);}
		//设置水平线的长度,如果小于允许的最小值,设为最小值.
		void SetLineIndent(int width)			{TreeView_SetIndent(_Handle,width);}
		//返回水平线的长度,缺省值大约为20.
		int GetLineIndent()						{return TreeView_GetIndent(_Handle);}
		//设置划线的颜色.
		COLORREF SetLineColor(COLORREF color)	{return TreeView_SetLineColor(_Handle,color);}
		//获取划线的颜色.
		COLORREF GetLineColor()					{return TreeView_GetLineColor(_Handle);}
		//开始编辑标签,返回编辑控件的句柄.
		HWND BeginEdit(HTREEITEM hItem)			{return TreeView_EditLabel(_Handle,hItem);}
		//完成标签编辑,cancel = 1表示还原文本.
		bool EndEdit(bool cancel = 0)			{return TreeView_EndEditLabelNow(_Handle,cancel)!=0;}
		//排序子项;all--是否对多级子项进行排序.
		bool SortChildren(HTREEITEM hti,bool all = 0)	{return TreeView_SortChildren(_Handle,hti,all)!=0;}
		//使特定的项可见.(通过展开或滚动).如果仅仅是滚动使项可见,返回1,否则返回0.
		bool EnsureVisible(HTREEITEM hItem)				{return TreeView_EnsureVisible(_Handle,hItem)!=0;}
		//展开或收起特定的项.
		bool Expand(HTREEITEM hItem,UINT tve = TVE_TOGGLE)	{return TreeView_Expand(_Handle,hItem,tve)!=0;}
		//获取绑定的编辑控件的句柄.
		HWND GetEditControl()			{return TreeView_GetEditControl(_Handle);}
		//返回项的总数.最大为16位整数的最大值.
		int GetItemCount()				{return TreeView_GetCount(_Handle);}
		//获取项的矩形.
		bool GetItemRect(HTREEITEM hItem,LPRECT rect,bool onlyText = 1){return TreeView_GetItemRect(_Handle,hItem,rect,onlyText)!=0;}
		//获取项的高度.
		int GetItemHeight(){return TreeView_GetItemHeight(_Handle);}
		//设置项的高度.
		int SetItemHeight(int height){return TreeView_SetItemHeight(_Handle,height);}		
		//设置ToolTip控件.
		HWND SetToolTipCtrl(HWND hToolTip){return TreeView_SetToolTips(_Handle,hToolTip);}
		//取得关联的ToolTip控件.
		HWND GetToolTipCtrl(){return TreeView_GetToolTips(_Handle);}		
		//设置项的参数
		bool SetItemInfo(LPTVITEMEX pti){return TreeView_SetItem(_Handle,pti)!=0;}
		//获取项的参数.
		bool GetItemInfo(LPTVITEMEX pti){return TreeView_GetItem(_Handle,pti)!=0;}
		//设置当前选定项为指定项.
		bool SetSelectItem(HTREEITEM hti){return TreeView_SelectItem(_Handle,hti)!=0;}
		//获取项的额外数据参数.
		bool GetItemData(HTREEITEM hti,LPARAM* pParam);
		//设置项的额外数据参数.
		bool SetItemData(HTREEITEM hti,LPARAM data);
		//返回根.
		HTREEITEM GetRoot()							{return TreeView_GetRoot(_Handle);}
		//返回最下面的展开项.
		HTREEITEM GetLastItem()						{return TreeView_GetLastVisible(_Handle);}
		//返回第一个子项(或0如果没有).
		HTREEITEM GetFirstChild(HTREEITEM hti)		{return TreeView_GetChild(_Handle,hti);}
		//下一个可见项(不考虑级),hitem必须可见.
		HTREEITEM GetNextVisible(HTREEITEM hti)		{return TreeView_GetNextVisible(_Handle,hti);}
		//前一个可见项(不考虑级),hitem必须可见.
		HTREEITEM GetPreVisible(HTREEITEM hti)		{return TreeView_GetPrevVisible(_Handle,hti);}
		//返回下一个同级项.
		HTREEITEM GetNext(HTREEITEM hti)			{return TreeView_GetNextSibling(_Handle,hti);}
		//返回前一个同级项.
		HTREEITEM GetPrevious(HTREEITEM hti)		{return TreeView_GetPrevSibling(_Handle,hti);}
		//返回父项的句柄;如果没有父项,返回0.
		HTREEITEM GetParent(HTREEITEM hti)			{return TreeView_GetParent(_Handle,hti);}
		//返回当前选定项.
		HTREEITEM GetSelectItem()					{return TreeView_GetSelection(_Handle);}
		//取得特定位置的项。
		HTREEITEM GetPosItem(int x,int y)			{TVHITTESTINFO ti;ti.pt.x = x;ti.pt.y = y;return TreeView_HitTest(_Handle,&ti);}
		//返回一个项的句柄,具体由tvgn确定.它实际上是前面所有函数功能的组合,而且都可以用前面的那些函数取代.
		HTREEITEM GetItem(UINT tvgn,HTREEITEM hti = 0)	{return TreeView_GetNextItem(_Handle,hti,tvgn);}
		//获取项的CheckBox选中状态，0：未选中，1：选中，-1：没有CheckBox。
		int GetItemChecked(HTREEITEM hti)			{return TreeView_GetCheckState(_Handle,hti);}
		//设置项的CheckBox选中状态，0：未选中，1：选中。
		void SetItemChecked(HTREEITEM hti,bool check)	{TreeView_SetCheckState(_Handle,hti,(UINT)check);}

	protected:
		//void _OnNotify(LPCTRLMSGPARAM pcm);
	};
	//ToolBar Control
	//调用AddButton后调用toolBar.SendMessage(TB_SETMAXTEXTROWS,(WPARAM)0,0)，
	//按钮将不显示文字，而是作为tooltip，需要TBSTYLE_TOOLTIPS风格，
	//MSDN上的 TBSTYLE_EX_MIXEDBUTTONS 方法和 TBN_GETINFOTIP 无效;
	interface CONCISEUI_API ToolBarPtr : virtual public WndPtr
	{
	public:
		ToolBarPtr(){}
		ToolBarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//生成控件.
		INT_PTR Create(HWND hParent);
		//添加按钮,cmdID:按钮的WM_COMMAND消息ID,image:图标在列表中的索引,text:显示的文本,tbn:按钮的风格,tbstate:按钮的状态,index:添加的位置.
		bool AddButton(WORD cmdID,int image,LPCWSTR text,BYTE tbn = BTNS_BUTTON|BTNS_AUTOSIZE,BYTE tbstate = TBSTATE_ENABLED,int index = -1);
		//添加一个分隔条,index是位置索引.
		bool AddSplit(int index = -1);
		//设置图像列表控件,ToolBar控件包含3个图像列表控件,0:普通,1:DISABLE状态,2:鼠标停留状态.
		HIMAGELIST SetImageList(int type,HIMAGELIST hImgl);
		//获取图像列表控件.0:普通,1:DISABLE状态,2:鼠标停留状态.
		HIMAGELIST GetImageList(int type);
		//返回按键的状态,通过ID索引.
		inline int GetButtonState(int id)	{return (int)SendMessage(TB_GETSTATE,id,0);}
		//设置按钮的状态,通过ID索引.
		bool SetButtonState(int id,int state,DWORD option = 1);
		//通过TBBUTTON结构可以一次添加多个按钮.
		inline bool AddButton(TBBUTTON* tbb,int count = 1){return SendMessage(TB_ADDBUTTONS,count,(LPARAM)tbb)!=0;}
		//删除特定的按钮.
		inline bool DelButton(int index){return (int)SendMessage(TB_DELETEBUTTON,index,0)!=0;}
		//Enable或Disable一个按钮.
		inline bool EnableButton(int id,bool enable = 0){return (int)SendMessage(TB_ENABLEBUTTON,id,enable)!=0;}
		//隐藏或显示一个按钮.
		inline bool HideButton(WORD id,bool hide = 1){return (int)SendMessage(TB_HIDEBUTTON,id,MAKELONG(hide,0))!=0;}
		//切换一个按钮.
		inline bool CheckButton(WORD id,bool check = 1){return (int)SendMessage(TB_CHECKBUTTON,id,MAKELONG(check,0))!=0;}//使一个按钮处于check状态,而不管这个按钮是否是一个check按钮,如果这不是一个check按钮,则不能通过点击鼠标使它处于非check状态.
		//模拟按键动作.
		inline bool PressButton(WORD id,bool pressed = 1){return (int)SendMessage(TB_PRESSBUTTON,id,MAKELONG(pressed,0))!=0;}//使一个按钮处于下压状态,可以通过点击鼠标解除这种状态.
		//
		inline bool GetAnchorHighLight(){return (int)SendMessage(TB_GETANCHORHIGHLIGHT ,0,0)!=0;}
		//
		inline bool SetAnchorHighLight(bool high){return (int)SendMessage(TB_SETANCHORHIGHLIGHT,high,0)!=0;}
		//
		bool LoadBitmap(int type,int cx,COLORREF mask,ResID,bool delpre = 1);//如果不希望前一个Image List被释放,传delpre = 0.
		//
		inline bool SetIconSize(int cx,int cy){return (int)SendMessage(TB_SETBITMAPSIZE,0,MAKELONG(cx,cy))!=0;}//它的缺省值是图标尺寸.
		//
		inline bool SetButtonSize(int cx,int cy){return (int)SendMessage(TB_SETBUTTONSIZE,0,MAKELONG(cx,cy))!=0;}//在AddButton之后调用,只能调大,不能小于图像高度.
		//
		inline Twin16 GetButtonSize(){return (int)SendMessage(TB_GETBUTTONSIZE,0,0);}
		//
		bool GetButtonRect(int index,LPRECT lpRect,bool byIndex = 1);
		//
		bool SetLimitButtonWidth(int min,int max){return (int)SendMessage(TB_SETBUTTONWIDTH,0,MAKELONG(min,max))!=0;}
		//
		inline UINT GetStyle(){return (int)SendMessage(TB_GETSTYLE,0,0);}
		//设置ToolBar的风格，option = 0：添加这个风格，option = 1：移除这个风格，option = 2：设置为这个风格。
		UINT SetStyle(UINT style,DWORD option = 0);
		//
		inline UINT GetExStyle(){return (int)SendMessage(TB_GETEXTENDEDSTYLE,0,0);}
		//设置ToolBar的额外风格，option = 0：添加这个风格，option = 1：移除这个风格，option = 2：设置为这个风格。
		UINT SetExStyle(UINT exStyle,DWORD option = 0);
		//
		inline bool Refresh(){return (int)SendMessage(TB_AUTOSIZE,0,0)!=0;}
		//
		inline int Count(){return (int)SendMessage(TB_BUTTONCOUNT,0,0);}
		//
		inline int IndexOf(int id){return (int)SendMessage(TB_COMMANDTOINDEX,id,0);}
		//
		inline bool SetButtonIcon(int id,int image){return (int)SendMessage(TB_CHANGEBITMAP,id,image)!=0;}//I_IMAGENONE = -2
		//
		inline int GetButtonIcon(int id){return (int)SendMessage(TB_GETBITMAP,id,0);} 
		//
		inline bool SetButtonInfo(WORD id,LPTBBUTTONINFO pbi){return (int)SendMessage(TB_SETBUTTONINFO,id,pbi)!=0;}
		//
		inline int GetButtonInfo(WORD id,LPTBBUTTONINFO pbi){return (int)SendMessage(TB_GETBUTTONINFO,id,pbi);}
		//
		bool SetButtonInfo(WORD id,DWORD tbif,INT_PTR value);
		//
		INT_PTR GetButtonInfo(WORD id,DWORD tbif);//return -1 represnt false.-2 indecate no image,除了TBIF_BYINDEX其它风格只能有一个.
		//
		inline HWND GetToolTipCtrl(){return (HWND)SendMessage(TB_GETTOOLTIPS,0,0);}
		//
		inline HWND SetToolTipCtrl(HWND hToolTip){return (HWND)SendMessage(TB_SETTOOLTIPS,hToolTip,0);}
		//
		inline int GetButtonText(int id,LPWSTR buffer){return (int)SendMessage(TB_GETBUTTONTEXT,id,buffer);}
		//
		inline Twin GetBarSize(){Twin size(0,0);SendMessage(TB_GETMAXSIZE,0,&size);return size;}
	};
	//ProgressBar Control
	interface CONCISEUI_API ProgressBarPtr : virtual public WndPtr
	{
	public:
		ProgressBarPtr(){}
		ProgressBarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//返回值是上限和下限的低16值.如果上限下限值是一个32位值,用GetRange函数取得.
		inline Twin16 SetRange(int low,int hight){return (Twin16)SendMessage(PBM_SETRANGE32,low,hight);}
		//取得32位的上下限值.
		inline void GetRange(Twin& range){SendMessage(PBM_GETRANGE,0,&range);}
		//设置当前位置
		inline void SetPosition(int pos){SendMessage(PBM_SETPOS,pos,0);}
		//取得当前位置.
		inline uint GetPosition(){return (uint)SendMessage(PBM_GETPOS,0,0);}
		//设置最小步进.
		inline void SetIncrement(int delta){SendMessage(PBM_DELTAPOS,delta,0);}
		//设置步进增量.
		inline void SetStepValue(int step){SendMessage(PBM_SETSTEP,step,0);}
		//前进一个步进的值.
		inline void Step(){SendMessage(PBM_STEPIT,0,0);}
		//开始和停止进度条，适用于没有（PBS_MARQUEE风格）设置范围的情况。refresh是刷新周期，单位是毫秒。
		inline void SetMarquee(bool stop = 0,int refresh = 100){SendMessage(PBM_SETMARQUEE,!stop,refresh);}
	protected:
	};
	//ToolTip Control 这个控件必须在启用6.0版本控件时才可用.也就是说,必须要有MANIFEST.
	interface CONCISEUI_API ToolTipPtr : virtual public WndPtr
	{
	public:
		ToolTipPtr(){}
		ToolTipPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		INT_PTR Create(HWND hParent);
		//关闭或打开控件的提示功能.
		inline void Enable(bool enable = 1){this->SendMessage(TTM_ACTIVATE,enable,0);}
		//向一个窗口添加一个ToolTip,owner:是鼠标停留时要显示Tip的窗口,text:提示信息,id,pr,这两个参数指定窗口要显示提示
		//的区域和ID,因为要显示不同的提示,用ID区分.ttf:显示风格.重复的ID(或者HWND)参数,会覆盖上一个设置。
		bool AddTip(HWND owner,LPCWSTR text,UINT id = 0,LPRECT pr = 0,UINT ttf = 0);
		//删除一个提示。
		void DelTip(HWND owner,UINT uID = 0);
		//设置响应提示的窗口区域。
		bool SetTipRect(LPRECT pr,HWND owner,UINT uID = 0);
		//设置一个提示的信息细节,因为TI的结构比较复杂，正常的做法是先调用GetTipInfo初始化ti的值，（cbSize,hwnd 和 uId 必须初始化）
		//然后设置要改变的成员的值，再调用这个函数。
		inline bool SetTipInfo(LPTOOLINFO ti){return SendMessage(TTM_SETTOOLINFO,0,&ti)!=0;}
		//取得一个提示的信息细节,ti的cbSize成员必须初始化,hwnd 和 uId 成员也必须指定,因为信息是以它们为索引的.
		inline bool GetTipInfo(LPTOOLINFO ti){return 0!=SendMessage(TTM_GETTOOLINFO,0,ti);}
		//只有当TTS_BALLOON风格被设置时，这个函数才可用。title:要显示的标题文本,tti：要显示的图标，0，没有图标，1，信息，2，警告，3，错误。
		inline bool SetTitle(LPCWSTR title,int tti = TTI_NONE){return 0!=SendMessage(TTM_SETTITLE,tti,title);}
		//设置提示的文本。
		bool SetTipText(LPCWSTR text,HWND owner,UINT id);
		//设置提示的风格。
		bool SetTipFlag(UINT ttf,HWND owner,UINT id);
		//设置提示文本颜色。
		inline bool SetTextColor(COLORREF color){return 0!=SendMessage(TTM_SETTIPTEXTCOLOR,color,0);}
		//设置提示窗口的背景颜色。
		inline bool SetBkColor(COLORREF color){return 0!=SendMessage(TTM_SETTIPBKCOLOR,color,0);}
		//设置提示窗口的宽度。
		inline int SetTextWidth(UINT width){return (int)this->SendMessage(TTM_SETMAXTIPWIDTH,0,width);}
		//设置显示提示的延迟时间。
		inline void SetDelayTime(UINT ms,UINT ttdt = TTDT_INITIAL){this->SendMessage(TTM_SETDELAYTIME,ttdt,ms);}
		//强制提示窗口重新绘制。
		inline void UpDate(){SendMessage(TTM_UPDATE,0,0);}
		//设置提示的显示时间参数，单位是毫秒。flag:2-show,3-delay,1-reshow，0-all default.
		inline void SetTimeInfo(int t,UINT flag){SendMessage(TTM_SETDELAYTIME,flag,t);}
		//取回时间参数，flag:2-show,3-delay,1-reshow
		inline int GetTimeInfo(UINT flag){return (int)SendMessage(TTM_GETDELAYTIME,flag,0);}
	};

	//TabPage控件本身并不提供对控件的操作,它只提供一个框架,然后点击页按钮会引发相应的消息事件.在这些事件中处理控件的行为,使它表现的类似于处在不同的页面.
	interface CONCISEUI_API TabPagePtr : virtual public WndPtr
	{
	public:
		TabPagePtr(){}
		TabPagePtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//插入一个属性页,index是插入的位置索引,title是显示的标题,image是图标在图标列表控件中的索引,param是一个用户自定义参数.
		bool Add(int index,LPWSTR title = 0,int image = -1,LPARAM param = 0);
		//删除指定索引的项.
		bool Delete(int index){return 0!=SendMessage(TCM_DELETEITEM,index,0);}
		//返回项目数量.
		int GetCount(){return (int)SendMessage(TCM_GETITEMCOUNT,0,0);}
		//清空所有属性页.
		bool Clear(){return 0!=SendMessage(TCM_DELETEALLITEMS,0,0);}
		//返回当前属性页的索引.
		int GetCurSel(){return (int)SendMessage(TCM_GETCURSEL,0,0);}
		//设置指定索引的属性页为当前页.
		void SetCurSel(int index){SendMessage(TCM_SETCURSEL,index,0);}
		//设置图像列表控件.
		HIMAGELIST SetImageList(HIMAGELIST imgl){return (HIMAGELIST)SendMessage(TCM_SETIMAGELIST,0,imgl);}
		//取得图像列表控件.
		HIMAGELIST GetImageList(){return (HIMAGELIST)SendMessage(TCM_GETIMAGELIST,0,0);}
	};
	typedef union HotkeyParam
	{
		struct{
			unsigned char VirtualKey,CombineKey;
		};
		struct{
			unsigned short Value;
		};
		struct{
			WPARAM wParam;
		};
		void* operator new(size_t size){return MemoryAlloc((int)size);}
		void operator delete(void* p){MemoryFree(p);}
	}HotkeyParam;
	//表示组合键的一个结构.
	//热键控件
	interface CONCISEUI_API HotKeyPtr : virtual public WndPtr
	{
	public:
		HotKeyPtr(){}
		HotKeyPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//这个函数设置无效的键组合,hkcomb是要屏蔽的组合键,可以是多个HKCOMB_类常数的组合,当按下这些组合时,会被指定的hotkeyf取代.
		void SetRules(int hkcomb,int hotkeyf){SendMessage(HKM_SETRULES,MAKEWPARAM(hkcomb,0),MAKELPARAM(hotkeyf,0));}
		//设置组合键.
		void SetHotKey(unsigned char comb,unsigned char vk);
		//设置组合键.
		void SetHotKey(HotkeyParam hk){SendMessage(HKM_SETHOTKEY,hk.wParam,0);}
		//取得组合键,低位是键,高位是组合.
		HotkeyParam GetHotKey();
		//绑定到一个窗口,不是能是子窗口.
		bool Register(HWND frame){return (int)SendMessage(WM_SETHOTKEY,GetHotKey().wParam,0)>0;}
	};
	interface CONCISEUI_API RebarPtr : virtual public WndPtr{
		RebarPtr(){}
		RebarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	interface CONCISEUI_API ListViewPtr : virtual public WndPtr
	{
	public:
		ListViewPtr(){}
		ListViewPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//添加一个列，返回列的索引位置或者-1，参数：列标题名，宽度，位置（-1表示添加到最后）。
		int AddColumn(LPCWSTR title,int width,int index = -1);
		//添加一个项目，参数，第一列的文本，列位置（-1）表示添加到最后，返回位置索引或-1.
		int AddItem(LPCWSTR text,int index = -1);
		//设置一个条目的文本，参数：文本，行索引，列索引。
		bool SetItemText(LPCWSTR text,int index,int column);
		//设置绑定到项的数据
		bool SetItemData(int index,INT_PTR data);
		//删除指定索引的列，但是0索引列无法删除，如果要删除，先插入一个0宽度的列，再删除。
		bool DelColumn(int index){return 0!=this->SendMessage(LVM_DELETECOLUMN,index,0);}
		//删除指定行。
		bool DelItem(int index){return 0!=this->SendMessage(LVM_DELETEITEM,index,0);}
		//情况所有。
		bool Clear(){return 0!=this->SendMessage(LVM_DELETEALLITEMS,0,0);}
		//编辑标签。
		HWND EditItem(int index){SetFocus(_Handle);return (HWND)this->SendMessage(LVM_EDITLABEL,index,0);}
		//取消标签编辑。
		void CancelEdit(){this->SendMessage(LVM_CANCELEDITLABEL,0,0);}
		//返回列的总数。
		int GetColumnCount();
		//返回行的总数。
		int GetItemCount(){return (int)this->SendMessage(LVM_GETITEMCOUNT,0,0);}
		//取得指定列column和行index的文本。
		int GetItemText(String& text,int index,int column);
		//取得满足特定条件的项的索引，falg 用来确定项的类型和选项，比如 LVNI_SELECTED 用来取得选中项的索引。
		//after 指定搜索从这一索引项后面开始，-1 表示搜索所有项，注意如果这个值是 0 ，它不搜索第0项。
		int GetItemIndex(DWORD flag = LVNI_SELECTED,int after = -1){return (int)SendMessage(LVM_GETNEXTITEM,after,flag);}
		//
		inline LRESULT SetItem(LPLVITEM plvi){
			return SendMessage(LVM_SETITEM,0,(LPARAM)plvi);
		}
		//
		inline LRESULT GetItem(LPLVITEM plvi){
			return SendMessage(LVM_GETITEM,0,(LPARAM)plvi);
		}
		//
		//
		//
	};
	interface CONCISEUI_API StatusBarPtr : virtual public WndPtr
	{
	public:
		StatusBarPtr(){};
		StatusBarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//设置 StatusBar 分为几个部分，这个函数可以反复调用，以最后一次调用为准，多于原来的则增加分段，
		//少于原来的则删除后面多的分段。width 是一个整数数组，长度为 parts。每个数组元素的值对应每个分
		//段的右边界，单位是像素，如果某个元素比前面的值小，意味着这个段会被前面的段遮挡而不可见。如果
		//这个值是-1，则表示这个段直到控件的末尾（后面的段都将被遮挡）。
		bool SetParts(int parts,int* width){return 0!=SendMessage(SB_SETPARTS,parts,width);}
		//设置段的文本，index 是段的索引，type 是文字的显示风格，比如 SBT_POPOUT 。
		bool SetText(int index,LPCWSTR text,int type = 0){return 0!=SendMessage(SB_SETTEXT,index|type,text);}
	};
	interface CONCISEUI_API UpDownPtr : virtual public WndPtr
	{
	public:
		UpDownPtr(){}
		UpDownPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}

		//设置关联窗口，一般是一个文本窗口，返回前一个关联控件句柄。
		HWND SetBuddy(HWND buddy){return (HWND)SendMessage(UDM_SETBUDDY,buddy,0);}
		//取得关联窗口。
		HWND GetBuddy(){return (HWND)SendMessage(UDM_GETBUDDY,0,0);}
		//设置范围。
		void SetRange(int nLower,int nUpper){SendMessage(UDM_SETRANGE32,nLower,nUpper);}
		//取得范围。
		Twin GetRange(){Twin rg;SendMessage(UDM_GETRANGE32,&rg.x,&rg.y);return rg;}
		//设置位置。
		int SetPosition(int nPos){return (int)SendMessage(UDM_SETPOS32,0,nPos);}
		//取得位置，实际上这个数值是从buddy窗口取得的，所以如果buddy窗口不存在或者文本不是一个数字，返回的值是不可用的。
		int GetPosition(){return (int)SendMessage(UDM_GETPOS32,0,0);}
	};
	interface CONCISEUI_API TrackBarPtr : virtual public WndPtr
	{
		TrackBarPtr(){}
		TrackBarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		void SetRange(WORD min,WORD max){SendMessage(TBM_SETRANGE,1,MAKELPARAM(min,max));}
		void SetPos(LONG pos){SendMessage(TBM_SETPOS,1,pos);}
		int GetPos(){return (LONG)SendMessage(TBM_GETPOS,0,0);}
	};
	interface CONCISEUI_API HeaderPtr : virtual public WndPtr{
		HeaderPtr(){}
		HeaderPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//set header control size to appropriate parent window dimension,
		//return the control height.when width<0,width will equal parent client width.
		int AdjustSize(HWND parent,int x = 0,int y = 0,int width = -1);
		//insert a item to header control,return the new column item position index or -1 if failed.
		int AddItem(LPCWSTR text,int width,int index = -1);
		//delete item of index.
		bool DelItem(int index){return 0!=SendMessage(HDM_DELETEITEM,(WPARAM)index,0);}
		//set header control column text and width.
		bool SetItem(int index,LPCWSTR text,int width);
		//get column's width and title text,return width of column.
		int GetItem(int index,String& text);
		//get count of all column.
		int GetCount(){return (int)SendMessage(HDM_GETITEMCOUNT,0,0);}
		//set item width.
		bool SetItemWidth(int index,int width);
		//set item text
		bool SetItemText(int index,LPCWSTR title);
		//set item feature
		bool SetItem(int index,HDITEM* hdi);
		//get item info.
		bool GetItem(int index,HDITEM* hdi);
	};
	interface CONCISEUI_API LinkPtr : virtual public WndPtr{
		LinkPtr(){}
		LinkPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	interface CONCISEUI_API IpAddressPtr : virtual public WndPtr{
		IpAddressPtr(){}
		IpAddressPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
}