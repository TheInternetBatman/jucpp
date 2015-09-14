#pragma once
namespace cs{
	//快速内存，因为是用MapViewOfFile分配的内存，可能不适用于某些 API 函数，比如
	//SetWindowText等，会返回句柄无效错误，需要使用系统内存。但是这种情况的出现是随机的。
	//内存对象,这个类提供内存的分配和释放操作,它所有操作的单位的相应类型的整数.
	//传给函数的长度不能大于最大整数除以类型大小，因为那样会引起数值计算越界。
	template<typename T> class CONCISETL_API Memory : public HandleType<T*>{
	protected:
		uint _Size;
		bool _sys,_bind;
	public:
		Memory():_Size(0),_sys(0),_bind(0){}	//构造函数
		//构造函数,length是初始空间的大小,注意,内存空间的大小是这个值乘以元素的大小.
		Memory(bool sys):_Size(0),_bind(0){
			_sys = sys;
		}
		~Memory(){			//析构函数
			if(_Handle&&!_bind) MemoryFree(_Handle,_sys);
		}
		//设置使用的内存是系统内存还是高速内存, 默认是高速内存. 必须在实例分配的内存为空的状态下, 才能设置.
		//因为高速内存大小有限, 如果分配大量内存, 而且不用频繁的分配和释放, 就无需使用高速分配内存.
		inline void SetUseSysMemory(bool sys = 0){_sys = sys;}
		//或者内存类型.
		inline bool GetUseSysMemory(){return _sys;}
		void Bind(T* pointer,uint size = 0){
			if(_Handle&&!_bind){
				MemoryFree(_Handle,_sys);
			}
			_Handle = pointer;
			_Size = size;
			_bind = true;
		}
		//作为数组使用.
		inline T& operator () (uint index){
			_ASSERT((index>=0)&&(index<_Size));
			return _Handle[index];
		}
		//设置空间的大小.如果length为零,内存被释放.
		bool SetLength(uint length){
			if(_bind) return false;
			if(length==_Size) return true;
			if(length==0){
				bool r = MemoryFree(_Handle,_sys);
				if(r){
					_Size = 0;
					_Handle = 0;
				}
				return r;
			}else{
				T* handle = (T*)MemoryRealloc(_Handle,length*sizeof(T),_sys);
				if(handle==0) return 0;
				_Size = length;
				_Handle = handle;
				return true;
			}
		}
		//设置空间的值为0
		void Zero(){
			::RtlZeroMemory(_Handle,_Size*sizeof(T));
		}

		/*@function uint Move(uint dst,uint src,int len)
		@text 这个函数在本空间内移动指定长度的内存
		@param dst -- uint
		@text 目的位置偏移，对于非绑定模式，这个值可以处于大于已有的空间位置，内存会自动扩充。对于绑定模式，如果超出范围，函数返回 0 .
		@param src -- uint
		@text 源位置偏移，这个值必须在已有的内存范围内。
		@param len -- int
		@text 移动内存的长度，这个是一个 int 值，如果 len 小于 0，标识从源地址到当前末尾的数据
		@return uint
		@text 返回实际移动的数据长度。*/
		uint Move(uint dst,uint src,int len){
			if(src>_Size) return 0;	//源地址不能超过现有范围
			if(len<0) len = _Size - src;	//如果长度小于零表示到末尾
			else if(len>(int)(_Size-src)) return 0;//源长度不能超过现范围
			uint useLen = (dst+len)*sizeof(T);	//可能需要的长度
			if(useLen>_Size){
				if(_bind) return 0; //bind mode can't expand memory
				if(!SetLength(useLen)) return 0;	//如果长度无法扩充返回 0
			}
			::RtlMoveMemory(_Handle+dst,_Handle+src,len*sizeof(T));
			return len;
		}
		//扩充(或缩减)空间,如果length<0,则空间减小.index 是扩充(或缩减)的位置(此位置后面的数据会被移动),小于0或大于等于_Size则从末尾添加.
		bool Expand(int length,uint index = -1){
			if(_bind) return false;
			if(length==0) return 1;
			T* handle;
			if(length>0){
				handle = (T*)MemoryRealloc(_Handle,(_Size+length)*sizeof(T),_sys);
				if(handle==0) return 0;
				if((index>=0)&&(index<_Size))
					::RtlMoveMemory(handle+index+length,handle+index,sizeof(T)*(_Size-index));
			}else{
				if((index>=0)&&(index<_Size))
					::RtlMoveMemory(_Handle+index+length,_Handle+index,sizeof(T)*(_Size-index));
				handle = (T*)MemoryRealloc(_Handle,(_Size+length)*sizeof(T),_sys);
				if(handle==0) return 0;
			}
			_Handle = handle;
			_Size += length;
			return 1;
		}
		//容量加倍,如果当前的_Size为0,则设置为0x20, 如果指定了minSize，内存加倍后达不到nimSize的话，会被设为minSize.
		bool Double(int minSize = 0){
			if(_bind) return false;
			int size;
			if(_Size){ 
				size = _Size*2;
			}else{ 
				size = 0x20;
			}
			if(size<minSize) size = minSize;
			void* handle = (T*)MemoryRealloc(_Handle,size*sizeof(T),_sys);
			if(handle){
				_Handle = (T*)handle;
				_Size = size;
			}
			return handle!=0;
		}
		//释放内存,如果是绑定状态，会解除绑定的指针，并且重置绑定状态，但是这个函数不改变是否使用系统内存状态.
		bool Free(){
			if(_bind){
				_Handle = 0;
				_Size = 0;
				_bind = 0;
				return 1;
			}else if(MemoryFree(_Handle,_sys)){
				_Handle = 0;
				_Size = 0;
				return 1;
			}
			return 0;
		}
		//绑定的内存必须考虑是系统内存还是高速内存.
		bool Attach(T* handle,bool sys = 0){
			if(_Handle&&!_bind) MemoryFree(_Handle,_sys);
			_Size = MemorySize(handle)/sizeof(T);
			_Handle = handle;
			_sys = sys;
			_bind = false;
			return 1;
		}
		//解除绑定的内存句柄.
		T* Detach(){
			if(_bind) return 0;
			T* handle = _Handle;
			_Handle = 0;
			_Size = 0;
			return handle;
		}
		/*@function uint CopyFrom(const T* handle,uint length,uint offset = 0)
		@text 把数据从指定的内存拷贝到自身，在非绑定模式下，如果需要内存可能被扩大。
		@param handle -- const T*
		@text 源内存句柄
		@param length -- uint
		@text 要拷贝的内存长度
		@param offset -- uint
		@text 拷贝到的位置偏移
		@return uint
		@text 成功拷贝的长度*/
		uint CopyFrom(const T* handle,uint length,uint offset = 0){
			uint requirLen = length + offset;
			if(requirLen<length) return 0; //超出整数的范围
			if(requirLen>_Size){
				if(_bind) return 0;	//bind模式，内存不能扩充。
				if(!SetLength(requirLen)) return 0;
			}
			MoveMemory(_Handle+offset,handle,length*sizeof(T));
			return length;
		}
		//length是要拷贝的长度,offset是拷贝的起始位置,如果设置的长度和位置超出边界,则length会被缩小.返回值是实际拷贝的长度.
		uint CopyTo(T* handle,int length,uint offset = 0){
			if(offset>=_Size||length==0) return 0;
			if(length<0) length = _Size - offset;
			if((_Size-offset)<(uint)length) return 0; //指定的长度太大
			MoveMemory(handle,_Handle+offset,length*sizeof(T));
			return length;
		}
		//返回实例的大小,也就是可以存储相应类型的元素个数.
		inline uint Length(){return _Size;}
		//是否绑定模式
		inline bool IsBind(){
			return _bind;
		}
	};
	typedef Memory<char> Mbs;
	//虚拟内存,这种内存分配方式的特点是,可以分配很大的内存,但直到真得使用时(写入数据)才会占用物理内存.
	struct CONCISE_API VirtualAddress : public HandleType<void*>{
	public:
		VirtualAddress();
		inline ~VirtualAddress(){::VirtualFree(_Handle,0,MEM_RELEASE);}
		//函数分配的内存的最小单位是 64KB,section = 1,则分配64KB; section = 2则分配128KB...
		VirtualAddress(uint section);
		//这个函数称为保留而不是分配可以暗示它的作用,它可以用于重新分配保留的地址空间大小.
		bool Reserve(uint section);
		//激活保留的地址空间，只有提交之后,内存才可以使用.如果length为0,则提交整个保留的地址空间.注意提交的内存必须在保留的范围之内.提交的最小单位是页，在XP下是4096（4K)字节。
		void* Commit(LPVOID p = 0,uint length = 0,DWORD protect = PAGE_READWRITE);
		//重新保留一段已提交的区域,如果p为0,则保留整个区域.
		bool DeCommit(LPVOID p = 0,uint length = 0);
		//丢弃数据,但内存仍可用.
		void* Desert(LPVOID p = 0,uint length = 0);
		//取回信息。
		bool QueryInfo(PMEMORY_BASIC_INFORMATION pbi,void* p = 0);
		//设置内存的保护方式
		DWORD SetProtect(DWORD protect,void* p = 0,DWORD length = 0);
		//加载到物理内存,提高访问速度.
		bool Lock(LPVOID p = 0,uint length = 0);
		//释放加载的内存。
		bool Unlock(LPVOID p = 0,uint length = 0);
		//返回内存的字节长度。
		inline uint Length(){return _Size;}
		//返回内存的区块数。
		inline uint Section(){return _Size/0x10000;}
		//返回内存的页数。
		inline uint Page(){return _Size/0x1000;}
		//完全释放。
		bool Free();
	protected:
		uint _Size;
	};
	//高速内存管理, 可以自己使用这个类生成高速内存管理, 系统的高速内存就是一个FastMemory实例.
	class CONCISE_API IMemory : public _class{
	protected:
		class CONCISE_API CS : public _class{
		protected:
			CRITICAL_SECTION _Handle;
		public:
			CS(){InitializeCriticalSection(&_Handle);}
			~CS(){DeleteCriticalSection(&_Handle);}
			//开始锁定代码，调用后，在这个线程没有调用Unlock之前，其它线程对Lock的调用都会挂起。
			void Lock(){EnterCriticalSection(&_Handle);}
			//解除锁定，挂起的线程随机的一个被解除阻塞。
			void Unlock(){LeaveCriticalSection(&_Handle);}
		};
		//使用LocalCriticalSection类可以无需调用Lock和Unlock函数，自动锁定整个函数的代码。
		//此类只在函数中作为局部变量使用。
		class CONCISE_API LCS : public _class{
		protected:
			CS* _handle;
		public:
			LCS(CS& cs){_handle = &cs;_handle->Lock();}
			~LCS(){_handle->Unlock();}
		};
		void*	_handle;
		uint	_length;
		char*	_cur;
		CS		_cs;

