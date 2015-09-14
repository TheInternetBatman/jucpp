#pragma once
namespace cs{
	/*-------------------------------------------------------------------------------------------------------------
	SystemTime是一个标识时间的结构,它的成员分别代表年,月,星期,日,时,分,秒,毫秒.时间分标准时间和当地时间,它们相差一个
	时差.GetLocalTime,GetSystemTime可以用来分别取得这两种时间.FileTime是一个标识时间的抽象结构.本质上,时间用64个字节
	表示已经足够,而SystemTime有128个字节,这是为了分解成人们习惯的形式,但不方便计算.文件时间标识是用FileTime结构存贮的
	,如果要取得它的可读形式,就要用到FileTimeToLocalFileTime和FileTimeToSystemTime函数.显然只有标准时间才能真正代表时
	间,但它却不符合人们的时常习惯,人们日常的时间是当地时间,同一标准时间,在不同时区设置的计算机上,当地时间是不同的.所
	以先用FileTimeToLocalFileTime把系统时间转换为当地时间,再用FileTimeToSystemTime,把抽象时间转化为可读时间.当然,也可
	以,直接用FileTimeToSystemTime把时间转化为可读时间,再用SystemTimeToTzSpecificLocalTime求出当地时间.
	---------------------------------------------------------------------------------------------------------------*/
#pragma pack(push,1)
	//FileTime是时间的64位整数表示。
	typedef struct CONCISE_API FileTime : public _struct{
		//求2个时间的差.这个值的1/10000代表1毫秒.
		void operator -= (FileTime& t){
			Value -= t.Value;
		}
		//从日期求得时间值.
		void operator = (SYSTEMTIME& st){
			SystemTimeToFileTime(&st,(LPFILETIME)this);
		}
		void operator = (FILETIME& ft){
			memcpy(this,&ft,sizeof(ft));
		}
		operator FILETIME& (){
			return (*(LPFILETIME)this);
		}
		operator LPFILETIME (){
			return (LPFILETIME)this;
		}
		//取得当前的标准时间值,这个值在世界的任何计算机上,同一时刻应该是相同的.
		void GetCurrentTime(){
			::GetSystemTimeAsFileTime((LPFILETIME)this);
		}
		union{
			/*HighValue 除以36为小时数,LowValue除以10000000为秒数(精确到毫秒,也就是说,后面的值恒定为0).这个值随着时间的推移是递增的*/
			struct{
				DWORD LowValue;
				DWORD HighValue;
			};
			struct{
				unsigned __int64 Value;//时间是从1601年1月1日开始的.
			};
		};
	}FileTime;
	//SystemTime是时间的分量表示
	typedef struct CONCISE_API SystemTime : public _struct{
		WORD Year; 
		WORD Month;
		WORD DayOfWeek; //星期,星期日这个值是0,星期一是1,....
		WORD Day; 
		WORD Hour;
		WORD Minute;
		WORD Second; 
		WORD Milliseconds;

		void operator = (SYSTEMTIME& st){
			*(LPSYSTEMTIME)this = st;
		}
		operator SYSTEMTIME& (){
			return *(LPSYSTEMTIME)this;
		}
		operator LPSYSTEMTIME (){
			return (LPSYSTEMTIME)this;
		}
		void operator = (FILETIME& ft){
			::FileTimeToSystemTime((LPFILETIME)&ft,(LPSYSTEMTIME)this);
		}
		//取得标准时间。
		inline void GetStandardTime(){
			::GetSystemTime((LPSYSTEMTIME)this);
		}
		//设置系统时间（作为标准时）。
		inline bool SetStandardTime(){
			return ::SetSystemTime((LPSYSTEMTIME)this)!=0;
		}
		//取得地区时间。
		inline void GetLocalTime(){
			::GetLocalTime((LPSYSTEMTIME)this);
		}
		//设置系统时间（作为地区时）。
		inline bool SetLocalTime(){
			return ::SetLocalTime((LPSYSTEMTIME)this)!=0;
		}
		//转换为标准时。
		bool ToStandard(){
			TIME_ZONE_INFORMATION tzi;
			if(::GetTimeZoneInformation(&tzi)==TIME_ZONE_ID_INVALID) return false;
			return ::TzSpecificLocalTimeToSystemTime(&tzi,(LPSYSTEMTIME)this,(LPSYSTEMTIME)this)!=0; 
		}
		//转换为地区时。
		bool ToLocal(){
			TIME_ZONE_INFORMATION tzi;
			if(::GetTimeZoneInformation(&tzi)==TIME_ZONE_ID_INVALID) return false;
			return ::SystemTimeToTzSpecificLocalTime(&tzi,(LPSYSTEMTIME)this,(LPSYSTEMTIME)this)!=0;
		}
	}SystemTime;
	//物理键盘的按键
	typedef union CONCISE_API KeyState{
		struct{
			bool Toggled		//每一个键都会有切换(不光CapsLock,NumLock这些键),这个成员表示处于哪种状态.true表示状态ON.
				,Pressing;		//这个键是否正在被按下.
		};
		struct{
			short State;		//代表键状态的一个值.
		};
		//这个函数从纯种的消息队列来取得信息.
		void GetKeyState(int vk){
			State = ::GetKeyState(vk);
		}
		//这个函数从硬件中断取得信息.
		void GetAsyncState(int vk){
			State = ::GetAsyncKeyState(vk);
		}
		void* operator new(size_t size){
			return MemoryAlloc((int)size);
		}
		void operator delete(void* p){
			MemoryFree(p);
		}
	}KeyState;
#pragma pack(pop)
}