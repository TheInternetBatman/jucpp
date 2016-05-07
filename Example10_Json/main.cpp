#include "../juwnd.h"
#pragma usexpstyle

void logJson(ju::Json& json){
	ju::String str;
	json.ToString(str);
	ju::Log(str);
}
WINMAIN{
	ju::LogShowWindow();
	ju::String str;
	ju::Json json;
	ju::LogfI(L"begin loading a.bson");
	json.LoadBytes(L"a.bson");
	ju::LogfI(L"begin loading a.json");
	json.LoadFromFile(L"a.json");
	ju::LogfI(L"complete");

	ju::MsgLoop::Start();
}
