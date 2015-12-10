#include "../juwnd.h"
#pragma usexpstyle
#include <Wincrypt.h>
#pragma comment(lib, "Advapi32.lib")

int findLine(ju::String& str,int pos);

int findIt(ju::String& str,int wstart,ju::String& nstr){
	int start = str.Find(L"Î¢ÐÅ¹«ÖÚºÅ£ºaiddream",wstart);
	if(start==-1){
		nstr.CopyFrom(str+wstart);
		return -1;
	}
	int pos = start;
	for(int i=0;i<4;i++){
		pos = findLine(str,pos);
		if(pos==-1) break;
	}
	if(pos==-1) return -1;
	nstr.CopyFrom(str+wstart,start-wstart);
	ju::String sub;
	sub.CopyFrom(str+start,pos-start,0);
	ju::LogfI(L"%s",sub.Handle());
	return pos;
}
int findLine(ju::String& str,int pos){
	int r = str.Find(L"\r\n",pos);
	if(r==-1) return -1;
	ju::String sub;
	sub.CopyFrom(str+pos,r-pos,0);
	return r + 2;
}
void findAll(ju::String& str,ju::String& nstr){
	int pos = 0;
	while(pos!=-1){
		pos = findIt(str,pos,nstr);
	}
}
void remove(){
	ju::FileStream fs;
	fs.Create(L"word.txt");
	ju::String str,nstr;
	fs.ReadString(str);
	findAll(str,nstr);
	fs.SetLength(0);
	fs.WriteString(nstr,0,0,CP_UTF8);
}
inline bool not_az(int ch){
	return ch<'a'||ch>'z';
}
bool isVerb(ju::String& line);
void oneLine(){
	ju::FileStream fs;
	fs.Create(L"word.txt");
	ju::String str,nstr;
	fs.ReadString(str,0,CP_UTF8);

	ju::StringList sl;
	sl.SplitString(str,L"\r\n");
	for(uint i=0;i<sl.Count();i++){
		ju::String& line = sl[i];
		if(not_az(line[0])){
			nstr += line;
		}else{
			if(nstr.Length()>0) nstr += L"\r\n";
			nstr += line;
		}
	}
	fs.SetLength(0);
	fs.WriteString(nstr,0,0,CP_UTF8);
}
bool isVerb(ju::String& line){
	int pos = line.Find(L".");
	if(pos==-1) return false;
	for(int i=0;i<pos;i++){
		if(not_az(line[i])) return false;
	}
	return true;
}
void createWord(ju::Json* jw,ju::String& line){
	int pos1 = line.Find(L"[");
	if(pos1==-1){
		ju::LogfE(L"error not find [:%s",line.Handle());
		return;
	}
	int pos2 = line.Find(L"]",pos1);
	if(pos1==-1){
		ju::LogfE(L"error not find ]:%s",line.Handle());
		return;
	}
	ju::String word;
	word.CopyFrom(line,pos1);
	word.Trim();
	jw->SetPropertyStr(L"word",word);
	ju::String ph;
	ph.CopyFrom(line+pos1+1,pos2-pos1-1);
	jw->SetPropertyStr(L"phonetic",ph);
	ju::String exp;
	exp.CopyFrom(line+pos2+1);
	exp.Trim();
	jw->SetPropertyStr(L"explain",exp);
}
void toJson(){
	ju::FileStream fs;
	fs.Create(L"word.txt");
	ju::String str,nstr;
	fs.ReadString(str,0,CP_UTF8);

	ju::Json json;
	ju::StringList sl;
	sl.SplitString(str,L"\r\n");
	for(uint i=0;i<sl.Count();i++){
		ju::String& line = sl[i];
		ju::Json* word = json.AddArrayElm();
		createWord(word,line);
	}
	json.SaveToFile(L"word.json",CP_UTF8);
}
class Random : public ju::_class{
protected:
	HCRYPTPROV _handle;
public:
	Random():_handle(0){
		CryptAcquireContext(&_handle, NULL, NULL, PROV_RSA_FULL, 0);
	}
	~Random(){
		CryptReleaseContext(_handle, 0);
	}
	uint getRandom(){
		BYTE         pbData[4];
		if (!CryptGenRandom(_handle,4,pbData)) return 0;
		return *(uint*)pbData;
	}
};
/*class PjMem{
protected:
	pj_caching_pool _handle;
	pj_pool_t *_pool;

public:
	PjMem(){
		int status = pj_init();
		pj_caching_pool_init(&_handle, NULL, 1024*1024 );
		// Must create pool before we can allocate anything
		_pool = pj_pool_create(&_handle.factory, // the factory
			"pool1", // pool's name
			4000, // initial size
			4000, // increment size
			NULL); // use default callback.
	}
	~PjMem(){
		pj_pool_release(_pool);
		pj_caching_pool_destroy(&_handle);
	}
	void* alloc(int size){
		return pj_pool_alloc(_pool,size);
	}
};*/
void randomMalloc(){
	Random rand;
	ju::Memory<void*> buf(90000);
	buf.SetLength(90000);
	ju::LogfI(L"start");
	for(uint i=0;i<90000;i++){
		uint r = rand.getRandom()%10000;
		//buf[i] = malloc(r);
		buf[i] = ju::SYSAlloc(r);
		uint j = r*90;
		if(j<=i&&buf[j]!=0){
			ju::SYSFree(buf[r]);
			//free(buf[i]);
			buf[r] = 0;
		}
	}
	ju::LogfI(L"end");
}
WINMAIN{
	/*void* mem1 = ju::MemoryAlloc(2442);
	void* mem2 = ju::MemoryAlloc(235);
	ju::MemoryFree(mem1);
	mem2 = ju::MemoryAlloc(235);
	mem2 = ju::MemoryRealloc(mem2,1235);*/
	ju::LogShowWindow();
	//remove();
	//oneLine();
	//toJson();
	randomMalloc();
	ju::MsgLoop::Start();
}
