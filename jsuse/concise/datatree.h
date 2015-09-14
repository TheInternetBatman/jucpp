#pragma once
namespace cs{
	/*DataTree的数据格式非常简单，它包含3个数据部分：Name，Value，Node，Name和Value的前2个
	字节是数据的长度，后面是数据；Node部分，前两个字节是Node的个数，每个Node也包含Name、
	Value、Node 3部分。*/
	class CONCISE_API DataTree : public TPTree<DataTree>
	{
	protected:
	public:
		Memory<uchar> Name;
		Memory<uchar> Value;
		//ObjectList<DataTree> Node;
		//把对象存储在内存中，mem：要存储的内存，offset：存储在内存中的位置，返回使用的内存大小
		int ToMemory(Memory<uchar>& mem,int offset = 0);
		//从一段内存加载，len是内存的大小。返回值是正确解析的长度，如果数据正确，它应该返回len。
		int FromMemory(uchar* mem,int len);
		bool Save(LPCWSTR name);
		bool Load(LPCWSTR name,bool always = 1);
		DataTree* OpenNode(void* name,int len,bool always = 0);
		bool SetValue(void* data,int len);
		bool SetName(void* name,int len);
	};
	/*把DataTree保存为特定的文件类，它提供的唯一功能就是有一个默认文件名，
	如果不指定这个文件名，或者指定为 NULL 或者空串，则默认名称为exe文件名+.dtf后缀*/
	class CONCISE_API DataTreeFile : public DataTree{
	protected:
		String fileName;
	public:
		DataTreeFile();
		//指定文件名，如果 name = NULL 或是一个空串，则fileName被设置为调用的exe文件名+.dtf后缀
		void SetFileName(LPCWSTR name);
		//获取当前文件名。
		LPCWSTR GetFileName(){return fileName;}
		//从文件加载
		bool Load();
		//保存为文件
		bool Save();
	};
}