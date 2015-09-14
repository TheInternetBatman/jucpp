#include "stdafx.h"
#include "base.h"
namespace base{
	void initKeyDiction(int* _dic,LPCWSTR Key){
		::ZeroMemory(_dic,128*4);
		for(uint i=0;i<64;i++){
			_dic[Key[i]] = i;
		}
	}
	Code64::Code64(){
		Key = L"PaAwO65goUf7IK2vi9-xq8cFTEXLCDY1Hd3tV0ryzjbpN_BlnSs4mGRkQWMZJeuh";
		initKeyDiction(_dic,Key);
	}
	uint Code64::Encode(byte* a,uint len){
		uint sLen = (len/3)*4;
		uint res = len % 3; //3个字节一组进行处理, 余下特殊处理
		if(res==1) sLen += 2;
		else if(res==2) sLen += 3;
		if(Code.Cubage()<sLen) Code.SetCubage(sLen);
		Code[sLen] = 0;
		uint i = 2, j = 0, v;
		for (; i < len; i += 3,j += 4) {//每3个字节用4个字符表示, 相当于3个字符(实际上是6个字节)用8个字符编码(实际为16个字节), 看起来容量膨胀了很多, 但是在启用压缩的情况下, 这些又被抵消掉了.
			v = a[i - 2] + (a[i - 1] << 8) + (a[i] << 16);
			Code[j] = Key[v & 0x3f];
			Code[j+1] = Key[(v >> 6) & 0x3f];
			Code[j+2] = Key[(v >> 12) & 0x3f];
			Code[j+3] = Key[(v >> 18)];
		}
		if (res == 1) {//字节余一位时候, 补2个字符, 64*64>256
			v = a[i - 2];
			Code[j] = Key[v & 0x3f];
			Code[j+1] = Key[(v >> 6) & 0x3f];
		} else if (res == 2) {//字节余2位的时候, 补3个字节, 64*64*64>256*256, 所以是可行的.
			v = a[i - 2] + (a[i - 1] << 8);
			Code[j] = Key[v & 0x3f];
			Code[j+1] = Key[(v >> 6) & 0x3f];
			Code[j+2] = Key[(v >> 12) & 0x3f];
		}
		Code.Realize();
		return sLen;
	}
	uint Code64::GetCodeLen(){
		uint sLen = Code.Length();
		for(uint i=0;i<Code.Length();i++){
			if(Code[i]>=128||_dic[Code[i]]==0){
				sLen = i;
				Code[i] = 0;
				Code.Realize();
				break;
			}
		}
		return sLen;
	}
	uint GetMemLen(uint codeLen){
		uint res = codeLen % 4;
		uint len = (codeLen/4)*3;
		if(res==2) len += 1;
		else if(res==3) len += 2;
		return len;
	}
	void Code64::Decode(byte* mem){
		uint codeLen = Code.Length();
		uint res = codeLen % 4;
		uint i = 3, j = 0, v;
		for (; i < codeLen; i += 4,j += 3) {
			v = _dic[Code[i - 3]];
			v += _dic[Code[i - 2]] << 6;
			v += _dic[Code[i - 1]] << 12;
			v += _dic[Code[i]] << 18;
			mem[j] = v & 0xff;
			mem[j+1] = (v >> 8) & 0xff;
			mem[j+2] = (v >> 16) & 0xff;
		}
		if (res == 2) {//正确的字节数肯定是余2或3, 没有1的情况, 如果出现, 舍弃.
			v = _dic[Code[i - 3]];
			v += _dic[Code[i - 2]] << 6;
			mem[j] = v & 0xff;
		} else if (res == 3) {
			v = _dic[Code[i - 3]];
			v += _dic[Code[i - 2]] << 6;
			v += _dic[Code[i - 1]] << 12;
			mem[j] = v & 0xff;
			mem[j+1] = (v >> 8) & 0xff;
		}
	}
}