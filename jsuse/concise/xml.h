/* 精简的XML文件解析类，有如下3个要素：节点名称，节点属性集，节点子节点集。
节点属性集的元素是一系列的节点属性对象，节点属性是一个有属性名称和属性值两
个字串成员的结构，子节点集的元素是一系列的Xml对象。节点名称，节点属性名称
不能含字符：空格，TAB（“\t”），回车“\r”，换行“\n”，“"”，“=”，“<”，
“>”；节点属性值不能含“"”字符*/

#pragma once
namespace concise{
	class CONCISE_API  Xml : public TPTree<Xml>{
	protected:
		typedef struct _prop:public _struct{String Name;String Value;}prop;
		String name;
		String value;
		ObjectList<prop>	atrb;

		uint _FromString(LPCWSTR str,uint len);
		void _ToString(String& ns,int Indent = 0);
	public:

		//返回 Xml 节点的名称。
		LPCWSTR GetName(){return name;}
		//设置 Xml 节点的名称，名称不能含：空格，TAB（“\t”），回车“\r”，换行“\n”，“"”，“=”，“<”，“>”。
		bool SetName(LPCWSTR name);
		//返回 Xml 节点的值。
		LPCWSTR GetValue(){return value;}
		//设置 Xml 节点的值，值不能含：“<”，“>”，如果以回车换行开头则会被移除，开头的空格和tab字符也会被移除。
		bool SetValue(LPCWSTR val);
		//设置节点值为整数
		void SetValueInt(int v,int radix = 16);
		//获取节点的整数值，如果此值包含非10进制整数字符，返回false，但是v仍然可能被赋值能够识别的部分。
		bool GetValueInt(int& v,int radix = 16);
		//设置节点值为double值，fwidth指定小数的位数，并且四舍五入。
		void SetValueDouble(double v,int fwidth = -1);
		//获取节点的double值，如果此值不能转化为double，返回 0 ，但是v仍然被赋值可识别的部分。
		bool GetValueDouble(double& v);
		//设置指定的值为数据的16进制文本表示。
		void SetValueHex(const void* data,int len);
		//获取节点的二进制值，如果此值不能转化为数值，返回 0 ,设置data为NULL，返回需要的内存长度。
		int GetValueHex(void* data);

		//返回节点属性列表中指定索引的属性名称
		LPCWSTR GetPropertyName(int index);
		//返回节点属性列表中指定索引的属性值，如果属性不存在，返回 0.因为任何值都是字串表示的，所以这个函数可以取回任何类型的值，不仅仅是字串类型
		LPCWSTR GetProperty(int index);
		//返回节点指定名声属性的值，名称不能含：空格，TAB（“\t”），回车“\r”，换行
		//“\n”，“"”，“=”，“<”，“>”。如果指定名称属性不存在，返回 0 。
		LPCWSTR GetProperty(LPCWSTR pName);
		//设置节点属性，如果此属性不存在，自动添加它。
		bool SetProperty(LPCWSTR pName,LPCWSTR pValue);
		//返回指定节点属性集元素个数。
		uint GetPropertyCount(){return atrb.Count();}
		//设置节点属性值为整数
		bool SetPropertyInt(LPCWSTR name,int v,int radix = 16);
		//获取节点属性的整数值，或者指定名称的属性不存在，返回 0 ，如果此值不能转化为整数，返回能转化的字串的结果。
		bool GetPropertyInt(LPCWSTR name,int& v,int radix = 16);
		//设置节点属性值为double值，fwidth指定小数的位数，并且四舍五入。
		bool SetPropertyDouble(LPCWSTR name,double v,int fwidth = -1);
		//获取节点属性的double值，如果此值不能转化为double，或者指定名称的属性不存在，返回 0 。
		bool GetPropertyDouble(LPCWSTR name,double& v);
		//设置指定属性的值为数据的16进制文本表示。
		bool SetPropertyHex(LPCWSTR name,const void* data,int len);
		//获取节点属性的二进制值，如果此值不能转化为数值，或者指定名称的属性不存在，返回 0 。
		//设置data为NULL，返回需要的内存长度。
		int GetPropertyHex(LPCWSTR name,void* data);
		//删除所有属性
		void ClearProperty();
		//删除指定名称的属性
		bool DelProperty(LPCWSTR name);

		//返回指定名称的子节点，如果always=1，则当子节点不存在是，创建它，否则返回 0 。
		Xml* GetNode(LPCWSTR name,bool always = 0);
		//添加一个指定名称的新节点到子节点列表。
		Xml* AddNode(LPCWSTR name,int index = -1);
		//添加一个指定名称和值的新节点到子节点列表。
		Xml* AddNode(LPCWSTR name,LPCWSTR val,int index = -1);
		//删除指定名称的子节点。
		bool DelNode(LPCWSTR name,int start = 0);

		//返回特定名称子节点的值，当 openalways = 1 时，如果不存在指定名称的子节点则生成它，否则返回 0 。
		//ns是一个you分隔的类似于注册表项名称的字串，但是首段不包含根节点的名称（因为一个TreeLink对象只有一个根节点）。
		//Name和Value屬性可以含任意字符，但是，如果Name里含有字符"/"則ns會把它作為分割符，所以不要使用含有字符“/”的Name。
		Xml* OpenMultiLevelNode(LPCWSTR ns,bool openalways = 0);
		//删除指定名称的（多级）子节点。
		bool DeleteMultiLevelNode(LPCWSTR ns);

		//此函数返回0可能表示已经正确解析了部分字串
		bool FromString(LPCWSTR str);
		//把xml转换为字串，它自动插入回车换行符和2个空格的缩进。
		void ToString(String& ns);
		//把xml保存为文件，它自动插入回车换行符和2个空格的缩进。
		bool Save(LPCWSTR file,LPCWSTR cp = L"UTF-8");
		//加载一个特点编码的xml文件，使用CodePage类获取指定名称的编码
		bool Load(LPCWSTR file,DWORD cp = CP_UTF8);
		//从资源加载，使用CodePage类获取指定名称的编码。
		bool Load(ResID id,DWORD cp = CP_UTF8);
	};
	//RegisterTree用于把信息以文本的方式保存为xml文件.
	class CONCISE_API RegisterTree : public _class{
	protected:
		void* _innerobj; //it's a register_struct pointer.
	public:
		//此时并不实例化内部的 Xml 对象
		RegisterTree();
		//实例化一个 Xml 对象，绑定file文件，并且阻塞这个进程其它RegisterTree对象对file的使用。
		RegisterTree(LPCWSTR file);
		//打开指定文件读写, 内部会创建一个对象, 并且读取file文件的内容来初始化这个对象.
		bool Create(LPCWSTR file);
		//写操作前, 应该调用Lock, 独占内部对象, 后续的Lock将会被挂起, 直到调用Unlock函数.
		bool Lock();
		//解除Lock, 因为为了保持文件内容的一致, 同一时间只能有一个打开操作, Unlock会自动调用Flush。
		void Unlock();
		//彻底释放Create操作打开的对象, 当外部程序改变了文件的内容时.
		//需要Close后, 重新用Create打开, 内容才会刷新, 因为内部对象一旦生成就不会再从文件读取内容.
		void Close();
		//写入文件, Unlock也会执行这个操作, 所以除非在还没调用Unlock函数但是需要把信息写入文件时, 才需要调用这个函数。
		void Flush();
		//返回内部的Xml对象, 进行读写操作.
		Xml* GetXml();
		//析构
		~RegisterTree();
	};
	//RegisterTree用于把信息以文本的方式保存为xml文件.
}