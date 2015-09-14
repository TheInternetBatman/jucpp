#include "../stdafx.h"
#include "../../concise.h"

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
namespace cs{
	class GdiplusInit{
		ULONG_PTR _Token;
		Gdiplus::GdiplusStartupInput _Version;
	public:
		GdiplusInit(){
			Gdiplus::GdiplusStartup(&_Token,&_Version,0);
		}
		~GdiplusInit(){
			Gdiplus::GdiplusShutdown(_Token);
		}
	};
	//class ImgEncode
	bool GetImageEncoderClsid(const wchar_t* type,CLSID* pClsid){
		static Memory<char> buffer;
		static UINT number = 0;
		Gdiplus::ImageCodecInfo* imageCodecInfo = (Gdiplus::ImageCodecInfo*)buffer.Handle();
		if(imageCodecInfo==0){
			UINT size = 0;
			Gdiplus::GetImageEncodersSize(&number,&size);
			if(!size) return 0;
			buffer.SetLength(size);
			imageCodecInfo = (Gdiplus::ImageCodecInfo*)buffer.Handle();
			Gdiplus::GetImageEncoders(number,size,imageCodecInfo);
		}
		Registry reg;
		if(!reg.Create(type,HKEY_CLASSES_ROOT)) return 0;
		String str(16);
		if(!reg.GetStrValue(L"Content Type",str)) return 0;
		for(UINT i=0;i<number;i++){
			if(WcsEqual(str,imageCodecInfo[i].MimeType)){
				*pClsid = imageCodecInfo[i].Clsid;
				return 1;
			}
		}
		return 0;
	}
	class _Image : public Gdiplus::Image{
	public:
		_Image(LPCWSTR file){

		}
		_Image(HBITMAP bmp){
			Gdiplus::GpBitmap *bitmap = NULL;
			lastResult = Gdiplus::DllExports::GdipCreateBitmapFromHBITMAP(bmp,0,&bitmap);
			SetNativeImage(bitmap);
		}
		_Image(IStream* stream){
			Gdiplus::GpBitmap *bitmap = NULL;
			lastResult = Gdiplus::DllExports::GdipCreateBitmapFromStream(stream,&bitmap);
			SetNativeImage(bitmap);
		}
	};
	Image::Image():_image(0){
		static GdiplusInit gi;
	}
	bool Image::Create(HBITMAP bmp){
		if(_image) delete (Gdiplus::Image*)_image;
		_image = new _Image(bmp);
		return Width()!=0;
	}
	bool Image::Create(const wchar_t* file){
		if(_image) delete (Gdiplus::Image*)_image;
		_image = new Gdiplus::Image(file,1);
		return Width()!=0;
	}

