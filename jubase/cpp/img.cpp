#include "../stdafx.h"
#include "../../jubase.h"

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
namespace ju{
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
		static Memory<char>* buffer = 0;
		if(buffer==0){
			MutexLock ml(L"{CF0869EE-45F2-4df9-AA5F-FE632ACDD8A4}");
			if(buffer==0){
				void* buf = mallocGlobalStaticMem(sizeof(Memory<char>));
				buffer = new (buf) Memory<char>;
			}
		}
		static UINT number = 0;
		Gdiplus::ImageCodecInfo* imageCodecInfo = (Gdiplus::ImageCodecInfo*)buffer->Handle();
		if(imageCodecInfo==0){
			UINT size = 0;
			Gdiplus::GetImageEncodersSize(&number,&size);
			if(!size) return 0;
			buffer->SetLength(size);
			imageCodecInfo = (Gdiplus::ImageCodecInfo*)buffer->Handle();
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
	/*class Gdiplus::Bitmap : public Gdiplus::Bitmap{
	public:
		_Image(){}
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
	};*/
#define himg ((Gdiplus::Bitmap*)_image)
	Gdibmp::Gdibmp():_image(0){
		static GdiplusInit gi;
	}
	bool Gdibmp::Create(HBITMAP bmp){
		if(_image) delete himg;
		Bitmap csbmp;
		if(!csbmp.Attach(bmp)) return 0;
		Twin sz = csbmp.Size();
		if(sz.x==0) return 0;
		Memory<byte> srcpix;
		srcpix.SetUseSysMemory(true);
		csbmp.GetPixelData(&srcpix,32);
		csbmp.Detach();
		_image = new Gdiplus::Bitmap(sz.x,sz.y,PixelFormat32bppARGB);
		Gdiplus::BitmapData bd;
		himg->LockBits(NULL,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&bd);
		uint lastLine = srcpix.Length() - bd.Stride;
		for(uint i=0;i<bd.Height;i++){
			uint offset = i*bd.Stride;
			byte* dst = (byte*)bd.Scan0 + lastLine - offset;
			srcpix.CopyTo(dst,bd.Stride,offset);
		}
		srcpix.Free();
		himg->UnlockBits(&bd);
		return true;
	}
	bool Gdibmp::Create(const wchar_t* file){
		if(himg) delete himg;
		_image = new Gdiplus::Bitmap(file,1);
		return Width()!=0;
	}

	bool Gdibmp::Create(void* mem,int len){
		if(himg) delete himg;
		IStream* stream;
		if(CreateStreamOnHGlobal(0,1,&stream)) return 0;
		ULONG ws = 0;
		stream->Write(mem,len,&ws);
		_image = new Gdiplus::Bitmap(stream);
		return himg!=0;
	}
	bool Gdibmp::Save(const wchar_t* file,long quality){
		if(himg==0) return 0;
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
		return himg->Save(file,&clsid,&eps)==0;
	}
	int Gdibmp::Save(Memory<char>& mem,LPCWSTR type,long quality){
		if(himg==0) return 0;

		CLSID clsid;
		//未知原因，如果不用一个变量转换一下，type是一个常量字串的话，如果把 type 直接传给 GetImageEncoderClsid 会引起程序崩溃。
		//wchar_t tp[256];
		//这样也不行。
		//WcsCopy(tp,type);
		String tp = type;
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
		if(himg->Save(stream,&clsid,&eps)!=0) return 0;

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
	bool Gdibmp::Draw(HDC hdc,int x,int y,int cx,int cy){
		if(himg==0) return 0;
		Gdiplus::Graphics gph(hdc);
		if(cx==0) cx = himg->GetWidth();
		if(cy==0) cy = himg->GetHeight();
		Gdiplus::Point pt[3] = {Gdiplus::Point(x,y),Gdiplus::Point(x+cx,x),Gdiplus::Point(y,cy+y)};
		return gph.DrawImage(himg,pt,3)==0;
	}
	int Gdibmp::Width(){
		if(himg==0) return 0;
		return himg->GetWidth();
	}
	int Gdibmp::Height(){
		if(himg==0) return 0;
		return himg->GetHeight();
	}
	void Gdibmp::operator = (Gdibmp& img){
		if(himg) delete himg;
		_image = ((Gdiplus::Bitmap&)img).Clone(0,0,Width(),Height(),PixelFormat32bppARGB);
	}
	/*bool Gdibmp::Thumbnail(int cx,int cy,Gdibmp* srImg){
		Gdiplus::Bitmap* img;
		if(srImg) img = (Gdiplus::Bitmap*)srImg->_image;
		else img = himg;
		if(img==0) return 0;
		img = img->GetThumbnailImage(cx,cy,0,0);
		if(himg) delete himg;
		_image = img;
		return himg!=0;
	}*/
	uint Gdibmp::FrameCount(){
		if(!himg) return 0;
		UINT count = himg->GetFrameDimensionsCount();   
		Memory<GUID> ids;
		ids.SetLength(count);
		himg->GetFrameDimensionsList(ids, count);   
		return himg->GetFrameCount(ids);   
	}
	uint Gdibmp::GetPropertyItem(Memory<ImageItem>& tms){
		if(!himg) return 0;
		int nSize = himg->GetPropertyItemSize(PropertyTagFrameDelay);   
		tms.SetLength(nSize);
		himg->GetPropertyItem(PropertyTagFrameDelay, nSize, (Gdiplus::PropertyItem*)tms.Handle());   
		return nSize;
	}
	bool Gdibmp::SetActiveFrame(int index){
		if(himg==0) return 0;
		Gdiplus::Bitmap* img = himg;
		Gdiplus::Status status = img->SelectActiveFrame(&Gdiplus::FrameDimensionTime,index);
		return status==Gdiplus::Ok;
	}
	HBITMAP Gdibmp::GetBitmap(){
		if(himg==0) return 0;
		Gdiplus::Bitmap* img = himg;
		int cx = img->GetWidth();
		if(cx==0) return 0;
		int cy = img->GetHeight();
		if(cy==0) return 0;
		DC dc;
		dc.GetDC();
		HBITMAP bmp = CreateBitmap(dc,cx,cy);
		if(bmp==0){
			PrintLastErrorD(L"Gdibmp::GetBitmap: %s");
			return 0;
		}
		HDC hdc = ::CreateCompatibleDC(dc);
		dc.Close();
		HGDIOBJ old = ::SelectObject(hdc,bmp);
		Gdiplus::Graphics gph(hdc);
		Gdiplus::Point pt[3] = {Gdiplus::Point(0,0),Gdiplus::Point(cx,0),Gdiplus::Point(0,cy)};
		gph.DrawImage(himg,pt,3);
		bmp = (HBITMAP)::SelectObject(hdc,old);
		::DeleteDC(hdc);
		return bmp;
	}
	uint Gdibmp::PixelFormat(){
		if(himg==0) return 0;
		Gdiplus::Bitmap* img = himg;
		return img->GetPixelFormat();
	}
	/*int Gdibmp::GetPixel(int x,int y){
		if(himg==0) return 0;
		Gdiplus::Bitmap* img = himg;
		Gdiplus::Color color;
		color.Value = 0;
		img->GetPixel(x,y,&color);
		return color.Value;
	}
	bool Gdibmp::SetPixel(int x,int y,int col){
		if(himg==0) return 0;
		Gdiplus::Bitmap* img = himg;
		Gdiplus::Color color;
		color.Value = col;
		return 0==img->SetPixel(x,y,color);
	}*/
	void Gdibmp::Close(){
		if(!himg) return;
		delete himg;
		_image = 0;
	}
	Gdibmp::~Gdibmp(){
		if(himg) delete himg;
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