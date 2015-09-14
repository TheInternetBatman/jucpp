#pragma once
namespace cs{
	class CONCISE_API Registry : public HandleType<HKEY>{
	public:
		enum REGRESULT{
			REG_ERROR		=	0,	//函数失败
			REG_CREATE		=	1,	//生成了新的键
			REG_OPEN		=	2	//打开了原有的键
		};
		Registry(void);
		~Registry(void);
		//绑定一个新的注册表键句柄,原来的键会被关闭(如果已经打开一个注册表键的话).
		bool Attach(HKEY handle);
		//取消绑定.
		HKEY Detach();
		//生成或打开(指定的键已经存在)一个注册表键,返回值 : REG_ERROR，打开失败;REG_CREATE，生成了一个新的键;REG_OPEN，打开了一个已经存在的键.
		//subKey：要打开的子键名称；key：已经打开的键或者根；create：如果键不存在，是否生产它；access：设置以何种权限打开；option：打开选项
		REGRESULT Create(LPCWSTR subKey,HKEY key = HKEY_CURRENT_USER,bool create = 0,REGSAM access = KEY_ALL_ACCESS,DWORD option = REG_OPTION_NON_VOLATILE);
		//和Create的唯一区别是, 键名可以是一个通过斜杠或反斜杠分隔的表示多级子键的字串, 注册表键名是可以含斜杠(反斜杠)的, 所以这个方法无法打开含斜杠(反斜杠)的键。
		REGRESULT MultiCreate(LPCWSTR multikey,HKEY key = 0,bool create = 0,REGSAM access = KEY_ALL_ACCESS,DWORD option = REG_OPTION_NON_VOLATILE);
		//打开当前键的一个子键,并且绑定它.
		REGRESULT ToSubKey(LPCWSTR subKey,bool create = 0,REGSAM access = KEY_ALL_ACCESS,DWORD option = REG_OPTION_NON_VOLATILE);
		//此函数和ToSubKey的区别是键名可以是用斜杠(反斜杠)分隔的表示多级子键字串.
		REGRESULT ToMultiSubKey(LPCWSTR multiKey,bool create = 0,REGSAM access = KEY_ALL_ACCESS,DWORD option = REG_OPTION_NON_VOLATILE);
		//关闭实例绑定的注册表键.
		bool Close();
		//取得项的数据,尽量初始化data的空间大小,如果知道数据的长度.如果不知道数据的长度,也可以不初始化data的大小.type返回数据的类型.
		bool GetValue(LPCWSTR valueName,Memory<BYTE>* data,OUT LPDWORD type = NULL);
		//设置项的数据.
		bool SetValue(LPCWSTR valueName,BYTE* data,uint len,IN DWORD type);
		//设置Int值.
		bool SetIntValue(LPCWSTR valueName,DWORD value);
		//返回项的整数值.
		bool GetIntValue(LPCWSTR valueName,DWORD& val);
		//设置字符串值.
		bool SetStrValue(LPCWSTR valueName,LPCWSTR wstr);
		//取得一个字符串值.
		bool GetStrValue(LPCWSTR valueName,String& wstrValue);
		//枚举子键的名称.
		int EnumSubKey(StringMemList* subkeys);
		//枚举键的项的名称.
		int EnumValueName(StringMemList* values);
		//枚举当前键的字符串类型项的名称和值.
		int EnumStrValue(StringMemList* names,StringMemList* values);
		//枚举当前键的整数类型项的名称和值.
		int EnumIntValue(StringMemList* names,IntegerList<DWORD>* values);
		//把缓存刷新到文件.
		LONG Flush();
		//删除指定名称的子键以及它之下的所有内容。
		bool DelSubKey(LPCWSTR sub);
		//删除键下的值。
		bool DelValue(LPCWSTR name);
	};
	/*这个类从注册表读取系统的 CodePage 信息*/
	class CONCISE_API CodePage : public _class{
	protected:
		Registry reg;
	public:
		bool Create();
		//返回注册表中全部的 CodePage 名称，这些名称有些是别称，对应的 CodePage 值可以重复。
		int GetAllCodePage(StringMemList* sml);
		//返回指定名称的码表值，0 表示没有指定名称的代码页，名称不区分大小写。
		DWORD GetCode(LPCWSTR cpName);
		bool Close();
	};
	//从名称取得代码页, 这个函数内部维护一个列表, 如果遇到没有的名称回去查询注册表,
	//如果已经查询成功的代码页名称, 会存储在列表中, 下次直接返回, 空串返回 CP_THREAD_ACP(3), 这个函数比使用 CodePage 要快, 它内置了几个基本的代码页信息, CodePage 的 Create 函数会从注册表读取全部的代码也信息.
	DWORD CONCISE_API GetCodePage(LPCWSTR name);
}