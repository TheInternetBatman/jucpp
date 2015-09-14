#pragma once
namespace cs{
	//判断一个路径的类型,0：路径不存在，1：是一个文件，2：是一个文件夹。
	CONCISE_API int GetFileType(LPCWSTR file);
	//对应非法的目录名称，系统的 GetFileAttributes 可能返回一个错误的值。这个函数添加了检测目录合法性。
	CONCISE_API DWORD GetFileAttributes(LPCWSTR path);
	//生成一个文件夹，file=1表示path是一个文件，生成它的文件夹，否则，path是文件夹的的路径。
	CONCISE_API bool CreateFolder(LPCWSTR path,bool file = 0);
	//删除一个文件或空文件夹。
	CONCISE_API bool RemoveFile(LPCWSTR file);
	//转换为全路径，name 返回文件名保护后缀，name 是 path 的某个位置。
	CONCISE_API bool FPToFullPath(String& path,LPCWSTR* name = 0);
	//比较两个字串是否表示相同的路径，但是不验证字串是否合法的路径名称。
	CONCISE_API bool FPIsSame(LPCWSTR path1,LPCWSTR path2);
	//取得字串的路径，末尾没有斜杠或反斜杠。如果字串本身是一个路径，则取得上一级目录。如果没有上一级目录，则字串不变化。
	CONCISE_API void FPToParent(String& ps);
	//把斜杠转换为反斜杠。
	CONCISE_API void FPToBackSlash(String& ps);
	//把反斜杠转化为斜杠。
	CONCISE_API void FPToSlash(String& ps);
	//取得后缀名，不包含字符"."。
	CONCISE_API void FPToExt(String& ps);
	//取得文件的标题（不包含后缀名的文件名），返回规则是先搜索末尾的“.”，再搜索斜杠或反斜杠，把之间的字串拷贝都title中。
	CONCISE_API void FPToTitle(String& ps);
	//取得文件名(文件标题和后缀)。
	CONCISE_API void FPToFileName(String& ps);
	//去掉末尾的斜杠或反斜杠。
	CONCISE_API void FPDelTailSlash(String& ps);
	//保持末尾有一个斜杠或反斜杠，表示字串是一个目录。
	CONCISE_API void FPSetTailSlash(String& ps);
	//链接两个字串，中间保持一个“.”号。
	CONCISE_API void FPLinkExt(String& ps,LPCWSTR ext);
	//连接两个字串，中间保持一个反斜杠。
	CONCISE_API void FPLinkPath(String& ps,LPCWSTR path);
	typedef struct DRIVEINFO
	{
		wchar_t Name[65],Path[3],FileSystemType[6];
		DWORD Type,Flag,MaxPath;
	}DRIVEINFO,*LPDRIVEINFO;
	class CONCISE_API Drives
	{
	public:
		Drives();
		~Drives();
		bool GetInfo(int index,LPDRIVEINFO lpDriveInfo);
		bool GetInfo(LPCWSTR path,LPDRIVEINFO lpDriveInfo);
		int Count(){return _Count;}
	protected:
		int _Count;
		StringMemList Paths;
	};
	typedef struct ListData : public _struct
	{
		WIN32_FIND_DATA*	FindData;		//文件的相关属性和信息。
		LPCWSTR				Original;		//原始路径, 就是搜索的参数.
		LPCWSTR				Relative;		//相对路径，这个路径包含一个末尾的反斜杠。
		WORD				Flag;			//这个成员指示呼叫方式，如果是文件夹，且_Pre方式呼叫，此值为1，_After方式呼叫此值为2，文件时，此值为0.
		bool				Stop;			//是否取消搜索的标识。
		bool				Skip;			//在PreCall时这个值如果设为 true 搜索将跳过这个文件夹的文件（不搜索它的文件）。
		void*				ExtraData;		//用户可能需要传递额外的数据。
	}ListData;
	//
	//FileTree用于枚举一个目录的所有文件,可以搜索多级子文件夹.OnSearch是一个回调函数,每搜索到一个文件(或文件夹),这个函数
	//会被呼叫.文件被列举的次序和具体情况有关,一般NTFS分区和CDFS分区是字母顺序被列举,而FAT32分区和文件在磁盘上的结构有关.
	//设置搜索选项，sub指示是否搜索子文件夹，dir指示是否接受文件夹（dir为0时，不接受文件夹，但如果sub=1子文件夹中的文件仍然会被搜索），
	//preCall指示搜索到文件夹时，先返回文件夹再搜索它的内容，afterCall指示搜索
	//到文件夹是，先搜索它的子文件，在返回它本身。如果两个都为TRUE，则搜索到文件夹时会返回两次。如果都为0，则不返回文件夹。
	//对于dir为 0，sub 不为 0 的情形，在执行存储搜索时，因为子文件夹的文件需要保存在这个分支下，所以，文件夹仍然会被保存。
	//FileTree可以异步执行多个搜索，OnList 和 OnCompete 可以重新指定，而不会影响已经执行的搜索。但是，改变过滤器和搜索选项，
	//会影响正在执行的搜索。
	//Search（）函数的dir参数如果是一个含通配符的字串，或者是一个文件，搜索会返回符合通配符的文件（夹),或者文件本身。如果是一个目录，则执行的是搜索目录。
	class CONCISE_API FileTree : public _class
	{
	protected:
		Thread th;
		StringMemList _Filter;
		bool _Sub,_Pre,_After;

