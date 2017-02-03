
//https://github.com/makerdiary/Espruino

#include "../juwnd.h"
#pragma usexpstyle

WINMAIN{
	ju::Frame main;
	main.Param->Text = "Espruino";
	main.Create();
	ju::MsgLoop::Start();
}
