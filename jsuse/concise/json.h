#pragma once
namespace cs{
	/*JavaScript 语言的变量有 7 种，其中 1 个 Null 类型，这种类型只有一个值，即
	变量的缺省初始值；4 种值类型：布尔类型、整数类型、浮点类型和字串类型；2 种
	复合类型：数组和对象。数组是一系列的变量集合，变量可以是 7 种类型的任何一种。
	对象是一个字典，它由一系列的 key 和 value 对组成，其中 key 必须是字串类型，
	值可以是 7 种类型的任何类型。

	Json 以 JavaScript 语言数据结构为原型，在 C++ 中完成相应的功能。
	*/
	enum JSON_TYPE{
		json_null = 0,
		json_boolean = 1,
		json_integer = 2,
		json_double = 3,
		json_string = 4,
		json_array = 5,
		json_object = 6,
		json_binary = 7
	};
	class CONCISE_API Json : public _class{
	public:
		typedef struct DICTION : public _struct{
			String key;
			Json* val;
			DICTION(){
				val = new Json;
			}
			~DICTION(){
				delete val;
			}
		}DICTION;
		union{
			bool _boolValue;
			int64 _intValue;
			double _doubleValue;
			String* _strValue;
			ObjectLink<Json>* _arrayValue;
			ObjectLink<DICTION>* _objectValue;
			Memory<char>*	_binaryValue;
		};
		JSON_TYPE _type;
		~Json(){
			SetToNull();
		}
		Json():_type(json_null),_doubleValue(0){
		}
		Json(bool val):_type(json_null){
			*this = val;
		}
		void operator = (bool val){
			SetToNull();
			_boolValue = val;
			_type = json_boolean;
		}
		Json(const wchar_t* str):_type(json_null){
			*this = str;
		}
		void operator = (const wchar_t* str){
			SetString(str);
		}
		Json(const char* str):_type(json_null){
			*this = str;
		}
		void operator = (const char* mbs){
			SetMbStr(mbs);
		}
		Json(int64 val):_type(json_null){
			*this = val;
		}
		Json(int val):_type(json_null){
			*this = val;
		}
		void operator = (int64 val){
			SetToNull();
			_intValue = val;
			_type = json_integer;
		}
		void operator = (int val){
			SetToNull();
			_intValue = val;
			_type = json_integer;
		}
		Json(double val):_type(json_null){
			*this = val;
		}
		void operator = (double val){
			SetToNull();
			_doubleValue = val;
			_type = json_double;
		}
		Json(Json& val):_type(json_null){
			*this = val;
		}
		void operator = (Json& val);
		bool IsNull(){
			return json_null==_type;
		}
		bool IsBoolean(){
			return json_boolean==_type;
		}
		bool IsInteger(){
			return json_integer==_type;
		}
		bool IsDouble(){
			return json_double==_type;
		}
		//整数或浮点类型
		bool IsNumber(){
			return json_double==_type||json_integer==_type;
		}
		bool IsString(){
			return json_string==_type;
		}
		bool IsObject(){
			return json_object==_type;
		}
		bool IsArray(){
			return json_array==_type;
		}
		bool IsBinary(){
			return json_binary==_type;
		}
		bool Boolean(bool def = 0){
			if(json_boolean==_type) return _boolValue;
			else return def;
		}
		bool GetBoolean(bool& val){
			if(json_boolean!=_type) return false;
			val = _boolValue;
			return true;
		}
		operator bool (){
			return Boolean(false);
		}
		double Double(double def = 0){
			if(json_double==_type) return _doubleValue;
			else if(json_integer==_type) return (double)_intValue;
			else return def;
		}
		bool GetDouble(double& val){
			if(json_double==_type||json_integer==_type){
				val = _doubleValue;
				return true;
			}
			return false;
		}
		operator double (){
			return Double(0.0);
		}
		int64 Int64(int64 def = 0){
			if(json_integer==_type) return _intValue;
			else if(json_double==_type) return (int64)_doubleValue;
			else return def;
		}
		int64 GetInt64(int64& val){
			if(json_integer==_type) val = _intValue;
			else if(json_double==_type) val = (int64)_doubleValue;
			else return false;
			return true;
		}
		operator int64 (){
			return Int64(0);
		}
		int Int32(int def = 0){
			if(json_integer==_type) return (int)_intValue;
			else if(json_double==_type) return (int)_doubleValue;
			else return def;
		}
		bool GetInt32(int& val){
			if(json_integer==_type) val = (int)_intValue;
			else if(json_double==_type) val = (int)_doubleValue;
			else return false;
			return true;
		}
		operator int (){
			return Int32(0);
		}
		LPCWSTR Str(LPCWSTR def = 0){
			if(json_string==_type) return _strValue->Handle();
			else return def;
		}
		bool GetString(LPCWSTR& val){
			if(json_string==_type) val = _strValue->Handle();
			else return false;
			return true;
		}
		void SetString(LPCWSTR str,int len = -1);
		void SetMbStr(LPCSTR mbs,int len = -1,DWORD code = CP_THREAD_ACP);
		operator LPCWSTR (){
			return Str(L"");
		}
		bool SetBinary(LPCVOID data,int len);
		LPCVOID GetBinary(int* len = 0);
		//SetTo函数把对象设置为指定类型，如果对象本来就是这个类型，此函数什么都不做，否则，会强制设为新类型的缺省值。
		void SetToArray();
		void SetToBoolean();
		void SetToInteger();
		void SetToDouble();
		void SetToString();
		void SetToObject();
		void SetToBinary();
		void SetToNull();
		//不改变对象的类型，但是值设置为对应类型的缺省值。
		void Empty();
		//Property函数针对当前对象为object类型时的情况，如果当前类型不是 object 或者 null，createIfNotExist = false,所有操作会失败。如果 createIfNotExist = true，对象会被强制转换为 object 类型。
		Json* GetProperty(LPCWSTR prop,bool createIfNotExist = false);
		//获取属性的字串值，如果属性不存在或者属性不是字串类型，返回false。
		bool GetPropertyStr(LPCWSTR prop,LPCWSTR& val){
			Json* js = GetProperty(prop);
			if(!js||!js->IsString()) return 0;
			val = js->Str();
			return 1;
		}
		//返回属性的字串值，如果属性不存在或者属性不是字串类型，返回设置的缺省值 def 。
		LPCWSTR PropertyStr(LPCWSTR prop,LPCWSTR def = 0){
			Json* js = GetProperty(prop);
			if(!js||!js->IsString()) return def;
			return js->Str();
		}
		//获取属性的32位整数值，如果属性不存在或者属性不是数字（整数或浮点）类型，返回 false，如果这个值比32位整数大，会被截断。
		bool GetPropertyInt32(LPCWSTR prop,int& val){
			Json* js = GetProperty(prop);
			if(!js||!js->IsNumber()) return 0;
			val = js->Int32();
			return 1;
		}
		//返回属性的32位整数值，如果属性不存在或者属性不是数字（整数或浮点）类型，返回设置的缺省值 def ，如果这个值比32位整数大，会被截断。
		int PropertyInt32(LPCWSTR prop,int def = 0){
			Json* js = GetProperty(prop);
			if(!js||!js->IsNumber()) return def;
			return js->Int32();
		}
		//获取属性的64位整数值，如果属性不存在或者属性不是数字（整数或浮点）类型，返回false。
		bool GetPropertyInt64(LPCWSTR prop,int64& val){
			Json* js = GetProperty(prop);
			if(!js||!js->IsNumber()) return 0;
			val = js->Int64();
			return 1;
		}
		//返回属性的64位整数值，如果属性不存在或者属性不是数字（整数或浮点）类型，返回设置的缺省值 def 。
		int64 PropertyInt64(LPCWSTR prop,int64 def = 0){
			Json* js = GetProperty(prop);
			if(!js||!js->IsNumber()) return def;
			return js->Int64();
		}
		//获取属性的 bool 值，如果属性不存在或者属性不是布尔类型，返回false。
		bool GetPropertyBool(LPCWSTR prop,bool& val){
			Json* js = GetProperty(prop);
			if(!js||!js->IsBoolean()) return 0;
			val = js->Boolean();
			return 1;
		}
		//返回属性的 bool 值，如果属性不存在或者属性不是布尔类型，返回设置的缺省值 def 。
		bool PropertyBool(LPCWSTR prop,bool def = 0){
			Json* js = GetProperty(prop);
			if(!js||!js->IsBoolean()) return def;
			return js->Boolean();
		}
		//获取属性的浮点值，如果属性不存在或者属性不是数字（整数或浮点）类型，返回false。
		bool GetPropertyDouble(LPCWSTR prop,double& val){
			Json* js = GetProperty(prop);
			if(!js||!js->IsNumber()) return 0;
			val = js->Double();
			return 1;
		}
		//返回属性的浮点值，如果属性不存在或者属性不是数字（整数或浮点）类型，返回设置的缺省值 def 。
		double PropertyDouble(LPCWSTR prop,double def = 0.0){
			Json* js = GetProperty(prop);
			if(!js||!js->IsNumber()) return def;
			return js->Double();
		}
		//获取属性的二进制值，如果属性不存在或者属性不是二进制类型，返回false，len 返回数据的长度。
		bool GetPropertyBinary(LPCWSTR prop,LPCVOID& val,int* len = 0){
			Json* js = GetProperty(prop);
			if(!js||!js->IsBinary()) return 0;
			val = js->GetBinary(len);
			return 1;
		}
		//返回属性的二进制值，如果属性不存在或者属性不是二进制类型，返回设置 NULL ，len 返回数据的长度。
		LPCVOID PropertyBinary(LPCWSTR prop,int* len = 0){
			Json* js = GetProperty(prop);
			if(!js||!js->IsBinary()) return 0;
			return js->GetBinary(len);
		}
		//设置属性为指定的字串值
		Json* SetPropertyStr(LPCWSTR prop,LPCWSTR val){
			Json jv = val;
			return SetProperty(prop,jv);
		}
		//设置属性为指定的字串值
		Json* SetPropertyMbs(LPCWSTR prop,LPCSTR val){
			Json jv = val;
			return SetProperty(prop,jv);
		}
		//设置属性为指定的32位整数值
		Json* SetPropertyInt32(LPCWSTR prop,int val){
			Json jv = val;
			return SetProperty(prop,jv);
		}
		//设置属性为指定的64位整数值
		Json* SetPropertyInt64(LPCWSTR prop,int64 val){
			Json jv = val;
			return SetProperty(prop,jv);
		}
		//设置属性为指定的浮点值
		Json* SetPropertyDouble(LPCWSTR prop,double val){
			Json jv = val;
			return SetProperty(prop,jv);
		}
		//设置属性为指定的布尔值
		Json* SetPropertyBool(LPCWSTR prop,bool val){
			Json jv = val;
			return SetProperty(prop,jv);
		}
		//设置属性为指定的二进制值
		Json* SetPropertyBinary(LPCWSTR prop,const void* data,int len){
			Json jv;
			if(!jv.SetBinary(data,len)) return 0;
			return SetProperty(prop,jv);
		}
		//设置当前对象的属性值为空，如果当前对象不是 Object 或者 Null 类型，操作失败，返回 NULL。
		//Json* SetProperty(LPCWSTR prop);
		//设置当前对象的属性值为指定的值，如果当前对象不是 Object 或者 Null 类型，操作失败，返回 NULL。
		Json* SetProperty(LPCWSTR prop,Json& val);
		//Property也可以通过数字索引操作，但是仅限于读取和删除，一般不要通过索引来设置属性。
		//GetPropertyByIndex 获取的 key 和 pval 都指向对象内部的值，所以应该只用于读取它们的值，不要更改它们。
		Json* GetPropertyByIndex(int index,LPCWSTR* key = 0);
		bool RemoveProperty(LPCWSTR prop);
		bool RemovePropertyByIndex(int index);
		//Array类型是属性的数字索引形式，内部用链表实现，所以速度比属性快，特别是数据量比较大的时候，删除添加查找都效率更高。
		//把链表指针移动到开头
		void ArrayFirst(){
			if(_type!=json_array) return;
			_arrayValue->First();
		}
		//把链表指针移动到末尾
		void ArrayLast(){
			if(_type!=json_array) return;
			_arrayValue->Last();
		}
		//移动链表指针，正数向后，负数向前
		int ArrayMove(int step){
			if(_type!=json_array) return 0;
			return _arrayValue->Move(step);
		}
		//返回数组的当前元素，如果当前链表为空，返回 NULL。
		Json* GetArrayElm(){
			if(_type!=json_array) return 0;
			return _arrayValue->Element();
		}
		//设置数组的当前元素的值，并且返回这个元素，如果当前对象不是数组，返回 NULL。
		Json* SetArrayElm(Json& val){
			if(_type!=json_array) return 0;
			Json* elm = _arrayValue->Element();
			*elm = val;
			return elm;
		}
		//插入一个数组元素, pos = LINK_FIRST（0） 插入到最前面，pos = LINK_CURRENT（1），
		//插入到当前位置的后面；pos = LINK_LAST（2），插入到最后面。这个操作不改变当前元素的指针。
		//返回插入的新元素。
		Json* AddArrayElm(int pos = LINK_LAST){
			if(_type==json_null) SetToArray();
			if(_type!=json_array) return 0;
			return _arrayValue->Add(pos);
		}
		//移除当前数组元素，当前指针指向后一个元素，如果没有后一个元素，链表溢出。
		void RemoveArrayElm(){
			if(_type!=json_array) return;
			_arrayValue->Delete();
		}
		//链表是否处于溢出状态
		bool ArrayOverFlow(){
			return _arrayValue->OverFlow();
		}
		bool GetArrayStr(LPCWSTR& val){
			Json* js = GetArrayElm();
			if(!js||!js->IsString()) return 0;
			val = js->Str();
			return 1;
		}
		bool GetArrayInt32(int& val){
			Json* js = GetArrayElm();
			if(!js||!js->IsInteger()) return 0;
			val = js->Int32();
			return 1;
		}
		bool GetArrayInt64(int64& val){
			Json* js = GetArrayElm();
			if(!js||!js->IsInteger()) return 0;
			val = js->Int64();
			return 1;
		}
		bool GetArrayBool(bool& val){
			Json* js = GetArrayElm();
			if(!js||!js->IsBoolean()) return 0;
			val = js->Boolean();
			return 1;
		}
		bool GetArrayDouble(double& val){
			Json* js = GetArrayElm();
			if(!js||!js->IsDouble()) return 0;
			val = js->Double();
			return 1;
		}
		Json* SetArrayStr(LPCWSTR val){
			Json jv(val);
			return SetArrayElm(jv);
		}
		Json* SetArrayMbs(LPCSTR val){
			Json jv(val);
			return SetArrayElm(jv);
		}
		Json* SetArrayBool(bool val){
			Json jv(val);
			return SetArrayElm(jv);
		}
		Json* SetArrayInt32(int val){
			Json jv(val);
			return SetArrayElm(jv);
		}
		Json* SetArrayInt64(int64 val){
			Json jv(val);
			return SetArrayElm(jv);
		}
		Json* SetArrayDouble(double val){
			Json jv(val);
			return SetArrayElm(jv);
		}