	bool Image::Create(void* mem,int len){
		if(_image) delete (Gdiplus::Image*)_image;
		IStream* stream;
		if(CreateStreamOnHGlobal(0,1,&stream)) return 0;
		ULONG ws = 0;
		stream->Write(mem,len,&ws);
		_image = new _Image(stream);
		return _image!=0;
	}
	bool Image::Save(const wchar_t* file,long quality){
		if(_image==0) return 0;
		String fp;
		fp = file;
		String type = file;
		FPToExt(type);
		type.Insert(L".",0);
		CLSID clsid;
		if(!GetImageEncoderClsid(type,&clsid)) return 0;
		if(quality>100) quality = 100;
		if(quality<0) quality = 0;
		Gdiplus::EncoderParameters eps;
		eps.Count = 1;
		eps.Parameter[0].Guid = Gdiplus::EncoderQuality;
		eps.Parameter[0].NumberOfValues = 1;
		eps.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
		eps.Parameter[0].Value = &quality;
		return ((_Image*)_image)->Save(file,&clsid,&eps)==0;
	}
	int Image::Save(Memory<char>& mem,LPCWSTR type,long quality){
		if(_image==0) return 0;

		CLSID clsid;
		//未知原因，如果不用一个变量转换一下，type是一个常量字串的话，如果把 type 直接传给 GetImageEncoderClsid 会引起程序崩溃。
		//wchar_t tp[256];
		//这样也不行。
		//cs::WcsCopy(tp,type);
		cs::String tp = type;
		if(!GetImageEncoderClsid(tp,&clsid)) return 0;

		if(quality>100) quality = 100;
		if(quality<0) quality = 0;
		Gdiplus::EncoderParameters eps;
		eps.Count = 1;
		eps.Parameter[0].Guid = Gdiplus::EncoderQuality;
		eps.Parameter[0].NumberOfValues = 1;
		eps.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
		eps.Parameter[0].Value = &quality;

		IStream* stream;
		if(FAILED(CreateStreamOnHGlobal(0,1,&stream))) return 0;
		if(((_Image*)_image)->Save(stream,&clsid,&eps)!=0) return 0;

		ULONG ws = 0;
		LARGE_INTEGER li;
		ULARGE_INTEGER npos;
		li.QuadPart = 0;
		stream->Seek(li,STREAM_SEEK_END,&npos);
		if(mem.Length()<npos.LowPart){
			mem.SetUseSysMemory(1);
			if(!mem.SetLength(npos.LowPart)) return 0;
		}
		stream->Seek(li,STREAM_SEEK_SET,0);

		HRESULT ret = stream->Read(mem.Handle(),mem.Length(),&ws);
		stream->Release();
		if(FAILED(ret)) return 0;
		return ws;
	}
	bool Image::Draw(HDC hdc,int x,int y,int cx,int cy){
		if(_image==0) return 0;
		Gdiplus::Graphics gph(hdc);
		if(cx==0) cx = ((_Image*)_image)->GetWidth();
		if(cy==0) cy = ((_Image*)_image)->GetHeight();
		Gdiplus::Point pt[3] = {Gdiplus::Point(x,y),Gdiplus::Point(x+cx,x),Gdiplus::Point(y,cy+y)};
		return gph.DrawImage((_Image*)_image,pt,3)==0;
	}
	int Image::Width(){
		if(_image==0) return 0;
		return ((_Image*)_image)->GetWidth();
	}
	int Image::Height(){
		if(_image==0) return 0;
		return ((_Image*)_image)->GetHeight();
	}
	void Image::operator = (Image& img){
		if(_image) delete (_Image*)_image;
		_image = ((_Image*)img._image)->Clone();
	}
	bool Image::GetThumbnail(int cx,int cy,Image* srImg){
		_Image* img;
		if(srImg) img = (_Image*)srImg->_image;
		else img = (_Image*)_image;
		if(img==0) return 0;
		img = (_Image*)img->GetThumbnailImage(cx,cy,0,0);
		if(_image) delete (_Image*)_image;
		_image = img;
		return _image!=0;
	}
	uint Image::FrameCount(){
		if(!_image) return 0;
		UINT count = ((_Image*)_image)->GetFrameDimensionsCount();   
		Memory<GUID> ids;
		ids.SetLength(count);
		((_Image*)_image)->GetFrameDimensionsList(ids, count);   
		return ((_Image*)_image)->GetFrameCount(ids);   
	}
	uint Image::GetPropertyItem(Memory<ImageItem>& tms){
		if(!_image) return 0;
		int nSize = ((_Image*)_image)->GetPropertyItemSize(PropertyTagFrameDelay);   
		tms.SetLength(nSize);
		((_Image*)_image)->GetPropertyItem(PropertyTagFrameDelay, nSize, (Gdiplus::PropertyItem*)tms.Handle());   
		return nSize;
	}
	bool Image::SetActiveFrame(int index){
		if(_image==0) return 0;
		_Image* img = ((_Image*)_image);
		Gdiplus::Status status = img->SelectActiveFrame(&Gdiplus::FrameDimensionTime,index);
		return status==Gdiplus::Ok;
	}
	HBITMAP Image::GetBitmap(){
		if(_image==0) return 0;
		_Image* img = ((_Image*)_image);
		int cx = img->GetWidth();
		if(cx==0) return 0;
		int cy = img->GetHeight();
		if(cy==0) return 0;
		DC dc;
		dc.GetDC();
		HBITMAP bmp = CreateBitmap(dc,cx,cy);
		if(bmp==0){
			PrintLastErrorD(L"Image::GetBitmap: %s");
			return 0;
		}
		HDC hdc = ::CreateCompatibleDC(dc);
		dc.ReleaseDC();
		HGDIOBJ old = ::SelectObject(hdc,bmp);
		Gdiplus::Graphics gph(hdc);
		Gdiplus::Point pt[3] = {Gdiplus::Point(0,0),Gdiplus::Point(cx,0),Gdiplus::Point(0,cy)};
		gph.DrawImage((_Image*)_image,pt,3);
		bmp = (HBITMAP)::SelectObject(hdc,old);
		::DeleteDC(hdc);
		return bmp;
	}
	uint Image::PixelFormat(){
		if(_image==0) return 0;
		_Image* img = ((_Image*)_image);
		return img->GetPixelFormat();
	}
	/*int Image::GetPixel(int x,int y){
		if(_image==0) return 0;
		_Image* img = ((_Image*)_image);
		Gdiplus::Color color;
		color.Value = 0;
		img->GetPixel(x,y,&color);
		return color.Value;
	}
	bool Image::SetPixel(int x,int y,int col){
		if(_image==0) return 0;
		_Image* img = ((_Image*)_image);
		Gdiplus::Color color;
		color.Value = col;
		return 0==img->SetPixel(x,y,color);
	}*/
	void Image::Close(){
		if(!_image) return;
		delete (_Image*)_image;
		_image = 0;
	}
	Image::~Image(){
		if(_image) delete (_Image*)_image;
	}
	/* Gdiplus::Bitmap 操作像素数据示例
	Bitmap bmp(L"a.jpg");
	BitmapData bd;
	Rect rect(0,0,w,h);
	Status ret = bmp.LockBits(&rect,ImageLockModeWrite,PixelFormat24bppRGB,&bd);
	for(int i=0;i<w;i++){
		for(int j=0;j<h;j++){
			uchar* d = (uchar*)bd.Scan0;
			int index = 3*(j+i*h);
			int ac = d[index] + d[index+1] + d[index+2];
			ac /= 3;
			d[index] = (uchar)ac;
			d[index+1] = (uchar)ac;
			d[index+2] = (uchar)ac;
		}
	}
	bmp.UnlockBits(&bd);
	*/
}