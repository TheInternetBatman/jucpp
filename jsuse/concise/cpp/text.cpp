#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	void WcsLower(wchar_t* wcs)
	{
		if(wcs==0) return;
		for(wchar_t* p = wcs;*p;p++) if((*p>=L'A')&&(*p<=L'Z')) *p += 0x20;
	}
	void WcsUpper(wchar_t* wcs)
	{
		if(wcs==0) return;
		for(wchar_t* p = wcs;*p;p++) if((*p>=L'a')&&(*p<=L'z')) *p -= 0x20;
	}
	uint WcsLength(const wchar_t* wcs,uint maxlen)
	{
		if(wcs==0) return 0;
		wchar_t* eof = (wchar_t*)wcs;
		uint len = 0;
		while((*(eof+len))&&(len<maxlen)) len++;
		return len;
	}
	uint WcsLength(const wchar_t* wcs)
	{
		if(wcs==0) return 0;
		wchar_t* eof = (wchar_t*)wcs;
		while(*eof++);
		return (uint)(eof-wcs-1);
	}
	uint WcsCopy(wchar_t* ds,const wchar_t* sc,uint length)
	{
		if((ds==0)||(sc==0)) return 0;
		uint len = 0;
		while(len<length)
		{
			if(*sc)
			{
				*ds = *sc;
				ds++,(wchar_t*)sc++;
				len++;
			}
			else
			{
				*ds = *sc;
				break;
			}
		}
		return len;
	}
	bool WcsEqual(const wchar_t* str1,const wchar_t* str2,uint length){
		if(str1==str2) return 1;
		if(!str1) str1 = L"";
		if(!str2) str2 = L"";
		for(uint i=0;i<length;i++){
			if(str1[i]!=str2[i]) return 0;
			if(str1[i]==0) return 1;
		}
		return 1;
	}
	uint WcsCompare(const wchar_t* str1,const wchar_t* str2,uint length){
		if((str1==0)||(str2==0)) return 0;
		wchar_t* s1 = (wchar_t*)str1,*s2 = (wchar_t*)str2;
		while(*s1==*s2){
			s1++,s2++;
			if(*s1==0) return (uint)(s1-str1);
			if(*s2==0) return (uint)(s2-str2);
			if((s1-str1)==length) return length;
		}
		return 0;
	}
	bool WcsInclude(const wchar_t* wcs,const wchar_t* sub)
	{
		if((wcs==0)||(sub==0)) return 0;
		wchar_t* s1 = (wchar_t*)wcs,*s2 = (wchar_t*)sub;
		while(true)
		{
			if(*s2==0) return 1;
			if(*s1==0) return 0;
			if(*s1!=*s2) return 0;
			s1++,s2++;
		}
	}
	bool WcsEqualNoCase(const wchar_t* str1,const wchar_t* str2,uint length){
		if(str1==str2) return 1;
		if(!str1) str1 = L"";
		if(!str2) str2 = L"";
		wchar_t* s1 = (wchar_t*)str1,*s2 = (wchar_t*)str2;
		for(uint i=0;i<length;i++){
			short ds = *s1 - *s2;
			switch(ds)
			{
			case 0:
				if(*s1==0)
					return 1;
				break;
			case 32:
				if((*s1>L'z')||(*s1<L'a'))
					return 0;
				break;
			case -32:
				if((*s2>L'z')||(*s2<L'a'))
					return 0;
				break;
			default:
				return 0;
			}
			if((*s1==0)||(*s2==0)) return 0;
			s1++,s2++;
		}
		return 1;
	}
	uint WcsCompareNoCase(const wchar_t* s1,const wchar_t* s2,uint length){
		if((s1==0)||(s2==0)) return 0;
		for(uint i=0;i<length;i++){
			if(s1[i]==0||s2[i]==0) return i;
			short ds = s1[i] - s2[i];
			if(ds==0) continue;
			wchar_t ch;
			if(ds<0){
				ds = -ds;
				ch = s2[i];
			}else{
				ch = s1[i];
			}
			if(ds==32&&ch>='a'&&ch<='z') continue;
			else return i;
		}
		return length;
	}
	bool WcsIncludeNoCase(const wchar_t* wcs,const wchar_t* sub)
	{
		if((wcs==0)||(sub==0)) return 0;
		wchar_t* s1 = (wchar_t*)wcs,*s2 = (wchar_t*)sub;
		while(true)
		{
			if(*s2==0) return 1;
			short ds = *s1 - *s2;
			switch(ds)
			{
			case 0:
				break;
			case 32:
				if((*s1>L'z')||(*s1<L'a'))
					return 0;
				break;
			case -32:
				if((*s2>L'z')||(*s2<L'a'))
					return 0;
				break;
			default:
				return 0;
			}
			if(*s1==0) return 0;
			s1++,s2++;
		}
	}
	bool SameCase(wchar_t ch1,wchar_t ch2)
	{
		short ds = ch1 - ch2;
		if(ds==0) return true;
		else if(ds==32)
		{
			if((ch1>'z')||(ch1<'a')) return false;
			return true;
		}
		else if(ds==-32)
		{
			if((ch2>'z')||(ch2<'a')) return false;
			return true;
		}
		return false;

	}
	uint WcsFindNoCase(const wchar_t* wcs,const wchar_t* sub)
	{
		if((wcs&&sub)==0) return -1;
		wchar_t* s = (wchar_t*)wcs,*s2 = (wchar_t*)sub;
		while(*s)
		{
			s2 = (wchar_t*)sub;
			wchar_t* s1 = s;
			while(true)
			{
				if(*s1!=*s2)
				{				
					short ds = *s1 - *s2;
					if(ds==32)
					{
						if((*s1>'z')||(*s1<'a')) break;
					}
					else if(ds==-32)
					{
						if((*s2>'z')||(*s2<'a')) break;
					}
					else break;
				}
				s1++,s2++;
				if(!*s2) return SubHandle(s,wcs)/2;
			}
			s++;
		}
		return -1;
	}
	uint WcsFind(const wchar_t* wcs,const wchar_t* sub)
	{
		if((wcs&&sub)==0) return -1;
		wchar_t* s = (wchar_t*)wcs,*s2 = (wchar_t*)sub;
		while(*s)
		{
			s2 = (wchar_t*)sub;
			wchar_t* s1 = s;
			while(*s1==*s2)
			{
				s1++,s2++;
				if(!*s2) 
					return SubHandle(s,wcs)/2;
			}
			s++;
		}
		return -1;
	}
	void WcsReverse(wchar_t* string)
	{
		if(string==0) return;
		wchar_t *start = string;
		wchar_t *left = string;
		while (*string++);
		string -= 2;
		while(left<string)
		{
			wchar_t ch = *left;
			*left++ = *string;
			*string-- = ch;
		}
	}
	//多字节字串函数.
	uint MbsByteOfChar(const char* str,uint index)
	{
		if(!str) return 0;
		char* s = (char*)str;
		uint i = 0;
		while(i<index)
		{
			if(*s<0)
			{
				s++;
			}
			if(*s==0) break;
			i++;
			s++;
		}
		return (uint)(s-str);
	}
	uint MbsByteLen(const char* str,uint maxlen)
	{
		if(str==0) return 0;
		char* eof = (char*)str;
		for(uint i=0;i<maxlen;i++)
		{
			if(*eof==0)
				return i;
			eof++;
		}
		return maxlen;
	}
	uint MbsByteLen(const char* str)
	{
		if(str==0) return 0;
		char* eof = (char*)str;
		while(*eof) eof++;
		return (uint)(eof-str);
	}
	uint MbsCharLen(const char* str)
	{
		if(str==0) return 0;
		char* eof = (char*)str;
		uint len = 0;
		while(*eof)
		{
			if(*eof<0) eof++;
			eof++;
			len++;
		}
		return len;
	}
	uint MbsCharLen(const char* str,uint maxlen)
	{
		if(str==0) return 0;
		char* eof = (char*)str;
		uint len = 0;
		for(uint i=0;i<maxlen;i++)
		{
			if(*eof<0)
			{
				i++;
				eof++;
			}
			else if(*eof==0)
				return len;
			eof++;
			len++;
		}
		return len;
	}
	bool MbsEqual(const char* str,const char* str2,uint length)
	{
		if((str&&str2)==0) return 0;
		char* s1 = (char*)str,*s2 = (char*)str2;
		for(uint i=0;i<length;i++)
		{
			if(*s1!=*s2)
				break;
			if(*s1==0)
				return 1;
			s1++,s2++;
		}
		return 0;
	}
	bool MbsEqualNoCase(const char* str,const char* str2,uint length)
	{
		if((str&&str2)==0) return 0;
		if(length==0) return true;
		char* s1 = (char*)str,*s2 = (char*)str2;
		length -= 1;
		for(uint i=0;i<=length;i++)
		{
			if(*s1!=*s2)
			{
				if((*s1<='z')&&(*s1>='a'))
				{
					if(*s2!=(*s1-32)) break;
				}
				else if((*s2<='z')&&(*s2>='a'))
				{
					if(*s1!=(*s2-32)) break;
				}
				else break;
			}
			else
			{
				if(*s1==0||i==length)
					return 1;
			}
			s1++,s2++;
		}
		return 0;
	}
	uint MbsFind(const char* wcs,const char* sub,uint maxlen){
		if((wcs&&sub)==0) return -1;
		uint sublen = strlen(sub);
		if(maxlen<sublen) return -1;
		maxlen -= sublen;
		char* str = (char*)wcs;
		while(*str){
			char* cs = str,*s = (char*)sub;
			while(*cs==*s){
				cs++,s++;
				if(*s==0)
					return (uint)(str-wcs);
			}
			if(maxlen==0) break;
			maxlen--;
			str++;
		}
		return -1;
	}
	uint MbsFindNoCase(const char* wcs,const char* sub,uint maxlen){
		if((wcs&&sub)==0) return -1;
		char* str = (char*)wcs;
		uint sublen = strlen(sub);
		if(sublen>maxlen) return -1;
		maxlen -= sublen;
		while(*str){
			char* cs = str,*s = (char*)sub;
			while(true){
				if(*cs!=*s){
					if((*cs<='z')&&(*cs>='a')){
						if(*s!=(*s-32)) break;
					}else if((*s<='z')&&(*s>='a')){
						if(*cs!=(*cs-32)) break;
					}
					else break;
				}
				cs++,s++;
				if(*s==0)
					return (uint)(str-wcs);
			}
			str++;
			if(maxlen==0) break;
			maxlen--;
		}
		return -1;
	}
	void MbsReverse(char* str)
	{
		uint len = MbsByteLen(str);
		if(len==0) return;
		Memory<char> buf;
		buf.SetLength(len);
		buf.CopyFrom(str,len);
		for(uint i=0,j=len-1;i<len;i++,j--)
		{
			if(buf[i]<0)
			{
				j--;
				str[j] = buf[i];
				i++;
				str[j+1] = buf[i];
			}
			else
				str[j] = buf[i];
		}
	}
	bool MbsIsUtf8(LPCSTR str,uint len,bool strict){
		int length = cs::MbsByteLen(str,len);
		for (int i=0; i < length; i++){
			byte c = str[i];
			int n;
			if (c < 0x80)
				n = 0; // 0bbbbbbb
			else if ((c & 0xE0) == 0xC0)
				n=1; // 110bbbbb
			else if ((c & 0xF0) == 0xE0)
				n=2; // 1110bbbb
			else if ((c & 0xF8) == 0xF0)
				n=3; // 11110bbb
			else if ((c & 0xFC) == 0xF8)
				n=4; // 111110bb
			else if ((c & 0xFE) == 0xFC)
				n=5; // 1111110b
			else
				return false; // Does not match any model
			for (int j=0; j<n; j++) { 
				i++;
				if(strict&&i == length) return false;
				if ((str[i] & 0xC0) != 0x80)
					return false;
			}
		}
		return true;
	}

	//unicode
	String::String():_Length(0),_sys(0){
		_Handle = (LPWSTR)MemoryAlloc(7*2+2,_sys);
		if(_Handle==0){
#ifdef _DEBUG
			MessageBox(0,L"String(uint) failed",L"String",MB_ICONWARNING);
#endif
			return;
		}
		_Cubage = 7;
		::ZeroMemory(_Handle,_Cubage*2+2);
	}
	String::String(uint capacity,bool sys):_Length(0){
		_sys = sys;
		if(capacity==0){
			_Cubage = 0;
			_Handle = 0;
			return;
		}
		_Handle = (LPWSTR)MemoryAlloc(capacity*2+2,_sys);
		if(_Handle==0){
#ifdef _DEBUG
			MessageBox(0,L"String(uint) failed",L"String",MB_ICONWARNING);
#endif
			return;
		}
		_Cubage = capacity;
		::ZeroMemory(_Handle,_Cubage*2+2);
	}
	String::String(LPCWSTR wstr,bool sys):_Length(0),_Cubage(0){
		_sys = sys;
		*this = wstr;
	}
	String::String(String& wstr,bool sys){
		_sys = sys;
		_Length = wstr.Length();
		_Cubage = _Length;
		if(_Length==0) return;
		_Handle = (LPWSTR)MemoryAlloc(_Cubage*2+2,_sys);
		memcpy(_Handle,wstr.Handle(),_Cubage*2+2);
	}
	String::String(LPCSTR mbs,bool sys):_Length(0),_Cubage(0){
		_sys = sys;
		*this = mbs;
	}
	String::~String()
	{
		if(_Handle)
			MemoryFree(_Handle,_sys);
	}
	bool String::Attach(LPWSTR wstr)
	{
		if(_Handle)
			MemoryFree(_Handle,_sys);
		_Handle = wstr;
		_Length = WcsLength(wstr);
		_Cubage = _Length;
		return 1;
	}
	LPWSTR String::Detach()
	{
		LPWSTR handle = _Handle;
		_Handle = 0;
		_Cubage = 0;
		_Length = 0;
		return handle;
	}
	int String::Format(LPCWSTR fms,...)
	{
		va_list vaList;
		va_start(vaList,fms);
		int bufSize = _vscwprintf(fms,vaList);
		if(bufSize==-1)
		{
			*this = L"";
			va_end(vaList);
			return 0;
		}
		SetCubage(bufSize);
		int n = vswprintf_s(_Handle,bufSize+1,fms,vaList);
		va_end(vaList);
		Realize();
		return n;
	}
	void String::Lower(void)
	{
		WcsLower(_Handle);
	}
	void String::Upper(void)
	{
		WcsUpper(_Handle);
	}
	uint String::CopyTo(wchar_t* wstr,uint start,uint length)
	{
		if((start>=_Length))
			return 0;
		uint max = _Length - start + 2;
		if(max<length) length = max;
		::RtlCopyMemory(wstr,_Handle+start,length*2);
		return length;
	};
	uint String::CopyFrom(LPCWSTR str,uint length,uint offset){
		length = WcsLength(str,length);
		if(length==0) return 0;
		if(offset==-1) offset = _Length;
		uint len = length + offset;
		if(len<length) return 0;
		if(len>_Cubage){
			if(!SetCubage(len)) return 0;
		}
		::RtlMoveMemory(_Handle+offset,str,length*2);
		if((int)_Length<len){
			_Length = len;
			_Handle[_Length] = 0;
		}
		return length;
	}
	bool String::SetCubage(uint capacity,bool dbSpace)
	{
		capacity++;
		if(capacity<=_Length) return 0;
		int size;
		if(dbSpace)
		{
			size = 8;
			while((uint)size<capacity)
			{
				size *= 2;
				if(size<=0) return 0;
			}
		}
		else
			size = capacity;
		size *= 2;
		if(size<=0) return 0;//无法分配超过2G的内存。
		void* handle = MemoryRealloc(_Handle,size,_sys);
		if(handle==0) return 0;
		_Cubage = size/2 - 1;
		_Handle = (LPWSTR)handle;
		_Handle[_Cubage] = 0;//空间被填满时，不必设置结尾的 0 字符。
		_Handle[_Length] = 0;//当原来的空间为 0 时，这一句是必须的。
		return 1;
	}
	void String::Realize(bool resetsize)
	{
		if(!_Handle) return;
		_Length = WcsLength(_Handle);
		if(!resetsize) return;
		_Cubage = _Length;
		_Handle = (LPWSTR)MemoryRealloc(_Handle,_Cubage*2 + 2,_sys);
	}
	uint String::Find(wchar_t ch,uint start)
	{
		for(uint i=start;i<_Length;i++)
		{
			if(_Handle[i]==ch)
				return i;
		}
		return -1;
	};
	uint String::Find(LPCWSTR wstr,uint start)
	{
		if(start>=_Length) return -1;
		uint p = WcsFind(_Handle+start,wstr);
		if(p==-1) return -1;
		return p+start;
	}
	uint String::FindNoCase(wchar_t ch,uint start)
	{
		if((ch>'z')||(ch<'A')||((ch>'Z')&&(ch<'a'))) return Find(ch,start);
		for(uint i=start;i<_Length;i++)
		{
			short ds = _Handle[i] - ch;
			switch(ds)
			{
			case 0:
				return i;
			case -32:
				if((ch<=L'z')&&(ch<=L'a'))
					return i;
			case 32:
				if((_Handle[i]<=L'z')||(_Handle[i]<=L'a'))
					return i;
			}
		}
		return -1;
	};
	uint String::FindNoCase(LPCWSTR wstr,uint start)
	{
		if(!_Handle) return -1;
		uint p = WcsFindNoCase(_Handle+start,wstr);
		if(p==-1) return -1;
		return p+start; 
	}
	bool String::FromMultiByte(LPCSTR str,uint length,DWORD codepage){
		if(codepage==1200||codepage==0){
			uint len = length/2;
			CopyFrom((LPCWSTR)str,len,0);
			_Length = len;
			_Handle[_Length] = 0;
			return true;
		}
		length = MbsByteLen(str,length);
		if((codepage==CP_UTF8||codepage==0)&&length>=3&&(uchar)str[0]==0xEF&&(uchar)str[1]==0xBB&&(uchar)str[2]==0xBF){
			str += 3;
			length -= 3;
			codepage = CP_UTF8;
		}
		if(length==0){
			if(!_Handle) SetCubage(0,1);
			_Handle[0] = 0;
			_Length = 0;
			return 1;
		}
		uint len = ::MultiByteToWideChar(codepage,0,str,length,0,0);
		if(len==0) return 0;
		SetCubage(len);
		len = ::MultiByteToWideChar(codepage,0,str,length,_Handle,len);
		_Handle[len] = 0;
		//MultiByteToWideChar返回的值并不可靠，对于尾部没有转换的字符，它仍然会计算在内，而且0字符不会截断转换，需要重新计算字符长度。
		_Length = WcsLength(_Handle);
		return 1;
	}
	uint String::ToMultiByte(LPSTR buffer,uint length,DWORD codepage){
		if(!_Handle) return 0;
		if(codepage==1200||codepage==0){
			if(buffer==0){
				return _Length*2;
			}
			if(length!=-1) length = length/2;
			uint n = CopyTo((LPWSTR)buffer,0,length);
			return n*2;
		}
		int len = ::WideCharToMultiByte(codepage,0,_Handle,_Length,buffer,length,0,0);
		return len;
	}
	uint String::ToMultiByte(Memory<char>* buffer,DWORD codepage){
		if(!_Handle) return 0;
		if(buffer==0) return 0;
		if(codepage==1200){
			uint len = _Length*2;
			if(len>buffer->Length())
				buffer->SetLength(len);
			uint n = CopyTo((LPWSTR)buffer->Handle(),0,_Length);
			return len;
		}
		uint len = ::WideCharToMultiByte(codepage,0,_Handle,_Length,0,0,0,0);
		if(len==0) return 0;
		if(buffer->Length()<(len+1)){//为了保持末尾有一位null字符的空位。
			if(!buffer->SetLength(len+1)) return 0;
		}
		return ::WideCharToMultiByte(codepage,0,_Handle,_Length,buffer->Handle(),len,0,0);
	}
	void String::operator = (LPCWSTR wstr){
		uint len = WcsLength(wstr);
		if(len>_Cubage) SetCubage(len);
		if(_Handle==0){
			_Handle = (LPWSTR)MemoryAlloc(2,_sys);
		}
		::RtlMoveMemory(_Handle,wstr,len*2);
		_Length = len;
		_Handle[len] = 0;
	}
	void String::operator = (String& str)
	{
		uint len = str._Length;
		if(len==0)
		{
			if(_Length)
			{
				_Length = 0;
				_Handle[0] = 0;
			}
			return;
		}
		if(len>_Cubage)
			SetCubage(len,1);
		_Length = len;
		if(_Handle) RtlCopyMemory(_Handle,str._Handle,len*2+2);
	}
	void String::operator = (LPCSTR str)
	{
		uint len = MbsByteLen(str);
		if(len==0){
			if(_Handle==0) return;
			_Handle[0] = 0;
			_Length = 0;
			return;
		}
		uint buflen = ::MultiByteToWideChar(CP_THREAD_ACP,0,str,len,0,0);
		if(buflen==0) return;
		if(buflen>_Cubage) SetCubage(buflen,1);
		_Length = ::MultiByteToWideChar(CP_THREAD_ACP,0,str,len,_Handle,buflen);
		_Handle[_Length] = 0;
	}
	void String::operator += (String& wstr)
	{
		if(wstr.Length()==0) return;
		if((wstr.Length()+_Length)>_Cubage)
			SetCubage(wstr.Length()+_Length,1);
		RtlCopyMemory(_Handle+_Length,wstr.Handle(),wstr.Length()*2);
		_Length += wstr.Length();
		_Handle[_Length] = 0;
	}
	void String::operator += (LPCWSTR wstr)
	{
		uint len = WcsLength(wstr);
		if(len==0) return;
		if((len+_Length)>_Cubage)
			SetCubage(len+_Length,1);
		RtlCopyMemory(_Handle+_Length,wstr,len*2);
		_Length += len;
		_Handle[_Length] = 0;
	}
	void String::Insert(wchar_t wc,uint pos)
	{
		if(wc==0) return;
		if((1+_Length)>_Cubage) SetCubage(1+_Length,1);
		if(pos>=_Length) pos = _Length;
		else ::RtlMoveMemory(_Handle+pos+1,_Handle+pos,(_Length-pos)*2);
		_Handle[pos] = wc;
		_Length++;
		_Handle[_Length] = 0;
	}
	uint String::Insert(LPCWSTR wstr,uint pos,uint length)
	{
		length = WcsLength(wstr,length);
		if(length==0) return 0;
		if((length+_Length)>_Cubage) SetCubage(length+_Length,1);
		if(pos>=_Length) pos = _Length;
		else ::RtlMoveMemory(_Handle+pos+length,_Handle+pos,(_Length-pos)*2);
		::RtlCopyMemory(_Handle+pos,wstr,length*2);
		_Length += length;
		_Handle[_Length] = 0;
		return length;
	}
	void String::InsertNumber(INT_PTR number,int index)
	{
		String num;
		num.Format(L"%d",number);
		Insert(num,index);
	}
	uint String::Replace(LPCWSTR ws,uint pos,uint length)
	{
		if((uint)pos>(uint)_Length) pos = _Length;
		if(length>(_Length - pos)) length = _Length - pos;
		uint slen = WcsLength(ws);
		if(_Cubage<(_Length-length+slen))
			SetCubage(_Length-length+slen,1);
		if((_Length-pos-length)>0)
			::RtlMoveMemory(_Handle+pos+slen,_Handle+pos+length,(_Length-pos-length)*2);
		::RtlCopyMemory(_Handle+pos,ws,slen*2);
		_Length += slen - length;
		_Handle[_Length] = 0;
		return slen;
	}
	bool String::Tail(int index)
	{
		if(index<0)
		{
			if((_Length+index)<0) return 0;
			::RtlMoveMemory(_Handle,_Handle+(_Length+index),-index*2+2);
			_Length = -index;
		}
		else if(index>0)
		{
			if(index>(int)_Length) return 0;
			::RtlMoveMemory(_Handle,_Handle+index,(_Length-index)*2+2);
			_Length -= index;
		}
		return 1;
	}
	bool String::Head(int index)
	{
		if(index<0)
		{
			if(((int)_Length+index)<0) return 0;
			_Length += index;
			_Handle[_Length] = 0;
		}
		else if(index>0)
		{
			if(index>(int)_Length) return 0;
			_Length = index;
			_Handle[index] = 0;
		}
		return 1;
	}
	void String::FromInt32(int v,int radix,bool sign){
		if(_Cubage<32) SetCubage(32);
		if(sign)
			_ltow_s(v,_Handle,33,radix);
		else
			_ultow_s(v,_Handle,33,radix);
		Realize();
		Upper();
	}
	int String::ToInt32(int radix){
		if(_Handle==0) return 0;
		return wcstoul(_Handle,0,radix);
	}
	bool String::ToInt32(int& v,int radix){
		if(_Handle==0) return 0;
		wchar_t* os = 0;
		v = wcstoul(_Handle,&os,radix);
		return os==0||os[0]==0;
	}
	void String::FromInt64(__int64 v,int radix,bool sign){
		if(_Cubage<64) SetCubage(64);
		if(sign)
			_i64tow_s(v,_Handle,65,radix);
		else
			_ui64tow_s(v,_Handle,65,radix);
		Realize();
		Upper();
	}
	__int64 String::ToInt64(int radix)
	{
		if(_Handle==0) return 0;
		return _wcstoui64(_Handle,0,radix);
	}
	bool String::ToInt64(__int64& v,int radix){
		if(_Handle==0) return 0;
		wchar_t* os = 0;
		v = _wcstoui64(_Handle,&os,radix);
		return os==0||os[0]==0;
	}
	double String::ToFloat()
	{
		if(_Handle==0) return 0;
		return wcstod(_Handle,0);
	}
	bool String::ToFloat(double& v){
		if(_Handle==0) return 0;
		wchar_t* os = 0;
		v = wcstod(_Handle,&os);
		return os==0||os[0]==0;
	}
	void String::HexBinary(const void* data,int len)
	{
		*this = L"";
		if(len<0) return;
		SetCubage(len*2);
		for(int i=0;i<len;i++){
			wsprintf(_Handle+i*2,L"%02X",((uchar*)data)[i]);
		}
		_Handle[len*2] = 0;
		_Length = len*2;
	}
	inline bool isHexChar(wchar_t ch){
		return (((ch<='9')&&(ch>='0'))||((ch<='f')&&(ch>='a'))||((ch<='F')&&(ch>='A')));
	}
	int String::ToBinary(void* data,int bufsize){
		Memory<wchar_t> buf;
		buf.SetLength(_Length+1);
		int j = 0;
		for(uint i=0;i<_Length;i++){
			if(isHexChar(_Handle[i])){
				buf[j] = _Handle[i];
				j++;
			}
		}
		buf[j] = 0;
		wchar_t hex[3];
		hex[2] = 0;
		j = j/2;
		if(data==0) return j;
		if(bufsize>j) bufsize = j;
		for(int i=0;i<bufsize;i++){
			hex[0] = buf[2*i];
			hex[1] = buf[2*i+1];
			((uchar*)data)[i] = (uchar)wcstol(hex,0,16);
		}
		return bufsize;
	}
	int String::ToBinary(Memory<char> &data,int pos){
		Memory<wchar_t> buf;
		buf.SetLength(_Length+1);
		int j = 0;
		for(uint i=0;i<_Length;i++){
			if(isHexChar(_Handle[i])){
				buf[j] = _Handle[i];
				j++;
			}
		}
		buf[j] = 0;
		wchar_t hex[3];
		hex[2] = 0;
		j = j/2;
		if((uint)pos>data.Length()) pos = data.Length();
		if((int)(data.Length()-pos)<j) data.SetLength(j+pos);
		for(int i=0;i<j;i++){
			hex[0] = buf[2*i];
			hex[1] = buf[2*i+1];
			data[i+pos] = (uchar)wcstol(hex,0,16);
		}
		return j;
	}
	void String::Release(){
		MemoryFree(_Handle,_sys);
		_Handle = 0;
		_Length = 0;
		_Cubage = 0;
	}
	static LPCWSTR _spaceList = L" \n\r\t\f\x0b\xa0\u2000\u2001\u2002\u2003\u2004\u2005\u2006\u2007\u2008\u2009\u200a\u200b\u2028\u2029\u3000";
	inline bool _isSpaceChar(wchar_t ch){
		for(int i=0;i<22;i++){
			if(_spaceList[i]==ch) return true;
		}
		return false;
	}
	void String::Trim(){
		TrimRight();
		TrimLeft();
	}
	void String::TrimLeft(){
		for (uint i=0; i < _Length; i++) {
			if(_isSpaceChar(_Handle[i])) continue;
			Tail(i);
			break;
		}
	}
	void String::TrimRight(){
		for (uint i = _Length - 1; i >= 0; i--) {
			if(_isSpaceChar(_Handle[i])) continue;
			Head(i+1);
			break;
		}
	}

	//StringList
	uint StringList::AddSub(LPCWSTR wstr,uint length,uint index)
	{
		if(index>=_Strs.Count()) index = _Strs.Count();
		_Strs.Add(index)->CopyFrom(wstr,length);
		return index;
	}
	bool StringList::Delete(LPCWSTR str,uint start)
	{
		for(uint i=start;i<_Strs.Count();i++)
		{
			if(_Strs[i]==str)
			{
				_Strs.Delete(i);
				return 1;
			}
		}
		return 0;
	}
	uint StringList::SplitString(LPCWSTR wstr,LPCWSTR sp,uint index)
	{
		if(wstr==0||wstr[0]==0) return 0;
		uint slen = WcsLength(sp);
		uint length = WcsLength(wstr);
		if((length<slen)||(slen==0)){
			Add(wstr,index);
			return 1;
		}
		if(index>_Strs.Count()) index = _Strs.Count();
		uint fLen,last = 0;
		uint n = _Strs.Count();
		for(uint i=0;i<=length;){
			if(wmemcmp(wstr+i,sp,slen)==0){
				fLen = i - last;
				_Strs.Add(index)->CopyFrom(wstr+last,fLen);
				index++;
				i += slen;
				last = i;
			}
			else
				i++;
		}
		_Strs.Add(index)->CopyFrom(wstr+last,-1);
		return _Strs.Count() - n;
	}
	uint StringList::AddFromMemory(LPCWSTR wstr,uint length,uint index)
	{
		if(wstr==0) return 0;
		if(index>_Strs.Count()) index = _Strs.Count();
		uint fLen,last = 0;
		uint n = _Strs.Count();
		for(uint i=0;;)
		{
			fLen = WcsLength(wstr+i,length);
			if(fLen==0) break;
			_Strs.Add(index)->CopyFrom(wstr+last,fLen);
			index++;
			fLen++;
			i += fLen;
			if(length<=fLen) break;
			length -= fLen;
			last = i;
		}
		return _Strs.Count() - n;
	}
	uint StringList::Clear(LPCWSTR rs)
	{
		uint n = 0;
		for(int i=(int)_Strs.Count()-1;i>=0;i--)
		{
			if(_Strs[i]==rs)
			{
				_Strs.Delete(i);
				n++;
			}
		}
		return n;
	}
	uint StringList::IndexOf(LPCWSTR str,uint start)
	{
		for(uint i=start;i<_Strs.Count();i++)
		{
			if(_Strs[i]==str)
			{
				return i;
			}
		}
		return -1;
	}
	//StringDiction
	uint StringDiction::AddSub(LPCWSTR wstr,uint length,uint index)
	{
		String tmp;
		tmp.CopyFrom(wstr,length);
		uint i = IndexOf(tmp,0);
		if(i!=-1) return -1;
		if(index>=_Strs.Count()) index = _Strs.Count();
		*_Strs.Add(index) = tmp;
		return index;
	}
	uint StringDiction::SplitString(LPCWSTR wstr,LPCWSTR sp,uint index)
	{
		if(wstr==0) return 0;
		uint slen = wcslen(sp);
		uint length = wcslen(wstr);
		if((length<slen)||(slen==0))
		{
			if(Add(wstr,index)!=-1)
				return 1;
			else
				return 0;
		}
		if(index>_Strs.Count()) index = _Strs.Count();
		uint fLen,last = 0;
		uint n = _Strs.Count();
		for(uint i=0;i<=length;)
		{
			if(wmemcmp(wstr+i,sp,slen)==0)
			{
				fLen = i - last;
				String tmp;
				tmp.CopyFrom(wstr+last,fLen);
				if(IndexOf(tmp,0)==-1)
				{
					_Strs.Add(index)->CopyFrom(wstr+last,fLen);
					index++;
				}
				i += slen;
				last = i;
			}
			else
				i++;
		}
		if(IndexOf(wstr+last,0)==-1)
		{
			_Strs.Add(index)->CopyFrom(wstr+last,-1);
		}
		return _Strs.Count() - n;
	}
	uint StringList::SplitStandString(LPCWSTR wstr,LPCWSTR sp,uint index)
	{
		uint slen = WcsLength(sp);
		if(slen==0) return 0;
		uint length = WcsLength(wstr);
		if(length==0) return 0;
		if(index>_Strs.Count()) index = _Strs.Count();
		uint fLen,last = 0;
		uint n = _Strs.Count();
		for(uint i=0;i<=length;)
		{
			if(wmemcmp(wstr+i,sp,slen)==0)
			{
				fLen = i - last;
				_Strs.Add(index)->CopyFrom(wstr+last,fLen);
				index++;
				i += slen;
				last = i;
			}
			else
				i++;
		}
		return _Strs.Count() - n;
	}
	uint StringDiction::AddFromMemory(LPCWSTR wstr,uint length,uint index)
	{
		if(wstr==0) return 0;
		if(index>_Strs.Count()) index = _Strs.Count();
		uint fLen,last = 0;
		uint n = _Strs.Count();
		for(uint i=0;;)
		{
			fLen = WcsLength(wstr+i,length);
			if(fLen==0) break;
			if(IndexOf(wstr+last,0)==-1)
			{
				_Strs.Add(index)->CopyFrom(wstr+last,fLen);
				index++;
			}
			fLen++;
			i += fLen;
			if(length<=fLen) break;
			length -= fLen;
			last = i;
		}
		return _Strs.Count() - n;
	}
	//StringMemList
	StringMemList::StringMemList():_Length(0),_Size(8)
	{
		_Handle = (LPWSTR)MemoryAlloc(_Size*2,1);
	}
	StringMemList::StringMemList(uint size):_Length(0)
	{
		_Handle = (LPWSTR)MemoryAlloc(size*2,1);
		if(_Handle!=0)
			_Size = size*2;
	}
	StringMemList::~StringMemList()
	{
		if(_Handle!=0)
			MemoryFree(_Handle,1);
	}
	void StringMemList::Clear()
	{
		_Length = 0;
		_Strs.Clear();
	}
	bool StringMemList::SetSize(uint len)
	{
		if(len<_Length)
			return 0;
		if(len<1)
			len = 1;
		void* handle = MemoryRealloc(_Handle,len*2,1);
		if(handle==NULL)
			return 0;
		if(handle!=(void*)_Handle)
		{
			uint offset = (DWORD)(INT_PTR)handle - (DWORD)(INT_PTR)_Handle;
			offset /= 2;//must suppose offset is always a even number
			for(uint i=0;i<_Strs.Count();i++)
				_Strs[i] += offset;//这个调用会改变_Strs内部元素的值.
		}
		_Handle = (LPWSTR)handle;
		_Size = len;
		return 1;
	}
	void StringMemList::Realize()
	{
		SetSize(_Length);
	}
	uint StringMemList::SplitString(LPCWSTR wstr,LPCWSTR sp,uint index)
	{
		if(wstr==0) return 0;
		uint len = (uint)wcslen(wstr);
		uint slen = (uint)wcslen(sp);
		if((slen==0)||(slen>len))
		{
			Add(wstr,index);
			return 1;
		}
		Memory<wchar_t> buffer;
		buffer.SetLength(len+1);
		LPWSTR pb = buffer.Handle();
		uint length = len - slen,last = 0,clen;
		LPWSTR ps = (LPWSTR)wstr;
		IntegerList<uint> strs;
		for(uint i=0;i<=length;)
		{
			if(memcmp(wstr+i,sp,slen*2)==0)
			{
				clen = i - last;
				memcpy(pb,ps,clen*2);
				i += slen;
				strs.Add(pb-buffer.Handle());
				last = i;
				pb[clen] = 0;
				pb += clen + 1;
				ps = (LPWSTR)wstr + last;
			}
			else
				i++;
		}
		clen = (uint)wcslen(ps);
		memcpy(pb,ps,clen*2);
		strs.Add(pb-buffer.Handle());
		pb[clen] = 0;
		pb += clen + 1;
		length = pb -  buffer.Handle();//所需擴展
		SetSize(length+_Length);
		if(index<_Strs.Count())
		{
			ps = _Strs[index];//
			MoveMemory(ps+length,ps,(_Length-(ps-_Handle))*2);
			for(uint i=index;i<_Strs.Count();i++)
			{
				_Strs[i] += length;
			}
		}
		else
			ps = _Handle + _Length;
		memcpy(ps,buffer.Handle(),length*2);
		_Length += length;
		index = _Strs.Expand(strs.Count(),index);
		for(uint i=0;i<strs.Count();i++)
			_Strs[i+index] = ps + strs[i];
		return strs.Count();
	}
	void StringMemList::Release()
	{
		MemoryFree(_Handle,1);
		_Strs.Clear();
		_Length = 0;
		_Handle = 0;
	}
	uint StringMemList::Clear(LPCWSTR rs)
	{
		uint count = 0;
		for(uint i=0;i<_Strs.Count();i++)
		{
			if(WcsCompare(_Strs[i],rs)==0)
			{
				_Strs.Delete(i);
				count++;
			}
		}
		return count;
	}
	uint StringMemList::AddFromMemory(LPCWSTR handle,uint length,uint index)
	{
		if(handle==NULL) return 0;
		if(length==0) return 0;
		if(index>_Strs.Count()) index = _Strs.Count();
		IntegerList<uint> strs;
		uint offset = 0;
		while(offset<length)
		{
			uint slen = (uint)wcslen(handle+offset);
			if((length==-1)&&(slen==0)) break;//length=-1时表示内存截止到双NULL字符。
			strs.Add(offset);
			offset += slen + 1;
			if(offset>=length)
			{
				offset = length + 1;
				break;
			}
		}
		length = offset;
		if((length+_Length)<length) return 0;//防止长度太大，其实这种情况，早就内存溢出了。
		if(_Size<(length+_Length))
		{
			if(!SetSize(length+_Length)) return 0;
		}
		LPWSTR insert;
		if(index<_Strs.Count())
		{
			int ml = SubHandle(_Handle+_Length,_Strs[index]);
			insert = (LPWSTR)_Strs[index];
			::MoveMemory((void*)(insert+length),insert,ml);
			for(uint i=index;i<_Strs.Count();i++)
				_Strs[i] += length;
		}
		else
		{
			insert = _Handle + _Length;
		}
		::MoveMemory(insert,handle,length*2);
		insert[length-1] = 0;
		_Length += length;
		_Strs.Expand(strs.Count(),index);
		for(uint i=0;i<strs.Count();i++)
			_Strs[i+index] = strs[i] + insert;
		return strs.Count();
	}
	bool StringMemList::AddSub(LPCWSTR str,uint len,uint index){
		if(str==0) return 0;
		len = WcsLength(str,len);
		if((index<0)||(index>_Strs.Count()))
			index = _Strs.Count();
		if(str==0)
			str = L"";
		len += 1;
		if(_Size<(len+_Length)){
			if(!SetSize((len+_Length)*2)) return 0;
		}
		LPWSTR newStr;
		if(index<_Strs.Count()){
			DWORD ml = (DWORD)(INT_PTR)&_Handle[_Length] - (DWORD)(INT_PTR)_Strs[index];
			newStr = (LPWSTR)_Strs[index];
			::MoveMemory((void*)(newStr+len),newStr,ml);
			for(uint i=index;i<_Strs.Count();i++)
				_Strs[i] += len;
		}
		else
			newStr = _Handle + _Length;
		RtlMoveMemory(newStr,str,len*2);
		newStr[len-1] = 0;
		_Strs.Add(newStr,index);
		_Length += len;
		return 1;
	}
	uint StringMemList::IndexOf(LPCWSTR wstr,uint start)
	{
		for(uint i=start;i<_Strs.Count();i++)
		{
			if(wcscmp(_Strs[i],wstr)==0)
				return i;
		}
		return -1;
	}
	bool StringMemList::Delete(LPCWSTR wstr,uint start)
	{
		return Delete(IndexOf(wstr,start))!=0;
	}
	uint StringMemList::Delete(uint index,uint count)
	{
		if((index<0)||(index>=_Strs.Count()))
			return 0;
		if(count<0)
			count = _Strs.Count() - index;
		int end = index + count;
		if(end>(int)_Strs.Count())
			end = _Strs.Count();
		DWORD len = 0;
		for(int i=index;i<end;i++)
			len += (DWORD)wcslen(_Strs[i]) + 1;
		if(end<(int)_Strs.Count())
		{
			DWORD ml = (DWORD)(INT_PTR)(_Handle+_Length) - (DWORD)(INT_PTR)_Strs[end];
			::MoveMemory((void*)_Strs[index],_Strs[end],ml);
		}
		_Strs.Delete(index,count);
		end = _Strs.Count() - end;
		for(int i=index;i<end;i++)
			_Strs[i] -= len;
		_Length -= len;
		return len;
	}
	//unicode diction
	StringMemDiction::StringMemDiction(void)
	{
	}
	StringMemDiction::~StringMemDiction(void)
	{
	}
	bool StringMemDiction::AddFromMemory(LPCWSTR handle,uint length,uint index)
	{
		StringMemList _Strs;
		if(_Strs.AddFromMemory(handle,length)==0)
			return 0;
		if((index<0)||(index>_Strs.Count()))
			index = _Strs.Count();
		for(uint i=0;i<_Strs.Count();i++)
		{
			uint ins = index + i;
			Add(_Strs[i],&ins);
		}
		return 1;
	}
	bool StringMemDiction::Add(LPCWSTR wstr,uint* index)
	{
		for(uint i=0;i<_Strs.Count();i++)
		{
			if(!wcscmp(_Strs[i],wstr))
			{
				if(index) *index = i;
				return 0;
			}
		}
		if(index)
		{
			if(!StringMemList::Add(wstr,*index)) return 0;
			if((uint)(*index)>=(uint)_Strs.Count()) *index = _Strs.Count() - 1;
			return 1;
		}
		else
		{
			return StringMemList::Add(wstr);
		}
	}
}