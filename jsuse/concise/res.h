#pragma once

namespace cs{
	//ResID结构用于表示一个资源的标识,因为它通常是一个LPCWSTR类型,但也可以是一个数字(此时高位为0),这个结构把他们统一.
	//你可以用这个结构做为函数的参数,可以向这个函数传递整数值和字串值,不用重写两个函数或者进行类型转换.
	typedef struct CONCISE_API ResID : public _class{
		HINSTANCE	Instance;
		LPCWSTR		Handle;

		inline ResID(){
			Instance = GetInstance();
			Handle = 0;
		}
		inline ResID(ResID& ri){
			*this = ri;
		}
		inline ResID(int id){
			Handle = (LPCWSTR)(INT_PTR)id;
			Instance = GetInstance();
		}
		inline ResID(LPCWSTR name){
			Handle = name;
			Instance = GetInstance();
		}
		inline ResID(LPCWSTR name,HINSTANCE inst){
			Handle = name;
			Instance = inst;
		}
		inline bool IsOrdinal(){
			return (0xFFFF0000&(INT_PTR)Handle)==0;
		}
		inline void operator = (ResID& ri){
			Handle = ri.Handle;
			this->Instance = ri.Instance;
		}
		inline void operator = (LPCWSTR name){
			Handle = name;
		}
		inline void operator = (int id){
			Handle = (LPCWSTR)(INT_PTR)id;
		}
		inline operator LPCWSTR (){return Handle;}
	}ResID;
	//ResName用于一个资源的标识,当这个标识是一个LPCWSTR类型时,需要分配内存,这个类可以自动分配或释放相应的内存.
	//它也可以安全的做为函数参数(传值方式),但那种场合用ResID就足够了(资源占用要小).
	//ResName不像ResID那样,仅仅保存资源ID的值,而且保存这个值指向的字符串，如果ResID指向的是一个
	//字串，那么当它指向的字串销毁了，那么它也不再有效了，但是ResID保存这个字串，而不依赖于
	//赋值的原字串。
	class CONCISE_API  ResName : public _class
	{
	protected:
		LPCWSTR _Handle;
	public:
		ResName():_Handle(0){
		}
		ResName(ResID name):_Handle(0){
			*this = name;
		}
		//这个函数可以避免ResName作为参数类型时的成员复制.哪种情况是隐含的.
		ResName(ResName& rn):_Handle(0){
			*this = rn;
		}
		void operator = (ResID name);
		inline bool IsOrdinal(){
			return (0xFFFF0000&(INT_PTR)_Handle)==0;
		}
		inline void operator = (ResName& rn){
			*this = rn._Handle;
		}
		inline operator LPCWSTR (){
			return _Handle;
		}
		~ResName();
	};
	//获取资源的数据指针,module = 0表示从当前程序加载资源.这个函数用于提供资源的数据指针,并不加载资源.
	//对于Windows内置的资源类型,有相应的API提供加载函数,不要用这个函数来加载相应的资源.
	CONCISE_API void* GetResource(ResID name,ResID type,DWORD* size = 0);

	/* 菜单本质上也是一个窗口,菜单的句柄就是窗口的句柄.每一个子菜单,就是一个窗口(它可以包含若干菜单项).菜单的另一
	部分是树结构的菜单项,每一个菜单项包含文本,菜单ID(也就是命令ID),子菜单等信息(这种菜单项一般是不可以点的).ID有两
	种用途,一种是用来标识菜单,另一种是点击菜单项,向关联窗口发送的WM_COMMAND的ID.前一种作用,ID的取值可以是UINT,后一
	种用途,ID必须是一WORD值.所以,凡是可以点击的菜单,ID必须是一个WORD值,而包含子菜单的菜单项或Split菜单项的ID可以超
	出WORD的值,事实上系统总是给这一类菜单项分配一个超出WORD的值(生成这类菜单项一般不考虑ID参数,而是系统自动分配ID).
	另外,ID可以相同,所以用ID来索引菜单项,并不可靠.
	    用位置索引菜单项快速可靠,但某些时候不如使用ID索引方便,比如ID可以是菜单的多级子项,而这种情况不能用位置索引,
	位置索引只适用于当前菜单的菜单项.
	    其实菜单分为顶级菜单和子菜单,它们有本质的区别.比如一个弹出菜单就是一个子菜单,而一个窗口菜单是一个顶级菜单,
	它们是不能混用的.LoadMenu加载的是一个顶级菜单.对应的函数分别是CreateMenu和CreatePopupMenu(前者生成一个顶级菜单,
	后者生成一个子菜单;*/
	class CONCISE_API  Menu : public HandleType<HMENU>
	{
	public:
		Menu();
		~Menu();
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
		HMENU AddSubMenu(UINT uPos,LPCWSTR text,HMENU sub = NULL,UINT id = 0,UINT mf = MF_BYPOSITION);
		//添加一个菜单分隔线(它本质上也是一个菜单项,但仅仅起一个分隔作用,不具备菜单的其它功能).
		bool AddSplit(UINT uPos,UINT mf = MF_BYPOSITION);
		//一般来说子菜单ID是未知的（系统自动分配，我们无需知道），所以子菜单只由位置索引。
		HMENU GetSubMenu(UINT uPos);
		//取得当前菜单的菜单项个数，含菜单项和子菜单。
		int GetItemCount();

