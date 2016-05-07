#include "../stdafx.h"
#include "../../jubase.h"

namespace ju{
	inline bool isSpaceOrDot(wchar_t ch){
		return (ch==' ')||(ch=='.');
	}
	inline bool isSlashOrBackOrNull(wchar_t ch){
		return (ch=='\\')||(ch=='/')||(ch==0);
	}
	inline bool isSlashOrBack(wchar_t ch){
		return (ch=='\\')||(ch=='/');
	}
	//检测是否是一个合法的文件名，末尾不能是空格或者dot。
	inline bool isValidFileTitle(String& title){
		if((title.Length()==0)||(title==L".")||(title==L".."))
			return 1;
		return !isSpaceOrDot(title[title.Length()-1]);
	}
	//检测文件目录是否为合法字符，Windows在处理文件名的时候，有些函数会把末尾的“.”和
	//“空格”移除，而有些函数不会。
	bool checkFolderPath(LPCWSTR path)
	{
		int len = WcsLength(path);
		String title;
		int start = 0;
		for(int i=0;i<=len;i++)
		{
			if(isSlashOrBackOrNull(path[i]))
			{
				title = L"";
				title.CopyFrom(path+start,i-start,0);
				start = i + 1;
				if(!isValidFileTitle(title)) return 0;
			}
		}
		return 1;
	}
	bool CreateDirectory(LPCWSTR path)
	{
		if(!checkFolderPath(path))
		{
			SetLastError(123);
#ifdef _DEBUG
			::OutputDebugString(L"in windows envirement path first chractor or last charactor can't be a space or dot\r\n");
#endif
			return 0;
		}
		return 0!=::CreateDirectory(path,0);
	}
	DWORD GetFileAttributes(LPCWSTR path)
	{
		if(!checkFolderPath(path))
			return -1;
		return ::GetFileAttributes(path);
	}
	//文件操作函数。
	bool CreateFolder(LPCWSTR path,bool file){
		String str = path;
		//如果不是生成文件的父目录，保持末尾有一个反斜杠。
		if(!file){
			FPSetTailSlash(str);
		}
		for(uint i=0;i<str.Length();i++){
			//是否反斜杠、斜杠
			if(!isSlashOrBack(str[i])) continue;
			str[i] = 0;	
			//Windows一般不允许文件（夹）名的末尾是一个空格或者点，但是某些函数又允许，为了取得一致，消除这种情况
			if(!isValidFileTitle(str)){
				SetLastError(123);//设置LastError为文件名不合法。
				return 0;
			}
			int type = GetFileType(str);
			if(type==0){
				if(!::CreateDirectory(str.Handle(),0)) return 0;
			}else if(type==1){
				return 0;
			}
			str[i] = '\\';
		}
		return 1;
	}
	int GetFileType(LPCWSTR path){
		DWORD attr = GetFileAttributes(path);
		if(attr==INVALID_FILE_ATTRIBUTES) return 0;
		if(attr&FILE_ATTRIBUTE_DIRECTORY) return 2;
		else return 1;
	}
	bool RemoveFile(LPCWSTR path){
		DWORD attr = GetFileAttributes(path);
		if(attr==INVALID_FILE_ATTRIBUTES) return 0;
		if(attr&FILE_ATTRIBUTE_READONLY)
		{
			attr -= FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes(path,attr);
		}
		if(attr&FILE_ATTRIBUTE_DIRECTORY)
			return ::RemoveDirectory(path)!=0;
		else
			return ::DeleteFile(path)!=0;
	}
	bool FPToFullPath(String& path,LPCWSTR* name){
		DWORD bufLen = MAX_PATH;
		Memory<wchar_t> buf;
		buf.SetLength(bufLen);
		bufLen = ::GetFullPathName(path,bufLen,buf.Handle(),(LPWSTR*)name);
		if(bufLen>MAX_PATH){
			buf.SetLength(bufLen);
			bufLen = ::GetFullPathName(path,bufLen,buf.Handle(),(LPWSTR*)name);
		}
		if(bufLen==0){
			path = L"";
			return false;
		}
		path = buf.Handle();
		return true;
	}
	bool FPIsSame(LPCWSTR path1,LPCWSTR path2){
		String p1(path1),p2(path2);
		p1.Lower();
		p2.Lower();
		int len1 = p1.Length();
		if(len1<2) return 0;
		int len2 = p2.Length();
		if(len2<2) return 0;
		if((p1[len1-1]=='\\')||(p1[len1-1]=='/')){
			p1.Head(-1);
		}
		if((p2[len2-1]=='\\')||(p2[len2-1]=='/')){
			p2.Head(-1);
		}
		if(p1.Length()!=p2.Length()) return 0;
		for(uint i=0;i<p1.Length();i++){
			if(p2[i]==p1[i]) continue;
			if((p1[i]=='/')&&(p2[i]=='\\')) continue;
			if((p1[i]=='\\')&&(p2[i]=='/')) continue;
			return 0;
		}
		return 1;
	}
	void FPToBackSlash(String& Handle){
		for(uint i=0;i<Handle.Length();i++){
			if(Handle[i]=='/') Handle[i] = '\\';
		}
	}
	void FPToSlash(String& Handle){
		for(uint i=0;i<Handle.Length();i++){
			if(Handle[i]=='\\') Handle[i] = '/';
		}
	}
	void FPDelTailSlash(String& Handle){
		while(1){
			int tail = Handle.Length()-1;
			if(tail>=0){
				if((Handle[tail]=='/')||(Handle[tail]=='\\')){
					Handle.Head(-1);
				}
				else break;
			}
			else break;
		}
	}
	void FPSetTailSlash(String& Handle){
		FPDelTailSlash(Handle);
		if(Handle.Length()) Handle += L"\\";
	}
	void FPLinkExt(String& Handle,LPCWSTR str){
		if(str==0) return;
		int tail = Handle.Length() - 1;
		int i;
		for(i=tail;i>=0;i--)
		{
			if(Handle[i]=='.') continue;
			break;
		}
		Handle.Head(i+1);
		i = 0;
		Handle += L".";
		while(str[i]=='.') i++;
		Handle += str + i; 
	}
	void FPLinkPath(String& Handle,LPCWSTR str){
		if(str==0) return;
		if(str[0]==0) return;
		FPSetTailSlash(Handle);
		int i = 0;
		while((str[i]=='\\')||(str[i]=='/')){
			i++;
		}
		Handle += str + i; 
	}
	void FPToParent(String& Handle){
		for(int i=Handle.Length()-2;i>=0;i--){
			if((Handle[i]=='\\')||(Handle[i]=='/')){
				Handle.Head(i);
				return;
			}
		}
		Handle = L"";
	}
	void FPToExt(String& Handle){
		for(int i=Handle.Length()-1;i>=0;i--){
			if((Handle[i]=='\\')||(Handle[i]=='/')){
				break;
			}
			if(Handle[i]=='.'){
				Handle = Handle + i + 1;
				return;
			}
		}
		Handle = L"";
	}
	void FPToTitle(String& Handle){
		int tail = Handle.Length()-1;
		int i = tail;
		for(;i>=0;i--){
			if(Handle[i]=='.'){
				tail = i;
				break;
			}
		}
		for(;i>=0;i--){
			if((Handle[i]=='\\')||(Handle[i]=='/')){
				break;
			}
		}
		Handle.Head(tail);
		Handle.Tail(i+1);
	}
	void FPToFileName(String& Handle){
		for(int i=Handle.Length()-1;i>=0;i--){
			if((Handle[i]=='\\')||(Handle[i]=='/')){
				Handle.Tail(i+1);
				return;
			}
		}
	}