		Tree<String>* _Store(Tree<String>* files,LPCWSTR file,bool sub = 0);
		bool _search(LPCWSTR dir,void* extra = 0);
		bool _Search(LPCWSTR original,LPCWSTR relative,void* extra = 0);
		bool _searchStore(LPCWSTR dir,Tree<String>* tl);
		bool _SearchStore(LPCWSTR original,LPCWSTR relative,Tree<String>* tl);
	public:
		//搜索回调函数。
		Function<void,ListData*> OnList;
		//异步调用时，搜索线程结束通知函数，同步调用不会调用这个函数。void* 是搜索时指定的 extra 或 tl 实例自身的指针，LPCWSTR 是搜索的文件夹。
		Function<void,void*,LPCWSTR> OnComplete;
		//Function<void,void*,LPCWSTR,bool> OnStart;

		FileTree();
		//返回文件类型过滤器，它是一个字符串列表。成员是文件后缀名，不包含“.”和“*”字符如“jpg”而不是".jpg"和"*.jpg"，不支持通配符，不区分大小写。
		StringMemList* GetFilter(){return &_Filter;}
		//设置是否搜索子文件夹
		void SetSearchSub(bool enable){_Sub = enable;}
		//设置回调函数先返回一次子文件夹呼叫，再返回它包含的文件。
		void SetPreCall(bool enable){_Pre = enable;}
		//设置回调函数返回它包含的文件之后，才执行一次返回文件夹呼叫。
		void SetAfterCall(bool enable){_After = enable;}
		//dir是要搜索的目录,extra是用户设置的数据，可以在回调函数中接收.asyn指示调用是否是异步的。如果dir参数不是一个文件夹，而是一个文件，函数返回fasle。
		bool Search(LPCWSTR dir,void* extra = 0,bool asyn = 0);
		//dir是要搜索的目录,tl是用于存储文件名的树形数据结构.asyn指示调用是否是异步的。
		bool Search(LPCWSTR dir,Tree<String>* tl,bool asyn = 0);
	};
	//FilsSystem Copy ,Delete, Move 函数可能的返回值。
#define FS_OK						0	//操作成功
#define FS_FALSE					1	//BOOL函数 返回 FALSE。
#define FS_SAME_NAME				2	//目的文件和源文件同名
#define FS_OPEN_SOURCE_FAILED		3	//打开源文件失败
#define FS_OPEN_DEST_FAILED			4	//打开目的文件失败
#define FS_FILE_COPYED				5	//文件已经在之前拷贝完成。
#define FS_COPY_PAUSE				6	//拷贝操作中断
#define FS_CANNOT_ACCESS			7	//文件属性无法设置。
#define FS_DEL_FAILED				8	//删除文件操作失败
#define FS_MOVE_FAILED				9	//文件移动失败
#define FS_MOVE_COPY_FAILED			10	//移动操作时文件拷贝失败
#define FS_MAX						10
	//文件系统提供文件的基本操作。
	class CONCISE_API FileSystem : public _class{
	protected:
		String _name;
	public:
		void* UserData;
		FileSystem():UserData(0){}
		//用一个文件名初始化实例。
		FileSystem(LPCWSTR name);
		//用一个文件名初始化实例，如果文件不存在返回 0.
		bool Initialize(LPCWSTR name);
		//返回文件的全称。
		LPCWSTR GetName(){return _name;}
		//是否为空。
		bool IsNull(){return _name.IsNull();}
		//是否不为空。
		bool NotNull(){return _name.NotNull();}
		//复制文件。check = 1: 检测目标文件的大小和写入时间，如何和源文件相同则不执行真正的数据拷贝，而且函数返回操作成功标志1.
		//如果设置check = 1：且检测目标文件不存在或者大小或写入时间和源文件不同，则拷贝数据，且在完成之后，会设置文件的最后写入时间和源文件相同。
		//Copy操作可以续传数据，正常取消操作后，下次执行同一文件时，数据可以续传。但是非正常终止的操作是不可以续传的。
		HRESULT Copy(LPCWSTR newName,bool check = 1);
		//删除文件。
		HRESULT Delete();
		//移动文件。同分区的移动不必拷贝数据，不同分区则是结合了 Copy 和 Delete 操作。
		HRESULT Move(LPCWSTR newName);
		//文件拷贝数据时的进度回调函数。参数分别为，已经拷贝的数据，数据总长，是否停止拷贝，如果返回true，
		//拷贝数据的操作将停止，但是之后的调用可以续传，如过源数据没有改变的话。每拷贝1Mb的数据，这个函数会呼叫一次。
		Function<void,uint64,uint64,bool&,void*> OnProgress;
	};
	//获取错误代码描述
	LPCWSTR CONCISE_API FSGetErrorMsg(uint fs_error);
	//文件夹操作使用的结构
	typedef struct DirData{
		bool Stop;			//是否停止整个操作。
		bool Skip;		//是否取消当前文件的操作。
		uchar Error;			//操作返回的代码，FS_OK（0）表示成功
		uchar PreOrAfter;	//对当前文件的那一次调用，0——操作前的调用，此时可以设置取消操作，1——操作后的调用，此时成功标志有效。
		LPCWSTR SourceDir;	//当前操作的源文件（或文件夹）的父目录。
		LPCWSTR DestDir;	//当前操作的目的文件（或文件夹）的父目录。
		WIN32_FIND_DATA* FindData;	//文件信息结构，包含文件的信息。
	}DirData;
	class CONCISE_API  DSParam : public _class{
	public:
		void* UserData;
		String Source,Destinate;
		//返回文件拷贝进度回调函数。
		Function<void,uint64,uint64,bool&,void*> OnFileProgress;
		//文件操作回调函数，每操作一个文件（或文件夹）这个函数返回2次，第一次是操作前，
		//可以设置取消操作当前文件，第二次返回是操作之后，Success 成员指示当前文件的操作是否成功。
		Function<void,DirData*,void*> OnProgress;
		//所有操作已经完成，LPCWSTR 指示源目录,LPCWSTR 指示目的目录，在 Delete 操作中为 0 。
		Function<void,LPCWSTR,LPCWSTR,void*> OnComplete;
		DSParam():UserData(0){}
	};
	//文件夹操作
	class CONCISE_API DirectorySystem : public _class{
	protected:
		static void __stdcall _Copy(ListData* ld);
		static void __stdcall _Move(ListData* ld);
		static void __stdcall _Delete(ListData* ld);
	public:
		//复制文件，参数为源目录和目的目录，不能是文件。
		static bool Copy(DSParam* dsp);
		//删除文件夹。
		static bool Delete(DSParam* dsp);
		//移动文件夹。
		static bool Move(DSParam* dsp);
	};
	//class FileWatch
	class CONCISE_API FileWatch : public _class
	{
		IntegerList<HANDLE> _hdir;
		StringMemDiction	_folder;
		VirtualAddress _buffer;
		HANDLE _CompPortHandle;
		DWORD _Type;	//监视类型，
		bool _Sub;		//是否监视子文件夹，
		Thread	_thread;
		void _watch(); 
	public:
	//FileSystem使用的结构。
	enum WatchType{
		FileName = FILE_NOTIFY_CHANGE_FILE_NAME,
		DirName = FILE_NOTIFY_CHANGE_DIR_NAME,
		Atrributes = FILE_NOTIFY_CHANGE_ATTRIBUTES,
		Size = FILE_NOTIFY_CHANGE_SIZE,
		LastWrite = FILE_NOTIFY_CHANGE_LAST_WRITE,
		LastAccess = FILE_NOTIFY_CHANGE_LAST_ACCESS,
		Creation = FILE_NOTIFY_CHANGE_CREATION,
		Security = FILE_NOTIFY_CHANGE_SECURITY
	};
		FileWatch();
		~FileWatch();
		//设置监视类型.
		void SetWatchType(DWORD type){_Type = type;}
		//设置是否监视子文件夹.
		void SetWatchSub(bool sub){_Sub = sub;};
		//启动监视，必须先停止，才能再次启动，否则返回fasle。
		bool Start();
		//停止监视.
		bool Stop();
		//添加要监视的文件夹，如果文件夹在监视列表已经存在，返回 false 。这个函数不检测文件夹是否有效。如果监视已经启动,添加的文件夹在下次启动监视是起作用.
		bool AddFolder(LPCWSTR folder);
		//移除监视的文件夹，下次启动生效。
		bool DelFolder(LPCWSTR folder);