		//Count返回当前类型为数组或Object时的子项数量，其它类型返回 0 。
		uint Count(){
			if(_type==json_array) return _arrayValue->Count();
			else if(_type==json_object) return _objectValue->Count();
			return 0;
		}
		//readStyle指定字串是否是阅读风格，阅读风格有回车换行和缩进，而且不转义中文字符，适合阅读。
		//如果readStyle = false，中文字符会被编码，而且没有换行和缩进，适合传输和保存。
		void ToString(String& str,bool readStyle = true);
		bool Parse(LPCWSTR jstr,int len = -1);
		bool Parse(LPCSTR jstr,int len,DWORD codepage = CP_THREAD_ACP);
		bool LoadFromFile(LPCWSTR fn,DWORD codepage = CP_THREAD_ACP);
		bool SaveToFile(LPCWSTR fn,DWORD codepage = CP_THREAD_ACP,bool readStyle = true);

		Json& operator [] (int index){
			if(_type==json_array){
				if((int)_arrayValue->Count()>index){
					_arrayValue->First();
					_arrayValue->Move(index);
					return *_arrayValue->Element();
				}else{
					Json* a;
					for(int i=_arrayValue->Count();i<=index;i++)
						a = _arrayValue->Add();
					return *a;
				}
			}else if(_type!=json_null){
				_ASSERT(0);
				return *(Json*)0;
			}
			_arrayValue = new ObjectLink<Json>;
			_type = json_array;
			Json* a;
			for(int i=0;i<=index;i++)
				a = _arrayValue->Add();
			return *a;
		}
		Json& operator [] (LPCWSTR key){
			if(_type!=json_object){
				return *(Json*)0;
			}
			return *GetProperty(key,true);
		}
	};
	/*使用方法，可以使用局部变量的方式 Config cfg(0); 0 表示默认配置文件，然后用 Lock 函数获取
	Json 对象。无需调用 Close 和 Unlock 函数，因为它们在析构的时候会自动调用。
	如果不是局部变量，同一线程多次调用 Lock 函数，第二次会失败，也就是 Lock 和 Unlock 必须
	严格成对调用，Unlock 调用之前无论是其它线程，还是当前线程，都无法再次成功调用 Lock 函数
	这就保证，任何时候，打开读取和关闭写入中间不会有同时一个以上的操作，保持了文件和数据的一致。
	*/
	class CONCISE_API Config : public _class{
	protected:
		void* _innerobj; //it's a register_struct pointer.
	public:
		//此时并不实例化内部的 Xml 对象
		Config();
		//实例化一个 Json 对象le的使用。
		Config(LPCWSTR file);
		//打开指定文件读写, 内部会创建一个对象, 并且读取file文件的内容来初始化这个对象.
		bool Create(LPCWSTR file);
		//写操作前, 应该调用Lock, 独占内部对象, 后续的Lock将会被挂起, 直到调用Unlock函数.
		//锁定对单线程无效（无法锁定），因为同文件名只有一个内部对象，同线程操作同一个对象，一般不会引起冲突，除非恰好操作了Json对象的同一个
		//位置，但是这种冲突是可控的，和多线程的访问完全不可预知是不同的。
		//尽管可以同时在不冲突的情况下操作配置文件，但是应该保证打开和写入中间只有一个操作者，因为如果有
		//其它程序打开文件，自己读取和写入，而没有使用Config对象，就会引发混乱。
		//如果wait = true，会等待其它打开操作关闭，如果wait = false，有其它线程操作的话，会失败。
		Json* Lock(bool wait = true);
		//解除Lock, 因为为了保持文件内容的一致, 同一时间只能有一个打开操作, Unlock会自动调用Flush。
		void Unlock();
		//彻底释放Create操作打开的对象, 当外部程序改变了文件的内容时.
		//需要Close后, 重新用Create打开, 内容才会刷新, 因为内部对象一旦生成就不会再从文件读取内容.
		void Close();
		//写入文件, Unlock也会执行这个操作, 所以除非在还没调用Unlock函数但是需要把信息写入文件时, 才需要调用这个函数。
		//void Flush();
		//返回内部的Xml对象, 进行读写操作.
		//Json* GetJson();
		//析构
		~Config();
	};
}
