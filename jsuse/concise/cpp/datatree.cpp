#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	//把对象存储在内存中，mem：要存储的内存，offset：存储在内存中的位置，返回使用的内存大小
	int DataTree::ToMemory(Memory<uchar>& mem,int offset)
	{
		uint len = Name.Length() + Value.Length() + 6;
		len += offset;
		while(mem.Length()<len)
			mem.Double();
		WORD nLen = Name.Length();
		mem.CopyFrom((uchar*)&nLen,2,offset);
		offset += 2;
		mem.CopyFrom(Name.Handle(),Name.Length(),offset);
		offset += Name.Length();
		nLen = Value.Length();
		mem.CopyFrom((uchar*)&nLen,2,offset);
		offset += 2;
		mem.CopyFrom(Value.Handle(),Value.Length(),offset);
		offset += Value.Length();
		nLen = (WORD)_childs.Count();
		mem.CopyFrom((uchar*)&nLen,2,offset);
		offset += 2;
		for(uint i=0;i<_childs.Count();i++)
		{
			offset = _childs[i].ToMemory(mem,offset);
		}
		return offset;
	}
	//从一段内存加载，len是内存的大小。返回值是正确解析的长度，如果数据正确，它应该返回len。
	int DataTree::FromMemory(uchar* mem,int len)
	{
		if(mem==0) return 0;
		if(len<6) return 0;
		WORD nLen = *(WORD*)mem;
		if(nLen>(0xffff-6)) return 0;
		if((nLen+4)>(WORD)len) return 0;
		WORD vLen = *(WORD*)(mem+nLen+2);
		if(vLen>(0xfffb-nLen-6)) return 0;
		if((nLen+vLen+6)>(WORD)len) return 0;
		if(nLen&&(!Name.SetLength(nLen))) return 0;
		if(vLen&&(!Value.SetLength(vLen))) return 0;
		Name.CopyFrom(mem+2,nLen);
		Value.CopyFrom(mem+4+nLen,vLen);
		WORD count = *(WORD*)(mem+nLen+vLen+4);
		uint offset = nLen + vLen + 6;
		_childs.Clear();
		for(uint i=0;i<count;i++)
		{
			DataTree* pdt = _childs.Add();
			pdt->_parent = this;
			uint pLen = pdt->FromMemory(mem+offset,len-offset);
			if(pLen==0) break;
			offset += pLen;
		}
		return offset;
	}
	bool DataTree::Save(LPCWSTR name)
	{
		File f;
		if(!f.Create(name,CREATE_ALWAYS)) return 0;
		Memory<uchar> mem;
		int len = ToMemory(mem);
		return f.Write(mem.Handle(),len)==len;
	}
	bool DataTree::Load(LPCWSTR name,bool always)
	{
		File f;
		DWORD OP;
		if(always) 
			OP = OPEN_ALWAYS;
		else
			OP = OPEN_EXISTING;
		if(!f.Create(name,OP)) return 0;
		Memory<uchar> mem;
		uint len = f.Read(&mem);
		FromMemory(mem.Handle(),len);
		return 1;
	}
	DataTree* DataTree::OpenNode(void* name,int len,bool always)
	{
		for(uint i=0;i<_childs.Count();i++)
		{
			DataTree& dt = _childs[i];
			if(dt.Name.Length()==len)
			{
				if(0==memcmp(dt.Name.Handle(),name,len))
				{
					return &dt;
				}
			}
		}
		if(!always) return 0;
		DataTree* pdt = _childs.Add();
		pdt->Name.SetLength(len);
		pdt->Name.CopyFrom((uchar*)name,len);
		return pdt;
	}
	bool DataTree::SetValue(void* data,int len)
	{
		if(!Value.SetLength(len)) return 0;
		Value.CopyFrom((uchar*)data,len);
		return 1;
	}
	bool DataTree::SetName(void* name,int len)
	{
		if(!Name.SetLength(len)) return 0;
		Name.CopyFrom((uchar*)name,len);
		return 1;
	}

	DataTreeFile::DataTreeFile(){
		SetFileName(0);
	}
	bool DataTreeFile::Load(){
		return DataTree::Load(fileName);
	}
	bool DataTreeFile::Save(){
		return DataTree::Save(fileName);
	}
	void DataTreeFile::SetFileName(LPCWSTR name){
		if(WcsLength(name)==0){
			String fp;
			GetCurrentExeFileName(fp);
			fp.Head(-4);
			FPLinkExt(fp,L"dtf");
			fileName = fp;
		}else{
			fileName = name;
		}
	}
}