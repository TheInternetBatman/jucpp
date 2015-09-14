#pragma once
namespace cs{
	//以下是宽字符字串函数.

	//转化为小写字母
	CONCISE_API void WcsLower(wchar_t* wcs);
	//转化为大写字母.
	CONCISE_API void WcsUpper(wchar_t* wcs);
	//取得字串的字符个数,(开头到第一个为零的字符距离).maxlen是检查的最大范围.
	CONCISE_API uint WcsLength(const wchar_t* wcs,uint maxlen);
	//取得字串的字符个数,(开头到第一个为零的字符距离).maxlen是检查的最大范围.
	CONCISE_API uint WcsLength(const wchar_t* wcs);
	//字串拷贝,length是拷贝指定的长度.
	CONCISE_API uint WcsCopy(wchar_t* ds,const wchar_t* sc,uint length = -1);
	//判断两个字符是否是同一字符,不考虑大小写.
	CONCISE_API bool SameCase(wchar_t ch1,wchar_t ch2);
	//判断两字串是否相同.最多比较length位
	CONCISE_API bool WcsEqual(const wchar_t* str1,const wchar_t* str2,uint length = -1);
	//WcsCompare函数比较两个字串前面字符相同的个数.最多比较length位.如果length大于某一字串的长度,则只比较较短字串长度个字符.
	CONCISE_API uint WcsCompare(const wchar_t* str1,const wchar_t* str2,uint length = -1);
	//判断一个字串(wcs)的前若干位是否和另一字串(sub)相同.	
	CONCISE_API bool WcsInclude(const wchar_t* wcs,const wchar_t* sub);
	//判断两字串是否相同.最多比较length位.不考虑大小写.
	CONCISE_API bool WcsEqualNoCase(const wchar_t* str1,const wchar_t* str2,uint length = -1);
	//WcsCompareNoCase函数获取两个字串前面字符相同（不区分大小写）的个数.
	//最多比较length位，如果length大于某一字串的长度，会被重置为较短字串长度.
	CONCISE_API uint WcsCompareNoCase(const wchar_t* str1,const wchar_t* str2,uint length = -1);
	//判断一个字串(wcs)的前若干位是否和另一字串(sub)相同.	不考虑大小写.
	CONCISE_API bool WcsIncludeNoCase(const wchar_t* wcs,const wchar_t* sub);
	//反转字串.
	CONCISE_API void WcsReverse(wchar_t* string);
	//在一个字串中查找另一个字串,返回查找到的位置或-1(没有查找到的话)
	CONCISE_API uint WcsFind(const wchar_t* wcs,const wchar_t* sub);
	//在一个字串中查找另一个字串,忽略字母的大小写,返回查找到的位置或-1(没有查找到的话)
	CONCISE_API uint WcsFindNoCase(const wchar_t* wcs,const wchar_t* sub);

	//下面是多字节字串相关的函数.

