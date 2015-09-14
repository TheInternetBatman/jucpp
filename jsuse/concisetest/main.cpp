#include <concise/conciseui.h>
//#include <time.h>
//#include <stdio.h>
#include "base58.h"
#include <WinBase.h>
#pragma usexpstyle

void __stdcall onProgress(cs::DirData* dd,void* param){
	uint preAfter = dd->PreOrAfter;
	cs::Print(L"%s,%d",dd->FindData->cFileName,preAfter);
	cs::Print(L"%s,%s",dd->SourceDir,dd->DestDir);
}
void __stdcall onDataProgress(uint64 offset,uint64 length,bool& cancel,void* param){
	uint of = (uint)offset,len = (uint)length;
	cs::Print(L"%d,%d",of,len);
}
void __stdcall onComplete(LPCWSTR path,LPCWSTR fn,void*){
	cs::Print(L"%s,%s\r\n",path,fn);
}
void copy(){
	LPCWSTR src = L"D:\\SoftProject\\Chrome";
	LPCWSTR dst = L"D:\\SoftProject\\Chrome2";
	cs::DSParam dsp;
	dsp.Destinate = dst;
	dsp.Source= src;
	dsp.OnProgress.BindStd(&onProgress);
	dsp.OnFileProgress.BindStd(&onDataProgress);
	dsp.OnComplete.BindStd(&onComplete);
	cs::DirectorySystem::Copy(&dsp);
}
void del(){
	LPCWSTR src = L"D:\\SoftProject\\Chrome";
	LPCWSTR dst = L"D:\\SoftProject\\Chrome2";
	cs::DSParam dsp;
	dsp.Destinate = dst;
	dsp.Source= dst;
	dsp.OnProgress.BindStd(&onProgress);
	dsp.OnFileProgress.BindStd(&onDataProgress);
	dsp.OnComplete.BindStd(&onComplete);
	cs::DirectorySystem::Delete(&dsp);
}
void __stdcall onList(cs::ListData* ld){
	cs::Print(L"%s,%d",ld->FindData->cFileName,ld->Flag);
}
void list(){
	cs::FileTree ft;
	ft.SetAfterCall(1);
	ft.SetPreCall(1);
	ft.SetSearchSub(1);
	ft.OnList.BindStd(&onList);
	LPCWSTR src = L"D:\\SoftProject\\Chrome";
	ft.Search(src);
}
void __stdcall proc(WORD,WORD,void*){
	;
}
//DT_WORDBREAK		换行，但是不会切断单词，如果一行放不下这个单词，单词会被剪切，但是仍然不换行。左右不会输出半个字体，如果矩形宽度放不下一个字符，就一个字符也不输出，但是上下会输出部分无法完全显示的单词。
//DT_EDITCONTROL	模拟Edit控件，配合DT_WORDBREAK，区别如下，无法单行写下的单词，会在下一行继续输出，而不会剪切，不会显示半行文本。
//DT_NOCLIP			持续输出，无视矩形的高度，所以文本都会被输出。
//DT_END_ELLIPSIS	无法显示的尾部字符用三个点号表示，这个属性不能和 DT_WORDBREAK 一起用，否则无效果。
//DT_EXPANDTABS		显示tab字符，默认tab字符是不输出的，每个tab字符占8个普通字符（4个中文字符）的宽度。
//DT_NOPREFIX		关闭前缀字符的处理，通常DrawText解释助记前缀字符，&为给其后的字符加下划线，解释&&为显示单个&。指定DT_NOPREFIX，这种处理被关闭。下划线可以被添加到空格和tab键的下面。
//DT_TABSTOP		设置tab字符的最大宽度，在DRAWTEXTPARAMS参数的iTabLength里设置，如果这个值是0，会设为默认 8 。
//DT_SINGLELINE		无视换行符，持续在一行输出。默认无论回车还是换行符，都会换行，但是连续的\r\n值换行一次，DT_SINGLELINE 会导致 uiLengthDrawn 失效，总是返回字串的长度。
int shfile(LPCWSTR dst,LPCWSTR src,UINT func,FILEOP_FLAGS flag){
	cs::String from = src;
	cs::FPToFullPath(from);
	from.SetCubage(from.Length()+1);
	from[from.Length()] = 0;

	cs::String to = dst;
	cs::FPToFullPath(to);
	to.SetCubage(to.Length()+1);
	to[to.Length()] = 0;

	SHFILEOPSTRUCT fos;
	::ZeroMemory(&fos,sizeof(fos));
	fos.wFunc = func;
	fos.pFrom = from;
	fos.pTo = to;
	fos.fFlags = flag;
	fos.lpszProgressTitle = L"jsuser";

	return SHFileOperation(&fos);
}
int shcopy(LPCWSTR dst,LPCWSTR src,bool noundo = false,bool noask = false){
	FILEOP_FLAGS flag = FOF_SIMPLEPROGRESS;
	if(!noundo) flag |= FOF_ALLOWUNDO;
	if(noask) flag |= FOF_NOCONFIRMATION;
	flag |= FOF_NOERRORUI;
	return shfile(dst,src,FO_COPY,flag);
}
int shmove(LPCWSTR dst,LPCWSTR src,bool noundo = false,bool noask = false){
	FILEOP_FLAGS flag = FOF_SIMPLEPROGRESS;
	if(!noundo) flag |= FOF_ALLOWUNDO;
	if(noask) flag |= FOF_NOCONFIRMATION;
	return shfile(dst,src,FO_MOVE,flag);
}
int shrename(LPCWSTR dst,LPCWSTR src,bool noundo = false,bool askexist = false){
	FILEOP_FLAGS flag = FOF_SIMPLEPROGRESS;
	if(!noundo) flag |= FOF_ALLOWUNDO;
	if(!askexist) flag |= FOF_RENAMEONCOLLISION;
	return shfile(dst,src,FO_RENAME,flag);
}
int shdelete(LPCWSTR path,bool noundo = false,bool noask = false){
	FILEOP_FLAGS flag = FOF_SIMPLEPROGRESS;
	if(!noundo) flag |= FOF_ALLOWUNDO;
	if(noask) flag |= FOF_NOCONFIRMATION;
	return shfile(0,path,FO_DELETE,flag);
}
int sh(){
	LPCWSTR src = L"D:\\Documents\\Downloads\\abc";
	LPCWSTR dst = L"D:\\Documents\\Downloads\\ABC3\\abc3";
	return shmove(dst,src,false,true);
}