	Drives::Drives()
	{
		DWORD drives = ::GetLogicalDrives(),d = 1;
		wchar_t dr[3];
		dr[2] = 0;
		for(char ch='A';ch<='Z';ch++)
		{
			if(d&drives)
			{
				dr[0] = ch;
				dr[1] = ':';
				Paths.Add(dr);
			}
			d<<=1;
		}
		_Count = Paths.Count();
	}
	Drives::~Drives()
	{
	}
	bool Drives::GetInfo(int index,LPDRIVEINFO lpDriveInfo)
	{
		return GetInfo(Paths[index],lpDriveInfo);
	}
	bool Drives::GetInfo(LPCWSTR path,LPDRIVEINFO lpDriveInfo)
	{
		if(!path) return 0;
		wchar_t dn[4];
		::ZeroMemory(lpDriveInfo,sizeof(DRIVEINFO));
		WcsCopy(lpDriveInfo->Path,path);
		WcsCopy(dn,path);
		dn[2] = '\\';
		dn[3] = 0;
		return 0!=::GetVolumeInformationW(dn,lpDriveInfo->Name,65,&lpDriveInfo->Type,&lpDriveInfo->MaxPath,&lpDriveInfo->Flag,lpDriveInfo->FileSystemType,6);
	}
	inline bool IsDir(DWORD atr){return (atr&FILE_ATTRIBUTE_DIRECTORY)!=0;}
	bool ValidFile(LPCWSTR file)
	{
		if(!file[0]) return 0;
		if(file[0]=='.')
		{
			if(file[1]==0) return 0;
			if(file[1]=='.')
			{
				if(file[2]==0) return 0;
			}
		}
		return 1;
	}
	//文件类型通过过滤，notin指示是过滤去掉还是过滤通过，无后缀总是通过。
	bool IsFilter(StringMemList* filter,LPCWSTR file,int filterType){
		if(filterType==0) return true;
		bool notin = filterType==2;
		if(!filter->Count()) return notin;
		String ext = file;
		FPToExt(ext);
		if(!ext.Length()) return notin;
		for(uint i=0;i<filter->Count();i++){
			if(WcsEqualNoCase(filter->Element(i),ext)) return !notin;
		}
		return notin;
	}
	//FileSearch使用的结构。
#define LISTDATA_FLAG_ISFILE	0
#define LISTDATA_FLAG_PRECALL	1
#define LISTDATA_FLAG_AFTERCALL	2
	//class FileSearch
	FileSearch::FileSearch():_Sub(0),_Pre(1),_After(0),_filterType(FILTER_TYPE_DISABLE){
	}
	bool FileSearch::Search(LPCWSTR dir,void* extra,Json* json,bool asyn){
		int len = WcsLength(dir)*2+2;
		wchar_t* ps = (wchar_t*)MemoryAlloc(len);
		memcpy(ps,dir,len);
		Json* files = 0;
		if(json){
			json->SetPropertyStr(L"name",dir);
			files = json->SetPropertyArray(L"files");
		}
		if(asyn){
			Thread th;
			return th.Call<bool>(this,&FileSearch::_search,(LPCWSTR)ps,extra,files);
		}else{
			bool rst = _Search(dir,L"",extra,files);
			if(!rst) dir = 0;
			if(!OnComplete.IsNull()) OnComplete(extra,dir);
			return rst;
		}
	}
	bool FileSearch::_search(LPCWSTR dir,void* extra,Json* json){
		bool b = _Search(dir,L"",extra);
		if(!OnComplete.IsNull()) OnComplete(extra,dir);
		MemoryFree((void*)dir);
		return b;
	}
	bool FileSearch::_Search(LPCWSTR original,LPCWSTR relative,void* extra,Json* files){
		String sp;
		String path = original;
		DWORD atr = GetFileAttributes(original);
		if(((atr==INVALID_FILE_ATTRIBUTES)||((atr&FILE_ATTRIBUTE_DIRECTORY)==0))&&(relative[0]==0)){
			sp = original;
			FPToParent(path);
			original = path;
		}else{
			FPLinkPath(path,relative);
			FPSetTailSlash(path);
			sp = path;
			sp += L"*";
		}
		WIN32_FIND_DATA wfd;
		HANDLE hFind = FindFirstFile(sp,&wfd);
		if(hFind==INVALID_HANDLE_VALUE){
			//if(!OnComplete.IsNull()) OnComplete(extra,0);
			return 0;
		}
		int find = 1;
		while(find){
			if(!ValidFile(wfd.cFileName)){
				find = FindNextFile(hFind,&wfd);
				continue;
			}
			ListData sd;
			sd.data = &wfd;
			sd.original = original;
			sd.relative = relative;
			sd.extra = extra;
			sd.json = 0;
			sd.stop = 0;
			sd.skip = 0;
			if(!IsDir(wfd.dwFileAttributes)){
				if(IsFilter(&_Filter,wfd.cFileName,_filterType)){
					sd.flag = 0;
					if(!OnList.IsNull()) OnList(&sd);
					else if(files){
						sd.json = files->AddArrayElm();
						sd.json->SetPropertyStr(L"name",wfd.cFileName);
					}
				}
			}else{
				Json* sfiles = 0;
				if(files){
					sd.json = files->AddArrayElm();
					sd.json->SetPropertyStr(L"name",wfd.cFileName);
					sfiles = sd.json->SetPropertyArray(L"files");
				}
				if(_Pre){
					sd.flag = 1;
					if(!OnList.IsNull()) OnList(&sd);
				}
				if(sd.stop) break;
				if(!sd.skip&&_Sub){
					String nr = relative;
					FPLinkPath(nr,wfd.cFileName);
					_Search(original,nr,extra,sfiles);
				}
				if(_After){
					sd.flag = 2;
					if(!OnList.IsNull()) OnList(&sd);
				}
			}
			if(sd.stop) break;
			find = FindNextFile(hFind,&wfd);
		}
		FindClose(hFind);
		return true;
	}
	LPCWSTR _fileSystemErrorTable[] = {
		L"操作成功",
		L"BOOL函数 返回 FALSE。",
		L"目的文件和源文件同名",
		L"打开源文件失败",
		L"打开目的文件失败",
		L"文件已经在之前拷贝完成。",
		L"拷贝操作中断",
		L"文件属性无法设置。",
		L"删除文件操作失败",
		L"文件移动失败",
		L"移动操作时文件拷贝失败"
	};
	LPCWSTR FSGetErrorMsg(uint fs_error){
		if(fs_error>FS_MAX) return 0;
		return _fileSystemErrorTable[fs_error];
	}
	//class FileSystem
	FileSystem::FileSystem(LPCWSTR name):UserData(0){
		_name = name;
		if(!FPToFullPath(_name)) return;
	}
	bool FileSystem::Initialize(LPCWSTR name){
		if(GetFileType(name)!=1) return 0;
		_name = name;
		return FPToFullPath(_name);
	}
	inline bool ftneq(FILETIME& ft1, FILETIME& ft2) {
		return *(uint64*)&ft1 != *(uint64*)&ft2;
	}
	inline bool fteq(FILETIME& ft1, FILETIME& ft2) {
		return *(uint64*)&ft1 == *(uint64*)&ft2;
	}
	typedef struct MFT {
		FILETIME ct,wt;
		inline bool operator == (MFT& mft) {
			return fteq(ct, mft.ct) && fteq(wt,mft.wt);
		}
		inline bool operator != (MFT& mft) {
			return ftneq(ct, mft.ct) || ftneq(wt, mft.wt);
		}
		inline bool GetTime(HANDLE file) {
			return 0!=::GetFileTime(file,&ct,0,&wt);
		}
		inline bool SetTime(HANDLE file) {
			return 0 != ::SetFileTime(file, &ct, 0, &wt);
		}
	}MFT, *LPMFT;
	HRESULT FileSystem::Copy(LPCWSTR newName,bool check){
		String fp = newName;
		if(!FPToFullPath(fp)) return FS_OPEN_DEST_FAILED;
		if(FPIsSame(_name,newName)) return FS_SAME_NAME;
		FPToParent(fp);
		if(!CreateFolder(fp)) return FS_OPEN_DEST_FAILED;
		FileStream sfm,dfm;
		if(!sfm.Create(_name,OPEN_EXISTING,FILE_SHARE_READ|FILE_SHARE_WRITE,GENERIC_READ)) return FS_OPEN_SOURCE_FAILED;
		if(!dfm.Create(newName,OPEN_ALWAYS,FILE_SHARE_READ|FILE_SHARE_WRITE,GENERIC_READ|GENERIC_WRITE)) return FS_OPEN_DEST_FAILED;
		uint64 u = sfm.GetLength()/10;
		uint unit = (uint)(u<0x1000000 ? u : 0x1000000);
		if(unit<0x100000) unit = 0x100000;
		MFT st,dt;
		if(!dt.GetTime(dfm.Handle())) return FS_OPEN_DEST_FAILED;
		if(!st.GetTime(sfm.Handle())) return FS_OPEN_SOURCE_FAILED;
		uint64 sLength = sfm.GetLength();
		uint64 dLength = dfm.GetLength();
		while(check) {
			if(sLength!=dLength) break;
			if(st!=dt) break;
			return FS_FILE_COPYED;
		}
		uint64 offset = 0;
		Memory<char> buf;
		buf.SetLength(unit);
		uchar inf[24];
		while(dfm.GetLength()==(sfm.GetLength()+24)){
			dfm.SetPointer(sLength);
			dfm.Read(inf,24);
			if(memcmp(&st, inf, sizeof(st)) != 0) break;
			memcpy(&offset,inf+sizeof(st),8);
			if(offset>=sLength){
				offset = 0;
				break;
			}
			if(offset%unit){
				offset = 0;
				break;
			}
			break;
		}
		uint64 left = sLength - offset;
		while(1){
			uint len;
			if(left>unit)
				len = unit;
			else
				len = (uint)left;
			sfm.SetPointer(offset);
			sfm.Read(buf.Handle(),unit);
			dfm.SetPointer(offset);
			dfm.Write(buf.Handle(),unit);
			offset += len;
			left -= len;
			bool stop = 0;
			if(!OnProgress.IsNull()) OnProgress(offset,sLength,stop,UserData);
			if(left==0){
				dfm.SetLength(sLength);
				if(check){
					dfm.Close();
					dfm.Create(newName, OPEN_ALWAYS, FILE_SHARE_READ | FILE_SHARE_WRITE, GENERIC_READ | GENERIC_WRITE);
					//源文件时间设置给目的文件。
					st.SetTime(dfm);
				}
				return FS_OK;
			}
			if(stop){
				dfm.SetPointer(sLength);
				//源文件时间和当前位置写入后24字节
				memcpy(inf,&st,sizeof(st));
				memcpy(inf + sizeof(st), &offset, 8);
				dfm.Write(inf,24);
				dfm.SetLength(sLength+24);
				return FS_COPY_PAUSE;
			}
		}
		return FS_CANNOT_ACCESS;
	}
	HRESULT FileSystem::Delete(){
		DWORD attr = GetFileAttributes(_name);
		if((attr!=INVALID_FILE_ATTRIBUTES)&&(attr&FILE_ATTRIBUTE_READONLY)){
			if(!SetFileAttributes(_name,(attr|FILE_ATTRIBUTE_READONLY)-FILE_ATTRIBUTE_READONLY)) return FS_CANNOT_ACCESS;
		}
		if(!DeleteFile(_name)) return FS_DEL_FAILED;
		return FS_OK;
	}
	HRESULT FileSystem::Move(LPCWSTR newName){
		String fp = newName;
		if(!FPToFullPath(fp)) return FS_OPEN_DEST_FAILED;
		if(FPIsSame(_name,newName)) return FS_SAME_NAME;
		FPToParent(fp);
		if(!CreateFolder(fp)) return FS_OPEN_DEST_FAILED;
		if(_name[0]==newName[0]){
			DWORD attr = GetFileAttributes(newName);
			if((attr!=INVALID_FILE_ATTRIBUTES)&&(attr&FILE_ATTRIBUTE_READONLY)){
				if(!SetFileAttributes(_name,(attr|FILE_ATTRIBUTE_READONLY)-FILE_ATTRIBUTE_READONLY)) return FS_CANNOT_ACCESS;
			}
			if(!MoveFileEx(_name,newName,MOVEFILE_REPLACE_EXISTING)) return FS_MOVE_FAILED;
		}else{
			if(FS_OK!=Copy(newName)) return FS_MOVE_COPY_FAILED;
			if(FS_OK!=Delete()) return FS_DEL_FAILED;
		}
		_name = newName;
		return FS_OK;
	}
	//class DirectorySystem
	bool DirectorySystem::Move(DSParam* dsp){
		if(dsp==0) return 0;
		String src = dsp->Source,dst = dsp->Destinate;
		if(!FPToFullPath(src)) return 0;
		if(!FPToFullPath(dst)) return 0;
		if(FPIsSame(src,dst)) return 0;
		int type = GetFileType(src);
		if(type==0) return 0;
		bool isdir = type==2;
		bool r = true;
		if(isdir){
			if(!CreateFolder(dst)) return 0;
			FileSearch ft;
			ft.SetAfterCall(1);
			ft.SetPreCall(1);
			ft.SetSearchSub(1);
			ft.OnList.BindStd(&DirectorySystem::_Move);
			r = ft.Search(src,dsp);
			r &= RemoveFile(src);
		}else{
			FileSystem fs(src);
			fs.OnProgress = dsp->OnFileProgress;
			r = FS_OK==fs.Move(dst);
		}
		if(!dsp->OnComplete.IsNull())
			dsp->OnComplete(src,dst,dsp->UserData);
		return r;
	}
	void DirectorySystem_onList_init(ListData* ld,DSParam* dsp,DirData& dd,String& spath,String& dpath,String& src,String& dst){
		spath = ld->original;
		FPLinkPath(spath,ld->relative);
		dd.SourceDir = spath;
		dd.Skip = 0;
		dd.PreOrAfter = 0;
		dd.Stop = 0;
		dd.Error = 0;
		dpath = dsp->Destinate;
		FPLinkPath(dpath,ld->relative);
		dd.DestDir = dpath;
		dd.FindData = ld->data;
		dst = dd.DestDir;
		FPLinkPath(dst,ld->data->cFileName);
		src = dd.SourceDir;
		FPLinkPath(src,ld->data->cFileName);
	}
	bool DirectorySystem_onList_pre(ListData* ld,DSParam* dsp,DirData& dd){
		if(!dsp->OnProgress.IsNull())
			dsp->OnProgress(&dd,dsp->UserData);
		if(dd.Stop){
			ld->stop = 1;
			return 1;
		}
		if(dd.Skip) return 1;
		return 0;
	}
	void DirectorySystem_onList_after(ListData* ld,DSParam* dsp,DirData& dd){
		dd.PreOrAfter = 1;
		if(!dsp->OnProgress.IsNull())
			dsp->OnProgress(&dd,dsp->UserData);
		ld->stop = dd.Stop;
	}
	void DirectorySystem::_Move(ListData* ld){
		String src,dst,spath,dpath;
		DSParam* dsp = (DSParam*)ld->extra;
		DirData dd;
		DirectorySystem_onList_init(ld,dsp,dd,spath,dpath,src,dst);

		if(ld->flag==LISTDATA_FLAG_ISFILE){
			bool rst = DirectorySystem_onList_pre(ld,dsp,dd);
			if(rst) return;

			FileSystem fs;
			fs.Initialize(src);
			fs.OnProgress = dsp->OnFileProgress;
			fs.UserData = dsp->UserData;
			dd.Error = (uchar)fs.Move(dst);

			DirectorySystem_onList_after(ld,dsp,dd);
		}else if(ld->flag==LISTDATA_FLAG_PRECALL){
			bool rst = DirectorySystem_onList_pre(ld,dsp,dd);
			if(rst) return;
			dd.Error = !CreateFolder(dst);
		}else if(ld->flag==LISTDATA_FLAG_AFTERCALL){
			dd.Error = (uchar)!RemoveFile(src);
			DirectorySystem_onList_after(ld,dsp,dd);
		}
	}
	void DirectorySystem::_Delete(ListData* ld){
		String src,dst,spath,dpath;
		DSParam* dsp = (DSParam*)ld->extra;
		DirData dd;
		DirectorySystem_onList_init(ld,dsp,dd,spath,dpath,src,dst);
		dd.DestDir = 0;

		if(ld->flag==LISTDATA_FLAG_ISFILE){
			bool rst = DirectorySystem_onList_pre(ld,dsp,dd);
			if(rst) return;

			dd.Error = (uchar)!RemoveFile(src);

			DirectorySystem_onList_after(ld,dsp,dd);
		}else if(ld->flag==LISTDATA_FLAG_PRECALL){
			bool rst = DirectorySystem_onList_pre(ld,dsp,dd);
			if(rst) return;
		}else if(ld->flag==LISTDATA_FLAG_AFTERCALL){
			dd.Error = (uchar)!RemoveFile(src);
			DirectorySystem_onList_after(ld,dsp,dd);
		}
	}
	bool DirectorySystem::Delete(DSParam* dsp){
		if(dsp==0) return 0;
		dsp->Destinate = L"";
		String src = dsp->Source;
		if(!FPToFullPath(src)) return 0;
		int type = GetFileType(src);
		if(type==0) return 0;
		bool isdir = type==2;
		bool r = true;
		if(isdir){
			FileSearch ft;
			ft.SetAfterCall(1);
			ft.SetPreCall(1);
			ft.SetSearchSub(1);
			ft.OnList.BindStd(&DirectorySystem::_Delete);
			r = ft.Search(src,dsp);
			r &= RemoveFile(src);
		}else{
			r = FS_OK==RemoveFile(src);
		}
		if(!dsp->OnComplete.IsNull())
			dsp->OnComplete(src,0,dsp->UserData);
		return r;
	}
	void DirectorySystem::_Copy(ListData* ld){
		String src,dst,spath,dpath;
		DSParam* dsp = (DSParam*)ld->extra;
		DirData dd;
		DirectorySystem_onList_init(ld,dsp,dd,spath,dpath,src,dst);

		if(ld->flag==LISTDATA_FLAG_ISFILE){
			bool rst = DirectorySystem_onList_pre(ld,dsp,dd);
			if(rst) return;

			FileSystem fs;
			fs.OnProgress = dsp->OnFileProgress;
			fs.Initialize(src);
			fs.UserData = dsp->UserData;
			dd.Error = (uchar)fs.Copy(dst);

			DirectorySystem_onList_after(ld,dsp,dd);
		}else if(ld->flag==LISTDATA_FLAG_PRECALL){
			bool rst = DirectorySystem_onList_pre(ld,dsp,dd);
			if(rst) return;
			dd.Error = !CreateFolder(dst);
		}else if(ld->flag==LISTDATA_FLAG_AFTERCALL){
			DirectorySystem_onList_after(ld,dsp,dd);
		}
	}
	bool DirectorySystem::Copy(DSParam* dsp){
		if(dsp==0) return 0;
		String src = dsp->Source,dst = dsp->Destinate;
		if(!FPToFullPath(src)) return 0;
		if(!FPToFullPath(dst)) return 0;
		if(FPIsSame(src,dst)) return 0;
		int type = GetFileType(src);
		if(type==0) return 0;
		bool isdir = type==2;
		bool r = true;
		if(isdir){
			if(!CreateFolder(dst)) return 0;
			FileSearch ft;
			ft.SetAfterCall(1);
			ft.SetPreCall(1);
			ft.SetSearchSub(1);
			ft.OnList.BindStd(&DirectorySystem::_Copy);
			r = ft.Search(src,dsp);
		}else{
			FileSystem fs(src);
			fs.OnProgress = dsp->OnFileProgress;
			HRESULT hr = fs.Copy(dst);
			r = (FS_OK==hr||FS_FILE_COPYED==hr);
		}
		if(!dsp->OnComplete.IsNull())
			dsp->OnComplete(src,dst,dsp->UserData);
		return r;
	}
	//class FileWatch
	FileWatch::FileWatch():_Sub(1),_CompPortHandle(0)
	{
		_Type = FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_CREATION;
		_buffer.Reserve(1);
		_buffer.Commit();
	}
	FileWatch::~FileWatch()
	{
		Stop();
	}
	bool FileWatch::AddFolder(LPCWSTR folder)
	{
		if(folder==0) return 0;
		if(folder[0]==0) return 0;
		String fp = folder;
		FPToBackSlash(fp);
		FPDelTailSlash(fp);
		return _folder.Add(fp);
	}
	bool FileWatch::DelFolder(LPCWSTR folder)
	{
		if(folder==0) return 0;
		if(folder[0]==0) return 0;
		String fp = folder;
		FPToBackSlash(fp);
		FPDelTailSlash(fp);
		return _folder.Delete(folder);
	}
	bool FileWatch::Start()
	{
		if(_CompPortHandle) return 0;
		for(uint i=0;i<_folder.Count();i++){
			//用于监视的文件夹句柄必须使用下面的参数.ReadDirectoryChangesW可以同步调用和异步调用,
			//对于同步调用,则去掉FILE_FLAG_OVERLAPPED.
			HANDLE hDir = ::CreateFile(_folder[i],FILE_LIST_DIRECTORY,
				FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
				NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,NULL);
			if(hDir==INVALID_HANDLE_VALUE) continue;
			//这个函数的第三个参数是一段内存,这里是传入,后面GetQueuedCompletionStatus函数的相应
			//参数(lpCompletionKey)返回的即是这个值.这个参数用于传递信息,因为GetQueuedCompletionStatus
			//返回时,并不知道是哪个文件夹的消息.这个参数就可以包含相应的识别标识.
			HANDLE cph = ::CreateIoCompletionPort(hDir,_CompPortHandle,(ULONG_PTR)_folder[i],0);
			if(cph){
				_CompPortHandle = cph;
				_hdir.Add(hDir);
			}else{
				::CloseHandle(hDir);
			}
		}
		if(_hdir.Count()==0) return 0;
		return _thread.Call<void>(this,&FileWatch::_watch);
	}
	bool FileWatch::Stop(){
		if(_CompPortHandle==0) return 0;
		::PostQueuedCompletionStatus(_CompPortHandle,0,0,NULL);
		if(_thread.Wait(1000)==WAIT_TIMEOUT)
			_thread.Stop();
		_thread.Close();
		for(uint i=0;i<_hdir.Count();i++){
			::CloseHandle(_hdir[i]);
		}
		_hdir.Clear();
		::CloseHandle(_CompPortHandle);
		_CompPortHandle = 0;
		return 1;
	}
	void FileWatch::_watch(){
		LPBYTE buffer = (LPBYTE)_buffer.Handle();
		DWORD numBytes;
		AutoStruct(OVERLAPPED,_ov);
		/*ReadDirectoryChangesW函数同步调用,只用这一个函数即可完成文件夹监视功能,lpOverlapped必须传0.
		此时函数不立即返回,已经进入监视状态.因此在单线程程序中无法完成多个文件夹的监视.当函数返回时,
		说明监视的状态发生了变化,信息在buffer中,是一系列的FILE_NOTIFY_INFORMATION.而异步调用时,这个
		函数立即返回,所以可以重复调用完成多个文件夹的监视.此时CreateIoCompletionPort函数也必须调用多次,
		这两个函数只有第一个参数是相同的,但它们的行为有非常大的关联.不进行CreateIoCompletionPort的调用,
		这个函数会失败.nBufferLength参数的最大值是0x10000也就是64K.
		*/
		for(uint i=0;i<_hdir.Count();i++){
			::ZeroMemory(&_ov,sizeof(_ov));//必须先置零。
			::ReadDirectoryChangesW(_hdir[i],buffer,0x10000,_Sub,_Type,&numBytes,&_ov,0);
		}
		LPOVERLAPPED pov = &_ov;
		PFILE_NOTIFY_INFORMATION pfni;
		while(true){
			//FILE_NOTIFY_INFORMATION是一个存贮文件变化信息的结构,大小是不确定的.第一个成员是下一个结构的偏移量,
			//如果为0,表示没有下一个结构.第二个成员存贮的是变化的类型,第三个参数表示文件名的长度,这里它和第一个
			//成员的功能其实是重复的,因为它们总是相差8.这里文件名长度是字节数,而不是字符数.而且文件名不是以NULL
			//结尾,这一点倒是够节俭.当有监视的事件发生时,这个函数才返回,信息存贮在ReadDirectoryChangesW的lpBuffer
			//参数中.lpCompletionKey就是CreateIoCompletionPort的CompletionKey参数.这个参数可以用于识别事件的身份.
			//如果不传,就会出现内存分配错误.
			LPCWSTR dir = 0;
			::GetQueuedCompletionStatus(_CompPortHandle,&numBytes,(PULONG_PTR)&dir,&pov,INFINITE);
			//一旦启动GetQueuedCompletionStatus函数，就不能通过ReadDirectoryChangesW和CreateIoCompletionPort再加入
			//新的监视目录，否则，就会出现异常。
			if(dir==0) break;//退出监视。
			pfni = (PFILE_NOTIFY_INFORMATION)buffer;
			while(1){
				String fn;
				fn.CopyFrom(pfni->FileName,pfni->FileNameLength/2);
				fn.Insert('\\',0);
				fn.Insert(dir,0);
				if(!OnChange.IsNull()) OnChange(pfni->Action,fn);
				if(0==pfni->NextEntryOffset) break;
				pfni = (PFILE_NOTIFY_INFORMATION)(buffer + pfni->NextEntryOffset);
			}
			::ZeroMemory(&_ov,sizeof(_ov));//必须先置零。
			int index = _folder.IndexOf(dir);
			if(index==-1) continue;
			::ReadDirectoryChangesW(_hdir[index],buffer,0x10000,_Sub,_Type,&numBytes,&_ov,0);
		}
	}
	//Class File
	File::File()
	{
		_Handle = INVALID_HANDLE_VALUE;
	}
	File::~File()
	{
		if(_Handle!=INVALID_HANDLE_VALUE) ::CloseHandle(_Handle);
	}
	File::File(LPCWSTR file)
	{
		_Handle = ::CreateFile(file,FILE_GENERIC_READ|FILE_GENERIC_WRITE,FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	}
	bool File::Attach(HANDLE file)
	{
		if(_Handle!=INVALID_HANDLE_VALUE) ::CloseHandle(_Handle);
		_Handle = file;
		return 1;
	}
	HANDLE File::Detach()
	{
		HANDLE handle = _Handle;
		_Handle = INVALID_HANDLE_VALUE;
		return handle;
	}
	bool File::SetPointer(uint64 offset,DWORD flag)//FILE_CURRENT,FILE_END
	{
		LARGE_INTEGER li;
		li.QuadPart = offset;
		return(::SetFilePointerEx(_Handle,li,0,flag)!=0);
	}
	uint64 File::GetPointer()
	{
		LARGE_INTEGER li,mi;
		mi.QuadPart = 0;
		if(!::SetFilePointerEx(_Handle,mi,&li,FILE_CURRENT)) return (uint64)-1;
		return li.QuadPart;
	}
	bool File::Flush()
	{
		return ::FlushFileBuffers(_Handle)!=0;
	}
	bool File::Create(LPCWSTR name,DWORD create,DWORD share,DWORD access,DWORD atr)
	{
		HANDLE handle = ::CreateFile(name,access,share,0,create,atr,0);
		if(handle==INVALID_HANDLE_VALUE)
			return 0;
		if((_Handle!=INVALID_HANDLE_VALUE)&&_Handle)
			::CloseHandle(_Handle);
		_Handle = handle;
		return 1;
	}
	uint64 File::GetLength(){
		LARGE_INTEGER li;
		if(::GetFileSizeEx(_Handle,&li)==0) return (unsigned __int64)(-1);
		return li.QuadPart;
	}
	int File::Write(const void* handle,int length)
	{
		DWORD writedBytes = 0;
		::WriteFile(_Handle,handle,length,&writedBytes,0); 
		return writedBytes;
	}
	int File::Read(LPVOID handle,int length)
	{
		DWORD size = 0;
		::ReadFile(_Handle,handle,length,&size,0);
		return size;
	};
	bool File::SetLength(uint64 length)
	{
		LARGE_INTEGER li;
		li.QuadPart = length;
		::SetFilePointerEx(_Handle,li,0,0);
		return ::SetEndOfFile(_Handle)!=0;
	}
	bool File::Close()
	{
		if(_Handle==INVALID_HANDLE_VALUE)
			return 0;
		bool r = ::CloseHandle(_Handle)!=0;
		_Handle = INVALID_HANDLE_VALUE;
		return r;
	};

	//class FileStream
	bool FileStream::Attach(HANDLE hFile)
	{
		if(_Handle!=INVALID_HANDLE_VALUE) CloseHandle(_Handle);
		if(GetFileSize(hFile,0)==INVALID_FILE_SIZE) return 0;
		_Handle = hFile;
		return 1;
	}
	int FileStream::ReadString(String& buffer,int position,DWORD code){
		int length = ::GetFileSize(_Handle,0);
		if(length==INVALID_FILE_SIZE) return 0;
		if(position<0) position = ::SetFilePointer(_Handle,0,0,FILE_CURRENT);
		else if(position>=length) return 0;
		else ::SetFilePointer(_Handle,position,0,FILE_BEGIN);
		length -= position;
		if(code!=0&&code!=1200){
			Memory<char> str;
			if(!str.SetLength(length)) return 0;
			length = Read(str.Handle(),length);
			if(code==-1){
				bool isUtf8 = MbsIsUtf8(str.Handle(),length);
				if(isUtf8) code = 65001;
				else code = 3;
			}
			buffer.FromMultiByte(str.Handle(),length,code);
		}else{
			if(!buffer.SetCubage(length/2+1)) return 0;
			length = Read(buffer.Handle(),length);
			buffer[length/2] = 0;
			buffer.Realize();
		}
		return length;
	}
	int FileStream::ReadMbsFromWs(Memory<char>& buffer,int position,DWORD code){
		int length = ::GetFileSize(_Handle,0);
		if(length==INVALID_FILE_SIZE) return 0;
		if(position<0) position = ::SetFilePointer(_Handle,0,0,FILE_CURRENT);
		else if(position>=length) return 0;
		else ::SetFilePointer(_Handle,position,0,FILE_BEGIN);
		length -= position;
		if(code!=0&&code!=1200){
			Memory<wchar_t> us;
			if(!us.SetLength(length/2)) return 0;
			length = Read(us.Handle(),length);
			int len = ::WideCharToMultiByte(code,0,us.Handle(),length/2,0,0,0,0);
			if(!buffer.SetLength(len+1)) return 0;
			len = ::WideCharToMultiByte(code,0,us.Handle(),length/2,buffer.Handle(),len,0,0);
			buffer[len] = 0;
		}else{
			if(!buffer.SetLength(length+1)) return 0;
			length = Read(buffer.Handle(),length);
			buffer[length] = 0;
		}
		return length;
	}
	int FileStream::ReadInt(int position,int defValue)
	{
		int length = ::GetFileSize(_Handle,0);
		if(length==INVALID_FILE_SIZE) return defValue;
		if(position<0) position = ::SetFilePointer(_Handle,0,0,FILE_CURRENT);
		if((position+4)>length) return defValue;
		else ::SetFilePointer(_Handle,position,0,FILE_BEGIN);
		Read(&defValue,4);
		return defValue;
	}
	char FileStream::ReadChar(int position,char defValue)
	{
		int length = ::GetFileSize(_Handle,0);
		if(length==INVALID_FILE_SIZE) return defValue;
		if(position<0) position = ::SetFilePointer(_Handle,0,0,FILE_CURRENT);
		if(position>=length) return defValue;
		else ::SetFilePointer(_Handle,position,0,FILE_BEGIN);
		Read(&defValue,1);
		return defValue;
	}
	int FileStream::WriteString(LPCWSTR wstr,int position,LPCWSTR affix,DWORD code)
	{
		if(position>=0) ::SetFilePointer(_Handle,position,0,FILE_BEGIN);
		int length,wb;
		if(code!=0&&code!=1200){
			Memory<char> str;
			String ws;
			ws.Attach((LPWSTR)wstr);
			length = ws.ToMultiByte(str,code);
			ws.Detach();
			wb = Write(str.Handle(),length);
			if(affix){
				ws.Attach((LPWSTR)affix);
				length = ws.ToMultiByte(str,code);
				ws.Detach();
				wb += Write(str.Handle(),length);
			}
		}else{
			length = WcsLength(wstr)*2;
			wb = Write((void*)wstr,length);
			if(affix){
				length = WcsLength(affix)*2;
				wb += Write((void*)affix,length);
			}
		}
		return wb;
	}
	int FileStream::WriteMbsToWs(LPCSTR str,int position,LPCSTR affix,DWORD code)
	{
		if(position>=0) ::SetFilePointer(_Handle,position,0,FILE_BEGIN);
		int length = MbsByteLen(str),wb;
		if(code!=0&&code!=1200)
		{
			String wstr;
			wstr.FromMultiByte(str,length,code);
			wb = Write(wstr.Handle(),wstr.Length()*2);
			if(affix) 
			{
				length = MbsByteLen(affix);
				wstr.FromMultiByte(affix,length,code);
				wb += Write(wstr.Handle(),wstr.Length()*2);
			}
		}
		else
		{
			wb = Write((void*)str,length);
			length = MbsByteLen(affix);
			wb += Write((void*)affix,length);
		}
		return wb;
	}
	bool FileStream::WriteInt(int value,int position)
	{
		if(position>=0) ::SetFilePointer(_Handle,position,0,FILE_BEGIN);
		Write(&value,4);
		return 1;
	}
	bool FileStream::WriteChar(char value,int position)
	{
		if(position>=0) ::SetFilePointer(_Handle,position,0,FILE_BEGIN);
		Write(&value,1);
		return 1;
	}
	//class FileMap
	FileMap::FileMap(){
	}
	FileMap::~FileMap(){
		if(_Handle) ::CloseHandle(_Handle);
	}
	bool FileMap::Create(HANDLE file,DWORD flag,uint64 size){
		if(_Handle) return 0;
		DWORD fAccess = GENERIC_READ;
		if(flag&FILE_MAP_WRITE)
			fAccess |= GENERIC_READ | GENERIC_WRITE;
		if(flag&FILE_MAP_EXECUTE)
			fAccess |= GENERIC_EXECUTE;
		//if(!_File.Create(file,OPEN_ALWAYS,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,fAccess)) return 0;
		DWORD pAccess = PAGE_READONLY;
		//_access = FILE_MAP_READ;
		if(flag&FILE_MAP_WRITE){
			pAccess = PAGE_READWRITE;
			//_access |= FILE_MAP_WRITE;
		}
		if(flag&FILE_MAP_EXECUTE){
			pAccess = PAGE_EXECUTE_READ;
			//_access |= FILE_MAP_EXECUTE;
		}
		if((flag&FILE_MAP_EXECUTE)&&(flag&FILE_MAP_WRITE)){
			pAccess = PAGE_EXECUTE_READWRITE;
		}
		if(flag&FILE_MAP_COPY){
			//_access = FILE_MAP_COPY;
		}
		LARGE_INTEGER li;
		if(size==0){
			::GetFileSizeEx(file,&li);
		}else{
			li.QuadPart = size;
		}
		if(li.QuadPart==0) return 0;
		_Handle = ::CreateFileMapping(file,0,pAccess,li.HighPart,li.LowPart,0);
		return (_Handle!=0);
	}
	void FileMap::Close()	{
		if(_Handle){
			::CloseHandle(_Handle);
			_Handle = NULL;
		}
	}
	MapView::MapView(){
		;
	}
	MapView::~MapView(){
		if(_Handle) ::UnmapViewOfFile(_Handle);
	}
	void MapView::Close(){
		if(_Handle){
			::UnmapViewOfFile(_Handle);
			_Handle = NULL;
		}
	}
	bool MapView::Create(HANDLE map,uint64 offset,uint size,DWORD access){
		if(_Handle) ::UnmapViewOfFile(_Handle);
		if(!map) return 0;//无效_Map，可能尚未初始化实例。
		LARGE_INTEGER li;
		li.QuadPart = offset;
		_Handle = ::MapViewOfFile(map,access,li.HighPart,li.LowPart,size);
		if(_Handle) _length = size;
		return _Handle!=0;
	}
	bool MapView::Flush(uint start,uint length){
		if(length==-1) length = _length;
		return ::FlushViewOfFile((char*)_Handle+start,length)!=0;
	}
}