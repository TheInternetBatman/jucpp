#pragma once
#include <shlobj.h>//this is use BIF_ const.

namespace cs{
	class CONCISEUI_API  FolderDialog : public _class
	{
	protected:
		Memory<wchar_t> _Path;
		Function<int,HWND,UINT,LPARAM,LPARAM> _bcp;
		int _BrowseCallbackProc(HWND hwnd,UINT msg,LPARAM lp,LPARAM pData);
	public:
		FolderDialog(void);
		bool Show(HWND owner = 0,LPCWSTR title = 0,LPCWSTR init = 0,DWORD flags = BIF_USENEWUI);
		LPCWSTR GetFolder(){return _Path;}
	};
	class CONCISEUI_API  FileDialog : public HandleType<HWND>
	{
	protected:
		//这个函数指定对话框的回调函数,可以在派生类中重新指定一个自定义函数,但那种情况下必须自己给_Handle赋值.
		Function<UINT_PTR,HWND,UINT,WPARAM,LPARAM> mDlgProc;
		UINT_PTR OFNHookProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
		//文件名缓存.
		VirtualAddress mFileNameBuffer;
		//文件类型过滤列表和选择的文件列表.
		StringMemList mFilter,mSelectFiles;
		//对话框窗口句柄(ONFHookProc函数中的hDlg是对话框控件的窗口句柄,和这个是不同的).这个句柄可用于CDM_类消息的函数.
		uint mFilterIndex;
	public:
		String Title					//对话框标题.
			,DefaultExt						//缺省文件类型(当键入的文件名不含后缀),这个值不需要加点，直接设置后缀名.
			,DefaultFile					//缺省文件名.
			,InitDir;						//初始化路径.
		DWORD Flags;						//对话框风格.
		bool FlagsEx;						//是否有额外的风格(左侧的面板).

		//点击OK按钮时发生,返回true对话框将保持打开,而不会关闭.
		Function<bool,LPOPENFILENAME,FileDialog*> OnFileOk;
		//类型筛选框变化时发生.参数是筛选索引.
		Function<void,int,FileDialog*>OnTypeChange;
		//选择项变化时发生.参数是目录和文件名（不含目录）。此消息有bug。
		//Function<void,LPCWSTR,LPCWSTR>OnSelChange;
		//当选择的文件夹变化时.
		Function<void,LPCWSTR,FileDialog*>OnFolderChange;

		FileDialog();
		//显示打开文件对话框.
		bool ShowOpen(HWND owner = 0);
		//显示保存文件对话框.
		bool ShowSave(HWND owner = 0);
		//如果对话框是单选(包括多选,但是只选了一个文件),index必须是0,返回的是全路径,如果是多选,0返回路径,其它返回文件名(不包括路径).
		LPCWSTR GetFileName(uint index = 0);
		//每调用一次这个函数在下面的文件类型选择组合框里添加一行.
		//格式:showText可以是任意字符串,filter是如下的格式 "*.txt;*.ini;*.cpp"
		void AddFilter(LPCWSTR showText,LPCWSTR filter,uint index = -1);
		//设置缺省的文件过滤器(索引).
		bool SetFilterIndex(uint index);		
		//删除已添加的文件过滤器.
		bool DeleteFilter(uint index);		
		//对话框主窗口的句柄.
		HWND Handle(){return _Handle;}
		//选择的文件数量(打开文件时).
		uint GetSelectCount();	
		//返回过滤器的索引.
		uint GetFilterIndex(){return mFilterIndex;}
	};
	//颜色对话框.
	class CONCISEUI_API  ColorDialog : public _class
	{
	public:
		enum ColorFlags{ColorDlgNormal=0,	//普通样式
			ColorDlgBasic=4,				//只显示基本颜色
			ColorDlgFullOpen=2};			//展开颜色选择.
		ColorDialog();
		//CC_SOLIDCOLOR(只显示基本颜色),CC_PREVENTFULLOPEN(灰选全部打开按键),CC_FULLOPEN(全部打开)
		bool Show(HWND hWnd = 0,ColorFlags flags = ColorDlgNormal);
		//设置和获取16个自定义颜色.
		static DWORD CustomColor[16];
		DWORD Color;
	};
	//显示一个字体对话框,因为mCf和mLf成员实际上是开放的,所以如果愿意,可以控制ChooseFont函数的任何选项.
	class CONCISEUI_API  FontDialog : public _class
	{
	protected:
		wchar_t mStyle[4];

		Function<UINT_PTR,HWND,UINT,WPARAM,LPARAM> mDlgProc;
		UINT_PTR CFHookProc(HWND hdlg,UINT uiMsg,WPARAM wParam,LPARAM lParam);
	public:
		FontDialog();
		//响应Apply按钮的回调函数，从LogFont成员取得选择的字体参数.
		Function<void,LPLOGFONT,FontDialog*> OnApply;
		//实际上虽然对话框会用mLogFont成员来初始化显示的值,但是只有下面的成员才会真正使用.还有一个颜色可以由mChooseFont来传递.
		void Initialize(int size,wchar_t* name,Color c = 0,bool bold = 0,bool italic = 0,bool underline = 0,bool strike = 0);
		//显示这个对话框.apply决定是否有应用按钮,effects用于决定是否显示效果选项.LogFont成员获取返回的值.
		bool Show(HWND hWnd = 0,bool apply = 0,bool effects = 0);
		//微软在设计的时候弄得非常乱，不得不用两个结构初始化和取回参数，而这两个结构的成员又很多都用不上，或者没有效果。
		Object<LOGFONT> LogFont; //这个成员即用于初始化对话框的参数，也用于取回选择的字体参数。
		Object<CHOOSEFONT> ChooseFont;//这个成员即用于初始化对话框的参数，也用于取回选择的字体参数.
	};
}