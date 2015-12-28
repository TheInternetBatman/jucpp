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
	ju::LogfI(L"begin loading");
	json.LoadFromFile(L"a.json");
	ju::LogfI(L"begin toString %d,%d",json.Count(),json.getUsedMemory());
	json.ToString(str);
	ju::LogfI(L"complete");
	ju::LogfI(L"begin fromString");
	json.Parse(str);
	ju::LogfI(L"complete %d",str.Length()*2);

	ju::LogfI(L"begin toBytes");
	ju::Memory<byte> buf;
	int len = json.ToBytes(buf);
	ju::LogfI(L"complete: %d",len);
	ju::LogfI(L"begin fromBytes");
	json.FromBytes(buf);
	ju::LogfI(L"complete");
	logJson(json);

	ju::MsgLoop::Start();
}
