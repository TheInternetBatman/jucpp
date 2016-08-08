#include "../juwnd.h"
#pragma usexpstyle

WINMAIN{
	ju::FileStream fs;
	bool r = fs.OpenExist(L"D:/Develop/Wamp/bin/mysql/mysql5.7.9/share/mengdb.sql");
	if(!r) {
		ju::PrintLastError();
		return 0;
	}
	ju::String buf;
	int len = fs.ReadString(buf, 0, CP_UTF8);
	ju::StringList sl;
	int count = sl.SplitString(buf, L"\n");
	ju::Print(L"%d", count);
	buf = L"";
	for(int i = 0; i < count; i++) {
		ju::String& line = sl[i];
		if(line.Find(L"INSERT INTO") == 0) continue;
		buf += line + L"\n";
	}
	fs.Close();
	fs.Create(L"mengdb.sql");
	fs.SetLength(0);
	fs.WriteString(buf, 0, 0, CP_UTF8);
	return 1;
}
