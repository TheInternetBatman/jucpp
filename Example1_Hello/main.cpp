#include "../juwnd.h"

class Main : public ju::Frame{
protected:
	void onCreate(ju::IWnd* wnd){
		;
	}
public:
	Main(){
		Param->Text = L"Hello jsuse!";
		OnCreate.Add(this,&Main::onCreate);
	}
};
WINMAIN{
	Main m;
	m.Create();
	return ju::MsgLoop::Start();
};