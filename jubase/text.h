#pragma once
namespace ju{
	//�����ǿ��ַ��ִ�����.

	//ת��ΪСд��ĸ
	JUBASE_API void WcsLower(wchar_t* wcs);
	//ת��Ϊ��д��ĸ.
	JUBASE_API void WcsUpper(wchar_t* wcs);
	//ȡ���ִ����ַ�����,(��ͷ����һ��Ϊ����ַ�����).maxlen�Ǽ������Χ.
	JUBASE_API uint WcsLength(const wchar_t* wcs,uint maxlen);
	//ȡ���ִ����ַ�����,(��ͷ����һ��Ϊ����ַ�����).maxlen�Ǽ������Χ.
	JUBASE_API uint WcsLength(const wchar_t* wcs);
	//�ִ�����,length�ǿ���ָ���ĳ���.
	JUBASE_API uint WcsCopy(wchar_t* ds,const wchar_t* sc,uint length = -1);
	//�ж������ַ��Ƿ���ͬһ�ַ�,�����Ǵ�Сд.
	JUBASE_API bool SameCase(wchar_t ch1,wchar_t ch2);
	//�ж����ִ��Ƿ���ͬ.���Ƚ�lengthλ
	JUBASE_API bool WcsEqual(const wchar_t* str1,const wchar_t* str2,uint length = -1);
	//WcsCompare�����Ƚ������ִ�ǰ���ַ���ͬ�ĸ���.���Ƚ�lengthλ.���length����ĳһ�ִ��ĳ���,��ֻ�ȽϽ϶��ִ����ȸ��ַ�.
	JUBASE_API uint WcsCompare(const wchar_t* str1,const wchar_t* str2,uint length = -1);
	//�ж�һ���ִ�(wcs)��ǰ����λ�Ƿ����һ�ִ�(sub)��ͬ.	
	JUBASE_API bool WcsInclude(const wchar_t* wcs,const wchar_t* sub);
	//�ж����ִ��Ƿ���ͬ.���Ƚ�lengthλ.�����Ǵ�Сд. str1 = NULL,str2 = NULL,return true.
	JUBASE_API bool WcsEqualNoCase(const wchar_t* str1,const wchar_t* str2,uint length = -1);
	//WcsCompareNoCase������ȡ�����ִ�ǰ���ַ���ͬ�������ִ�Сд���ĸ���.
	//���Ƚ�lengthλ�����length����ĳһ�ִ��ĳ��ȣ��ᱻ����Ϊ�϶��ִ�����.
	JUBASE_API uint WcsCompareNoCase(const wchar_t* str1,const wchar_t* str2,uint length = -1);
	//�ж�һ���ִ�(wcs)��ǰ����λ�Ƿ����һ�ִ�(sub)��ͬ.	�����Ǵ�Сд.
	JUBASE_API bool WcsIncludeNoCase(const wchar_t* wcs,const wchar_t* sub);
	//��ת�ִ�.
	JUBASE_API void WcsReverse(wchar_t* string);
	//��һ���ִ��в�����һ���ִ�,���ز��ҵ���λ�û�-1(û�в��ҵ��Ļ�)
	JUBASE_API uint WcsFind(const wchar_t* wcs,const wchar_t* sub,uint maxlen = -1);
	//��һ���ִ��в�����һ���ִ�,������ĸ�Ĵ�Сд,���ز��ҵ���λ�û�-1(û�в��ҵ��Ļ�)
	JUBASE_API uint WcsFindNoCase(const wchar_t* wcs,const wchar_t* sub,uint maxlen = -1);

	//�����Ƕ��ֽ��ִ���صĺ���.

