#pragma once
namespace cs{
	typedef struct CONCISEUI_API USERMENUPARAM : public _struct{
		uint ItemHeight,_TextCY,LeftMargin,
			ImageX,ImageY;
		HBITMAP BmpCheck;
		int SelectBkColor,DefaultBkColor,SelectBorderColor,TextColor;
		HBRUSH Brush;
		HPEN Pen;
		Font	Font;
		USERMENUPARAM();
	}USERMENUPARAM;
	/*菜单弹出必须有一个容器窗口（用来处理菜单消息），否则菜单无法关闭（点击任何地方都不消失），
	如果给菜单设置了容器（通过SetOwnerWnd函数，菜单消息总是在容器窗口中处理），弹出菜单的
	hOwner 参数将优先使用 SetOwnerWnd 的窗口句柄。如果没有调用 SetOwnerWnd （比如要绑定到非
	IWnd的窗口上，可以在消息里分别调用 Hook 函数。任何时候必须设置 hOwner 参数为一个有效的
	窗口句柄。

	此自绘菜单不能有ID相同的菜单项，包括子菜单项，使用这个类添加的子菜单会分配唯一 ID （但是不支持
	多线程同步）。

	如果一个窗体有两个自绘菜单，也不能有相同的ID，因为消息是混在一起的，id 是区分菜单项唯一的标识。*/

	class CONCISEUI_API UserMenu : public HandleType<HMENU>{
	protected:
		IWnd* _parent;
	public:
		BitmapList*		ImageList;
		USERMENUPARAM	Param;
		UserMenu();
		~UserMenu();
		//处理 WM_MEASUREITEM 的函数，如果没有有效的调用 SetParentWnd 函数，需要在菜单所属窗口的消息中调用这个函数。
		void HookMeasureItem(Message* msg,IWnd*);
		//处理 WM_DRAWITEM 的函数，如果没有有效的调用 SetParentWnd 函数，需要在菜单所属窗口的消息中调用这个函数。
		void HookDrawItem(Message* msg,IWnd*);
		//设置窗口的容器对象，parent 会自己调用父对象的 HookMeasureItem 和 HookDrawItem 。
		//parent = NULL，将会解除原来的绑定，如果parent是一个新值，原来的绑定会解除，并且绑定到新窗口上。
		void SetOwnerWnd(IWnd* parent);
		//如果不使用 UserMenu 内置的绘制，可以通过设置 OnMeasureItem，来绑定一个函数，自己处理。
		Function<void,LPMEASUREITEMSTRUCT> OnMeasureItem;
		//如果不使用 UserMenu 内置的绘制，可以通过设置 OnDrawItem，来绑定一个函数，自己处理。
		Function<void,LPDRAWITEMSTRUCT> OnDrawItem;

		//窗口菜单必须包含子菜单,而它自身不能是一个子菜单.一个Popup菜单就是一个子菜单.
		bool Create(bool popup = 0);
		//从资源加载菜单,0表示从当前应用程序实例加载.
		bool Load(ResID rMenu);
		//如果菜单已经设为一个窗口的菜单,这个操作会失败,也就是说,不能销毁一个绑定到窗口的菜单.而窗口在销毁时会自动清除菜单资源.
		void Destroy();
		//捆绑一个菜单
		void Attach(HMENU hMenu);
		HMENU Detach(){return HandleType::Detach();}
		//uPos是添加的位置,当由ID索引时,uPos可以是菜单的任意级子菜单的ID,表示插入到这个
		//菜单项的前面.当由位置索引时,插入菜单项只能是当前菜单的子项,成功插入菜单后,它的位置索引就是uPos.
		HMENU AddSubMenu(UINT uPos,LPCWSTR text,HMENU sub = NULL,HBITMAP image = 0,bool byPos = true);
		//添加一个菜单分隔线(它本质上也是一个菜单项,但仅仅起一个分隔作用,不具备菜单的其它功能).
		bool AddSplit(UINT uPos,bool byPos = true);
		//一般来说子菜单ID是未知的（系统自动分配，我们无需知道），所以子菜单只由位置索引。
		HMENU GetSubMenu(UINT uPos);
		//取得当前菜单的菜单项个数，含菜单项和子菜单。
		int GetItemCount();
		//把菜单项以及子项设置风格为自绘，load和attach非自绘风格的菜单需要调用这个函数。
		void ToOwnerStyle();