		Function<void,DWORD,LPCWSTR> OnChange;//接收文件系统消息的回调函数.
	};
	//class File File类集成基本的文件读写功能.
	class CONCISE_API File : public HandleType<HANDLE>
	{
	public:
		//构造
		File();
		//用一个文件名来初始化这个文件.
		File(LPCWSTR file);
		//析构
		~File();
		//判断是否是一个无效的文件句柄.
		bool IsNull(){return _Handle==INVALID_HANDLE_VALUE;}
		//实例是否包含一个有效的文件句柄.
		bool NotNull(){return _Handle!=INVALID_HANDLE_VALUE;}
		//绑定一个已知的文件句柄.
		bool Attach(HANDLE file);
		//解除一个已经绑定的文件句柄.
		HANDLE Detach();
		//生成或打开一个文件.
		bool Create(LPCWSTR name,DWORD create = OPEN_ALWAYS,DWORD share = FILE_SHARE_READ|FILE_SHARE_WRITE,DWORD access = FILE_GENERIC_READ|FILE_GENERIC_WRITE,DWORD attribute = FILE_ATTRIBUTE_NORMAL);
		//关闭文件.
		bool Close();
		//获取文件的大小.
		unsigned __int64 GetLength();
		//设置文件的长度(不能大于4G).
		bool SetLength(uint64 length);
		//设置当前指针位置为文件结尾.
		bool EndOfFile(){return SetEndOfFile(_Handle)!=0;}
		//把数据写入文件,返回实际写入的长度.
		int Write(const void* handle,int length);
		//从文件读取相应长度(length)的数据,返回实际读取的长度.
		int Read(void* handle,int length);
		//把数据写入文件,返回实际写入的长度.
		template<typename T> int Write(Memory<T>* buf){
			return Write(buf->Handle(),buf->Length()*sizeof(T))/sizeof(T);
		}
		//从文件读取相应长度(length)的数据,返回实际读取的长度.
		template<typename T> int Read(Memory<T>* buf){
			uint flen = GetFileSize(_Handle,0);
			uint pos = (uint)GetPointer();
			if(pos>=flen) return 0;
			uint buflen = flen - pos;
			uint count = buflen/sizeof(T);
			if(buf->Length()<count)
				if(!buf->SetLength(count)) return 0;
			return Read(buf->Handle(),buflen)/sizeof(T);
		}
		//设置文件的指针位置.
		bool SetPointer(uint64 offset,DWORD seek = FILE_BEGIN);
		//返回文件的指针位置.-1表示失败,但有时这个值也是一个有效值.
		uint64 GetPointer();
		//把缓存中的数据写入磁盘.
		bool Flush();
	};
	//class FileStream
	class CONCISE_API FileStream : public File
	{
	public:
		bool Attach(HANDLE hFile);
		