class Main : public cs::Frame{
	LPCWSTR str;
	uint count;
	cs::Button	debugBut;
	DRAWTEXTPARAMS	dtp; 
	void  onDraw(cs::DC* dc,cs::View*){
		cs::Rect r(20,20,210,216);
		dc->SetBkTransparent();
		::Rectangle(*dc,r.left,r.top,r.right,r.bottom);
		cs::Font* font = cs::GetDefaultFont();
		font->Select(*dc);
		DWORD format = DT_WORDBREAK|DT_EXPANDTABS|DT_TABSTOP|DT_EDITCONTROL|DT_SINGLELINE;
		int n = DrawTextEx(*dc,(LPWSTR)str,count,r,format,&dtp);
		font->Select(*dc);
	}
	void onCreate(cs::IWnd*){
		debugBut.Create(_Handle);
	}
	void onDebug(WORD id,WORD type,cs::IWnd*){
		cs::LogShowWindow(); 
		cs::Log(L"孙中山先是从檀香山到香港，然后再改乘一中国沙船回到香山县金星港。当船进入中国海域后，船主特别提醒乘客，对上船来盘查收税的清政府官吏千万要毕恭毕敬，耐心等候，免得招惹麻烦。");
	}
public:
	Main(){
		debugBut.Param->Text = L"debug";
		debugBut.Param->SetRect(480,0,80,30);
		debugBut.OnCommand.Add(this,&Main::onDebug);
		OnCreate.Add(this,&Main::onCreate);
		cs::Font* font = cs::GetDefaultFont();
		font->SetSize(12);
		font->Create();
		::ZeroMemory(&dtp,sizeof(dtp));
		dtp.cbSize = sizeof(dtp);
		dtp.iTabLength = 8;
		str = L"搜狗\t浏\r览\n器日前宣\t布，dgdsfgdsfgsdfgdsfgds为小米路由器带来了研发两年的的网页加速引擎技术“搜狗预取引擎”，目前这一新插件已上线。\r\n\
			   据官方介绍，“搜狗预取引擎”能精准预测下一步访问网页并提前加载，打开网页至少快一倍。小米路由器用户可以体验一下。";
		count = cs::WcsLength(str);
		OnDoubleDraw.Add(this,&Main::onDraw);
		cs::SetWndRememberPos(this,L"wnd");
	}
};
int decrypt(){
	LPCWSTR src = L"F:\\user\\Dropbox\\Dropbox\\Private\\pw2.enc";
	cs::File file;
	if(!file.Create(src)) return 0;
	cs::Memory<char> decData,encData;
	int srclen = file.Read(&encData);

	decData.SetLength(96);
	cs::Aes aes;
	LPCSTR key = "昨天下了一夜雨走起路来脚挂泥";
	LPCSTR iv = "天上有几丝云彩依然是好天气";
	aes.SetDecKey(key,strlen(key),cs::aes_256);
	aes.SetIV(iv);
	aes.DecryptCbc(decData.Handle(),encData.Handle(),srclen);
	::OutputDebugStringA(decData);
	int r = 0;
	return r;
}
int encrypt(){
	LPCWSTR src = L"F:\\user\\Dropbox\\Dropbox\\Private\\pw.txt";
	cs::File file;
	if(!file.Create(src)) return 0;
	cs::Memory<char> decData,encData;
	int srclen = file.Read(&decData);
	cs::Aes aes;
	LPCSTR key = "昨天下了一夜雨走起路来脚挂泥";
	LPCSTR iv = "天上有几丝云彩依然是好天气";
	aes.SetEncKey(key,strlen(key),cs::aes_256);
	aes.SetIV(iv);
	int enclen = 96;
	encData.SetLength(enclen);
	aes.EncryptCbc(encData.Handle(),decData.Handle(),srclen);

	LPCWSTR encfn = L"F:\\user\\Dropbox\\Dropbox\\Private\\pw2.enc";
	file.Create(encfn);
	file.SetLength(0);
	file.Write(encData.Handle(),enclen);
	return 0;
}
static int count = 0;
class A{
public:
	cs::CriticalSection _cs;
	cs::IntegerList<int> ids;
	void proc0(){
		;
	}
	void proc1(int sid){
		Sleep(100);
		DWORD id = GetCurrentThreadId();
		HANDLE h = cs::DuplicateCurrentThread();
		_cs.Lock();
		ids.DeleteValue(sid);
		//cs::Print(L"%d,%d,%d,%d",h,id,i,sid);
		//cs::Print(L"call:%d",sid);
		count++;
		_cs.Unlock();
	}
	void proc2(int p1,int){
		proc1(p1);
	}
	void proc3(int,int,int){
		;
	}
	void proc4(int,int,int,int){
		;
	}
	void proc5(int p1,int,int,int,int){
		proc1(p1);
	}
	void tasktest(){
		int count = 100;
		cs::TaskPool2<void,int,int,int,int,int> tp;
		for(int i=0;i<count;i++){
			ids.Add(i);
		}
		for(int i=0;i<count;i++){
			//Sleep(10);
			tp.StartTask(this,&A::proc5,i,0,0,0,0);
		}
	}
	void tasktest0(){
		cs::TaskPool2<void> tp;
		tp.StartTask(this,&A::proc0);
	}
	void tasktest2(){
		cs::TaskPool2<void,int,int> tp;
		tp.StartTask(this,&A::proc2,0,0);
	}
	void tasktest3(){
		cs::TaskPool2<void,int,int,int> tp;
		tp.StartTask(this,&A::proc3,0,0,0);
	}
	void tasktest4(){
		cs::TaskPool2<void,int,int,int,int> tp;
		tp.StartTask(this,&A::proc4,0,0,0,0);
	}
	void tasktest5(){
		cs::TaskPool2<void,int,int,int,int,int> tp;
		tp.StartTask(this,&A::proc5,0,0,0,0,0);
	}
	void show(){
		cs::Print(L"call count: %d",count);
		for(uint i=0;i<ids.Count();i++){
			cs::Print(L"ids:%d",ids[i]);
		}
	}
};
WINMAIN{
	A a;
	a.tasktest();
	a.show();
	return 0;
};