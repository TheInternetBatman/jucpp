#include "../stdafx.h"
#include "../../concise.h"
#include "../zlib/zlib.h"

#define CHUNK 8096

namespace cs{
#define _strm (*(z_stream*)strm)
	Ungzip::Ungzip():strm(0){}
	void Ungzip::Create(){
		if(!strm)
			strm = new z_stream; 
		ZeroMemory(&_strm,sizeof(z_stream));
		inflateEnd(&_strm);
		ZeroMemory(&_strm,sizeof(z_stream));
		inflateInit2(&_strm,47);
	}
	bool Ungzip::Input(char* data,int len){
		if(data==0||len==0||strm==0) return false;
		_strm.avail_in = len;  
		_strm.next_in = (Bytef*)data;  
		return true;
	}
	int Ungzip::Output(char* buf,int& len){
		if(buf==0||strm==0){
			len = 0;
			return 0;
		}
		_strm.avail_out = len;  
		_strm.next_out = (Bytef*)buf;  
		int ret = inflate(&_strm,Z_NO_FLUSH);  
		if(ret<0){
			inflateEnd(&_strm);
			len = 0;
			return -1;
		}
		len -= _strm.avail_out;
		if(ret>0){
			inflateEnd(&_strm);
			return 0;
		}
		if(_strm.avail_in>0) return 1;
		else return 0;
	}
	int Ungzip::Output(Memory<char>& buf){
		int totalsize = 0;
		do{  
			if((int)buf.Length()<(CHUNK+totalsize)) buf.SetLength(CHUNK+totalsize);
			_strm.avail_out = CHUNK;  
			_strm.next_out = (Bytef*)(buf.Handle()+totalsize);  
			int ret = inflate(&_strm,Z_FULL_FLUSH);  
			if(ret<0){
				inflateEnd(&_strm);
				break;
			}
			totalsize += CHUNK - _strm.avail_out;
			if(ret>0){
				inflateEnd(&_strm);
				break;
			}
		}while(_strm.avail_in); 
		return totalsize;
	}
	void Ungzip::Close(){
		if(strm){
			inflateEnd(&_strm);
			delete (z_stream*)strm;
			strm = 0;
		}
	}
	Ungzip::~Ungzip(){
		Close();
	}
	Gzip::Gzip():strm(0){
	}
	void Gzip::Create(int level){
		if(!strm)
			strm = new z_stream; 
		ZeroMemory(&_strm,sizeof(z_stream));
		deflateEnd(&_strm);
		ZeroMemory(&_strm,sizeof(z_stream));
		deflateInit(&_strm,level);
	}
	bool Gzip::Input(char* data,int len){
		if(strm==0||data==0||len==0) return false;
		_strm.avail_in = len;  
		_strm.next_in = (Bytef*)data;  
		return true;
	}
	int Gzip::Output(char* buf,int& len){
		if(strm==0||buf==0){
			len = 0;
			return 0;
		}
		_strm.avail_out = len;  
		_strm.next_out = (Bytef*)buf;  
		int ret = deflate(&_strm,Z_FULL_FLUSH);  
		if(ret<0){
			deflateEnd(&_strm);
			len = 0;
			return -1;
		}
		len -= _strm.avail_out;
		if(ret>0){
			deflateEnd(&_strm);
			return 0;
		}
		if(_strm.avail_in>0) return 1;
		else return 0;
	}
	int Gzip::Output(Memory<char>& buf){
		int totalsize = 0;
		do{  
			if((int)buf.Length()<(CHUNK+totalsize)) buf.SetLength(CHUNK+totalsize);
			_strm.avail_out = CHUNK;  
			_strm.next_out = (Bytef*)(buf.Handle()+totalsize);  
			int ret = deflate(&_strm,Z_FULL_FLUSH);  
			if(ret<0){
				deflateEnd(&_strm);
				break;
			}
			totalsize += CHUNK - _strm.avail_out;  
			if(ret>0){
				deflateEnd(&_strm);
				break;
			}
		}while(_strm.avail_in); 
		return totalsize;
	}
	void Gzip::Close(){
		if(!strm) return;
		deflateEnd(&_strm);
		strm = 0;
	}
	Gzip::~Gzip(){
		Close();
	}
	uint GetMaxCompressLen(uint len)
	{
		return compressBound(len);
	}
	uint Compress(const char* data,uint length,char* zData,uint bufLen)
	{
		uLongf destLen,sourceLen;
		destLen = bufLen;
		sourceLen = length;
		if(compress((Bytef*)zData,&destLen,(Bytef*)data,sourceLen)) return 0;
		return destLen;
	}
	uint DeCompress(const char* zData,uint length,char* data,uint bufLen)
	{
		uLongf destLen,sourceLen;
		destLen = bufLen;
		sourceLen = length;
		if(uncompress((Bytef*)data,&destLen,(Bytef*)zData,sourceLen)) return 0;
		return destLen;
	}
	uint Compress(const char* data,uint dlen,Memory<char>* zData)
	{
		zData->SetLength(compressBound(dlen));
		uint len = Compress(data,dlen,zData->Handle(),zData->Length());
		if(len==0) return 0;
		zData->SetLength(len);
		return len;
	}
	uint DeCompress(const char* zData,uint dlen,Memory<char>* data)
	{
		data->SetLength(3*dlen);
		while(1)
		{
			uLongf destLen = data->Length();
			uint r = uncompress((Bytef*)data->Handle(),&destLen,(Bytef*)zData,(uLongf)dlen);
			if(r==Z_OK)
			{
				data->SetLength(destLen);
				return destLen;
			}
			else if(r==Z_BUF_ERROR) data->Double();
			else return 0;
		}
	}
}