		//从指定位置读取以 code 编码保存的文本(至结尾)，buffer无须初始化，code = 0 表示数据使用 Unicode 格式保存，不进行任何编码转换。-1 表示从当前位置开始读取，返回读取的字符数。
		int ReadString(String& buffer,int position = -1,DWORD code = -1);
		//把文本写入文件，position 是写入的位置，code 是存储使用的编码，为 0  时表示直接使用 Unicode 编码保存（不进行编码转换），affix 是要插入的分隔符。
		int WriteString(LPCWSTR buffer,int position = -1,LPCWSTR affix = 0,DWORD code = CP_THREAD_ACP);
		//从指定位置读取 Unicode 格式保存的文本(至结尾)，buffer无须初始化，code 是要读取后要转换的编码,code = 0 表示数据不进行任何编码转换。-1 表示从当前位置开始读取，返回读取的字节数。
		int ReadMbsFromWs(Memory<char>& buffer,int position = -1,DWORD code = CP_THREAD_ACP);
		//把文本转换为 Unicode 数据并写入文件，position 是写入的位置，code 是 buffer 使用的编码，为 0 时表示把数据直接写入文件（不进行编码转换），affix 是要插入的分隔符。
		int WriteMbsToWs(LPCSTR buffer,int position = -1,LPCSTR affix = 0,DWORD code = 0);
		//code = 0,则文本使用字串的编码写入文件,code != 0 则文本以Unicode编码写入文件,code是buffer字串的编码.
		//以上函数返回值为读取或写入的字节数.
		int ReadInt(int position = -1,int defValue = 0);
		bool WriteInt(int value,int position = -1);

