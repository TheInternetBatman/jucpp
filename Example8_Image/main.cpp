#include "../juwnd.h"
#pragma usexpstyle

LPCWSTR src = L"D:/Develop/android/English/res/drawable-mdpi";
LPCWSTR dst = L"D:/Develop/android/English/res/drawable-hdpi";

void setPng(LPCWSTR name){
	ju::LocalString file = src;
	ju::FPLinkPath(file,name);
	ju::Bitmap png;
	if(!png.Load(file)){
		ju::LogfE(L"load failed: %s",file.Handle());
		return;
	}
	ju::Memory<byte> pix;
	int h = png.GetPixelData(&pix,32);
	for(int i=0;i<h;i++){
		for(int j=0;j<h;j++){
			int index = i*h*4 + j*4;
			if(pix[index+3]){
				pix[index] = 0x33;
				pix[index+1] = 0x33;
				pix[index+2] = 0x33;
			}
		}
	}
	png.SetPixelData(&pix);
	file = dst;
	ju::FPLinkPath(file,name);
	png.Save(file);
	ju::LogfI(L"success: %s",name);
}
void __stdcall onSearch(ju::ListData* fsd){
	setPng(fsd->data->cFileName);
}
void setSrcPng(){
	ju::FileSearch search;
	search.OnList.BindStd(&onSearch);
	search.Search(src);
}
WINMAIN{
	ju::LogShowWindow();
	src = dst;
	//src = L"D:\\Develop\\android\\Julib\\res\\drawable-hdpi";
	//dst = L"D:\\Develop\\android\\Julib\\res\\drawable-hdpi";
	setPng(L"ic_sort_48.png");
	ju::MsgLoop::Start();
}