	//返回字串的字节个数,不包括NULL结尾字符,bytes是计算的最大字节位置.
	CONCISE_API uint MbsByteLen(const char* str,uint bytes);
	//返回字符个数,返回字串的字节个数,不包括NULL结尾字符,bytes是计算的最大字节位置.
	CONCISE_API uint MbsCharLen(const char* str,uint bytes);
	//返回占用字节个数
	CONCISE_API uint MbsByteLen(const char* str);
	//返回字符个数.
	CONCISE_API uint MbsCharLen(const char* str);
	//返回字符索引为chars的字符的字节索引位置.(因为有些字符占两个字节,所以返回值总是大于chars).如果chars为负值
	//或大于字串的字符索引,返回字串的字节长度.(与MbsCharLen效果一样).也就是说它总是返回一个合法的字符索引.
	CONCISE_API uint MbsByteOfChar(const char* str,uint chars);
	//两个字串是否相同.只比较maxlen位.
	CONCISE_API bool MbsEqual(const char* str,const char* str2,uint maxlen = -1);
	//两个字串是否相同.
	CONCISE_API bool MbsEqualNoCase(const char* str,const char* str2,uint maxlen = -1);
	//在字串str中查找字串find,返回查找到的字节索引位置,否则,返回-1;
	CONCISE_API uint MbsFind(const char* str,const char* find,uint maxlen = -1);
	//在字串str中查找字串find,返回查找到的字节索引位置,否则,返回-1;不考虑大小写.
	CONCISE_API uint MbsFindNoCase(const char* str,const char* find,uint maxlen = -1);
	//反转字串.
	CONCISE_API void MbsReverse(char* str);
	//检测一个字串是否是utf8编码，len 指定检测的长度（某些时候为了加快速度可能只需要检测一部分），strict指定检测模式，utf-8 字串截断是有可能尾部不是一个完整的字符，当 strict = false 时，忽略这种情况，否则如果utf-8字串被截断，一定返回 false。
	CONCISE_API bool MbsIsUtf8(LPCSTR str,uint len = -1,bool strict = false);
	/*Window NT核心的操作系统, 在底层支持UNICODE字符,这就是说,所有的ANSI版本的API函数的
	字串参数都要经过一个编码转换成UNICODE字符,再调用UNICODE版本的API函数.而且操作UNICODE字串
	比操作多字节字串要方便的多,所以在大多数情况下应该使用UNICODE字串,也就是String字串类. 但
	是, 在某些时候,仍然离不多字节字符串,比如操作文本文件.大多数文本文件都是使用多字节编码.

	class String 一个未经非零或非空字符串初始化的的 String 实例的句柄是 NULL .
	String默认使用Fast内存方式, 所以如果需要大字串, 设置sys参数为 true*/
	class CONCISE_API String : public HandleType<LPWSTR>{
	protected:
		uint _Length,_Cubage;bool _sys;//Size--the size of the memory,Number--the caracter number of this string;
	public: 
		String();
		//创建一个指定初始容量的字符串实例,cubage = 0, 则不预先分配内存, 直到给它赋值.
		String(uint cubage,bool sys = 0);
		//用一个字串来初始化实例.
		String(LPCWSTR wstr,bool sys = 0);
		//= 操作符使用的函数.
		String(String& wstr,bool sys = 0);
		String(LPCSTR mbs,bool sys = 0);
		~String();
		//绑定.
		bool Attach(LPWSTR wstr);
		//解除绑定
		LPWSTR Detach();
		//在UNICODE和MULTIBYTE之间进行转换,如果数据不是合法字串,这种转换仍会完成,但可能会丢失某些字节,但它并不截断数据,甚至NULL字符.
		//但这种情况下字符类无法正确处理数据,你应该自己处理它,不要调用Realize函数,它在遇到NULL字符时会抛弃后面的数据.
		//设置 codepage = 0，能自动识别带bom头的utf-8编码的字串。
		bool FromMultiByte(LPCSTR str,uint length = -1,DWORD codepage = CP_THREAD_ACP);
		//length是buffer的大小,输出并不包含结尾的NULL字符.如果函数失败，返回 0 .如果length太小不足以保持全部的字符（不含结尾的NULL字符），
		//函数返回 0 ，但是，实际上转换是成功的，只不过只转换了前若干个字符，把buffer指定的长度填满。
		//如果buffer是0, 返回转换整个字串所需的字节空间, 这个空间不包括结尾的NULL字符
		uint ToMultiByte(LPSTR buffer,uint length,DWORD codepage = CP_THREAD_ACP);
		//把字符转换为 Mulitybyte 字符，不包括结尾的NULL字符。如果buffer太小，不足以存放所有字符，会自动被增大。
		//这个函数转换的字符并不包括结尾的NULL字符, 但是会保持 buffer 结尾有一个NULL字符的空位.
		uint ToMultiByte(Memory<char>* buffer,DWORD codepage = CP_THREAD_ACP);//
		//把实例中的字符拷贝到wstr中,从start开始,num是要拷贝的数量.num=-1表示拷贝至末尾.
		//wstr必须有足够的空间容纳字符,返回实际拷贝的字符数.
		uint CopyTo(wchar_t* wstr,uint start = 0,uint num = -1);
		//把实例的值置为wstr的前num个字符，如果num小于0，则拷贝wstr的整个字串。返回示例的字符长度。
		uint CopyFrom(LPCWSTR wstr,uint num = -1,uint pos = -1);
		//转化为小写
		void Lower(void);
		//转化为大写.
		void Upper(void);
		//反转字串次序.
		void Reverse();
		//因为可以操作实例的内存所以可以用这个函数来还原实例中的成员函数合法.
		void Realize(bool resetsize = 0);
		//设置实例的容量.
		bool SetCubage(uint cubage,bool dbSpace = 1);
		//在实例中查找字符,返回-1如果不存在.
		uint Find(wchar_t wch,uint start = 0);
		//在实例中查找字串.
		uint Find(LPCWSTR wstr,uint start = 0);
		//在实例中查找字符,返回-1如果不存在.
		uint FindNoCase(wchar_t wch,uint start = 0);
		//在实例中查找字串.
		uint FindNoCase(LPCWSTR wstr,uint start = 0);
		//插入字串,默认放在结尾.length是插入的字符个数.
		uint Insert(LPCWSTR wstr,uint pos = -1,uint length = -1);
		//插入字符.
		void Insert(wchar_t wc,uint pos = -1);
		//把指定位置的字串(从pos开始,长度为length)替换成ws,pos = -1表示从末尾置换(实际上是插入);length = -1表示从pos至末尾全部置换.返回插入字串的长度.
		uint Replace(LPCWSTR ws,uint pos = -1,uint length =-1);
		//截取字串的头部，index是截断的位置，如果为正数或0，则为通常的索引顺序，如果为负数，则末尾索引为0，向前依次为-1，-2，。。。
		bool Head(int index);
		//截取字串的尾部，index是截断的位置，如果为正数或0，则为通常的索引顺序，如果为负数，则末尾索引为0，向前依次为-1，-2，。。。
		bool Tail(int index);
		void operator = (String& wstr);
		void operator = (LPCWSTR wstr);
		void operator = (LPCSTR str);
		bool operator == (String& wstr){return WcsEqual(_Handle,wstr);}
		bool operator != (String& wstr){return !WcsEqual(_Handle,wstr);}
		inline bool operator == (LPCWSTR wstr){return WcsEqual(_Handle,wstr);}
		inline bool operator != (LPCWSTR wstr){return !WcsEqual(_Handle,wstr);}
		void operator += (LPCWSTR wstr);
		void operator += (String& wstr);
		void InsertNumber(INT_PTR number,int index = -1);
		inline wchar_t& Element(uint index){
			_ASSERT(index<_Length);
			return _Handle[index];
		}
		//逆向检索字串。
		inline wchar_t& RevElement(uint index){
			_ASSERT(index<_Length);
			return _Handle[_Length-index-1];
		}
		/*
		格式化规定符
		━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		符号 作用
		────────────────────────────────────────
		%d 格式化为十进制有符号整数输出到缓冲区
		%u 格式化为十进制无符号整数输出到缓冲区
		%f 格式化为浮点数输出到缓冲区
		%s 格式化为字符串输出到缓冲区
		%c 格式化为单个字符输出到缓冲区
		%e 格式化为指数形式的浮点数输出到缓冲区
		%x 格式化为无符号以十六进制表示的整数(a-f小写输出)输出到缓冲区
		%X 格式化为无符号以十六进制表示的整数(a-f大写输出)输出到缓冲区
		%0 格式化为无符号以八进制表示的整数输出到缓冲区
		%g 格式化为自动选择合适的表示法输出到缓冲区
		━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		说明:
		(1). 可以在"%"和字母之间插进数字表示最大场宽。
		例如: %3d 表示输出3位整型数, 不够3位右对齐。
		%9.2f 表示输出场宽为9的浮点数, 其中小数位为2, 整数位为6,小数点占一位, 不够9位右对齐。而且四舍五入。
		%8s 表示输出8个字符的字符串, 不够8个字符右对齐。
		如果字符串的长度、或整型数位数超过说明的场宽, 将按其实际长度输出。但对浮点数, 若整数部分位数超过了说明的整数位宽度, 将按

		实际整数位输出;若小数部分位数超过了说明的小数位宽度, 则按说明的宽度以四舍五入输出。
		另外, 若想在输出值前加一些0, 就应在场宽项前加个0。
		例如: %04d 表示在输出一个小于4位的数值时, 将在前面补0使其总宽度为4位。
		如果用浮点数表示字符或整型量的输出格式, 小数点后的数字代表最大宽度,小数点前的数字代表最小宽度。
		例如: %6.9s 表示显示一个长度不小于6且不大于9的字符串。若大于9, 则第9个字符以后的内容将被删除。
		(2). 可以在"%"和字母之间加小写字母l, 表示输出的是长型数。
		例如: %ld 表示输出long整数
		%lf 表示输出double浮点数
		(3). 可以控制输出左对齐或右对齐, 即在"%"和字母之间加入一个"-" 号可说明输出为左对齐, 否则为右对齐。
		例如: %-7d 表示输出7位整数左对齐
		%-10s 表示输出10个字符左对齐
		(4) %#.16g 输出16位精度，自动调整宽度 double 数据*/
		int Format(LPCWSTR fms,...);
		//把二进制数据以16进制输出，每字节占2位。
		void HexBinary(const void* data,int len);
		//把16进制字串输出为2进制数据，data=NULL返回需要的缓存大小。如果bufsize小于需要的大小，只有bufsize字节被填充。这个函数会自动剔除字串中非16进制数字的字符，而且字串不能有0x头（0会被保留，x被剔除）。
		int ToBinary(void* data,int bufsize);
		//把16进制字串输出为2进制数据
		int ToBinary(Memory<char>& data,int pos = 0);
		//把字串解析为数字：radix——基，如果字串不能解析为数字，则返回能解析部分的结果，初始值是 0 .
		int ToInt32(int radix);
		//把字串解析为数字：radix——基，如果字串解析出现错误，返回false，但v可能仍然被赋值。
		bool ToInt32(int& v,int radix);
		//把数字按指定基表示成字串：radix——（2-36），sing——10进制是是否转成负数。
		void FromInt32(int v,int radix,bool sign = true);
		//转换为64位整数
		__int64 ToInt64(int radix);
		//转换为64位整数
		bool ToInt64(__int64& v,int radix);
		//表示64位整数
		void FromInt64(__int64 v,int radix,bool sign = true);
		//转换为小数，这个函数的反函数是 Format 函数。
		double ToFloat();
		//转换为小数，这个函数的反函数是 Format 函数。
		bool ToFloat(double& v);
		//返回容量
		inline uint Cubage(){return _Cubage;}
		//返回字串长度,不包含末尾的NULL字符.
		inline uint Length(){return _Length;}
		//字串为空串.
		inline bool IsNull(){return (_Length==0);}
		//字串非空串。
		inline bool NotNull(){return (_Length!=0);}
		//释放字串的内存。
		void Release();
		//
		void Trim();
		void TrimLeft();
		void TrimRight();
	};
	class CONCISE_API StringList : public _class
	{
	protected:
		ObjectList<String> _Strs;
	public:
		//添加一个字串,index是添加的位置索引.
		inline uint Add(LPCWSTR wstr,uint index = -1){return AddSub(wstr,-1,index);}
		//把字串的一段添加到实例中,length是位置索引,-1表示末尾，返回新添加元素的索引.
		uint AddSub(LPCWSTR wstr,uint length = -1,uint index = -1);
		//删除指定数量的字串,count = -1删除至末尾.
		uint Delete(uint index,uint count = 1){return _Strs.Delete(index,count);}
		//删除指定字串,如果指定字串不在实例中,则返回False.
		bool Delete(LPCWSTR str,uint start = 0);
		//清空实例中的和rs相同的字串.
		uint Clear(LPCWSTR rs);
		//以separator字串为分界分割一个字串(wstr),并且添加到实例中,返回添加的元素数.
		uint SplitString(LPCWSTR wstr,LPCWSTR separator,uint index = -1);
		//this function only add a element when a sub string(enven "") in the
		//pre of separator,sub string after separator will not added. 
		//if wstr not include separator,no element will be added.separator can't
		//be null or "",in debug mode the value will raise a exception.and return 0.
		uint SplitStandString(LPCWSTR wstr,LPCWSTR separator,uint index = -1);
		//处理以NULL字符分隔的一段内存（首字符不能是NULL，否則函數返回 0）,这段内存中連續2個NULL字符,回事添加終止，此時可以设置length=-1,否则不要设为-1,返回添加的元素数.
		uint AddFromMemory(LPCWSTR handle,uint length,uint index = -1);
		//清空实例.
		void Clear(){_Strs.Clear();}
		//返回相应索引的字串.
		inline String& operator [](uint index){return _Strs[index];}
		//返回相应索引的字串,index 越界會出錯.
		inline String& Element(uint index){return *_Strs.Element(index);}
		//逆向检索字串。
		inline String& RevElement(uint index){return *_Strs.RevElement(index);}
		//返回相应字串在实例中的索引(从start开始),如果字串不在实例中,返回-1.
		uint IndexOf(LPCWSTR str,uint start = 0);
		//返回实例中字串的个数.
		inline uint Count(){return _Strs.Count();}
	};
	class CONCISE_API StringDiction : public StringList
	{
	public:
		//添加成功，返回添加项的索引，字串如果已经存在，添加失败，并且返回 -1。
		uint Add(LPCWSTR wstr,uint index = -1){return AddSub(wstr,-1,index);}
		//把字串的一段添加到实例中,length是位置索引,-1表示末尾，返回新添加元素的索引，字串已经存在，返回-1.
		uint AddSub(LPCWSTR wstr,uint length,uint index = -1);
		//以separator字串为分界分割一个字串(wstr),并且添加到实例中,返回添加的元素数.
		uint SplitString(LPCWSTR wstr,LPCWSTR separator,uint index = -1);
		//处理以NULL字符分隔的一段内存（首字符不能是NULL，否則函數返回 0）,这段内存中連續2個NULL字符,回事添加終止，此時可以设置length=-1,否则不要设为-1,返回添加的元素数.
		uint AddFromMemory(LPCWSTR handle,uint length,uint index = -1);
	};
	//StringMemList保存一系列的字串,用NULL字符分开,注意实例指针末尾并不是双NULL字符.
	class CONCISE_API StringMemList : public _class
	{
	protected:
		LPWSTR _Handle;
		uint _Size,_Length;
		IntegerList<LPWSTR> _Strs;
	public:
		inline LPWSTR Handle(){return _Handle;}
		StringMemList();
		~StringMemList();
		StringMemList(uint number);
		//添加一个字串,index是添加的位置索引.
		inline bool Add(LPCWSTR wstr,uint index = -1){return AddSub(wstr,-1,index);}
		//把字串的一段添加到实例中,length是位置索引,-1表示末尾.
		bool AddSub(LPCWSTR wstr,uint length = -1,uint index = -1);
		//删除指定数量的字串,count = -1删除至末尾.
		uint Delete(uint index,uint count = 1);
		//删除指定字串,如果指定字串不在实例中,则返回False.
		bool Delete(LPCWSTR str,uint start = 0);
		//下面三个函数返回添加的元素数.
		//以separator字串为分界分割一个字串(wstr),并且添加到实例中.
		uint SplitString(LPCWSTR wstr,LPCWSTR separator,uint index = -1);
		//处理以NULL字符分隔的一段内存,如果lenth=0，函数不加入任何元素，即使加入空串，也要传length = 1
		uint AddFromMemory(LPCWSTR handle,uint length,uint index = -1);
		//清空实例.
		void Clear();
		//清空实例中的和rs相同的字串.
		uint Clear(LPCWSTR rs);
		//返回相应索引的字串.
		inline LPCWSTR operator [](uint index){return _Strs[index];}
		//返回相应索引的字串,并且有安全检查,索引超界返回0.
		inline LPCWSTR Element(uint index){
			return _Strs[index];
		}
		//逆向检索字串。
		inline LPCWSTR RevElement(uint index){return _Strs.RevElement(index);}
		//返回相应字串在实例中的索引(从start开始),如果字串不在实例中,返回-1.
		uint IndexOf(LPCWSTR str,uint start = 0);
		//如果对实例的内存进行了直接的操作,就应该调用这个函数.这个函数可以释放多占用的内存.
		void Realize();
		//清空字串,并且释放所有内存.
		void Release();
		//设置实例的容量(所有字串和NULL字符的长度和).如果设置的长度小于实际长度,返回false.
		bool SetSize(uint number);
		//返回实例空间占用的内存大小（双字节）.
		inline uint Cubage()const{return _Size;}
		//返回实例中使用的内存长度（双字节）.
		inline uint Length()const{return _Length;}
		//返回实例中字串的个数.
		inline uint Count()const{return _Strs.Count();}
	};
	//这个类只保存不同的字串.
	class CONCISE_API StringMemDiction : public StringMemList
	{
	public:
		StringMemDiction();
		~StringMemDiction();
		bool AddFromMemory(LPCWSTR handle,uint length,uint index = -1);
		//返回
		bool Add(LPCWSTR str,uint* index = 0);
	};
};
