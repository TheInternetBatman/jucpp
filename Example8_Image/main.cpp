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
				pix[index] = 0xff;
				pix[index+1] = 0x99;
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
//生成android使用的不同大小的图标，src包含源文件名称，dst是android目录
void createAndroidIcon(LPCWSTR src,LPCWSTR dst){
	ju::Bitmap bmp;
	if(!bmp.Load(src)) return;
	ju::Twin sz = bmp.Size();
	ju::LogfI(L"成功加载源图像: %d X %d",sz.x,sz.y);
	ju::Bitmap newBmp;

	int w = 144;
	newBmp = bmp.GetClip();
	newBmp.Resize(144,144);
	ju::String dstfn;
	dstfn = dst;
	ju::FPLinkPath(dstfn,L"res\\drawable-xxhdpi\\ic_launcher.png");
	if(newBmp.Save(dstfn)) ju::LogfI(L"保存成功：%d X %d",w,w);

	w = 96;
	newBmp = bmp.GetClip();
	newBmp.Resize(96,96);
	dstfn = dst;
	ju::FPLinkPath(dstfn,L"res\\drawable-xhdpi\\ic_launcher.png");
	if(newBmp.Save(dstfn)) ju::LogfI(L"保存成功：%d X %d",w,w);

	w = 72;
	newBmp = bmp.GetClip();
	newBmp.Resize(72,72);
	dstfn = dst;
	ju::FPLinkPath(dstfn,L"res\\drawable-hdpi\\ic_launcher.png");
	if(newBmp.Save(dstfn)) ju::LogfI(L"保存成功：%d X %d",w,w);

	w = 48;
	newBmp = bmp.GetClip();
	newBmp.Resize(48,48);
	dstfn = dst;
	ju::FPLinkPath(dstfn,L"res\\drawable-mdpi\\ic_launcher.png");
	if(newBmp.Save(dstfn)) ju::LogfI(L"保存成功：%d X %d",w,w);
}
WINMAIN{
	ju::LogShowWindow();
	src = L"D:\\Develop\\android\\SportsCounter\\res\\drawable-hdpi";
	dst = L"D:\\Develop\\android\\SportsCounter\\res\\drawable-hdpi";

	//setSrcPng();
	//setPng(L"ic_help_48.png");
	createAndroidIcon(L"D:\\Develop\\android\\JuQr\\png\\qr.png",L"D:\\Develop\\android\\JuQr");
	ju::MsgLoop::Start();
}