		//如果在WM_CONTEXTMENU消息中调用这些函数，可以直接把lParam传给它。如果在鼠标右键消息中调用这些函数，需要把坐标转换为屏幕坐标，也就是调用 ClientToScreen.
		//弹出菜单，x，y是位置；hOwner是响应的窗口，不能为零，否则菜单不能销毁；tpm是弹出选项。
		bool Popup(short x,short y,HWND hOwner,UINT tpm = 0);
		//弹出菜单，lParam是位置；hOwner是响应的窗口，不能为零，否则菜单不能销毁；tpm是弹出选项。
		bool Popup(LPARAM lParam,HWND hOwner,UINT tpm = 0);
		//弹出子菜单，uIndex是菜单的位置索引，lParam是位置；hOwner是响应的窗口，不能为零，否则菜单不能销毁；tpm是弹出选项。
		bool PopSubMenu(UINT uIndex,LPARAM lParam,HWND hOwner,UINT tpm = 0);
		//弹出子菜单，uIndex是菜单的位置索引，x，y是位置；hOwner是响应的窗口，不能为零，否则菜单不能销毁；tpm是弹出选项。
		bool PopSubMenu(UINT uIndex,short x,short y,HWND hOwner,UINT tpm = 0);
		//因为对于顶级菜单弹出的只能是它的子菜单.
		bool Popup(HWND hOwner,UINT tpm = 0);
		bool PopupSubMenu(UINT uPos,HWND hOwner,UINT tpm = 0);
		//把实例和窗口菜单交换，类似于DC里面的GDI资源的Select函数.
		bool SelectWindow(HWND hWnd);
		//绑定一个窗口的菜单（菜单和窗口共同拥有这个菜单）。
		bool BindFromWindow(HWND hWnd);
		//绑定当前菜单到一个窗口（菜单和窗口共同拥有这个菜单）。
		bool BindToWindow(HWND hWnd);

		//菜单项的相关函数.
		bool SetItemState(UINT uPos,UINT mfs,UINT mf = MF_BYPOSITION);
		UINT GetItemState(UINT uPos,UINT mf = MF_BYPOSITION);//(8),MF_DISABLED(2),MF_MENUBREAK(64),MF_OWNERDRAW(0x100),MF_POPUP(0x10),MF_SEPARATOR(0x800) 

		bool DeleteItem(UINT uPos,UINT mf = MF_BYPOSITION);//把一个菜单项(可以是一个子菜单),从菜单中移除,并且删除它.
		bool AddItem(UINT uPos,LPCWSTR text,UINT msgId,UINT mf = MF_BYPOSITION);
		//一个子菜单项也可以有一个ID,但是这个ID只作为标识使用,所以尽量使用WORD之外的值,也可以由系统来指定一个值(它总是大于WORD).
		//一个子菜单,必须至少有一个菜单项,否则在自绘菜单中,会出错.返回子菜单的句柄,如果sub=0,这个函数创建一个空的子菜单.
		bool RemoveItem(UINT uPos,UINT mf = MF_BYPOSITION);//把一个菜单项(可以是一个子菜单),从菜单中移除,但并不删除它,可以在别处继续使用它.

		bool SetItemType(UINT uPos,UINT mft,UINT mf = MF_BYPOSITION);
		UINT GetItemType(UINT uPos,UINT mf = MF_BYPOSITION);

		bool SetItemCheck(UINT uPos,bool checked = 1,UINT mf = MF_BYPOSITION);
		bool GetItemCheck(UINT uPos,UINT mf = MF_BYPOSITION);

		bool SetItemDisable(UINT uPos,bool disable = 1,UINT mf = MF_BYPOSITION);
		bool GetItemDisable(UINT uPos,UINT mf = MF_BYPOSITION);

		bool SetItemText(UINT uPos,LPCWSTR text,UINT mf = MF_BYPOSITION);
		int GetItemText(UINT uPos,String& text,UINT mf = MF_BYPOSITION);

		bool SetItemID(UINT uPos,DWORD id,UINT mf = MF_BYPOSITION);
		UINT GetItemID(int index);
	};
};