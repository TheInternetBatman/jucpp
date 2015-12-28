#include "../stdafx.h"
#include "../../jubase.h"

namespace ju{
	//Json目前没有给出如果源字串有错误时候的错误位置，可以用一个静态变量来记录这个位置
	//这样，既不会增加正确解析的运算量，也不会增加Json本身的资源。
	inline void descapSpecialChar(wchar_t ch,wchar_t& ret){
		if(ch=='b'){
			ret = '\b';
		}else if(ch=='r'){
			ret = '\r';
		}else if(ch=='n'){
			ret = '\n';
		}else if(ch=='/'){
			ret = '/';
		}else if(ch=='\"'){
			ret = '\"';
		}else if(ch=='f'){
			ret = '\f';
		}else if(ch=='\\'){
			ret = '\\';
		}else if(ch=='t'){
			ret = '\t';
		}
	}
	inline bool escapSpecialChar(wchar_t ch,wchar_t& ret,bool readStyle){
		if(readStyle){
			if(ch=='\b'){
				ret = 'b';
				return true;
			}else if(ch=='\r'){
				ret = 'r';
				return true;
			}else if(ch=='\n'){
				ret = 'n';
				return true;
			}else if(ch=='\"'){
				ret = '\"';
				return true;
			}else if(ch=='\f'){
				ret = 'f';
				return true;
			}else if(ch=='\\'){
				ret = '\\';
				return true;
			}else if(ch=='\t'){
				ret = 't';
				return true;
			}
		}else{
			if(ch=='\b'){
				ret = 'b';
				return true;
			}else if(ch=='\r'){
				ret = 'r';
				return true;
			}else if(ch=='\n'){
				ret = 'n';
				return true;
			}else if(ch=='/'){
				ret = '/';
				return true;
			}else if(ch=='\"'){
				ret = '\"';
				return true;
			}else if(ch=='\f'){
				ret = 'f';
				return true;
			}else if(ch=='\\'){
				ret = '\\';
				return true;
			}else if(ch=='\t'){
				ret = 't';
				return true;
			}
		}
		return false;
	}
	void addSlash(LPCWSTR str,int len,String& slashStr,bool readStyle){
		String buf;
		slashStr = L"";
		slashStr.SetCubage(len+len/10+12);//set a size may enough。
		wchar_t ret;
		for(int i=0;i<len;i++){
			if(str[i]>256&&!readStyle){
				slashStr += L"\\u";
				buf.Format(L"%04x",str[i]);
				slashStr += buf;
			}else if(escapSpecialChar(str[i],ret,readStyle)){
				slashStr.Insert('\\');
				slashStr.Insert(ret);
			}else{
				slashStr.Insert(str[i]);
			}
		}
	}
	bool delSlash(String& str){
		String buf;
		wchar_t ret;
		int offset = 0;
		int len = str.Length();
		for(int i=0;i<len;i++){
			if(str[i]!='\\'){
				str[offset] = str[i];
				offset++;
				continue;
			}
			if(str[i+1]=='u'){
				if(i+6>len) return false;
				buf.CopyFrom(str+i+2,4,0);
				int ch = 0;
				if(!buf.ToInt32(ch,16)) return false;
				str[offset] = (wchar_t)ch;
				offset++;
				i += 5;
			}else{
				ret = 0;
				descapSpecialChar(str[i+1],ret);
				if(ret==0) return false;
				str[offset] = ret;
				offset++;
				i += 1;
			}
		}
		str[offset] = 0;
		str.Realize();
		return true;
	}
	inline bool _isSpaceChar(wchar_t ch){
		return ch==' '||ch=='\t'||ch=='\r'||ch=='\n'||ch=='\f';
	}
	int _parseString(LPCWSTR jstr,int pos,int maxlen,String* ps){
		for(int i=pos;i<maxlen;i++){
			bool isSlash = false;
			while(jstr[i]=='\\'){
				i++;
				isSlash = !isSlash;
				continue;
			}
			if(jstr[i]=='\"'){
				if(!isSlash){
					(*ps) = (LPCWSTR)0;
					ps->CopyFrom(jstr+pos,i-pos,0);
					delSlash(*ps);
					return i+1;
				}
			}
		}
		return -1;
	}
	//返回值位置是一个 space 字符，或者结尾 null 字符。
	int _getNextWord(LPCWSTR jstr,int pos,int maxlen){
		for(int i=pos;i<maxlen;i++){
			if(_isSpaceChar(jstr[i])||jstr[i]==','||jstr[i]=='}'||jstr[i]==']'){
				return i;
			}
		}
		return maxlen;
	}
	bool _isTrue(LPCWSTR jstr,int pos1,int pos2){
		if(pos2-pos1!=4) return false;
		return 0==memcmp(jstr+pos1,L"true",8);
	}
	bool _isFalse(LPCWSTR jstr,int pos1,int pos2){
		if(pos2-pos1!=5) return false;
		return 0==memcmp(jstr+pos1,L"false",10);
	}
	bool _isNull(LPCWSTR jstr,int pos1,int pos2){
		if(pos2-pos1!=4) return false;
		return 0==memcmp(jstr+pos1,L"null",8);
	}
	bool _isDouble(LPCWSTR jstr,int pos1,int pos2,double& v){
		String ds;
		ds.CopyFrom(jstr+pos1,pos2-pos1,0);
		return ds.ToFloat(v);
	}
	bool _isInteger(LPCWSTR jstr,int pos1,int pos2,__int64& v){
		String ds;
		ds.CopyFrom(jstr+pos1,pos2-pos1);
		return ds.ToInt64(v,10);
	}
	int _parseArray(LPCWSTR jstr,int pos,int maxlen,ObjectList<Json>* ja);
	int _parseObject(LPCWSTR jstr,int pos,int maxlen,ObjectList<Json::DICTION>* jo);
	int _parseBinary(LPCWSTR jstr,int pos,int maxlen,Memory<char>* mbin){
		String bin;
		for(int i=pos;i<maxlen;i++){
			if(jstr[i]=='\''){
				bin.CopyFrom(jstr+pos,i-pos);
				Memory<char> buf;
				int len = bin.ToMultiByte(buf);
				len = Base64::Decode(*mbin,buf.Handle(),len);
				mbin->SetLength(len);
				return i+1;
			}
		}
		return -1;
	}
	int _parseAll(LPCWSTR jstr,int pos,int maxlen,Json* jv){
		while(_isSpaceChar(jstr[pos])){
			pos++;
			if(pos==maxlen) return -1;
		}
		if(jstr[pos]=='\"'){
			jv->_type = json_string;
			jv->_strValue = new String;
			pos = _parseString(jstr,pos+1,maxlen,jv->_strValue);
		}else if(jstr[pos]=='\''){
			jv->_type = json_binary;
			jv->_binaryValue = new Memory<char>;
			pos = _parseBinary(jstr,pos+1,maxlen,jv->_binaryValue);
		}else if(jstr[pos]=='['){
			jv->_type = json_array;
			jv->_arrayValue = new ObjectList<Json>;
			jv->_arrayValue->SetGrow(1);
			pos = _parseArray(jstr,pos+1,maxlen,jv->_arrayValue);
		}else if(jstr[pos]=='{'){
			jv->_type = json_object;
			jv->_objectValue = new ObjectList<Json::DICTION>;
			jv->_objectValue->SetGrow(1);
			pos = _parseObject(jstr,pos+1,maxlen,jv->_objectValue);
		}else{
			int nextPos = _getNextWord(jstr,pos,maxlen);
			if(_isTrue(jstr,pos,nextPos)){
				jv->_type = json_boolean;
				jv->_boolValue = true;
			}else if(_isFalse(jstr,pos,nextPos)){
				jv->_type = json_boolean;
				jv->_boolValue = false;
			}else if(_isNull(jstr,pos,nextPos)){
				//jv本来就是 null 。
			}else if(_isInteger(jstr,pos,nextPos,jv->_intValue)){
				jv->_type = json_integer;
			}else if(_isDouble(jstr,pos,nextPos,jv->_doubleValue)){
				jv->_type = json_double;
			}else return -1;
			pos = nextPos;
		}
		return pos;
	}
	int _parseArray(LPCWSTR jstr,int pos,int maxlen,ObjectList<Json>* ja){
		while(_isSpaceChar(jstr[pos])){
			pos++;
			if(pos>=maxlen) return -1;
		}
		if(jstr[pos]==']'){
			pos++;
			return pos;
		}
		while(true){
			while(_isSpaceChar(jstr[pos])){
				pos++;
				if(pos>=maxlen) return -1;
			}
			Json* jv = new Json;
			pos = _parseAll(jstr,pos,maxlen,jv);
			if(pos==-1){
				delete jv;
				return -1;
			}else{
				ja->AddIn(jv);
			}
			while(_isSpaceChar(jstr[pos])){
				pos++;
				if(pos>=maxlen) return -1;
			}
			if(jstr[pos]==','){
				pos++;
				if(pos>=maxlen) return -1;
			}else if(jstr[pos]==']'){
				pos++;
				return pos;
			}
		}
	}
	int _parsePropety(LPCWSTR jstr,int pos,int maxlen,Json::DICTION* dic){
		pos = _parseString(jstr,pos,maxlen,&dic->key);
		if(pos==-1) return -1;
		while(_isSpaceChar(jstr[pos])){
			pos++;
			if(pos==maxlen) return -1;
		}
		if(jstr[pos]!=':') return -1;
		pos++;
		return _parseAll(jstr,pos,maxlen,dic->val);
	}
	int _parseObject(LPCWSTR jstr,int pos,int maxlen,ObjectList<Json::DICTION>* jo){
		while(_isSpaceChar(jstr[pos])){
			pos++;
			if(pos==maxlen) return -1;
		}
		if(jstr[pos]=='}'){
			pos++;
			return pos;
		}
		while(true){
			Json::DICTION* dic;
			while(_isSpaceChar(jstr[pos])){
				pos++;
				if(pos==maxlen) return -1;
			}
			if(jstr[pos]=='\"'||jstr[pos]=='\''){
				dic = jo->Add();
				pos = _parsePropety(jstr,pos+1,maxlen,dic);
			}else return -1;
			if(pos==-1) return -1;
			while(_isSpaceChar(jstr[pos])){
				pos++;
				if(pos==maxlen) return -1;
			}
			if(jstr[pos]==','){
				pos++;
				if(pos>=maxlen) return -1;
				continue;
			}else if(jstr[pos]=='}'){
				pos++;
				return pos;
			}
		}
	}
	int _parse(LPCWSTR jstr,int pos,int maxlen,Json* jv){
		int i = pos;
		for(;(uint)i<(uint)maxlen;i++){
			if(_isSpaceChar(jstr[i])){
				continue;
			}else if(jstr[i]=='{'){
				jv->_type = json_object;
				jv->_objectValue = new ObjectList<Json::DICTION>;
				jv->_objectValue->SetGrow(1);
				i = _parseObject(jstr,i+1,maxlen,jv->_objectValue);
			}else if(jstr[i]=='['){
				jv->_type = json_array;
				jv->_arrayValue = new ObjectList<Json>;
				jv->_arrayValue->SetGrow(1);
				i = _parseArray(jstr,i+1,maxlen,jv->_arrayValue);
			}else if(jstr[i]=='\"'){
				jv->_type = json_string;
				jv->_strValue = new String;
				i = _parseString(jstr,i+1,maxlen,jv->_strValue);
			}else if(jstr[i]=='\''){
				jv->_type = json_binary;
				jv->_binaryValue = new Memory<char>;
				i = _parseBinary(jstr,i+1,maxlen,jv->_binaryValue);
			}else{
				int nextPos = _getNextWord(jstr,i,maxlen);
				if(_isTrue(jstr,i,nextPos)){
					jv->_type = json_boolean;
					jv->_boolValue = true;
				}else if(_isFalse(jstr,i,nextPos)){
					jv->_type = json_boolean;
					jv->_boolValue = false;
				}else if(_isNull(jstr,i,nextPos)){
				}else if(_isDouble(jstr,i,nextPos,jv->_doubleValue)){
					jv->_type = json_double;
				}else if(_isInteger(jstr,i,nextPos,jv->_intValue)){
					jv->_type = json_integer;
				}else
					return -1;
				i = nextPos;
			}
			break;
		}
		return i;
	}
	void _toString(Json* jsn,String& str,String& tab,bool readStyle){
		if(jsn->_type==json_null){
			str = L"null";
		}else if(jsn->_type==json_boolean){
			str = jsn->_boolValue?L"true":L"false";
		}else if(jsn->_type==json_integer){
			str.FromInt64(jsn->_intValue,10);
		}else if(jsn->_type==json_double){
			str.Format(L"%#.16g",jsn->_doubleValue);
		}else if(jsn->_type==json_string){
			addSlash(jsn->_strValue->Handle(),jsn->_strValue->Length(),str,readStyle);
			str.Insert('"',0);
			str.Insert('"');
		}else if(jsn->_type==json_array){
			if(readStyle)
				str = L"[\r\n";
			else
				str = L"[";
			String sub;
			ObjectList<Json>& a = *jsn->_arrayValue;
			int i = 0;
			Json* sjsn = a.GetElement(i);
			if(sjsn){
				if(readStyle)
					tab += L"\t";
				_toString(sjsn,sub,tab,readStyle);
				str += tab;
				str += sub;
				while(true){
					i++;
					sjsn = a.GetElement(i);
					if(!sjsn) break;
					_toString(sjsn,sub,tab,readStyle);
					if(readStyle)
						str += L",\r\n";
					else
						str += L",";
					str += tab;
					str += sub;
				}
				tab.Head(-1);
				if(readStyle)
					str += L"\r\n";
			}
			str += tab;
			str += L"]";
		}else if(jsn->_type==json_object){
			if(readStyle)
				str = L"{\r\n";
			else
				str = L"{";
			String sub;
			ObjectList<Json::DICTION>& obj = *jsn->_objectValue;
			Json::DICTION* dic = obj.GetElement(0);
			if(dic){
				if(readStyle){
					tab += L"\t";
					str += tab;
				}
				str += L"\"";
				addSlash(dic->key,dic->key.Length(),sub,readStyle);
				str += sub;
				if(readStyle){
					str += L"\" : ";
				}else{
					str += L"\":";
				}
				_toString(dic->val,sub,tab,readStyle);
				str += sub;
				int i = 0;
				while(true){
					i++;
					dic = obj.GetElement(i);
					if(!dic) break;
					if(readStyle){
						str += L",\r\n";
						str += tab;
					}else{
						str += L",";
					}
					str += L"\"";
					addSlash(dic->key,dic->key.Length(),sub,readStyle);
					str += sub;
					if(readStyle){
						str += L"\" : ";
					}else{
						str += L"\":";
					}
					_toString(dic->val,sub,tab,readStyle);
					str += sub;
				}
				tab.Head(-1);
				if(readStyle)
					str += L"\r\n";
			}
			str += tab;
			str += L"}";
		}else if(jsn->_type==json_binary){
			Memory<char> code;
			int len = Base64::Encode(code,jsn->_binaryValue->Handle(),jsn->_binaryValue->Length());
			str.FromMultiByte((LPCSTR)code.Handle(),len);
			str.Insert('\'',0);
			str.Insert('\'');
		}
	}
	bool _fromStrBytes(String* str,Memory<byte>& data,uint& offset,uint maxlen){
		if(offset+4>maxlen) return false;
		uint len = *(uint*)(data+offset);
		offset += 4;
		if(offset+len<len||offset+len>maxlen) return false;//error len to big
		str->CopyFrom((LPCWSTR)(data+offset),len/2);
		//if(!str->FromMultiByte((char*)(data+offset),len,CP_UTF8)) return false;
		offset += len;
		return true;
	}
	bool _fromBytes(Json* jsn,Memory<byte>& data,uint& offset,uint maxlen){
		JSON_TYPE type = (JSON_TYPE)*(byte*)(data+offset);
		offset += 1;
		if(type==json_null){
			jsn->SetToNull();
		}else if(type==json_boolean){
			if(offset+1>maxlen) return false;
			*jsn = *(bool*)(data+offset);
			offset += 1;
		}else if(type==json_integer){
			if(offset+8>maxlen) return false;
			*jsn = *(int64*)(data+offset);
			offset += 8;
		}else if(type==json_double){
			if(offset+8>maxlen) return false;
			*jsn = *(double*)(data+offset);
			offset += 8;
		}else if(type==json_string){
			jsn->SetToString();
			if(!_fromStrBytes(jsn->_strValue,data,offset,maxlen)) return false;
		}else if(type==json_array){
			if(offset+4>maxlen) return false;
			jsn->SetToArray();
			uint count = *(uint*)(data+offset);
			offset += 4;
			for(uint i=0;i<count;i++){
				Json* sjn = jsn->AddArrayElm();
				if(!_fromBytes(sjn,data,offset,maxlen)) return false;
			}
		}else if(type==json_object){
			if(offset+4>maxlen) return false;
			jsn->SetToObject();
			uint count = *(uint*)(data+offset);
			offset += 4;
			for(uint i=0;i<count;i++){
				Json::DICTION* dic = jsn->_objectValue->Add();
				if(!_fromStrBytes(&dic->key,data,offset,maxlen)) return false;
				if(!_fromBytes(dic->val,data,offset,maxlen)) return false;
			}
		}else if(type==json_binary){
			if(offset+4>maxlen) return false;
			uint len = *(uint*)(data+offset);
			offset += 4;
			if(offset+len>maxlen) return false;
			jsn->SetToBinary();
			jsn->_binaryValue->CopyFrom((char*)(data+offset),len);
			offset += len;
		}else{
			return false;
		}
		return true;
	}
	void _strToBytes(String* str,Memory<byte>& data,uint& offset){
		uint len = str->Length()*2;
		data.Double(offset+len+4);
		*(uint*)(data+offset) = len;
		offset += 4;
		data.CopyFrom((byte*)str->Handle(),len,offset);
		offset += len;
		/*uint slen = str->ToMultiByte((char*)(data+offset+4),len,CP_UTF8);
		if(slen==0&&len!=0&&GetLastError()==ERROR_INSUFFICIENT_BUFFER){
			//缓存不足;
			Memory<char> buf;
			slen = str->ToMultiByte(buf,CP_UTF8);
			if(slen==0){
				CONASSERT(L"内存不足或异常");
			}
		}*/
	}
	void _toBytes(Json* jsn,Memory<byte>& data,uint& offset){
		data.Double(offset+1);
		*(byte*)(data+offset) = jsn->_type;
		offset++;
		if(jsn->_type==json_null){
		}else if(jsn->_type==json_boolean){
			data.Double(offset+1);
			*(bool*)(data+offset) = jsn->_boolValue;
			offset += 1;
		}else if(jsn->_type==json_integer){
			data.Double(offset+8);
			*(int64*)(data+offset) = jsn->_intValue;
			offset += 8;
		}else if(jsn->_type==json_double){
			data.Double(offset+8);
			*(double*)(data+offset) = jsn->_doubleValue;
			offset += 8;
		}else if(jsn->_type==json_string){
			_strToBytes(jsn->_strValue,data,offset);
		}else if(jsn->_type==json_array){
			data.Double(offset+4);
			ObjectList<Json>& a = *jsn->_arrayValue;
			*(uint*)(data+offset) = a.Count();
			offset += 4;
			for(uint i=0;i<a.Count();i++){
				Json* sjsn = a.GetElement(i);
				_toBytes(sjsn,data,offset);
			}
		}else if(jsn->_type==json_object){
			data.Double(offset+4);
			ObjectList<Json::DICTION>& obj = *jsn->_objectValue;
			*(uint*)(data+offset) = obj.Count();
			offset += 4;
			for(uint i=0;i<obj.Count();i++){
				Json::DICTION* dic = obj.GetElement(i);
				_strToBytes(&dic->key,data,offset);
				_toBytes(dic->val,data,offset);
			}
		}else if(jsn->_type==json_binary){
			uint len = jsn->_binaryValue->Length();
			data.Double(offset+4+len);
			*(uint*)(data+offset) = len;
			offset += 4;
			data.CopyFrom((byte*)jsn->_binaryValue->Handle(),len,offset);
			offset += len;
		}
	}
	//Json
	void Json::operator = (Json& val){
		SetToNull();
		if(val._type==json_null||val._type==json_boolean||val._type==json_integer||val._type==json_double){
			_intValue = val._intValue;
		}else if(val._type==json_string){
			_strValue = new String();
			*_strValue = *val._strValue;
		}else if(val._type==json_array){
			_arrayValue = new ObjectList<Json>;
			_arrayValue->SetGrow(1);
			ObjectList<Json>& a = *val._arrayValue;
			for(uint i=0;i<a.Count();i++){
				Json* sjn = a.GetElement(i);
				if(!sjn) break;
				Json* djn = _arrayValue->Add();
				*djn = *sjn;
			}
		}else if(val._type==json_object){
			_objectValue = new ObjectList<Json::DICTION>;
			_objectValue->SetGrow(1);
			ObjectList<Json::DICTION>& a = *val._objectValue;
			for(uint i=0;i<a.Count();i++){
				Json::DICTION* sjn = a.GetElement(i);
				if(!sjn) break;
				Json::DICTION* djn = _objectValue->Add();
				djn->key = sjn->key;
				*djn->val = *sjn->val;
			}
		}else if(val._type==json_binary){
			_binaryValue = new Memory<char>;
			_binaryValue->CopyFrom(val._binaryValue->Handle(),val._binaryValue->Length());
		}
		_type = val._type;
	}
	void Json::SetString(LPCWSTR str,int len){
		if(_type!=json_string){
			SetToNull();
			_strValue = new String();
		}
		*_strValue = (LPCWSTR)0;
		_strValue->CopyFrom(str,len,0);
		_type = json_string;
	}
	void Json::SetMbStr(LPCSTR mbs,int len,DWORD code){
		if(_type!=json_string){
			SetToNull();
			_strValue = new String();
		}
		*_strValue = (LPCWSTR)0;
		_strValue->FromMultiByte(mbs,len,code);
		_type = json_string;
	}
	bool Json::SetBinary(LPCVOID data,int len){
		if(_type!=json_binary){
			SetToNull();
			_binaryValue = new Memory<char>;
			_type = json_binary;
		}
		if(!_binaryValue->SetLength(len)){
			CONASSERT(L"Json 设置二进制内存失败");
			return 0;
		}
		_binaryValue->CopyFrom((LPCSTR)data,len);
		return 1;
	}
	LPCVOID Json::GetBinary(int* len){
		if(_type!=json_binary){
			if(len) *len = 0;
			return 0;
		}
		if(len) *len = _binaryValue->Length();
		return _binaryValue->Handle();
	}
	void Json::SetToArray(){
		if(_type==json_array) return;
		SetToNull();
		_arrayValue = new ObjectList<Json>;
		_arrayValue->SetGrow(1);
		_type = json_array;
	}
	void Json::SetToBoolean(){
		if(_type==json_boolean) return;
		SetToNull();
		_type = json_boolean;
	}
	void Json::SetToInteger(){
		if(_type==json_integer) return;
		SetToNull();
		_type = json_integer;
	}
	void Json::SetToDouble(){
		if(_type==json_double) return;
		SetToNull();
		_type = json_double;
	}
	void Json::SetToString(){
		if(_type==json_string) return;
		SetToNull();
		_strValue = new String;
		_type = json_string;
	}
	void Json::SetToObject(){
		if(_type==json_object) return;
		SetToNull();
		_objectValue = new ObjectList<Json::DICTION>;
		_objectValue->SetGrow(1);
		_type = json_object;
	}
	void Json::SetToBinary(){
		if(_type==json_binary) return;
		SetToNull();
		_binaryValue = new Memory<char>;
		_type = json_binary;
	}
	void Json::SetToNull(){
		if(json_null==_type) return;
		if(json_string==_type){
			delete _strValue;
		}else if(json_array==_type){
			delete _arrayValue;
		}else if(json_object==_type){
			delete _objectValue;
		}else if(json_binary==_type){
			delete _binaryValue;
		}
		_type = json_null;
		_intValue = 0;
	}
	void Json::Empty(){
		if(_type==json_null)
			return;
		else if(_type==json_integer||_type==json_double||_type==json_boolean)
			_intValue = 0;
		else if(_type==json_array)
			_arrayValue->Clear();
		else if(_type==json_object)
			_objectValue->Clear();
		else if(_type==json_string)
			*_strValue = (LPCWSTR)0;
		else if(_type==json_binary)
			_binaryValue->SetLength(0);
	}
	Json* Json::GetProperty(LPCWSTR prop,bool createIfNotExist){
		if(json_null==_type&&createIfNotExist){
			_objectValue = new ObjectList<Json::DICTION>;
			_objectValue->SetGrow(1);
			_type = json_object;
			Json::DICTION* dic = _objectValue->Add();
			dic->key = prop;
			return dic->val;
		}
		if(json_object!=_type){
			if(createIfNotExist){
				SetToObject();
			}else{
				return 0;
			}
		}
		for(uint i=0;i<_objectValue->Count();i++){
			Json::DICTION* dic = _objectValue->GetElement(i);
			if(!dic) break;
			if(dic->key==prop){
				return dic->val;
			}
		}
		if(createIfNotExist){
			Json::DICTION* dic = _objectValue->Add();
			dic->key = prop;
			return dic->val;
		}
		return 0;
	}
	Json* Json::SetProperty(LPCWSTR prop,Json& val){
		Json* jsn = SetProperty(prop);
		*jsn = val;
		return jsn;
	}
	Json* Json::SetProperty(LPCWSTR prop){
		if(_type==json_null){
			_objectValue = new ObjectList<Json::DICTION>;
			_objectValue->SetGrow(1);
			_type = json_object;
		}else if(_type==json_object){
		}else return 0;
		for(uint i=0;i<_objectValue->Count();i++){
			Json::DICTION* dic = _objectValue->GetElement(i);
			if(!dic) break;
			if(dic->key==prop){
				return dic->val;
			}
		}
		Json::DICTION* dic = _objectValue->Add();
		dic->key = prop;
		return dic->val;
	}
	Json* Json::GetPropertyByIndex(int index,LPCWSTR* key){
		if(_type!=json_object) return 0;
		DICTION* dic = _objectValue->GetElement(index);
		if(dic==NULL) return 0;
		if(key) *key = dic->key;
		return dic->val;
	}
	bool Json::RemoveProperty(LPCWSTR prop){
		if(_type!=json_object) return 0;
		for(uint i=0;i<_objectValue->Count();i++){
			DICTION* dic = _objectValue->GetElement(i);
			if(!dic) break;
			if(dic->key==prop){
				return 1==_objectValue->Delete(i);
			}
		}
		return 0;
	}
	bool Json::RemovePropertyByIndex(int index){
		if(_type!=json_object) return 0;
		return _objectValue->Delete(index)>0;
	}
	void Json::ToString(String& str,bool readStyle){
		String tab;
		_toString(this,str,tab,readStyle);
	}
	bool Json::Parse(LPCWSTR jstr,int len){
		len = WcsLength(jstr,len);
		SetToNull();
		if(len==0) return 1;
		return -1!=_parse(jstr,0,len,this);
	}
	bool Json::Parse(LPCSTR jstr,int len = -1,DWORD code){
		String str;
		str.FromMultiByte(jstr,len,code);
		return Parse(str);
	}
	bool Json::LoadFromFile(LPCWSTR fn,DWORD codepage){
		FileStream fs;
		if(!fs.Create(fn,OPEN_EXISTING)) return 0;
		String buf;
		fs.ReadString(buf,0,codepage);
		return Parse(buf);
	}
	bool Json::SaveToFile(LPCWSTR fn,DWORD codepage,bool readStyle){
		FileStream fs;
		if(!fs.Create(fn,OPEN_ALWAYS)) return 0;
		fs.SetLength(0);
		String buf;
		ToString(buf,readStyle);
		return 0!=fs.WriteString(buf,0,0,codepage);
	}
	uint Json::SaveBytes(LPCWSTR fn){
		Memory<byte> buf;
		if(!ToBytes(buf)) return 0;
		File file;
		if(!file.Create(fn,OPEN_EXISTING)) return 0;
		file.SetLength(0);
		return file.Write(&buf);
	}
	bool Json::LoadBytes(LPCWSTR fn){
		File file;
		if(!file.Create(fn,OPEN_ALWAYS)) return 0;
		Memory<byte> buf;
		uint len = file.Read(&buf);
		if(len==0) return false;
		return FromBytes(buf);
	}
	uint Json::getUsedMemory(){
		uint len = sizeof(this);
		if(_type==json_string){
			len += _strValue->getUsedMemory();
		}else if(_type==json_array){
			len += _arrayValue->getUsedMemory();
			for(uint i=0;i<_arrayValue->Count();i++){
				len += _arrayValue->Element(i)->getUsedMemory();
			}
		}else if(_type==json_object){
			len += _objectValue->getUsedMemory();
			for(uint i=0;i<_objectValue->Count();i++){
				DICTION* dic = _objectValue->Element(i);
				len += sizeof(DICTION);
				len += dic->key.getUsedMemory();
				len += dic->val->getUsedMemory();
			}
		}
		return len;
	}

static const uint json_header_tag = 0x3a5f3d97;
	uint Json::ToBytes(Memory<byte>& data){
		data.Double(4);
		*(uint*)data.Handle() = json_header_tag;
		uint offset = 4;
		_toBytes(this,data,offset);
		data.SetLength(offset);
		return offset;
	}
	bool Json::FromBytes(Memory<byte>& data){
		SetToNull();
		if(data.Length()<5) return 0;
		if(*(uint*)data.Handle()!=json_header_tag) return 0;
		uint offset = 4;
		return _fromBytes(this,data,offset,data.Length());
	}
	//Config
	class config_struct : public _class{
	public:
		Json Tree;
		String FileName;
		CriticalSection Lock;
		int icount;
	};
	static __declspec(thread) bool _cfglock = false;
	//打开一个Json对象，在同一进程内相同的 fn 返回相同的对象.
	//这些内存直到程序结束才会释放
	static CriticalSection _configCs;
	config_struct* getConfigObject(LPCWSTR name,config_struct* obj = 0){
		static ObjectList<config_struct>* dbList = 0;
		LocalCriticalSection lcs(_configCs);
		if(dbList==0){
			dbList = new (mallocGlobalStaticMem(sizeof(ObjectList<config_struct>))) ObjectList<config_struct>;
		}

		if(obj!=NULL){
			obj->icount--;
			if(obj->icount==0){
				dbList->Delete(obj);
			}
			return 0;
		}
		String fn,title;
		if(WcsLength(name)==0){
			fn = App::GetAppDirectory();
			title = App::GetAppName();
			FPLinkPath(fn,title);
			FPLinkExt(fn,L"Json");
		}else{
			fn = name;
		}

		config_struct* rs = 0;
		for(uint i=0;i<dbList->Count();i++){
			rs = &(*dbList)[i];
			if(FPIsSame(fn,rs->FileName)){
				rs->icount++;
				return rs;
			}
		}
		rs = new config_struct;
		rs->icount = 1;
		if(1!=GetFileType(fn)){
			File f;
			f.Create(fn);
		}
		if(!rs->Tree.LoadFromFile(fn)){
			CONASSERT(L"config file is not a valid Json format");
		}
		rs->FileName = fn;
		dbList->AddIn(rs);
		return rs;
	}
	Config::Config():_innerobj(0){}
	Config::Config(LPCWSTR fname):_innerobj(0){
		_innerobj = getConfigObject(fname);
		if(_innerobj==NULL) Print(L"Config Error: _innerobj is NULL");
	}