		char ReadChar(int position = -1,char defValue = 0);
		bool WriteChar(char value,int position = -1);
	};
	/*文件映射涉及到3个文件操作函数，CreateFile打开文件，CreateFileMap生成映像，MapViewOfFile映射到物理内存。对于大文件，MapViewOfFile
	经常会出现因为内存不足导致的失败。关于打开方式，文件打开一般涉及 GENERIC_READ（读文件）, GENERIC_WRITE（写文件）, 和 GENERIC_EXECUTE
	（执行文件）三种风格。注意，文件以只写方式打开时，是不能进行读取的。
	CreateFileMapping函数打开方式：PAGE_READONLY：只读方式；PAGE_READWRITE：读写方式；PAGE_WRITECOPY：写操作不会写入文件，但是映像是可读写的，只需要 GENERIC_READ 权限；
	PAGE_EXECUTE_READ：可读取和执行，文件必须用有 GENERIC_READ 和 GENERIC_EXECUTE 权限；PAGE_EXECUTE_READWRITE：可以执行和读写，文件打开时
	必须有 GENERIC_READ, GENERIC_WRITE, 和 GENERIC_EXECUTE权限。后两个方式只在 XP SP2 和 Server 2003 SP1 之后的操作系统中可用。
	MapViewOfFile函数负责把映像文件映射到地址空间，FILE_MAP_READ：读取权限；FILE_MAP_WRITE：写入权限；FILE_MAP_ALL_ACCESS：读写权限，实际上
	就是 FILE_MAP_READ | FILE_MAP_WRITE，不知道为什么不叫 FILE_MAP_READWRITE；FILE_MAP_EXECUTE：执行权限；FILE_MAP_COPY：对应PAGE_WRITECOPY
	的情形。*/
	//class FileMap
//FILE_MAP_READ		//内存是只读的
//FILE_MAP_WRITE	//内存是可读写的
//FILE_MAP_EXECUTE	//可内存是可执行的，它自动具备可读属性，但是可以和MapWrite组合使用，具备读写属性。
//FILE_MAP_COPY		//内存是可读写的，但是写入数据不刷新到文件，实际上文件和映像是以只读方式打开的。
	class CONCISE_API FileMap : public HandleType<HANDLE>{
	protected:
	public:
		FileMap();
		~FileMap();
		bool Create(HANDLE file,DWORD flag = FILE_MAP_WRITE,uint64 size = 0);
		void Close();
	};
	class CONCISE_API MapView : public HandleType<void*>{
	protected:
		uint _length;
	public:
		MapView();
		~MapView();
		void Close();
		//偏移量offset 必须是64K（64×1024 = 0x10000）的倍数，否则函数会失败。实际上length也是以这个数字为单位的，虽然指定的长度很少，写过界也没有问题。
		bool Create(HANDLE map,uint64 offset,uint length,DWORD access = FILE_MAP_WRITE|FILE_MAP_READ);
		bool Flush(uint start = 0,uint length = -1);
		inline uint Length(){return _length;}
	};
}