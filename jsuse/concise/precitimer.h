#pragma once
#include "mmsystem.h"

namespace cs{
	//生成毫秒级精度的定时器，这个类会启动一个独立的线程。但是多个实例好像只启动一个线程。
	class CONCISE_API  PreciTimer : public _class
	{
	protected:
		MMRESULT _Handle;
		Function<void,UINT,UINT,DWORD,DWORD,DWORD> _eventCB;
		void _cbProc(UINT,UINT,DWORD,DWORD,DWORD);
	public:
		Function<void,UINT,LPARAM> OnTimer;//计时器id，和用户数据

		PreciTimer();
		bool Create(int ms,void* user = 0);
		void Close();
		UINT Handle(){return _Handle;}
		operator UINT (){return _Handle;}
		~PreciTimer();
	};
}