		char* _findSpaceForward(uint len);
		char* _findSapceAfter(uint len);
		char* _allocIn(char* pt,uint len);
	public:
		IMemory();
		//对象是否为空.
		inline bool IsNull(){return _cur==NULL;}
		//分配内存
		void* Alloc(uint len);
		//取长度, 如果mem是一个无效的句柄, 此函数仍然会返回一个无意义的非零值.
		uint GetLength(void* mem);
		//重新分配长度，如果新长度比老长度短，实际上位置是不变的，也无需拷贝内存。
		void* ReAlloc(void* mem,uint size);
		//在Release模式下,这个函数无法判定mem的有效性, 而且如果mem不正确, 可能会破坏整个内存对象. 访问释放的内存并不会引起错误.
		bool Free(void* mem);
		//打印内部链表信息
		void PrintInfo();
	};
	class CONCISE_API ExecMemory : public IMemory{
	protected:
		HANDLE	_map;
	public:
		bool Create(int size);
		bool Close();
	};
	class CONCISE_API FastMemory : public IMemory{
	public:
		//初始化可用总内存大小, 此值不可更改, 32位系统能使用的内存是有限的, 这是可使用的最大内存, 不一定会分配这么多物理内存.
		bool Create(uint total);
		//按MB初始化
		inline bool CreateMB(uint n){return Create(n*0x100000);}
		//一个内存指针是否是预分配内存
		bool IsFastMemory(void* mem);
	};
#define FASTMEM_SIZE_MB	16
#define EXECMEM_SIZE	0x10000
	CONCISE_API FastMemory* GetGlobalFastMemory();
	CONCISE_API ExecMemory* GetGlobalExecMemory();
	CONCISE_API void InitConciseLib(uint fastMemSize = FASTMEM_SIZE_MB,uint execMemSize = EXECMEM_SIZE);
}