	//�����ִ����ֽڸ���,������NULL��β�ַ�,bytes�Ǽ��������ֽ�λ��.
	JUBASE_API uint MbsByteLen(const char* str,uint bytes);
	//�����ַ�����,�����ִ����ֽڸ���,������NULL��β�ַ�,bytes�Ǽ��������ֽ�λ��.
	JUBASE_API uint MbsCharLen(const char* str,uint bytes);
	//����ռ���ֽڸ���
	JUBASE_API uint MbsByteLen(const char* str);
	//�����ַ�����.
	JUBASE_API uint MbsCharLen(const char* str);
	//�����ַ�����Ϊchars���ַ����ֽ�����λ��.(��Ϊ��Щ�ַ�ռ�����ֽ�,���Է���ֵ���Ǵ���chars).���charsΪ��ֵ
	//������ִ����ַ�����,�����ִ����ֽڳ���.(��MbsCharLenЧ��һ��).Ҳ����˵�����Ƿ���һ���Ϸ����ַ�����.
	JUBASE_API uint MbsByteOfChar(const char* str,uint chars);
	//�����ִ��Ƿ���ͬ.ֻ�Ƚ�maxlenλ.
	JUBASE_API bool MbsEqual(const char* str,const char* str2,uint maxlen = -1);
	//�����ִ��Ƿ���ͬ.
	JUBASE_API bool MbsEqualNoCase(const char* str,const char* str2,uint maxlen = -1);
	//���ִ�str�в����ִ�find,���ز��ҵ����ֽ�����λ��,����,����-1;
	JUBASE_API uint MbsFind(const char* str,const char* find,uint maxlen = -1);
	//���ִ�str�в����ִ�find,���ز��ҵ����ֽ�����λ��,����,����-1;�����Ǵ�Сд.
	JUBASE_API uint MbsFindNoCase(const char* str,const char* find,uint maxlen = -1);
	//��ת�ִ�.
	JUBASE_API void MbsReverse(char* str);
	//���һ���ִ��Ƿ���utf8���룬len ָ�����ĳ��ȣ�ĳЩʱ��Ϊ�˼ӿ��ٶȿ���ֻ��Ҫ���һ���֣���strictָ�����ģʽ��utf-8 �ִ��ض����п���β������һ���������ַ����� strict = false ʱ����������������������utf-8�ִ����ضϣ�һ������ false��
	JUBASE_API bool MbsIsUtf8(LPCSTR str,uint len = -1,bool strict = false);
	/*Window NT���ĵĲ���ϵͳ, �ڵײ�֧��UNICODE�ַ�,�����˵,���е�ANSI�汾��API������
	�ִ�������Ҫ����һ������ת����UNICODE�ַ�,�ٵ���UNICODE�汾��API����.���Ҳ���UNICODE�ִ�
	�Ȳ������ֽ��ִ�Ҫ����Ķ�,�����ڴ���������Ӧ��ʹ��UNICODE�ִ�,Ҳ����String�ִ���. ��
	��, ��ĳЩʱ��,��Ȼ�벻���ֽ��ַ���,��������ı��ļ�.������ı��ļ�����ʹ�ö��ֽڱ���.

	class String һ��δ�������ǿ��ַ�����ʼ���ĵ� String ʵ���ľ���� NULL .
	StringĬ��ʹ��Fast�ڴ淽ʽ, ���������Ҫ���ִ�, ����sys����Ϊ true*/
	class JUBASE_API String : public HandleType<LPWSTR>{
	protected:
		uint _Length,_Cubage;//Size--the size of the memory,Number--the caracter number of this string;
	public: 
		String();
		//����һ��ָ����ʼ�������ַ���ʵ��,cubage = 0, ��Ԥ�ȷ����ڴ�, ֱ��������ֵ.
		String(uint cubage);
		//��һ���ִ�����ʼ��ʵ��.
		String(LPCWSTR wstr);
		//= ������ʹ�õĺ���.
		String(String& wstr);
		String(LPCSTR mbs);
		~String(){
			if(_Handle) MemoryFree(_Handle);
		}
		//��.
		bool Attach(LPWSTR wstr);
		//�����
		LPWSTR Detach();
		//��UNICODE��MULTIBYTE֮�����ת��,������ݲ��ǺϷ��ִ�,����ת���Ի����,�����ܻᶪʧĳЩ�ֽ�,���������ض�����,����NULL�ַ�.
		//������������ַ����޷���ȷ��������,��Ӧ���Լ�������,��Ҫ����Realize����,��������NULL�ַ�ʱ���������������.
		//���� codepage = 0�����Զ�ʶ���bomͷ��utf-8������ִ���
		bool FromMultiByte(LPCSTR str,uint length = -1,DWORD codepage = CP_THREAD_ACP);
		//length��buffer�Ĵ�С,�������������β��NULL�ַ�.�������ʧ�ܣ����� 0 .���length̫С�����Ա���ȫ�����ַ���������β��NULL�ַ�����
		//�������� 0 �����ǣ�ʵ����ת���ǳɹ��ģ�ֻ����ֻת����ǰ���ɸ��ַ�����bufferָ���ĳ���������
		//���buffer��0, ����ת�������ִ�������ֽڿռ�, ����ռ䲻������β��NULL�ַ�,codepade ȱʡ = CP_THREAD_ACP��3��
		uint ToMultiByte(LPSTR buffer,uint length,DWORD codepage);
		//���ַ�ת��Ϊ Mulitybyte �ַ�����������β��NULL�ַ������buffer̫С�������Դ�������ַ������Զ�������
		//�������ת�����ַ�����������β��NULL�ַ�, ���ǻᱣ�� buffer ��β��һ��NULL�ַ��Ŀ�λ.
		uint ToMultiByte(Memory<char>& buffer,DWORD codepage = CP_THREAD_ACP);//
		//��ʵ���е��ַ�������wstr��,��start��ʼ,num��Ҫ����������.num=-1��ʾ������ĩβ.
		//wstr�������㹻�Ŀռ������ַ�,����ʵ�ʿ������ַ���.
		uint CopyTo(wchar_t* wstr,uint start = 0,uint num = -1);
		//��ʵ����ֵ��Ϊwstr��ǰnum���ַ������numС��0���򿽱�wstr�������ִ�������ʾ�����ַ����ȡ�
		uint CopyFrom(LPCWSTR wstr,uint num = -1,uint pos = -1);
		//ת��ΪСд
		void Lower(void);
		//ת��Ϊ��д.
		void Upper(void);
		//��ת�ִ�����.
		void Reverse();
		//��Ϊ���Բ���ʵ�����ڴ����Կ����������������ԭʵ���еĳ�Ա�����Ϸ�.
		void Realize(bool resetsize = 0);
		//����ʵ��������.
		virtual bool SetCubage(uint cubage,bool dbSpace = 0);
		//��ʵ���в����ַ�,����-1���������.
		uint Find(wchar_t wch,uint start = 0,uint maxlen = -1);
		//��ʵ���в����ִ�.
		uint Find(LPCWSTR wstr,uint start = 0,uint maxlen = -1);
		//��ʵ���в����ַ�,����-1���������.
		uint FindNoCase(wchar_t wch,uint start = 0,uint maxlen = -1);
		//��ʵ���в����ִ�.
		uint FindNoCase(LPCWSTR wstr,uint start = 0,uint maxlen = -1);
		//�����ִ�,Ĭ�Ϸ��ڽ�β.length�ǲ�����ַ�����.
		uint Insert(LPCWSTR wstr,uint pos = -1,uint length = -1);
		//�����ַ�.
		void Insert(wchar_t wc,uint pos = -1);
		//��ָ��λ�õ��ִ�(��pos��ʼ,����Ϊlength)�滻��ws,pos = -1��ʾ��ĩβ�û�(ʵ�����ǲ���);length = -1��ʾ��pos��ĩβȫ���û�.���ز����ִ��ĳ���.
		uint Replace(LPCWSTR ws,uint pos = -1,uint length =-1);
		//��ȡ�ִ���ͷ����index�ǽضϵ�λ�ã����Ϊ������0����Ϊͨ��������˳�����Ϊ��������ĩβ����Ϊ0����ǰ����Ϊ-1��-2��������
		void Left(int index);
		//��ȡ�ִ���β����index�ǽضϵ�λ�ã����Ϊ������0����Ϊͨ��������˳�����Ϊ��������ĩβ����Ϊ0����ǰ����Ϊ-1��-2��������
		void Right(int index);
		void Sub(int start, uint length);
		void operator = (String& wstr);
		void operator = (LPCWSTR wstr);
		void operator = (LPCSTR str);
		bool operator == (String& wstr);
		bool operator != (String& wstr);
		inline bool operator == (LPCWSTR wstr){return WcsEqual(_Handle,wstr);}
		inline bool operator != (LPCWSTR wstr){return !WcsEqual(_Handle,wstr);}
		void operator += (LPCSTR mbs);
		void operator += (LPCWSTR wstr);
		void operator += (String& wstr);
		String operator + (LPCWSTR str);
		String operator + (LPCSTR str);
		void InsertNumber(INT_PTR number,int index = -1);
		inline wchar_t& Element(uint index){
			_ASSERT(index<_Length);
			return _Handle[index];
		}
		//��������ִ���
		inline wchar_t& RevElement(uint index){
			_ASSERT(index<_Length);
			return _Handle[_Length-index-1];
		}
		/*
		��ʽ���涨��
		��������������������������������������������������������������������������������
		���� ����
		��������������������������������������������������������������������������������
		%d ��ʽ��Ϊʮ�����з������������������
		%u ��ʽ��Ϊʮ�����޷������������������
		%f ��ʽ��Ϊ�����������������
		%s ��ʽ��Ϊ�ַ��������������
		%c ��ʽ��Ϊ�����ַ������������
		%e ��ʽ��Ϊָ����ʽ�ĸ����������������
		%x ��ʽ��Ϊ�޷�����ʮ�����Ʊ�ʾ������(a-fСд���)�����������
		%X ��ʽ��Ϊ�޷�����ʮ�����Ʊ�ʾ������(a-f��д���)�����������
		%0 ��ʽ��Ϊ�޷����԰˽��Ʊ�ʾ�����������������
		%g ��ʽ��Ϊ�Զ�ѡ����ʵı�ʾ�������������
		��������������������������������������������������������������������������������
		˵��:
		(1). ������"%"����ĸ֮�������ֱ�ʾ��󳡿�
		����: %3d ��ʾ���3λ������, ����3λ�Ҷ��롣
		%9.2f ��ʾ�������Ϊ9�ĸ�����, ����С��λΪ2, ����λΪ6,С����ռһλ, ����9λ�Ҷ��롣�����������롣
		%8s ��ʾ���8���ַ����ַ���, ����8���ַ��Ҷ��롣
		����ַ����ĳ��ȡ���������λ������˵���ĳ���, ������ʵ�ʳ�����������Ը�����, ����������λ��������˵��������λ���, ����

		ʵ������λ���;��С������λ��������˵����С��λ���, ��˵���Ŀ�����������������
		����, ���������ֵǰ��һЩ0, ��Ӧ�ڳ�����ǰ�Ӹ�0��
		����: %04d ��ʾ�����һ��С��4λ����ֵʱ, ����ǰ�油0ʹ���ܿ��Ϊ4λ��
		����ø�������ʾ�ַ����������������ʽ, С���������ִ��������,С����ǰ�����ִ�����С��ȡ�
		����: %6.9s ��ʾ��ʾһ�����Ȳ�С��6�Ҳ�����9���ַ�����������9, ���9���ַ��Ժ�����ݽ���ɾ����
		(2). ������"%"����ĸ֮���Сд��ĸl, ��ʾ������ǳ�������
		����: %ld ��ʾ���long����
		%lf ��ʾ���double������
		(3). ���Կ�������������Ҷ���, ����"%"����ĸ֮�����һ��"-" �ſ�˵�����Ϊ�����, ����Ϊ�Ҷ��롣
		����: %-7d ��ʾ���7λ���������
		%-10s ��ʾ���10���ַ������
		(4) %#.16g ���16λ���ȣ��Զ�������� double ����*/
		int Format(LPCWSTR fms,...);
		//�Ѷ�����������16���������ÿ�ֽ�ռ2λ��
		void HexBinary(const void* data,int len);
		//��16�����ִ����Ϊ2�������ݣ�data=NULL������Ҫ�Ļ����С�����bufsizeС����Ҫ�Ĵ�С��ֻ��bufsize�ֽڱ���䡣����������Զ��޳��ִ��з�16�������ֵ��ַ��������ִ�������0xͷ��0�ᱻ������x���޳�����
		int ToBinary(void* data,int bufsize);
		//��16�����ִ����Ϊ2��������
		int ToBinary(Memory<char>& data,int pos = 0);
		//���ִ�����Ϊ���֣�radix������������ִ����ܽ���Ϊ���֣��򷵻��ܽ������ֵĽ������ʼֵ�� 0 .
		int ToInt32(int radix);
		//���ִ�����Ϊ���֣�radix������������ִ��������ִ��󣬷���false����v������Ȼ����ֵ��
		bool ToInt32(int& v,int radix);
		//�����ְ�ָ������ʾ���ִ���radix������2-36����sing����10�������Ƿ�ת�ɸ�����
		void FromInt32(int v,int radix,bool sign = true);
		//ת��Ϊ64λ����
		__int64 ToInt64(int radix);
		//ת��Ϊ64λ����
		bool ToInt64(__int64& v,int radix);
		//��ʾ64λ����
		void FromInt64(__int64 v,int radix,bool sign = true);
		//ת��ΪС������������ķ������� Format ������
		double ToFloat();
		//ת��ΪС������������ķ������� Format ������
		bool ToFloat(double& v);
		//��������
		inline uint Cubage() const {return _Cubage;}
		//�����ִ�����,������ĩβ��NULL�ַ�.
		inline uint Length() const {return _Length;}
		//�ִ�Ϊ�մ�.
		inline bool IsNull(){return (_Length==0);}
		//�ִ��ǿմ���
		inline bool NotNull(){return (_Length!=0);}
		//�ͷ��ִ����ڴ档
		void Release();
		//
		void Trim();
		void TrimLeft();
		void TrimRight();
		uint getUsedMemory();
	};
#define LS_CUBAGE 255	//Ĭ�ϴ�С��
	class JUBASE_API LocalString : public String{
	protected:
		wchar_t _buf[LS_CUBAGE+1];
	public:
		LocalString();
		LocalString(uint cubage);
		LocalString(LPCWSTR ws,uint cubage = 0);
		LocalString(LocalString& ls);
		LocalString(String& str);
		LocalString(LPCSTR str);
		~LocalString(){
			if(_Handle==_buf) _Handle = 0;
		}
		bool SetCubage(uint capacity,bool dbSpace = 1);
		void operator = (LocalString& ls);
		void operator = (String& str);
		void operator = (LPCWSTR wstr);
		void operator = (LPCSTR mbs);
		void operator += (LocalString& ls);
		void operator += (LPCWSTR wcs);
		void operator += (String& str);
		void operator += (LPCSTR mbs){
			LocalString str = mbs;
			*this += str;
		}
		bool operator == (LocalString& str){
			if(str.Length()!=_Length) return 0;
			return memcmp(_Handle,str.Handle(),_Length)==0;
		}
		bool operator == (String& str){
			if(str.Length()!=_Length) return 0;
			return memcmp(_Handle,str.Handle(),_Length)==0;
		}
		bool operator == (LPCWSTR wcs);
		bool operator != (LPCWSTR wcs);
		bool operator != (LocalString& str){
			if(str.Length()!=_Length) return true;
			return memcmp(_Handle,str.Handle(),_Length)!=0;
		}
		bool operator != (String& str){
			if(str.Length()!=_Length) return true;
			return memcmp(_Handle,str.Handle(),_Length)!=0;
		}
	};
	class JUBASE_API StringList : public _class
	{
	protected:
		ObjectList<String> _Strs;
	public:
		//���һ���ִ�,index����ӵ�λ������.
		inline uint Add(LPCWSTR wstr,uint index = -1){return AddSub(wstr,-1,index);}
		//���ִ���һ����ӵ�ʵ����,length��λ������,-1��ʾĩβ�����������Ԫ�ص�����.
		uint AddSub(LPCWSTR wstr,uint length = -1,uint index = -1);
		//ɾ��ָ���������ִ�,count = -1ɾ����ĩβ.
		uint Delete(uint index,uint count = 1){return _Strs.Delete(index,count);}
		//ɾ��ָ���ִ�,���ָ���ִ�����ʵ����,�򷵻�False.
		bool Delete(LPCWSTR str,uint start = 0);
		//���ʵ���еĺ�rs��ͬ���ִ�.
		uint Clear(LPCWSTR rs);
		//��separator�ִ�Ϊ�ֽ�ָ�һ���ִ�(wstr),������ӵ�ʵ����,������ӵ�Ԫ����.
		uint SplitString(LPCWSTR wstr,LPCWSTR separator,uint index = -1);
		//this function only add a element when a sub string(enven "") in the
		//pre of separator,sub string after separator will not added. 
		//if wstr not include separator,no element will be added.separator can't
		//be null or "",in debug mode the value will raise a exception.and return 0.
		uint SplitStandString(LPCWSTR wstr,LPCWSTR separator,uint index = -1);
		//������NULL�ַ��ָ���һ���ڴ棨���ַ�������NULL����t�������� 0��,����ڴ����B�m2��NULL�ַ�,������ӽKֹ���˕r��������length=-1,����Ҫ��Ϊ-1,������ӵ�Ԫ����.
		uint AddFromMemory(LPCWSTR handle,uint length,uint index = -1);
		//���ʵ��.
		void Clear(){_Strs.Clear();}
		//������Ӧ�������ִ�.
		inline String& operator [](uint index){return _Strs[index];}
		//������Ӧ�������ִ�,index Խ������e.
		inline String& Element(uint index){return *_Strs.Element(index);}
		//��������ִ���
		inline String& RevElement(uint index){return *_Strs.RevElement(index);}
		//������Ӧ�ִ���ʵ���е�����(��start��ʼ),����ִ�����ʵ����,����-1.
		uint IndexOf(LPCWSTR str,uint start = 0);
		//����ʵ�����ִ��ĸ���.
		inline uint Count(){return _Strs.Count();}
	};
	class JUBASE_API StringDiction : public StringList
	{
	public:
		//��ӳɹ��������������������ִ�����Ѿ����ڣ����ʧ�ܣ����ҷ��� -1��
		uint Add(LPCWSTR wstr,uint index = -1){return AddSub(wstr,-1,index);}
		//���ִ���һ����ӵ�ʵ����,length��λ������,-1��ʾĩβ�����������Ԫ�ص��������ִ��Ѿ����ڣ�����-1.
		uint AddSub(LPCWSTR wstr,uint length,uint index = -1);
		//��separator�ִ�Ϊ�ֽ�ָ�һ���ִ�(wstr),������ӵ�ʵ����,������ӵ�Ԫ����.
		uint SplitString(LPCWSTR wstr,LPCWSTR separator,uint index = -1);
		//������NULL�ַ��ָ���һ���ڴ棨���ַ�������NULL����t�������� 0��,����ڴ����B�m2��NULL�ַ�,������ӽKֹ���˕r��������length=-1,����Ҫ��Ϊ-1,������ӵ�Ԫ����.
		uint AddFromMemory(LPCWSTR handle,uint length,uint index = -1);
	};
	//StringMemList����һϵ�е��ִ�,��NULL�ַ��ֿ�,ע��ʵ��ָ��ĩβ������˫NULL�ַ�.
	class JUBASE_API StringMemList : public _class
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
		//���һ���ִ�,index����ӵ�λ������.
		inline bool Add(LPCWSTR wstr,uint index = -1){return AddSub(wstr,-1,index);}
		//���ִ���һ����ӵ�ʵ����,length��λ������,-1��ʾĩβ.
		bool AddSub(LPCWSTR wstr,uint length = -1,uint index = -1);
		//ɾ��ָ���������ִ�,count = -1ɾ����ĩβ.
		uint Delete(uint index,uint count = 1);
		//ɾ��ָ���ִ�,���ָ���ִ�����ʵ����,�򷵻�False.
		bool Delete(LPCWSTR str,uint start = 0);
		//������������������ӵ�Ԫ����.
		//��separator�ִ�Ϊ�ֽ�ָ�һ���ִ�(wstr),������ӵ�ʵ����.
		uint SplitString(LPCWSTR wstr,LPCWSTR separator,uint index = -1);
		//������NULL�ַ��ָ���һ���ڴ�,���lenth=0�������������κ�Ԫ�أ���ʹ����մ���ҲҪ��length = 1
		uint AddFromMemory(LPCWSTR handle,uint length,uint index = -1);
		//���ʵ��.
		void Clear();
		//���ʵ���еĺ�rs��ͬ���ִ�.
		uint Clear(LPCWSTR rs);
		//������Ӧ�������ִ�.
		inline LPCWSTR operator [](uint index){return _Strs[index];}
		//������Ӧ�������ִ�,�����а�ȫ���,�������緵��0.
		inline LPCWSTR Element(uint index){
			return _Strs[index];
		}
		//��������ִ���
		inline LPCWSTR RevElement(uint index){return _Strs.RevElement(index);}
		//������Ӧ�ִ���ʵ���е�����(��start��ʼ),����ִ�����ʵ����,����-1.
		uint IndexOf(LPCWSTR str,uint start = 0);
		//�����ʵ�����ڴ������ֱ�ӵĲ���,��Ӧ�õ����������.������������ͷŶ�ռ�õ��ڴ�.
		void Realize();
		//����ִ�,�����ͷ������ڴ�.
		void Release();
		//����ʵ��������(�����ִ���NULL�ַ��ĳ��Ⱥ�).������õĳ���С��ʵ�ʳ���,����false.
		bool SetSize(uint number);
		//����ʵ���ռ�ռ�õ��ڴ��С��˫�ֽڣ�.
		inline uint Cubage()const{return _Size;}
		//����ʵ����ʹ�õ��ڴ泤�ȣ�˫�ֽڣ�.
		inline uint Length()const{return _Length;}
		//����ʵ�����ִ��ĸ���.
		inline uint Count()const{return _Strs.Count();}
	};
	//�����ֻ���治ͬ���ִ�.
	class JUBASE_API StringMemDiction : public StringMemList
	{
	public:
		StringMemDiction();
		~StringMemDiction();
		bool AddFromMemory(LPCWSTR handle,uint length,uint index = -1);
		//����
		bool Add(LPCWSTR str,uint* index = 0);
	};
};
