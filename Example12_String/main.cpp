#include "../juwnd.h"
#pragma usexpstyle

LPCWSTR dst = L"D:\\Develop\\Wamp\\www\\ainy17";
LPCWSTR src = L"D:\\Develop\\Wamp\\www\\ainy17.bak";

bool convertToUtf8(ju::String& fn) {
	ju::String src_f = src;
	ju::FPLinkPath(src_f, fn);
	ju::String dst_f = dst;
	ju::FPLinkPath(dst_f, fn);

	ju::FileStream fs;
	bool r = fs.OpenExist(src_f);
	if(!r) {
		ju::PrintLastError();
		return 0;
	}

	ju::String buf;
	int len = fs.ReadString(buf, 0, CP_THREAD_ACP);
	fs.Close();

	int pos = buf.Find(L"gb2312");
	if(pos != -1) {
		buf.Replace(L"UTF-8", pos, 6);
	}

	ju::Memory<char> utf8;
	len = buf.ToMultiByte(utf8, CP_UTF8);

	fs.Create(dst_f);
	fs.SetLength(0);
	fs.Write(utf8.Handle(), len);
}
void __stdcall onSearch(ju::ListData* ld) {
	ju::String fn;
	if(!ld->isFile()) {
		fn = dst;
		ju::FPLinkPath(fn, ld->relative);
		ju::FPLinkPath(fn, ld->data->cFileName);
		ju::CreateFolder(fn);
		return;
	}
	fn = ld->relative;
	ju::FPLinkPath(fn, ld->data->cFileName);
	ju::Print(L"%s", fn.Handle());
	convertToUtf8(fn);
}

WINMAIN{
	try {
		throw new ju::String("hello");
	} catch(ju::String* e) {
		ju::Print(*e);
		delete e;
	}
	return 0;
	ju::CreateFolder(dst);

	ju::FileSearch fs;
	fs.OnList.BindStd(&onSearch);
	fs.GetFilter()->Add(L"htm");
	fs.GetFilter()->Add(L"asp");
	fs.SetFilterType(1);
	fs.SetSearchSub(true);
	fs.Search(src);
	return 1;
}