		//如果在WM_CONTEXTMENU消息中调用这些函数，可以直接把lParam传给它。如果在鼠标右键消息中调用这些函数，需要把坐标转换为屏幕坐标，也就是调用 ClientToScreen.
		//弹出菜单，x，y是位置；hOwner是响应的窗口，不能为零，否则菜单不能销毁；tpm是弹出选项。
		//hOwner：如果设置了 parent 窗口（通过 SetParent 传递了有效的窗口对象），这个窗口句柄无效。
		bool Popup(short x,short y,HWND hOwner = 0,UINT tpm = 0);
		//弹出菜单，lParam是位置；hOwner是响应的窗口，不能为零，否则菜单不能销毁；tpm是弹出选项。
		//hOwner：如果设置了 parent 窗口（通过 SetParent 传递了有效的窗口对象），这个窗口句柄无效。
		bool Popup(LPARAM lParam,HWND hOwner = 0,UINT tpm = 0);
		//弹出子菜单，uIndex是菜单的位置索引，lParam是位置；hOwner是响应的窗口，不能为零，否则菜单不能销毁；tpm是弹出选项。
		bool PopSubMenu(UINT uIndex,LPARAM lParam,HWND hOwner,UINT tpm = 0);
		//弹出子菜单，uIndex是菜单的位置索引，x，y是位置；hOwner是响应的窗口，不能为零，否则菜单不能销毁；tpm是弹出选项。
		bool PopSubMenu(UINT uIndex,short x,short y,HWND hOwner,UINT tpm = 0);
		//因为对于顶级菜单弹出的只能是它的子菜单.
		bool Popup(HWND hOwner = 0,UINT tpm = 0);
		bool PopupSubMenu(UINT uPos,HWND hOwner = 0,UINT tpm = 0);
		//把实例和窗口菜单交换，类似于DC里面的GDI资源的Select函数.
		bool SelectWindow(HWND hWnd);
		//绑定一个窗口的菜单（菜单和窗口共同拥有这个菜单）。
		bool BindFromWindow(HWND hWnd);
		//绑定当前菜单到一个窗口（菜单和窗口共同拥有这个菜单）。
		bool BindToWindow(HWND hWnd);

		bool DeleteItem(UINT uPos,bool byPos = true);//把一个菜单项(可以是一个子菜单),从菜单中移除,并且删除它.
		bool AddItem(UINT uPos,LPCWSTR text,UINT msgId,HBITMAP bmp = 0,bool byPos = true);
		//一个子菜单项也可以有一个ID,但是这个ID只作为标识使用,所以尽量使用WORD之外的值,也可以由系统来指定一个值(它总是大于WORD).
		//一个子菜单,必须至少有一个菜单项,否则在自绘菜单中,会出错.返回子菜单的句柄,如果sub=0,这个函数创建一个空的子菜单.
		bool RemoveItem(UINT uPos,bool byPos = true);//把一个菜单项(可以是一个子菜单),从菜单中移除,但并不删除它,可以在别处继续使用它.

		//菜单项的相关函数.
		bool SetItemState(UINT uPos,UINT mfs,bool byPos = true);
		UINT GetItemState(UINT uPos,bool byPos = true);//(8),MF_DISABLED(2),MF_MENUBREAK(64),MF_OWNERDRAW(0x100),MF_POPUP(0x10),MF_SEPARATOR(0x800) 

		bool SetItemType(UINT uPos,UINT mft,bool byPos = true);
		UINT GetItemType(UINT uPos,bool byPos = true);

		bool SetItemCheck(UINT uPos,bool checked = 1,bool byPos = true);
		bool GetItemCheck(UINT uPos,bool byPos = true);

		bool SetItemDisable(UINT uPos,bool disable = 1,bool byPos = true);
		bool GetItemDisable(UINT uPos,bool byPos = true);

		bool SetItemText(UINT uPos,LPCWSTR text,bool byPos = true);
		int GetItemText(UINT uPos,String& text,bool byPos = true);

		bool SetItemID(UINT uPos,DWORD id,bool byPos = true);
		UINT GetItemID(int index);

		bool SetItemImage(uint pos,HBITMAP bmp,bool byPos = true);//设置菜单项的图像
		HBITMAP GetItemImage(uint pos,bool byPos = true);//获取菜单项的图像

		bool SetItemInfo(UINT uid,LPMENUITEMINFO mii,bool byPos = true);
		bool GetItemInfo(UINT uid,LPMENUITEMINFO mii,bool byPos = true);
	};
}