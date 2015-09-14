#pragma once
namespace cs{
	class CONCISE_API Ungzip : public _class{
	protected:
		void* strm;//z_stream 
	public:
		Ungzip();
		~Ungzip();
		inline bool IsNull(){return strm==NULL;}
		inline bool NotNull(){return strm!=NULL;}
		//初始化，这个函数可以重复调用，如果Output函数返回-1，表示出现错误，此时此对象已废弃，需要重新调用此函数。
		void Create();
		//把此对象重置为 NULL 。
		void Close();
		//输入要解压的数据。再次调用此此函数需要调用一次Output函数。
		bool Input(char* data,int len);
		//输出解压数据，返回 0 ，正常完成；
		//返回 1 ，还有数据，必须再次调用此函数输出数据，否则会造成错误。
		//返回 -1 ，出现错误。在返回 -1 以后，如果要再次使用这个实例，必须重新调用 Create() 函数。
		int Output(char* buf,int& len);
		//输出解压数据，返回输出的数据长度。出现错误返回值为 0 。
		int Output(Memory<char>& buf);
	};
	//Gzip就像一个压缩机，可以不断的调用Input输入数据，Output输出数据。
	class CONCISE_API Gzip : public _class{
	protected:
		void* strm;//z_stream 
	public:
		Gzip();
		~Gzip();
		inline bool IsNull(){return strm==NULL;}
		inline bool NotNull(){return strm!=NULL;}
		//初始化，level是压缩率的级别。这个函数可以重复调用，如果Output函数返回-1，表示出现错误，此时此对象已废弃，需要重新调用此函数。
		void Create(int level = 6);
		//把此对象重置为 NULL 。
		void Close();
		//输入要压缩的数据，buf或len为0 返回false。此函数不能连续调用，必须调用一次Output函数才能继续输入数据。
		bool Input(char* buf,int len);
		//输出压缩数据，返回 0 ，正常，可以继续输入数据。
		//返回 1 ，输入数据没有处理完成，需要继续调用此函数输出压缩数据，否则如果再次调用了Input函数，这些未处理的数据会丢失。
		//返回 -1 ，出现错误。如果要再次使用这个实例，必须重新调用 Create() 函数。
		int Output(char* buf,int &len);
		//输出全部数据，返回数据长度。
		int Output(Memory<char>& buf);
	};
	//返回压缩数据可能需要的最大缓冲区长度
	CONCISE_API uint GetMaxCompressLen(uint len);
	//压缩数据，返回压缩数据的长度：data：要压缩的数据源，length：源数据的长度，zData：压缩数据存放的缓冲区，bufLen：缓冲区的长度。
	CONCISE_API uint Compress(const char* data,uint length,char* zData,uint bufLen);
	//解压数据，返回解压数据的长度：zData：要解压缩的源数据，length：源数据的长度，data：解压数据的存放缓冲区，bufLen：缓冲区的大小。
	CONCISE_API uint DeCompress(const char* zData,uint length,char* data,uint bufLen);
	//压缩数据，返回压缩数据的长度：data：要压缩的数据源，zData：存放压缩数据的缓冲，不必设置大小，函数会自动调整。
	CONCISE_API uint Compress(const char* data,uint len,Memory<char>* zData);
	//解压数据，返回解压数据的长度：zData：要解压的数据源，data：存放解压数据的缓冲区，不必设置大小，函数会自动调整。
	CONCISE_API uint DeCompress(const char* zData,uint len,Memory<char>* data);
}