#include "../juwnd.h"
#pragma usexpstyle

WINMAIN{
	ju::Frame main;
	main.Param->Text = "MouseClick";
	main.Create();
	ju::MsgLoop::Start();
}
