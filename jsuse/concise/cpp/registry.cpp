#include "../stdafx.h"
#include "../../concise.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
namespace cs{
	Registry::Registry(void)
	{
	}
	Registry::~Registry(void)
	{
		if(_Handle) ::RegCloseKey(_Handle);//if this is a invalide key,then will make a valinte;
	}
	bool Registry::Attach(HKEY handle)
	{
		::RegCloseKey(_Handle);
		_Handle = handle;
		return 1;
	}
	HKEY Registry::Detach()
	{
		HKEY hkey = _Handle;
		_Handle = 0;
		return hkey;
	}
	bool Registry::Close(){
		if(_Handle&&::RegCloseKey(_Handle)==ERROR_SUCCESS){
			_Handle = 0;
			return 1;
		}
		return 0;
	}
	Registry::REGRESULT Registry::MultiCreate(LPCWSTR subKey,HKEY key,bool create,REGSAM access,DWORD option){
		String kn = subKey;
		for(uint i=0;i<kn.Length();i++){
			if(kn[i]=='\\') kn[i] = '/';
		}
		StringList sl;
		sl.SplitString(kn,L"/");

		if(key==0){
			if(WcsEqualNoCase(sl[0],L"HKEY_CURRENT_USER")){
				key = HKEY_CURRENT_USER;
			}else if(WcsEqualNoCase(sl[0],L"HKEY_CLASSES_ROOT")){
				key = HKEY_CLASSES_ROOT;
			}else if(WcsEqualNoCase(sl[0],L"HKEY_LOCAL_MACHINE")){
				key = HKEY_LOCAL_MACHINE;
			}else if(WcsEqualNoCase(sl[0],L"HKEY_USERS")){
				key = HKEY_USERS;
			}else if(WcsEqualNoCase(sl[0],L"HKEY_CURRENT_CONFIG")){
				key = HKEY_CURRENT_CONFIG;
			}else{
				key = 0;
			}
			if(key){
				sl.Delete((uint)0);
			}else{
				key = HKEY_CURRENT_USER;
			}
		}

		REGRESULT rst = Create(sl[0],key,create,access,option);
		if(REG_ERROR==rst) return REG_ERROR;
		for(uint i=1;i<sl.Count();i++){
			rst = ToSubKey(sl[i],create,access,option);
			if(REG_ERROR==rst){
				Close();
				break;
			}
		}
		return rst;
	}
	Registry::REGRESULT Registry::Create(LPCWSTR subKey,HKEY key,bool create,REGSAM access,DWORD option){//Return Value : 0 Open fails;1 Create a new key;2 Open a exist key.
		REGRESULT createKey = REG_OPEN;
		if(_Handle){
			::RegCloseKey(_Handle);
			_Handle = 0;
		}
		if(key==0) key = HKEY_CURRENT_USER;
		if(create){
			if(::RegCreateKeyEx(key,subKey,0,0,option,access,NULL,&_Handle,(LPDWORD)&createKey))	return REG_ERROR;
		}else{
			if(::RegOpenKeyEx(key,subKey,0,access,&_Handle)) return REG_ERROR;
		}
		return createKey;
	}
	Registry::REGRESULT Registry::ToMultiSubKey(LPCWSTR subKey,bool create,REGSAM access,DWORD option){
		String kn = subKey;
		for(uint i=0;i<kn.Length();i++){
			if(kn[i]=='\\') kn[i] = '/';
		}
		StringList sl;
		sl.SplitString(kn,L"/");

		REGRESULT rst = REG_ERROR;
		for(uint i=0;i<sl.Count();i++){
			rst = ToSubKey(sl[i],create,access,option);
			if(rst==REG_ERROR) break;
		}
		return rst;
	}
	Registry::REGRESULT Registry::ToSubKey(LPCWSTR subKey,bool create,REGSAM access,DWORD option)//Return Value : 0 Open fails;1 Create a new key;2 Open a exist key.
	{
		if(_Handle==0) _Handle = HKEY_CURRENT_USER;
		REGRESULT createKey = REG_OPEN;
		HKEY handle = 0;
		if(create){
			if(::RegCreateKeyEx(_Handle,subKey,0,0,option,access,NULL,&handle,(LPDWORD)&createKey)) return REG_ERROR;
		}else{
			if(::RegOpenKeyEx(_Handle,subKey,0,access,&handle)) return REG_ERROR;
		}
		if(((UINT_PTR)_Handle&0x80000000)==0)
			::RegCloseKey(_Handle);
		_Handle = handle;
		return createKey;
	}
	bool Registry::GetValue(LPCWSTR valueName,Memory<BYTE>* data,LPDWORD pType){
		DWORD size = data->Length(),type = 0;
		LONG r = ::RegQueryValueExW(_Handle,valueName,0,&type,data->Handle(),&size);
		if(size<=data->Length()&&(r==ERROR_SUCCESS||r==ERROR_MORE_DATA)){
			if(pType) *pType = type;
			return 1;
		}else{
			data->SetLength(size);
			r = ::RegQueryValueExW(_Handle,valueName,0,&type,data->Handle(),&size);
			if(r==ERROR_SUCCESS){
				if(pType) *pType = type;
				return 1;
			}
		}
		return 0;
	}
	bool Registry::SetValue(LPCWSTR valueName,BYTE* data,uint len,IN DWORD type){
		return ::RegSetValueExW(_Handle,valueName,0,type,data,len)==0;
	}
	bool Registry::GetStrValue(LPCWSTR valueName,String& strValue)
	{
		DWORD type,size,bufs;
		if(strValue.Handle()==0) bufs = 0;
		else bufs = strValue.Cubage()*2+2;
		size = bufs;
		LONG r = ::RegQueryValueExW(_Handle,valueName,0,&type,(LPBYTE)strValue.Handle(),&size);
		if(size>bufs)
		{
			strValue.SetCubage(size/2-1);
			r = ::RegQueryValueExW(_Handle,valueName,0,&type,(LPBYTE)strValue.Handle(),&size);
		}
		if(r==ERROR_SUCCESS)
		{
			if(type==REG_SZ)
			{
				strValue.Realize();
				return 1;
			}
			else if(type==REG_EXPAND_SZ)
			{
				DWORD len = ::ExpandEnvironmentStrings(strValue,0,0);
				String value(len);
				::ExpandEnvironmentStrings(strValue,value,len);
				value.Realize();
				strValue = value;
				return 1;
			}
		}
		return 0;
	}
	bool Registry::SetIntValue(LPCWSTR valueName,DWORD value)
	{
		return ::RegSetValueExW(_Handle,valueName,0,REG_DWORD,(LPBYTE)&value,sizeof(DWORD))==0;
	}
	bool Registry::SetStrValue(LPCWSTR valueName,LPCWSTR wstr)
	{
		return ::RegSetValueExW(_Handle,valueName,0,REG_SZ,(LPBYTE)wstr,(DWORD)wcslen(wstr)*2 + 2)==0;
	}
	bool Registry::GetIntValue(LPCWSTR valueName,DWORD& val)
	{
		DWORD type = 4,size = 4;
		return 0==::RegQueryValueExW(_Handle,valueName,0,&type,(LPBYTE)&val,&size);
	}
	int Registry::EnumSubKey(StringMemList* sl)
	{
		DWORD r = 0,size = 256,index = 0;
		String name;
		name.SetCubage(size);
		while(true)
		{
			r = ::RegEnumKeyExW(_Handle,index,name.Handle(),&size,0,NULL,NULL,NULL);
			if(r==ERROR_MORE_DATA)
			{
				name.SetCubage(size);
				r = ::RegEnumKeyExW(_Handle,index,name.Handle(),&size,0,NULL,NULL,NULL);
			}
			if(r==ERROR_NO_MORE_ITEMS)
				break;
			if(r==ERROR_SUCCESS)
				sl->Add(name);
			index++;
			size = 256;
		}
		return index;
	}
	int Registry::EnumValueName(StringMemList* sl)
	{
		DWORD size = 256,index = 0,type,r;
		String name(size);
		while(true)
		{
			r = ::RegEnumValueW(_Handle,index,name.Handle(),&size,0,&type,NULL,NULL);
			if(r==ERROR_MORE_DATA)
			{
				name.SetCubage(size);
				continue;
			}
			if(r==ERROR_NO_MORE_ITEMS)
				break;
			index++;
			if(!r)
				sl->Add(name);
			size = 256;
		}
		return index;
	}
	int Registry::EnumIntValue(StringMemList* names,IntegerList<DWORD>* il)
	{
		DWORD nameSize = 256,index = 0,type,r,value,dataSize = sizeof(DWORD);
		String name(256);
		while(true)
		{
			r = ::RegEnumValueW(_Handle,index,name.Handle(),&nameSize,0,&type,(LPBYTE)&value,&dataSize);
			if(r==ERROR_NO_MORE_ITEMS)
				break;
			if((type==REG_DWORD)&&(r==ERROR_SUCCESS))
			{
				names->Add(name);
				il->Add((int&)value);
			}
			if(r==ERROR_MORE_DATA)
			{
				name.SetCubage(nameSize);
				dataSize = sizeof(DWORD);
				continue;
			}
			index++;
			if(nameSize<256)
				nameSize = 256;
			dataSize = sizeof(DWORD);
		}
		return names->Count();
	}
	int Registry::EnumStrValue(StringMemList* names,StringMemList* values)
	{
		DWORD nameSize = 256,dataSize = 256,index = 0,type,r;
		String name(nameSize),value(dataSize);
		while(true)
		{
			r = ::RegEnumValueW(_Handle,index,name,&nameSize,0,&type,(LPBYTE)value.Handle(),&dataSize);
			if(r==ERROR_MORE_DATA)
			{
				if(nameSize>256)
					name.SetCubage(nameSize);
				if(dataSize>256)
					value.SetCubage(dataSize);
				r = ::RegEnumValueW(_Handle,index,name,&nameSize,0,&type,(LPBYTE)value.Handle(),&dataSize);
			}
			if(r==ERROR_NO_MORE_ITEMS)
				break;
			index++;
			if(r==ERROR_SUCCESS)
			{
				if(type==REG_SZ)
				{
					names->Add(name);
					if(values)
						values->Add(value);
				}
				else if(type==REG_EXPAND_SZ)
				{
					names->Add(name);
					if(values)
					{
						DWORD len = ::ExpandEnvironmentStrings(name,value,dataSize);
						if(len>dataSize)
						{
							value.SetCubage(len);
							::ExpandEnvironmentStringsW(name,value,len);
						}
						values->Add(value);
					}
				}
			}
			if(nameSize<256)
				nameSize = 256;
			if(dataSize<256)
				dataSize = 256;
		}
		return names->Count();
	}
	LONG Registry::Flush()
	{
		return ::RegFlushKey(_Handle);
	}
	bool Registry::DelSubKey(LPCWSTR sub)
	{
		return ERROR_SUCCESS==SHDeleteKey(_Handle,sub);
	}
	bool Registry::DelValue(LPCWSTR name)
	{
		return ERROR_SUCCESS==RegDeleteValue(_Handle,name);
	}


