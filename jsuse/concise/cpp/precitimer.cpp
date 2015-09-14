#include "../stdafx.h"
#include "../../concise.h"

#pragma comment(lib,"winmm.lib")
namespace cs{
	void PreciTimer::_cbProc(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
	{
		OnTimer(wTimerID,dwUser);
	}
	PreciTimer::PreciTimer():_Handle(0)
	{
		_eventCB.Bind(this,&PreciTimer::_cbProc);
	}
	bool PreciTimer::Create(int ms,void* user)
	{
		if(_Handle) return 0;
		_Handle = timeSetEvent(ms,0,_eventCB,(DWORD)user,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
		return _Handle!=0;
	}
	void PreciTimer::Close()
	{
		if(_Handle)
		{
			timeKillEvent(_Handle);
			_Handle = 0;
		}
	}
	PreciTimer::~PreciTimer()
	{
		if(_Handle) timeKillEvent(_Handle);
	}
}