#pragma once
namespace cs{
	//这个函数提取与文件关联的图标.如果文件不存在,返回一个缺省文件图标.如果传文件名为空串,返回文件夹的图标.
	//对一个文件夹来说,如果关联一个图标,则返回这个图标,反之返回一个文件夹图标.对可执行文件(包括DLL,OCX等),
	//如果这个可执行文件包含图标,则返回这个图标,并且以index为索引(小于0时返回0索引的图标);反之返回一个可执
	//行文件的缺省图标;对非EXE的可执行文件,index为-1时,返回类型图标.对于图像文件,当index为0时,返回这个文件
	//的缩略图,index为-1时,返回类型图标.也就是说,除了EXE和指定图标的文件夹,只要index传-1,则返回文件类型图标;
	CONCISE_API HICON GetFileIcon(LPCWSTR file,int index = 0);
	//设置一个文件的图标,iconfile可以是一个图标文件或可执行文件,iconindex是文件中图标的索引,toolinfo是提示信息.
	//这个函数是平台相关的,适用于XP,不保证适用于其它版本的操作系统.
	CONCISE_API bool SetFolderIcon(LPCWSTR folder,LPCWSTR iconfile,LPCWSTR iconindex = 0,LPCWSTR toolinfo = 0);
	//设置文件属性，add:此属性将被添加，del：此属性将被移除。
	CONCISE_API bool SetFileAttr(LPCWSTR file,DWORD add,DWORD del);
	//直接设置文件属性。
	CONCISE_API inline bool SetFileAttr(LPCWSTR file,DWORD attr){return 0!=SetFileAttributes(file,attr);}

	//搜索文件使用的数据结构
	typedef struct CONCISE_API FileSearchData : public _struct{
		WIN32_FIND_DATA*	FileInfo;		//文件的相关属性和信息。
		LPCWSTR				Original;		//文件夹路径，这个路径末尾不包含反斜杠。
		LPCWSTR				Relative;		//相对路径，这个路径末尾和开头不包含反斜杠。
		//下面两个标志可以在回调里随时终止和跳过搜索。
		bool				ExitSearch;		//是否取消搜索的标识。
		bool				SkipThisFolder;	//如果FileSearch设置了SubFolderPre标志和SearchSubFolder标志，
		//在搜索子文件夹前，回调会被呼叫一次，如果此时设置这个标识为1，会跳过搜索这个文件夹。
		//当前项是否是一个文件夹。
		inline bool			IsDir(){
			return (FileInfo->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
		}
		FileSearchData();
		FileSearchData(WIN32_FIND_DATA* wfd,LPCWSTR ori,LPCWSTR rel);
	}FileSearchData;
	class CONCISE_API FolderWatch : public _class{
	public:
		enum WatchType{
			FileName = FILE_NOTIFY_CHANGE_FILE_NAME,
			FolderName = FILE_NOTIFY_CHANGE_DIR_NAME,
			Atrributes = FILE_NOTIFY_CHANGE_ATTRIBUTES,
			Size = FILE_NOTIFY_CHANGE_SIZE,
			LastWrite = FILE_NOTIFY_CHANGE_LAST_WRITE,
			LastAccess = FILE_NOTIFY_CHANGE_LAST_ACCESS,
			Creation = FILE_NOTIFY_CHANGE_CREATION,
			Security = FILE_NOTIFY_CHANGE_SECURITY,
			Normal = FileName|FolderName|LastWrite|Creation
		};
		enum FileAction{
			Added = FILE_ACTION_ADDED,		// 1
			Removed = FILE_ACTION_REMOVED,	//2
			Modifyed = FILE_ACTION_MODIFIED,//3
			RenameOldName = FILE_ACTION_RENAMED_OLD_NAME,	//4
			RenameNewName = FILE_ACTION_RENAMED_NEW_NAME	//5   
		};
	protected:
		HANDLE					_compPortHandle;
		HANDLE					_handle;
		String			_name;
		Memory<char>		_buffer;
		DWORD					_type;	//监视类型，
		bool					_sub;	//是否监视子文件夹，
	public:
		Function<void,DWORD,LPCWSTR,FolderWatch*>	OnChange;	//接收文件系统消息的回调函数.
		Function<void,FolderWatch*>			OnStop;		//停止监视通知，异步调用时，你可以在这个成员绑定的函数里删除当前对象。
		void*										UserData;	
		FolderWatch();
		LPCWSTR GetWatchPath(){
			if(_name.IsNull()) return 0;
			return _name;
		}
		DWORD GetWatchType(){
			return _type;
		}
		bool GetWatchSub(){
			return _sub;
		}
		//关闭监视，因为Start函数是一个阻塞函数，而且直到调用这个函数，不会返回，所以这个函数和Start总是在两个线程里调用。
		//这个函数发送关闭消息，不会等待关闭结束的操作，知道Start返回，对象才会被完全关闭。
		bool Close();
		//建立监视环境。
		bool Create(LPCWSTR folder,bool subfolder = true,DWORD type = Normal);
		//这是个阻塞函数，如果不希望阻塞，在一个新的线程调用。
		bool Start();
		~FolderWatch(){
			Close();
		}
	};
	class CONCISE_API MultiFolderWatch : public _class{
	protected:
		ObjectList<FolderWatch> _watchList;
		TaskPool<bool>			_taskEngine;
		//_taskEngine放在_watchList的后面，可以保证先于_watchList被销毁，但是_taskEngine销毁需要等待所有线程退出
		//在析构里调用Stop函数结束监视线程，如果不调用，会一直等待。
		CriticalSection	_csList;

		void _onChange(DWORD action,LPCWSTR fn,FolderWatch* pfw);
		void _onStop(FolderWatch* pfw);
	public:
		//接收文件系统消息的回调函数，这个回调可能是多线程异步呼叫。
		Function<void,DWORD,LPCWSTR,FolderWatch*> OnChange;
		//开始监视一个文件夹，每次调用Start都会在一个新线程里。
		bool Start(LPCWSTR folder,void* userData = 0,bool subfolder = true,DWORD type = FolderWatch::Normal);
		//停止监视一个文件夹，但是对应的线程不一定结束。
		bool Stop(LPCWSTR folder);
		//停止监视所有文件夹，关闭所有线程，并且等待这些线程结束，才会返回。
		void Close();
		~MultiFolderWatch();
	};

}