	bool CodePage::Create(){
		reg.Create(L"SOFTWARE\\Classes\\MIME\\Database\\Charset",HKEY_LOCAL_MACHINE);
		return !reg.IsNull();
	}
	//返回注册表中全部的 CodePage 名称，这些名称有些是别称，对应的 CodePage 值可以重复。
	int CodePage::GetAllCodePage(StringMemList* sml){
		return reg.EnumSubKey(sml);
	}
	//返回指定名称的码表值，0 表示没有指定名称的代码页，名称不区分大小写。
	DWORD CodePage::GetCode(LPCWSTR cpName){
		Registry sreg;
		sreg.Create(cpName,reg);
		if(sreg.IsNull()) return 0;
		DWORD code = 0;
		if(!sreg.GetIntValue(L"InternetEncoding",code)){
			String skey;
			sreg.GetStrValue(L"AliasForCharset",skey);
			if(skey.Length()==0) return 0;
			sreg.Create(skey,reg);
			if(sreg.IsNull()) return 0;
			sreg.GetIntValue(L"InternetEncoding",code);
		}
		return code;
	}
	bool CodePage::Close(){
		return reg.Close();
	}
	DWORD GetCodePage(LPCWSTR name){
		static StringList nameList;
		static IntegerList<DWORD> codeList;
		static CriticalSection cs;
		if(name==0||name[0]==0) return CP_THREAD_ACP;
		LocalCriticalSection lcs(cs);
		if(nameList.Count()==0){
			nameList.Add(L"utf-8");
			codeList.Add(CP_UTF8);
			nameList.Add(L"utf-7");
			codeList.Add(CP_UTF7);
			nameList.Add(L"unicode");
			codeList.Add(1200);
		}
		String codeName = name;
		codeName.Lower();
		int index = nameList.IndexOf(codeName);
		if(index!=-1) return codeList[index];
		CodePage cp;
		cp.Create();
		DWORD code = cp.GetCode(codeName);
		if(code==0) return CP_THREAD_ACP;
		nameList.Add(codeName);
		codeList.Add(code);
		return code;
	}

}