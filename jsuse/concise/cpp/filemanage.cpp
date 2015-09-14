#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	//file string function
	bool SetFolderIcon(LPCWSTR folder,LPCWSTR iconfile,LPCWSTR iconindex,LPCWSTR toolinfo)
	{
		if(!SetFileAttr(folder,FILE_ATTRIBUTE_SYSTEM,0)) return 0;
		String buffer(folder);
		buffer += L"\\desktop.ini";
		SetFileAttr(buffer,0,FILE_ATTRIBUTE_READONLY);
		if(iconfile==0)
			return ::DeleteFile(buffer)!=0;
		HANDLE hfile = ::CreateFile(buffer,FILE_ALL_ACCESS,FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,0,CREATE_ALWAYS,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY,0);
		if(hfile==INVALID_HANDLE_VALUE)
			return 0;
		buffer = L"[.ShellClassInfo]\r\nInfoTip=";
		buffer += toolinfo;
		buffer += L"\r\niconindex=";
		buffer += iconindex;
		buffer += L"\r\niconfile=";
		buffer += iconfile;
		buffer += L"\r\n";
		Memory<char> buf;
		buffer.ToMultiByte(&buf);
		DWORD number;
		::WriteFile(hfile,buf,buf.Length(),&number,0);
		::CloseHandle(hfile);
		return 1;
	}
	bool SetFileAttr(LPCWSTR file,DWORD add,DWORD del)
	{
		DWORD ab = GetFileAttributes(file);
		if(ab==0xffffffff) return 0;
		ab |= add;
		ab |= del;
		ab -= del;
		return ::SetFileAttributes(file,ab)!=0;
	}
	HICON GetFileIcon(LPCWSTR file,int index)
	{
		if(file==0) return 0;
		WORD i = index;
		wchar_t filebuffer[MAX_PATH];//这个参数必须是可写的.
		WcsCopy(filebuffer,file);
		size_t len = wcslen(file);
		if((len>0)&&(GetFileAttributes(file)&FILE_ATTRIBUTE_DIRECTORY))
		{
			if(filebuffer[len-1]!=L'\\')
			{
				filebuffer[len] = L'\\';
				filebuffer[len+1] = 0;
			}
		}
		return ::ExtractAssociatedIconW(::GetModuleHandle(0),filebuffer,&i);
		//if all parameters is right,this function is allways success;
	}
	bool _ValidFileName(LPCWSTR file){
		if(!file[0]) return 0;
		if(file[0]=='.'){
			if(file[1]==0) return 0;
			if(file[1]=='.'){
				if(file[2]==0) return 0;
			}
		}
		return 1;
	}
	bool _IsFilter(StringList* filter,LPCWSTR fn){
		if(!filter->Count()) return 1;
		String ext = fn; 
		FPToExt(ext);
		for(uint i=0;i<filter->Count();i++){
			if(WcsEqualNoCase(filter->Element(i),ext)) return 1;
		}
		return 0;
	}
	/*
	FileSearchData::FileSearchData(){
		FileInfo = 0;
		Original = 0;
		Relative = 0;
		ExitSearch = 0;
		SkipThisFolder = 0;
	}
	FileSearchData::FileSearchData(WIN32_FIND_DATA* wfd,LPCWSTR ori,LPCWSTR rel){
		FileInfo = wfd;
		Original = ori;
		Relative = rel;
		ExitSearch = 0;
		SkipThisFolder = 0;
	}
	bool FileSearch::_search(LPCWSTR subFolder){
		String path = _path;
		FPLinkPath(path,subFolder);
		FPLinkPath(path,Pattern);

		WIN32_FIND_DATA wfd;
		HANDLE hFind = FindFirstFile(path,&wfd);
		if(hFind==INVALID_HANDLE_VALUE){
			PrintLastError(L"FileSearch.Search: %s");
			return 0;
		}
		int find = 1;

		while(find){
			if(!_ValidFileName(wfd.cFileName)){
				find = FindNextFile(hFind,&wfd);
				continue;
			}
			FileSearchData fsd(&wfd,_path,subFolder);

			if(fsd.IsDir()){//文件夹
				String nr = subFolder;
				FPLinkPath(nr,wfd.cFileName);
				if(SubFolderPre) OnSearch(&fsd,this);
				if(fsd.ExitSearch||_exit) break;
				if(!fsd.SkipThisFolder&&SearchSubFolder) _search(nr);
				if(SubFolderAfter) OnSearch(&fsd,this);
			}else{//文件
				if(_IsFilter(&Filter,wfd.cFileName)) OnSearch(&fsd,this);
			}
			if(fsd.ExitSearch||_exit) break;

			find = FindNextFile(hFind,&wfd);
		}
		FindClose(hFind);
		if(subFolder==0){
			_path = L"";
			OnComplete(this);
		}
		return true;
	}
	FileSearch::FileSearch(){
		UserData = 0;
		SearchSubFolder = 0;
		SubFolderPre = 1;
		SubFolderAfter = 0;
		Pattern = L"*";
		_exit = 0;
	}
	bool FileSearch::Search(LPCWSTR path){
		if(!_path.IsNull()){
			PrintD(L"FileSearch.Search: a Search is running");
			return 0;
		}
		_path = path;
		FPDelTailSlash(_path);
		if(AsynSearch){
			return _thread.Call<bool>(this,&FileSearch::_search,(LPCWSTR)0);
		}else{
			return _search(0);
		}
	}
	*/
	FolderWatch::FolderWatch(){
		_handle = INVALID_HANDLE_VALUE;
		_compPortHandle = 0;
		_sub = 1;
		_type = Normal;
		UserData = 0;
	}
	bool FolderWatch::Close(){
		if(_compPortHandle==0) return 0;
		if(_buffer.Length())//已经启动监视。
			return 0!=::PostQueuedCompletionStatus(_compPortHandle,0,0,NULL);
		CloseHandle(_compPortHandle);
		_compPortHandle = 0;
		CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
		return 1;
	}
	bool FolderWatch::Create(LPCWSTR folder,bool subfolder,DWORD type){
		if(_handle!=INVALID_HANDLE_VALUE){
			PrintD(L"FolderWatch.Create:instance has created");
			return 0;
		}

		_handle = ::CreateFile(folder,FILE_LIST_DIRECTORY,
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
			NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,NULL);
		if(_handle==INVALID_HANDLE_VALUE){
			PrintLastError(L"FolderWatch.Create:%s");
			return 0;
		}

		_type = type;
		_sub = subfolder;
		_name = folder;

		_compPortHandle = ::CreateIoCompletionPort(_handle,NULL,(ULONG_PTR)this,0);
		if(_compPortHandle==0){
			::CloseHandle(_handle);
			_handle = INVALID_HANDLE_VALUE;
			PrintLastError(L"FolderWatch.Create:%s");
			return 0;
		}
		return 1;
	}
	//这是个阻塞函数，如果不希望阻塞，在一个新的线程调用。
	bool FolderWatch::Start(){
		if(_compPortHandle==0){
			PrintD(L"FolderWatch.Start:not Create yet");
			return 0;
		}
		//_buffer用于缓存监视目录的信息，很难确定这个缓存需要多大，但是一般不是同时有大量信息的话，这个
		//值并不需要多大，通常它只需要几十字节大小，但是为了应付大量信息，设为64K已经很大了。
		_buffer.SetLength(0x10000);
		DWORD numBytes;
		AutoStruct(OVERLAPPED,_ov);

		LPOVERLAPPED pov = &_ov;
		PFILE_NOTIFY_INFORMATION pfni;
		while(true){
			::ZeroMemory(&_ov,sizeof(_ov));//必须先置零。
			BOOL r = ::ReadDirectoryChangesW(_handle,_buffer.Handle(),_buffer.Length(),_sub,_type,&numBytes,&_ov,0);
			if(!r){
				PrintD(L"FolderWatch.Start:ReadDirectoryChangesW failed");
				break;
			}

			FolderWatch* inst = 0;
			r = ::GetQueuedCompletionStatus(_compPortHandle,&numBytes,(PULONG_PTR)&inst,&pov,INFINITE);
			if(!r){
				PrintD(L"FolderWatch.Start:GetQueuedCompletionStatus failed");
				break;
			}
			if(inst==0){
				PrintD(L"FolderWatch.Start:well exit");
				break;//退出监视。
			}
			char* pBuf = _buffer.Handle();
			while(1){
				pfni = (PFILE_NOTIFY_INFORMATION)pBuf;
				String fn;
				fn.CopyFrom(pfni->FileName,pfni->FileNameLength/2);
				if(!OnChange.IsNull())
					OnChange(pfni->Action,fn,this);
				if(0==pfni->NextEntryOffset) break;
				pBuf += pfni->NextEntryOffset;
			}
		}
		::CloseHandle(_compPortHandle);
		_compPortHandle = 0;
		CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
		_buffer.Free();
		if(!OnStop.IsNull())
			OnStop(this);
		return 1;
	}
	void MultiFolderWatch::_onChange(DWORD action,LPCWSTR fn,FolderWatch* pfw){
		if(!OnChange.IsNull())
			OnChange(action,fn,pfw);
	}
	void MultiFolderWatch::_onStop(FolderWatch* pfw){
		PrintD(L"watch stop :%s",pfw->GetWatchPath());
		LocalCriticalSection lcs(_csList);
		_watchList.Delete(pfw);
	}

	bool MultiFolderWatch::Start(LPCWSTR folder,void* userData,bool subfolder,DWORD type){
		LocalCriticalSection lcs(_csList);
		FolderWatch* pfw = _watchList.Add();
		pfw->UserData = userData;
		if(!pfw->Create(folder,subfolder,type)) return 0;
		pfw->OnChange.Bind(this,&MultiFolderWatch::_onChange);
		pfw->OnStop.Bind(this,&MultiFolderWatch::_onStop);
		return _taskEngine.StartTask(pfw,&FolderWatch::Start);
	}
	bool MultiFolderWatch::Stop(LPCWSTR folder){
		LocalCriticalSection lcs(_csList);
		for(uint i=0;i<_watchList.Count();i++){
			FolderWatch& fw  = _watchList[i];
			if(!WcsEqual(fw.GetWatchPath(),folder)) continue;
			fw.Close();
			return 1;
		}
		return 0;
	}
	void MultiFolderWatch::Close(){
		_csList.Lock();
		for(uint i=0;i<_watchList.Count();i++){
			_watchList[i].Close();
		}
		_csList.Unlock();
		_taskEngine.Close();
	}
	MultiFolderWatch::~MultiFolderWatch(){
		Close();
	}
}