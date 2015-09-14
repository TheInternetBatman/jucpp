#pragma once
#include <new>
namespace cs{
	//List是一个集合类,保存同类实例的值.如果对象不是纯粹的内存对象，而包含函数的话，应该使用ObjectList类来保存。
	template<typename T> class CONCISETL_API List : public _class{
	protected:
		uint _Count,		//元素数量.
			_Grow;			//空间扩充时的增量,0表示加倍的扩容方式.
		Memory<T> _Handle;	//用于存储元素的内存.
	public:
		//默认构造函数的扩容方式为加倍(增量为0).
		List():_Count(0),_Grow(0){
		}
		//grow是容量扩充时的增量,为0则表示扩容时,容量加倍.
		List(uint grow):_Count(0)(0){
			if(grow<0) _Grow = 0;
			else _Grow = grow;
		}
		inline bool SetUseSysMemory(bool sys = 0){return _Handle.SetUseSysMemory(sys);}
		inline bool GetUseSysMemory(){return _Handle.GetUseSysMemory();}
		//设置容量的增量.为0则表示扩容时,容量加倍.
		void SetGrow(uint grow){
			if(grow<0) _Grow = 0;
			else _Grow = grow;
		}
		//设置容量,如果设置的数值小实例中元素的个数,这个函数什么也不做.
		void SetCubage(uint cubage){
			if(cubage>=_Count) _Handle.SetLength(cubage);
		}
		//使实例使用的内存正好能够存储当前的元素,也就是释放多余的的内存.
		void Realize(){
			_Handle.SetLength(_Count);
		}
		//添加一个元素,index 是添加的位置,-1 表示添加到末尾,这是效率最高的方式.返回添加元素的当前位置索引.
		uint Add(T& value,uint index = -1){
			if(_Count==_Handle.Length()){
				if(_Grow) _Handle.Expand(_Grow);
				else _Handle.Double();
			}
			if(index>_Count) index = _Count;
			_Handle.Move(index+1,index,_Count-index);
			_Handle[index] = value;
			_Count++;
			return index;
		}
		//这个函数一次性扩充列表分配的内存,但是被扩充位置的元素并未被赋值.注意扩充的内存可能大于所需的内存,和列表的扩充方式有关.
		//返回第一个插入元素的位置索引,元素总数Count（）会被重新赋值。
		uint Expand(uint count,uint index = -1){
			if((count+_Count)<=_Count) return -1;
			if(_Handle.Length()<(count+_Count)){
				uint length;
				if(_Grow){
					length = count + _Count - _Handle.Length();
					if(length%_Grow)
						length = (length/_Grow + 1)*_Grow;
				}else{
					length = _Handle.Length();
					if(length==0) length = 8;
					while(length<(count+_Count))
						length *= 2;
					length -= _Handle.Length();
				}
				if(!_Handle.Expand(length,index)) return -1;
			}
			if(index==-1) index = _Count;
			_Handle.Move(index+count,index,_Count-index);
			_Count += count;
			return index;
		}
		//返回删除的元素个数,这个函数并不会释放已经分配的内存.
		uint Delete(uint start,uint count = 1){
			if(start>=_Count) return 0;
			if((start+count)<=start) return 0;
			if((start+count)>_Count) count = _Count - start;
			_Handle.Move(start,start+count,_Count-start-count);
			_Count -= count;
			return count;
		}
		//删除搜索到的第一个值为 value 的元素,如果元素不存在,返回 0.
		bool DeleteValue(T& value,uint start = 0){
			for(uint i=start;i<_Count;i++){
				if(memcmp(&value,_Handle+i,sizeof(T))==0)
					return Delete(i,1)!=0;
			}
			return 0;
		}
		//清空所有元素.它并不释放占用的内存,用SetCubage来缩减内存的占用.用Release来释放内存.用Realize函数来释放多余的内存.
		inline void Clear(){
			_Count = 0;
		}
		//返回元素个数
		inline uint Count() const {return _Count;
		}
		//元素数组的指针
		inline const T* GetHandle(){return _Handle;}			
		//返回列表的容量.
		inline uint Cubage(){return _Handle.Length();}	
		//取得相当索引的元素.按地址返回的元素,可以从外部设置元素的值.没有边界检查.
		inline T& Element(uint index){
			_ASSERT(index<_Count);
			return _Handle[index];
		}
		//取得相当索引的元素.按地址返回的元素,可以从外部设置元素的值.没有边界检查.
		inline T& operator [] (uint index){
			_ASSERT(index<_Count);
			return _Handle[index];
		}
		//返回相应索引元素的指针,如果index超出边界,返回0.
		T* GetElement(uint index){
			if(index>=_Count) return 0;
			return _Handle + index;
		}
		//特定值在数组中的索引,从start开始搜索,如果没有这个值,返回-1.
		uint IndexOf(T& value,uint start = 0){
			for(uint i=start;i<_Count;i++)
				if(CompareMemory(_Handle+i,value,sizeof(T))==sizeof(T))
					return i;
			return -1;
		}
		//清空元素,并且释放所有内存.
		void Release(){
			_Handle.Free();
			_Count = 0;
		}
		//逆向索引，返回相应的元素，index=0对应的是列表的最后一个元素。
		inline T& RevElement(uint index){
			_ASSERT(index<_Count);
			return _Handle[_Count-index-1];
		}
	};
	//Diction类的与List类的区别在于它只存贮值不同的元素.
	template<typename T> class CONCISETL_API Diction : public List<T>{
	public:
		//返回添加的值的位置索引,不管它是否已经在列表中.exist用于确定添加的值是否已经在列表中.
		uint Add(T& value,uint index = -1,bool* exist = 0){
			for(uint i=0;i<_Count;i++) if(memcmp(_Handle+i,&value,sizeof(T))==0){
				if(exist) *exist = true;
				return i;
			}
			if(exist) *exist = 0;
			return List<T>::Add(value,index);
		}
	};
	//这个类和List是相同的,但出于效率的考虑,它只处理机器字长的元素.
	template<typename T> class CONCISETL_API IntegerList : public List<T>{
	public:
		IntegerList(){
			_ASSERT(sizeof(T)==sizeof(uint));
		}
		IntegerList(uint grow){
			_ASSERT(sizeof(T)==sizeof(uint));
			if(grow<0) _Grow = 0;
			else _Grow = grow;
		}
		//添加一个元素,index 是添加的位置,-1 表示添加到末尾,这是效率最高的方式.返回新元素的起始位置，-1表示内存不足导致失败。
		uint Add(T value,uint index = -1){
			if(_Count==_Handle.Length()){
				if(_Grow) _Handle.Expand(_Grow);
				else _Handle.Double();
			}
			if(index>_Count) index = _Count;
			_Handle.Move(index+1,index,_Count-index);
			_Handle[index] = value;
			_Count++;
			return index;
		}
		//删除搜索到的第一个值为 value 的元素,如果元素不存在,返回 0.
		bool RemoveValue(T value,uint start = 0){
			for(uint i=start;i<_Count;i++) if(value==_Handle[i]) return Delete(i,1)==1;
			return 0;
		}
		//有边界检查的元素值，如果索引超界，返回 0 。 
		inline T GetElement(uint index){
			if(index>=_Count) return 0;
			return _Handle[index];
		}
		//特定值在数组中的索引,从start开始搜索,如果没有这个值,返回-1.
		uint IndexOf(T value,uint start = 0){
			for(uint i=start;i<_Count;i++) if(_Handle[i]==value) return i;
			return -1;
		}
	};
	//IntegerList对应Dictin类.
	template<typename T> class CONCISETL_API IntegerDiction : public IntegerList<T>{
	public:
		//添加一个值,如果这个值已经包含在集合中,返回相应的索引.exist用于判断是否已经存在这个值.
		uint Add(T value,uint index = -1,bool* exist = 0){
			for(uint i=0;i<_Count;i++){
				if(_Handle[i]==value){
					if(exist) *exist = true;
					return i;
				}
			}
			if(exist) *exist = false;
			return IntegerList<T>::Add(value,index);
		}
	};
	//ObjectList保存的是实例的指针,它会自动生成和销毁这些实例.一般来说,因为new和delete和进程是相关的,所以尽量不要把
	//外部生成的实例的指针添加进来,因为如果不从中移除的话,这个类会自动用delete来删除实例,你要自己保证这是可靠的.
	template<typename T> class CONCISETL_API ObjectList : public _class{
	protected:
		IntegerList<T*> _Ptrs;
	public:
		inline ObjectList(){
		}
		//指定增量。
		inline ObjectList(uint grow):_Ptrs(grow){
		}
		//设置使用哪种内存, 这包括生成的对象实例的内存, 以及ObjectList自身使用的内存.
		inline bool SetUseSysMemory(bool sys = 0){
			return _Ptrs.SetUseSysMemory(sys);
		}
		inline bool GetUseSysMemory(){
			return _Ptrs.GetUseSysMemory();
		}
		//设置增量。
		inline void SetGrow(uint grow){
			_Ptrs.SetGrow(grow);
		}
		//设置容量。
		inline void SetCubage(uint cubage){
			_Ptrs.SetCubage(cubage);
		}
		//返回插入的元素指针
		T* Add(uint index = -1){
			T* obj = new T;
			_Ptrs.Add(obj,index);
			return obj;
		}
		//添加多个对象,返回插入的位置.-1表示没有元素被添加.
		uint Expand(uint count,uint index = -1){
			_ASSERT(count>0);//这是检查用的.
			if(count==0) return -1;
			if(index>_Ptrs.Count()) index = _Ptrs.Count();
			if(_Ptrs.Cubage()<(_Ptrs.Count()+count))
				index = _Ptrs.Expand(count,index);
			count += index;
			for(uint i=index;i<count;i++)
				_Ptrs[i] = new T;
			return index;
		}
		/*Delete函数从列表中移除若干个元素.并且删除实例本身.index--要移除元素的位置.count--移除元素的个数,当count大于index至末尾的个数时,删除至末尾.函数返回实际删除的个数.*/
		uint Delete(uint index,uint count = 1){
			if(index>=_Ptrs.Count()) return 0;
			if((_Ptrs.Count()-index)<count) count = _Ptrs.Count() - index;
			if(count==0) return 0;
			uint index2 = count + index;
			for(uint i=index;i<index2;i++) delete _Ptrs[i];
			return _Ptrs.Delete(index,count);
		}
		//删除特定的元素。
		inline bool Delete(T* object){
			delete object;
			return _Ptrs.RemoveValue(object);
		}
		//与Delete函数唯一不同的是,这个函数只移除元素,不从内存中删除实例.也就是说,你要自己手动删除实例.
		uint RemoveOut(uint index,uint count = 1){
			if(index>=_Ptrs.Count()) return 0;
			if((_Ptrs.Count()-index)<count) count = _Ptrs.Count() - index;
			if(count==0) return 0;
			return _Ptrs.Delete(index,count);
		}
		//把外部生成的实例添加到集合中,返回添加到的位置索引, 需要保持内存类型的一致.
		uint AddIn(T* object,uint index = -1){
			if(index>_Ptrs.Count()) index = _Ptrs.Count();
			_Ptrs.Expand(1,index);
			_Ptrs[index] = object;
			return index;
		}
		//返回指定索引的元素。
		inline T* Element(uint index){
			_ASSERT(index<_Ptrs.Count());
			return _Ptrs[index];
		}
		//返回指定索引的元素，不返回T*是为了防止外部改写元素，这和其它List不同，不允许外部直接改写元素值。
		inline T& operator [] (uint index){
			_ASSERT(index<_Ptrs.Count());
			return *_Ptrs[index];
		}
		//逆向索引，返回相应的元素，index=0对应的是列表的最后一个元素。
		inline T* RevElement(uint index){
			_ASSERT(index<_Ptrs.Count());
			return _Ptrs[_Ptrs.Count()-index-1];
		}
		//元素个数
		inline uint Count(){return _Ptrs.Count();}
		//返回指定索引的元素，如果索引超界，返回 0 。
		inline T* GetElement(uint index){
			return _Ptrs.GetElement(index);
		}
		//清空所有元素。
		void Clear(){
			for(uint i=0;i<_Ptrs.Count();i++) delete _Ptrs[i];
			_Ptrs.Clear();
		}
		//返回特点元素的索引，如果不在集合中，返回-1.
		inline uint IndexOf(T* pt,uint start = 0){
			return _Ptrs.IndexOf(pt,start);
		}
		//析构
		~ObjectList(){
			for(uint i=0;i<_Ptrs.Count();i++) delete _Ptrs[i];
		}
	};
	/*ObjectLink管理一系列的对象实例，它可以在析构自动删除所有成员，遍历元素使用Move调整位置，Element返回元素
	插入元素和删除元素在任何位置都是相同的，无需拷贝内存，这是和ObjectList类的区别，但是每个对象会多占用8字节的
	空间*/
#define LINK_FIRST		0
#define LINK_CURRENT	1
#define LINK_LAST		2
	template<typename T> class CONCISETL_API ObjectLink : public _class{
	protected:
		template<typename T> class CONCISETL_API  _Link : public _class{
		public:
			T* handle;
			_Link* previous,*next;
			_Link(){
				previous = 0;
				handle = 0;
				next = 0;
			}
		};
		uint _count;
		bool _sys;
		_Link<T>* _first,*_current,*_last;
	public:
		ObjectLink(bool sys = 0):_first(0),_current(0),_last(0),_count(0),_sys(sys){
		}
		~ObjectLink(){
			while(_last){
				delete _last->handle;
				_current = _last;
				_last = _last->previous;
				delete _current;
			}
		}
		inline bool GetUseSysMemory(){return _sys;}
		//添加元素，0：添加在开头，1：添加在当前，2：添加在结尾
		T* Add(int link_ = LINK_LAST){
			return Add(new T,link_);
		}
		//把指定的对象指针添加到集合，集合负责清理指针指向的对象。
		T* Add(T* t,int link_ = LINK_LAST){
			if((link_<0)||(link_>2)) return 0;
			if(_first==0){
				_first = new _Link<T>;
				_first->previous = 0;
				_first->next = 0;
				_first->handle = t;
				_current = _first;
				_last = _first;
				_count++;
				return _first->handle;
			}
			_Link<T>* link = new _Link<T>;
			link->handle = t;
			switch(link_)
			{
			case LINK_FIRST:
				link->previous = 0;
				link->next = _first;
				_first->previous = link;
				_first = link;
				break;
			case LINK_CURRENT:
				_ASSERT(_current!=0);
				if(_current==0){
					delete link;
					return 0;
				}
				link->previous = _current;
				link->next = _current->next;
				if(_current->next){
					_current->next->previous = link;
				}
				_current->next = link;
				break;
			case LINK_LAST:
				link->previous = _last;
				link->next = 0;
				_last = link;
				link->previous->next = link;
				break;
			}
			_count++;
			return link->handle;
		}
		bool Delete(T* obj){
			for(First();;Move(1)){
				T* elm = Element();
				if(elm==0) break;
				if(elm==obj){
					Delete();
					return true;
				}
			}
			return false;
		}
		//删除一个节点，0：删除第一个元素；1：删除当前位置元素；2：删除末尾元素。delObj指示是否删除绑定对象。
		//如果删除的元素是当前元素，当前元素自动指向下一个元素，如果没有下一个元素，链表溢出，即当前元素为 0 。
		bool Delete(int link_ = LINK_CURRENT,bool delObj = 1){
			if((link_<0)||(link_>2)) return 0;
			if(_first==0) return 0;
			_Link<T>* link;
			switch(link_)
			{
			case LINK_FIRST:
				link = _first->next;
				if(link) link->previous = 0;
				if(_current==_first){
					_current = link;
				}
				if(_last==_first) _last = 0;
				if(delObj) delete _first->handle;
				delete _first;
				_first = link;
				break;
			case LINK_CURRENT:
				_ASSERT(_current!=0);
				if(_current==0){
					return 0;
				}
				link = _current->next;
				if(_current->previous){
					_current->previous->next = _current->next;
				}
				if(_current->next){
					_current->next->previous = _current->previous;
				}
				if(delObj) delete _current->handle;
				delete _current;
				if(_current==_first){
					_first = link;
				}
				if(_current==_last){
					_last = link;
				}
				_current = link;
				break;
			case LINK_LAST:
				link = _last->previous;
				if(link) link->next = 0;//_last->next;
				if(delObj) delete _last->handle;
				delete _last;
				//if(_last->next) _last->next->previous = _last->previous;
				if(_current==_last) _current = 0;
				if(_first==_last) _first = 0;
				_last = link;
				break;
			}
			_count--;
			return 1;
		}
		//有的时候需要向列表的尾部添加一个元素，同时又需要在开头删除一个元素，就像整体上链表的元素都被从后面推
		//了一个位置，总的数量是不变的。对于这种特殊的操作，使用PushBack函数，可以避免生成和删除任何对象，从而
		//提高效率。这个操作不改变当前指向的元素，也不改变，overflow标志。所以特殊情况下，完全可能从指向头部变
		//为指向尾部。
		//PushBack不生成任何新元素，也不删除任何元素，返回的元素其实就是 first。
		T* PushBack(){
			if(_first==0) return 0;//对于空的链表，无法执行后推操作。
			if(_first->next){
				_Link<T>* link = _first;
				_first = _first->next;
				_first->previous = 0;
				_last->next = link;
				link->next = 0;
				link->previous = _last;
				_last = link;
			}
			return _last->handle;
		}
		//和PushBack相反，这个函数从头部添加，尾部删除元素。
		T* PushAhead(){
			if(_last==0) return 0;//空的链表无法执行前推操作。
			if(_last->previous){
				_Link<T>* link = _last;
				_last = _last->previous;
				_last->next = 0;
				_first->previous = link;
				link->previous = 0;
				link->next = _first;
				_first = link;
			}
			return _first->handle;
		}
		//集合元素的数量
		inline uint Count(){
			return _count;
		}
		//指向第一个元素。
		inline bool First(){
			_current = _first;
			return _current!=0;
		}
		//指向最后一个元素。
		inline bool Last(){
			_current = _last;
			return _current!=0;
		}
		//溢出状态，处于溢出状态的链表，当前元素被指向 NULL。溢出状态只有使用Move函数来达到，用于指示链表的步进已经达到最尾或最前。
		//注意，溢出状态只有一个，并不能区分向前Move导致的还是向后Move导致的，它们是相同的。
		//一旦链表处于溢出状态，无法使用Move函数来重置，必须使用First或Last函数来取消这个状态。
		//一个空的链表总是处于溢出状态。
		inline bool OverFlow(){
			return _current==NULL;
		} 
		//返回当前元素，如果集合为空，返回 0 .
		inline T* Element(){
			if(_current!=0)
				return _current->handle;
			else
				return 0;
		}
		//在不改变当前指针的情况下获取首元素
		inline T* GetFirstElement(){
			if(_first==0) return 0;
			return _first->handle;
		}
		//在不改变当前指针的情况下，获取尾元素。
		inline T* GetLastElement(){
			if(_last==0) return 0;
			return _last->handle;
		}
		//步进，正数向后，负数向前。返回实际步进的步数，直到溢出，步进会停止。
		int Move(int step){
			if(step==0) return 0;
			if(_current==0) return 0;
			int n = 0;
			if(step>0){
				while(step>n){
					_current = _current->next;
					if(_current==0) break;
					n++;
				}
			}else{
				while(step<n){
					_current = _current->previous;
					if(_current==0) break;
					n--;
				}
			}
			return n;
		}
		void Clear(){
			while(_last){
				delete _last->handle;
				_current = _last;
				_last = _last->previous;
				delete _current;
			}
			_first = 0;
			_current = 0;
			_last = 0;
			_count = 0;
		}
	};
	template<typename KEY,typename VALUE> class Map : public _class{
	protected:
		ObjectList<KEY> _keys;
		ObjectList<VALUE> _values;
	public:
		void Add(KEY* key,VALUE* val,uint index = -1){
			_keys.AddIn(key,index);
			_values.AddIn(val,index);
		}
		VALUE* Add(KEY* key,uint index = -1){
			_keys.AddIn(key,index);
			return _values.Add(index);
		}
		uint Add(uint index = -1){
			_keys.Add(index);
			_values.Add(index);
			if(index<=_values.Count()) return index;
			else return _keys.Count()-1;
		}
		VALUE* Value(KEY* key){
			uint index = _keys.IndexOf(key);
			return _values.GetElement(index);
		}
		VALUE* Value(uint index){
			return _values.GetElement(index);
		}
		KEY* Key(uint index){
			return _keys.GetElement(index);
		}
		bool Delete(KEY* key){
			uint index = _keys.IndexOf(key);
			if(index==-1) return 0;
			_values.Delete(index);
			_keys.Delete(index);
			return 1;
		}
		bool Delete(int index){
			if(index>=_keys.Count()) return 0;
			_keys.Delete(index);
			_values.Delete(index);
			return 1;
		}
		bool RemoveOut(uint index){
			if(index>=_keys.Count()) return 0;
			_keys.RemoveOut(index);
			_values.RemoveOut(index);
			return true;
		}
		VALUE* RemoveOut(KEY* key){
			uint index = _keys.IndexOf(key);
			if(index==-1) return 0;
			VALUE* val = _values.Element(index);
			_values.RemoveOut(index);
			return val;
		}
	};
	//必须用	class C : public TPTree<C>{
	//			};
	//这种形式使用TPTree类
	template<typename T> class TPTree : public _class{
	protected:
		T*				_parent;
		ObjectList<T>	_childs;
	public:
		TPTree(){
			_parent = 0;
		}
		T* GetParent(){
			return _parent;
		}
		T* Add(uint index = -1){
			T* child = _childs.Add(index);
			child->_parent = (T*)this;
			return child;
		}
		uint Delete(T* pt){
			return _childs.Delete(pt);
		}
		uint Delete(uint index,uint count = 1){
			return _childs.Delete(index,count);
		}
		uint IndexOf(T* pt){
			return _childs.IndexOf(pt);
		}
		uint AddIn(T* pt,uint index = -1){
			pt->_parent = (T*)this;
			return _childs.AddIn(pt,index);
		}
		T* RemoveOut(uint index){
			T* child = _childs.GetElement(index);
			if(child==0) return 0;
			child->_parent = 0;
			_childs.RemoveOut(index);
			return child;
		}
		bool RemoveOut(T* pt){
			uint index = _childs.IndexOf(pt);
			if(index==-1) return 0;
			_childs.RemoveOut(index);
			pt->_parent = 0;
			return true;
		}
		T* Element(uint index){
			return _childs.Element(index);
		}
		T* GetElement(uint index){
			return _childs.GetElement(index);
		}
		uint Count(){
			return _childs.Count();
		}
		void Clear(){
			_childs.Clear();
		}
	};
	//可以直接使用的一个Tree
	template<typename T> class Tree : public _class{
	protected:
		Tree*				_parent;
		ObjectList<Tree>	_childs;
	public:
		T					Handle;
		Tree(){
			_parent = 0;
		}
		Tree* GetParent(){
			return _parent;
		}
		Tree* Add(uint index = -1){
			Tree* child = _childs.Add(index);
			child->_parent = this;
			return child;
		}
		uint Delete(Tree* pt){
			return _childs.Delete(pt);
		}
		uint Delete(uint index,uint count = 1){
			return _childs.Delete(index,count);
		}
		uint IndexOf(Tree* pt){
			return _childs.IndexOf(pt);
		}
		uint AddIn(Tree* pt,uint index = -1){
			pt->_parent = this;
			return _childs.AddIn(pt,index);
		}
		Tree* RemoveOut(uint index){
			Tree* child = _childs.GetElement(index);
			if(child==0) return 0;
			child->_parent = 0;
			_childs.RemoveOut(index);
			return child;
		}
		bool RemoveOut(Tree* pt){
			uint index = _childs.IndexOf(pt);
			if(index==-1) return 0;
			_childs.RemoveOut(index);
			pt->_parent = 0;
			return true;
		}
		Tree* Element(uint index){
			return _childs.Element(index);
		}
		Tree* GetElement(uint index){
			return _childs.GetElement(index);
		}
		uint Count(){
			return _childs.Count();
		}
		void Clear(){
			_childs.Clear();
		}
	};
}