	Json* Config::Lock(bool wait){
		if(!_innerobj||_cfglock) return 0;
		config_struct* cfg = (config_struct*)_innerobj;
		if(!wait){
			if(!cfg->Lock.TryLock()) return 0;
		}else
			cfg->Lock.Lock();
		_cfglock = true;
		return &cfg->Tree;
	}
	void Config::Unlock(){
		if(_innerobj==0) return;
		config_struct* cfg = (config_struct*)_innerobj;
		cfg->Tree.SaveToFile(cfg->FileName);
		((config_struct*)_innerobj)->Lock.Unlock();
		_cfglock = false;
	}
	void Config::Close(){
		if(_innerobj==0) return;
		config_struct* cfg = (config_struct*)_innerobj;
		_innerobj = 0;
		if(_cfglock){
			cfg->Tree.SaveToFile(cfg->FileName);
			cfg->Lock.Unlock();
			_cfglock = false;
		}
		getConfigObject(0,cfg);
	}
	bool Config::Create(LPCWSTR fname){
		if(_innerobj) return 1;
		_innerobj = getConfigObject(fname);
		return _innerobj!=NULL;
	}
	/*Json* Config::GetJson(){
	if(_innerobj==0) return 0;
	return &((config_struct*)_innerobj)->Tree;
	}*/
	Config::~Config(){
		Close();
	}
}