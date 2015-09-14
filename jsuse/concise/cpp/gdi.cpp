#include "../stdafx.h"
#include "../../concise.h"

#pragma comment(lib,"msimg32.lib")//this is to call AlphaBlend function.
namespace cs{
	//union Color
	Color::Color(){
	}
	Color::Color(int value){
		this->Value = value;
	}
	Color::Color(BYTE r,BYTE g,BYTE b,BYTE a){
		red = r;
		green = g;
		blue = b;
		alpha = a;
	}
	void Color::SetValue(BYTE r,BYTE g,BYTE b,BYTE a){
		red = r;
		green = g;
		blue = b;
		alpha = a;
	}
	void Color::operator = (COLORREF color){
		Value = color;
	}
	Color::operator COLORREF (){
		return Value;
	}
	//class Rgn
	int Rgn::Combine(HRGN rgn,DWORD cbMode){
		if(cbMode==RGN_COPY)
			return ::CombineRgn(_Handle,rgn,0,cbMode);
		return ::CombineRgn(_Handle,_Handle,rgn,cbMode);
	}
	bool Rgn::Create(){
		return Attach(::CreateRectRgn(0,0,0,0));
	}
	bool Rgn::CreateRect(int x1,int y1,int x2,int y2){
		HRGN rgn = ::CreateRectRgn(x1,y1,x2,y2);
		if(!rgn) return 0;
		return Attach(rgn);
	}
	void Rgn::operator = (HRGN rgn){
		if(rgn){
			if(IsNull()) Create();
			::CombineRgn(_Handle,rgn,0,RGN_COPY);
		}else{
			Delete();
		}
	}
	void Rgn::operator = (Rgn& rgn){
		if(rgn.IsNull()){
			Delete();
		}else{
			if(IsNull()) Create();
			::CombineRgn(_Handle,rgn,0,RGN_COPY);
		}
	}
	//class Pen
	bool Pen::Create(COLORREF color,int style,int width){
		color &=0xffffff;
		return Attach(::CreatePen(style,width,color));
	}
	COLORREF Pen::SetDCColor(HDC hdc,COLORREF color){
		return SetDCPenColor(hdc,color);
	}
	COLORREF Pen::GetDCColor(HDC hdc){
		return ::GetDCPenColor(hdc);
	}
	COLORREF Pen::GetColor(){
		LOGPEN lp;
		GetLogPen(&lp);
		return lp.lopnColor;
	}

	//class Brush
	bool Brush::Create(COLORREF color,UINT style,DWORD hatch){
		LOGBRUSH lb;
		color &= 0xffffff;
		lb.lbColor = color;
		lb.lbHatch = hatch;
		lb.lbStyle = style;
		return Attach(::CreateBrushIndirect(&lb));
	}
	COLORREF Brush::GetColor(){
		LOGBRUSH lb;
		GetLogBrush(&lb);
		return lb.lbColor;
	}
	//class Font
	bool Font::Create(){
		return Attach(::CreateFontW(_LogFont->lfHeight,_LogFont->lfWidth,_LogFont->lfEscapement,_LogFont->lfOrientation,_LogFont->lfWeight,_LogFont->lfItalic,_LogFont->lfUnderline,_LogFont->lfStrikeOut,_LogFont->lfCharSet,_LogFont->lfOutPrecision,_LogFont->lfClipPrecision,_LogFont->lfQuality,_LogFont->lfPitchAndFamily,_LogFont->lfFaceName));
	}
	void Font::Reset(){
		_LogFont->lfHeight = 12;
		_LogFont->lfWidth = 0; 
		_LogFont->lfEscapement = 0; 
		_LogFont->lfOrientation = 0; 
		_LogFont->lfWeight = FW_NORMAL;//0--1000; 
		_LogFont->lfItalic = 0; 
		_LogFont->lfUnderline = 0; 
		_LogFont->lfStrikeOut = 0; 
		_LogFont->lfCharSet = DEFAULT_CHARSET;
		_LogFont->lfOutPrecision = OUT_DEFAULT_PRECIS; 
		_LogFont->lfClipPrecision = CLIP_DEFAULT_PRECIS; 
		_LogFont->lfQuality = DEFAULT_QUALITY;//0--5
		_LogFont->lfPitchAndFamily = 0; 
		::ZeroMemory(_LogFont->lfFaceName,64); 
	}
	// class DC
	bool DC::GetDC(HWND hWnd){
		if(_Handle) return 0;
		_Handle = ::GetDC(hWnd);
		return 0!=_Handle;
	}
	bool DC::GetWindowDC(HWND hWnd){
		if(_Handle) return 0;
		_Handle = ::GetWindowDC(hWnd);
		return 0!=_Handle;
	}
	bool DC::ReleaseDC(HWND hWnd){
		if(_Handle==0) return 0;
		return 0!=::ReleaseDC(hWnd,_Handle);
	}
	bool DC::DeleteDC(){
		if(!_Handle) return 0;
		return 0!=::DeleteDC(_Handle);
	}
	bool DC::CreateCompatible(HDC hdc){
		//hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
		_Handle = ::CreateCompatibleDC(hdc);
		return _Handle!=0;
	}
	//class Bitmap
	Twin Bitmap::Size(){
		AutoStruct(BITMAP,bp);
		::GetObject(_Handle,sizeof(BITMAP),&bp);
		return (*(Twin*)(&bp.bmWidth));
	}
	HBITMAP Bitmap::GetClip(int cx,int cy,int x,int y){
		Twin _Size = this->Size();
		if(!_Handle)
			return NULL;
		int dx = _Size.x - x;
		int dy = _Size.y - y;
		if((x<0)||(dx<0))
			return 0;
		if((y<0)||(dy<0))
			return 0;
		if((cx<=0)||(cx>dx))
			cx = dx;
		if((cy<=0)||(cy>dy))
			cy = dy;
		HDC bmpDC = ::CreateCompatibleDC(NULL);
		HBITMAP _Old = (HBITMAP)::SelectObject(bmpDC,_Handle);

		HBITMAP hbp = CreateBitmap(bmpDC,cx,cy);//create a bitmap;
		HDC hdc = ::CreateCompatibleDC(NULL);//create a dc;
		HBITMAP _OldBmp = (HBITMAP)::SelectObject(hdc,hbp);//combin dc and bitmap to draw;

		::BitBlt(hdc,0,0,cx,cy,bmpDC,x,y,SRCCOPY);//copy source bitmap to destination bitmap;
		::SelectObject(bmpDC,_Old);
		::DeleteObject(bmpDC);
		::SelectObject(hdc,_OldBmp);
		::DeleteDC(hdc);
		return hbp;
	}
	void Bitmap::operator = (HBITMAP hBmp){
		HBITMAP hbp = (HBITMAP)::CopyImage(hBmp,IMAGE_BITMAP,0,0,LR_COPYRETURNORG);
		Attach(hbp);
	}
	void Bitmap::operator = (Bitmap& bitmap){
		HBITMAP hbp = (HBITMAP)::CopyImage(bitmap.Handle(),IMAGE_BITMAP,0,0,LR_COPYRETURNORG);
		Attach(hbp);
	}
	bool Bitmap::ClipBitmap(int cx,int cy,int x,int y){
		if(!_Handle)
			return 0;
		Twin _Size = Size();
		int dx = _Size.x - x;
		int dy = _Size.y - y;
		if((x<0)||(dx<0))
			return 0;
		if((y<0)||(dy<0))
			return 0;
		if((cx<=0)||(cx>dx))
			cx = dx;
		if((cy<=0)||(cy>dy))
			cy = dy;

		HDC bmpDC = ::CreateCompatibleDC(NULL);
		HBITMAP _Old = (HBITMAP)::SelectObject(bmpDC,_Handle);

		HBITMAP hbp = CreateBitmap(bmpDC,cx,cy);//create a bitmap;
		HDC hdc = ::CreateCompatibleDC(NULL);//create a dc;
		HBITMAP _OldBmp = (HBITMAP)::SelectObject(hdc,hbp);//combin dc and bitmap to draw;

		::BitBlt(hdc,0,0,cx,cy,bmpDC,x,y,SRCCOPY);//copy source bitmap to destination bitmap;
		::SelectObject(bmpDC,_Old);
		::DeleteObject(bmpDC);
		::DeleteObject(_Handle);
		::SelectObject(hdc,_OldBmp);
		::DeleteDC(hdc);
		_Handle = hbp;
		return 1;
	}
	bool Bitmap::Resize(int cx,int cy){
		if(!_Handle) return 0;
		Twin size(cx,cy),_Size = Size();
		if(_Size==size) return 1;
		//create a new bitmap and dc.
		HDC sdc = ::GetDC(NULL);
		HDC hdc = ::CreateCompatibleDC(0);
		HBITMAP hbmp = CreateBitmap(sdc,cx,cy);
		::ReleaseDC(0,sdc);
		if(hbmp==0){
			::DeleteDC(hdc);
			return 0;
		}
		hbmp = (HBITMAP)::SelectObject(hdc,hbmp);
		//create a draw dc
		HDC bmpDC = ::CreateCompatibleDC(NULL);
		_Handle = (HBITMAP)::SelectObject(bmpDC,_Handle);
		//draw to new bitmap dc;
		//bool result = ::StretchBlt(hdc,0,0,cx,cy,bmpDC,0,0,_Size.x,_Size.y,SRCCOPY)!=0;
		BLENDFUNCTION bf;
		bf.AlphaFormat = 1;
		bf.BlendFlags = 0;
		bf.BlendOp = 0;//AC_SRC_OVER = 0;
		bf.SourceConstantAlpha = 255;
		BOOL result = ::AlphaBlend(hdc,0,0,cx,cy,bmpDC,0,0,_Size.x,_Size.y,bf)!=0;
		//delete resource
		_Handle = (HBITMAP)::SelectObject(bmpDC,_Handle);
		::DeleteDC(bmpDC);
		hbmp = (HBITMAP)::SelectObject(hdc,hbmp);
		::DeleteDC(hdc);
		if(result){
			::DeleteObject(_Handle);
			_Handle = hbmp;
		}
		else{
			::DeleteObject(hbmp);
		}
		return result!=0;
	}
	bool Bitmap::ToAlpha(Color c1,Color c2){
		Memory<unsigned char> data;
		if(!GetPixelData(&data,32)) return 0;
		for(uint i=0;i<data.Length();i+=4){
			int k = *(int*)(data+i);
			k &= 0xffffff;
			if((data[i]>=c1.red)&&(data[i]<=c2.red)&&(data[i+1]>=c1.green)&&(data[i+1]<=c2.green)&&(data[i+2]>=c1.blue)&&(data[i+2]<=c2.blue)){
				data[i] = data[i+1] = data[i+2] = data[i+3] = 0;
			}
		}
		return 0!=SetPixelData(&data,32);
	}
	bool Bitmap::ToAlpha(Color c){
		Memory<unsigned char> data;
		if(!GetPixelData(&data,32)) return 0;
		c.alpha = 0;
		for(uint i=0;i<data.Length();i+=4){
			int k = *(int*)(data+i);
			k &= 0xffffff;
			if(k==c){
				data[i] = data[i+1] = data[i+2] = data[i+3] = 0;
			}
		}
		return 0!=SetPixelData(&data,32);
	}

	bool Bitmap::Load(ResID res,int cx,int cy){
		HBITMAP hBmp = (HBITMAP)::LoadImage(res.Instance,res,IMAGE_BITMAP,cx,cy,0);
		return Attach(hBmp);
	}
	bool Bitmap::CreateFromMemory(LPVOID data,bool fileheader){
		if(fileheader) data = (LPBYTE)data + sizeof(BITMAPFILEHEADER);
		LPBITMAPINFOHEADER pbih = (LPBITMAPINFOHEADER)data;
		LPBYTE bitsData = (LPBYTE)data + PaletteSize(pbih->biBitCount);
		if(pbih->biClrUsed==0) bitsData += pbih->biSize;
		HDC hdc = ::GetDC(0);
		HBITMAP bmp = ::CreateDIBitmap(hdc,pbih,CBM_INIT,bitsData,(LPBITMAPINFO)data,DIB_RGB_COLORS);
		::ReleaseDC(0,hdc);
		return Attach(bmp);
	}
	HBITMAP Bitmap::CreateMonochroic(int cx,int cy){
		return ::CreateBitmap(cx,cy,1,1,0);
	}
	bool Bitmap::FromIcon(HICON hIcon,bool color){
		ICONINFO ici;
		if(!::GetIconInfo(hIcon,&ici))
			return 0;
		HBITMAP hbmp = 0;
		if(color){
			::DeleteObject(ici.hbmMask);
			hbmp = ici.hbmColor;
		}
		else{
			::DeleteObject(ici.hbmColor);
			hbmp = ici.hbmMask;
		}
		return Attach(hbmp);
	}
	int Bitmap::GetPixelData(Memory<unsigned char>* data,WORD BitsPixel){
		if(_Handle==NULL) return 0;
		if (BitsPixel==0) BitsPixel = 32;
		Memory<uchar> pv;
		pv.SetLength(sizeof(BITMAPINFOHEADER)+PaletteSize(BitsPixel)+16);
		pv.Zero();
		LPBITMAPINFO pbi = (LPBITMAPINFO)pv.Handle();
		pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		HPALETTE hpal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);
		DC dc;
		dc.GetDC();
		hpal = dc.SelectPalette(hpal);//this case must be call,or the data format is a screen color depth format.0 is select in dc and 1 is out;
		dc.RealizePalette();
		int count = ::GetDIBits(dc,_Handle,0,0,NULL,pbi,(DWORD)DIB_PAL_COLORS);//调用此函数不一定要把要提取的位图选入DC,这说明DC仅仅提供格式或功能
		if(!pbi->bmiHeader.biSizeImage) goto exit;
		pbi->bmiHeader.biBitCount = BitsPixel;
		pbi->bmiHeader.biCompression = 0;
		count = ::GetDIBits(dc,_Handle,0,0,NULL,pbi,(DWORD)DIB_PAL_COLORS);//调用此函数不一定要把要提取的位图选入DC,这说明DC仅仅提供格式或功能
		if(!count) goto exit;
		data->SetLength(pbi->bmiHeader.biSizeImage);
		count = ::GetDIBits(dc,_Handle,0L,pbi->bmiHeader.biHeight,data->Handle(),pbi,(DWORD)DIB_RGB_COLORS);//DIB_PAL_COLORS
exit:
		dc.SelectPalette(hpal);
		return count;
	}
	int Bitmap::SetPixelData(Memory<unsigned char>* data,WORD BitsPixel){
		return SetPixelData(data->Handle(),data->Length(),BitsPixel);
	}
	int Bitmap::SetPixelData(const unsigned char* data,uint len,WORD BitsPixel){
		if(!_Handle) return 0;
		if (BitsPixel==0) BitsPixel = 32;
		Memory<uchar> pv;
		pv.SetLength(sizeof(BITMAPINFOHEADER)+PaletteSize(BitsPixel));
		pv.Zero();
		LPBITMAPINFO pbi = (LPBITMAPINFO)pv.Handle();
		pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		HPALETTE hpal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);
		DC dc;
		dc.GetDC();
		hpal = dc.SelectPalette(hpal);
		dc.RealizePalette();
		int count = ::GetDIBits(dc,_Handle,0,0,NULL,pbi,(DWORD)DIB_PAL_COLORS);
		if(!count) goto exit;
		pbi->bmiHeader.biBitCount = BitsPixel;
		pbi->bmiHeader.biCompression = 0;
		count = ::GetDIBits(dc,_Handle,0,0,NULL,pbi,(DWORD)DIB_PAL_COLORS);
		if(!count) goto exit;
		count = ::SetDIBits(dc,_Handle,0,pbi->bmiHeader.biHeight,data,pbi,DIB_RGB_COLORS);
exit:
		dc.SelectPalette(hpal);
		return count;
	}
	bool Bitmap::CreateCompatibleBitmap(int cx,int cy){
		DC dc;
		dc.GetDC();
		return Attach(dc.CreateCompatibleBitmap(cx,cy));
	}

	bool Bitmap::SaveBmp(LPCWSTR szFileName,WORD BitsPixel){
		Memory<uchar> dib;
		int dibLen = GetDib(dib,BitsPixel);
		if(!dibLen) return 0;
		LPBITMAPINFO pbi = (LPBITMAPINFO)dib.Handle();

		DWORD headLen = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + PaletteSize(BitsPixel);
		BITMAPFILEHEADER bfh;
		bfh.bfType = 0x4d42;	//BMP文件标识,前两字节.
		bfh.bfReserved1 = 0;	//0,2字节
		bfh.bfReserved2 = 0;	//0,2字节
		bfh.bfOffBits = headLen;	//数据位置偏移.
		bfh.bfSize = pbi->bmiHeader.biSizeImage + headLen;

		HANDLE hFile = CreateFileW(szFileName,GENERIC_WRITE,
			FILE_SHARE_DELETE|FILE_SHARE_WRITE|FILE_SHARE_READ,
			NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile==INVALID_HANDLE_VALUE) return 0;
		DWORD dwBytes = 0;
		WriteFile(hFile,&bfh,sizeof(BITMAPFILEHEADER),&dwBytes,NULL);
		if(dwBytes!=sizeof(BITMAPFILEHEADER)){
			CloseHandle(hFile);
			return 0;
		}
		WriteFile(hFile,dib.Handle(),dibLen,&dwBytes,NULL);
		CloseHandle(hFile);

		return dwBytes==dibLen;
	}
	int Bitmap::Save(Memory<char>& mem,LPCTSTR type,long quality){
		Image img;
		img.Create(_Handle);
		return img.Save(mem,type,quality);
	}
	bool Bitmap::Save(LPCWSTR file,long quality){
		Image img;
		img.Create(_Handle);
		return img.Save(file,quality);
	} 
	bool Bitmap::Load(LPCWSTR file){
		Image img;
		if(!img.Create(file)) return 0;
		return Attach(img.GetBitmap());
	}
	bool Bitmap::Load(void* data,int len){
		Image img;
		if(!img.Create(data,len)) return 0;
		return Attach(img.GetBitmap());
	}
	int Bitmap::GetDib(Memory<unsigned char>& data,WORD bits){
		//一个BMP文件由三部分组成，文件头（BITMAPFILEHEADER），图像信息头（BITMAPINFOHEADER）和调色板数据（可能为零，比如32位图像），像素数据。
		if(_Handle==NULL) return 0;
		if(bits==0) bits = 32;

		DWORD hLen = sizeof(BITMAPINFOHEADER) + PaletteSize(bits);
		data.SetLength(hLen+16);
		data.Zero();

		LPBITMAPINFO pbi = (LPBITMAPINFO)data.Handle();
		HPALETTE hpal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);

		DC dc;
		dc.GetDC();
		hpal = dc.SelectPalette(hpal);//this case must be call,or the data format is a screen color depth format.0 is select in dc and 1 is out;
		dc.RealizePalette();

		pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		//填充pbi结构，这时候不能设置色深和压缩格式，否则函数会失败
		int count = ::GetDIBits(dc,_Handle,0,0,NULL,pbi,(DWORD)DIB_PAL_COLORS);//调用此函数不一定要把要提取的位图选入DC,这说明DC仅仅提供格式或功能
		if(!count) goto exit;

		pbi->bmiHeader.biCompression = 0;
		pbi->bmiHeader.biBitCount = bits;
		//设置了压缩格式和色深后，再次填充pbi结构，目的是得到 biSizeImage 的值。
		count = ::GetDIBits(dc,_Handle,0,0,NULL,pbi,(DWORD)DIB_PAL_COLORS);//调用此函数不一定要把要提取的位图选入DC,这说明DC仅仅提供格式或功能
		if(!count) goto exit;

		DWORD dibLen = pbi->bmiHeader.biSizeImage + hLen;
		data.SetLength(dibLen);
		pbi = (LPBITMAPINFO)data.Handle();
		//获取图像数据
		count = ::GetDIBits(dc,_Handle,0L,pbi->bmiHeader.biHeight,data.Handle()+hLen,pbi,(DWORD)DIB_RGB_COLORS);//DIB_PAL_COLORS
exit:
		dc.SelectPalette(hpal);
		if(!count) return 0;
		//pbi->bmiHeader.biClrUsed = PaletteSize(bits);
		return dibLen;
	}
	bool Bitmap::Draw(HDC hdc,int x,int y,int cx,int cy,int sx,int sy,int scx,int scy,DWORD raster){
		if(!_Handle) return 0;
		HDC bdc = ::CreateCompatibleDC(NULL);
		if(!bdc) return 0;

		Twin _Size = Size();
		if(cx<=0) cx = _Size.x;
		if(cy<=0) cy = _Size.y;
		if(sx<0) sx = 0;
		if(sy<0) sy = 0;
		//负数用于反转绘制，scx为负，左右反转，scy为负数上下反转
		if(scx==0) scx = _Size.x; 
		if(scy==0) scy = _Size.y;

		HGDIOBJ old = ::SelectObject(bdc,_Handle);
		bool r;
		if((scx==cx)&&(scy==cy)){
			r = ::BitBlt(hdc,x,y,cx,cy,bdc,sx,sy,raster)!=0;
		}else if(((scx+sx)<=_Size.x)&&((scy+sy)<=_Size.y)){
			::SetStretchBltMode(hdc,HALFTONE);
			r = ::StretchBlt(hdc,x,y,cx,cy,bdc,sx,sy,scx,scy,raster)!=0;
		}else{
			r = 0;
		}
		::SelectObject(bdc,old);
		::DeleteDC(bdc);
		return r;
	}
	bool Bitmap::DrawAlpha(HDC hdc,BYTE alpha,bool alphaChannel,int x,int y,int cx,int cy,int sx,int sy,int scx,int scy){
		if(!_Handle) return 0;

		Twin _Size = Size();
		BLENDFUNCTION bf;
		bf.AlphaFormat = alphaChannel;
		bf.BlendFlags = 0;
		bf.BlendOp = 0;	//AC_SRC_OVER = 0;
		bf.SourceConstantAlpha = alpha;
		if(cx<=0) cx = _Size.x;
		if(cy<=0) cy = _Size.y;
		if((sx<0)||(sy<0))
			return 0;
		if(scx==0) scx = _Size.x - sx;
		if(scy==0) scy = _Size.y - sy;
		if(((scx+sx)>_Size.x)||((scy+sy)>_Size.y))
			return 0;

		HDC bdc = ::CreateCompatibleDC(NULL);
		if(!bdc) return 0;
		HGDIOBJ old = ::SelectObject(bdc,_Handle);
		bool r = ::AlphaBlend(hdc,x,y,cx,cy,bdc,sx,sy,scx,scy,bf)!=0;
		//this function is most slowly,when alpha is not 255 and source rect width destination rect
		//is not equal in size,especially last reason.
		::SelectObject(bdc,old);
		::DeleteDC(bdc);
		return r;
	}
	//class DrawBitmap
	/*DrawBitmap::DrawBitmap():_Old(0){
	}
	DrawBitmap::~DrawBitmap(){
		Delete();
	}
	HBITMAP DrawBitmap::GetBitmap(){
		if(!_Handle) return 0;
		Twin size = Size();

		HBITMAP hbp = CreateBitmap(_Handle,size.x,size.y);//create a bitmap;
		HDC hdc = ::CreateCompatibleDC(NULL);//create a dc;
		HBITMAP oldBmp = (HBITMAP)::SelectObject(hdc,hbp);//combin dc and bitmap to draw;

		::BitBlt(hdc,0,0,size.x,size.y,_Handle,0,0,SRCCOPY);//copy source bitmap to destination bitmap;
		::SelectObject(hdc,oldBmp);
		::DeleteDC(hdc);
		return hbp;

	}
	Twin DrawBitmap::Size(){
		if(!_Handle) return Twin(0,0);
		HBITMAP bmp = (HBITMAP)::SelectObject(_Handle,_Old);
		if(bmp==0) return Twin(0,0);
		AutoStruct<BITMAP> bp;
		::GetObject(bmp,sizeof(BITMAP),&bp);
		_Old = (HBITMAP)::SelectObject(_Handle,bmp);
		return (*(Twin*)(&bp.bmWidth));
	}
	bool DrawBitmap::Resize(int cx,int cy){
		if(!_Handle) return 0;
		Twin size(cx,cy),_Size = Size();
		if(_Size==size) return 1;
		//create a new bitmap and dc.
		HDC sdc = ::GetDC(NULL);
		HDC hdc = ::CreateCompatibleDC(0);
		HBITMAP hbmp = CreateBitmap(sdc,cx,cy);
		::ReleaseDC(0,sdc);
		if(hbmp==0){
			::DeleteDC(hdc);
			return 0;
		}
		hbmp = (HBITMAP)::SelectObject(hdc,hbmp);
		//draw in new size
		BLENDFUNCTION bf;
		bf.AlphaFormat = 1;
		bf.BlendFlags = 0;
		bf.BlendOp = 0;//AC_SRC_OVER = 0;
		bf.SourceConstantAlpha = 255;
		BOOL result = ::AlphaBlend(hdc,0,0,cx,cy,_Handle,0,0,_Size.x,_Size.y,bf)!=0;
		//delete resource
		if(result){
			_Old = (HBITMAP)::SelectObject(_Handle,_Old);
			::DeleteObject(_Old);
			_Old = hbmp;
			::DeleteDC(_Handle);
			_Handle = hdc;
		}
		else{
			hbmp = (HBITMAP)::SelectObject(_Handle,hbmp);
			::DeleteDC(hdc);
			::DeleteObject(hbmp);
		}
		return result!=0;
	}
	bool DrawBitmap::Create(){
		if(_Handle){
			if(_Old) ::SelectObject(_Handle,_Old);
			::DeleteDC(_Handle);
		}
		_Handle = ::CreateCompatibleDC(NULL);
		return _Handle!=0;
	}
	bool DrawBitmap::AttachBitmap(HBITMAP hBmp){
		if(_Handle){
			HBITMAP bmp = (HBITMAP)::SelectObject(_Handle,hBmp);
			if(bmp) ::DeleteObject(bmp);
			if(hBmp==0&&bmp==0) return 0;
		}else{
			_Handle = ::CreateCompatibleDC(NULL);
			_Old = (HBITMAP)::SelectObject(_Handle,hBmp);
			if(_Old==0){//hBmp不是一个合法的图像.
				::DeleteDC(_Handle);
				return 0;
			}
		}
		return 1;
	}
	HBITMAP DrawBitmap::DetachBitmap(){
		if(!_Handle||!_Old) return 0;
		return (HBITMAP)::SelectObject(_Handle,_Old);
	}
	bool DrawBitmap::Delete(){
		if(_Handle==NULL) return false;
		_Old = (HBITMAP)::SelectObject(_Handle,_Old);
		::DeleteDC(_Handle);
		_Handle = 0;
		BOOL r = ::DeleteObject(_Old);
		_Old = 0;
		return r!=0;
	}
	bool DrawBitmap::Draw(HDC hdc,int x,int y,int cx,int cy,int sx,int sy,int scx,int scy,DWORD raster){
		Twin _Size = Size();
		if(!_Handle) return 0;
		if(cx<=0) cx = _Size.x;
		if(cy<=0) cy = _Size.y;
		if(sx<0) sx = 0;
		if(sy<0) sy = 0;
		//负数用于反转绘制，scx为负，左右反转，scy为负数上下反转
		if(scx==0) scx = _Size.x; 
		if(scy==0) scy = _Size.y;
		if((scx==cx)&&(scy==cy)){
			return ::BitBlt(hdc,x,y,cx,cy,_Handle,sx,sy,raster)!=0;
		}
		if(((scx+sx)>_Size.x)||((scy+sy)>_Size.y))
			return 0;
		BlackOnWhite = BLACKONWHITE,// Performs a Boolean AND operation using the color values for the eliminated and existing pixels. If the bitmap is a monochrome bitmap, this mode preserves black pixels at the expense of white pixels. 
		WhiteOnBlack = WHITEONBLACK,// Performs a Boolean OR operation using the color values for the eliminated and existing pixels. If the bitmap is a monochrome bitmap, this mode preserves white pixels at the expense of black pixels. 
		ColorOnColor = COLORONCOLOR,// Deletes the pixels. This mode deletes all eliminated lines of pixels without trying to preserve their information. 
		HalfTone = HALFTONE// Maps pixels from the source rectangle into blocks of pixels in the destination rectangle. The average color over the destination block of pixels approximates the color of the source pixels.  After setting the
		HALFTONE stretching mode, an application must call the SetBrushOrgEx function to set the brush origin. If it fails to do so, brush misalignment occurs.
		::SetStretchBltMode(hdc,HALFTONE);
		return ::StretchBlt(hdc,x,y,cx,cy,_Handle,sx,sy,scx,scy,raster)!=0;
	}
	bool DrawBitmap::DrawAlpha(HDC hdc,BYTE alpha,bool alphaChannel,int x,int y,int cx,int cy,int sx,int sy,int scx,int scy){
		Twin _Size = Size();
		if(!_Handle)
			return 0;
		BLENDFUNCTION bf;
		bf.AlphaFormat = alphaChannel;
		bf.BlendFlags = 0;
		bf.BlendOp = 0;//AC_SRC_OVER = 0;
		bf.SourceConstantAlpha = alpha;
		if(cx<=0) cx = _Size.x;
		if(cy<=0) cy = _Size.y;
		if((sx<0)||(sy<0))
			return 0;
		if(scx==0) scx = _Size.x - sx;
		if(scy==0) scy = _Size.y - sy;
		if(((scx+sx)>_Size.x)||((scy+sy)>_Size.y))
			return 0;
		return ::AlphaBlend(hdc,x,y,cx,cy,this->_Handle,sx,sy,scx,scy,bf)!=0;
		//this function is most slowly,when alpha is not 255 and source rect width destination rect
		// is not equal in size,especially last reason.
	}
	*///class Icon
	Icon::Icon(void):_Size(0,0),_HotX(0),_HotY(0),_IsIcon(1){
	}
	Icon::~Icon(void){
		Delete();
	}
	void Icon::operator = (HICON hIcon){
		Attach(::CopyIcon(hIcon));
	}
	void Icon::operator = (Icon& icon){
		Attach(::CopyIcon(icon));
	}
	bool Icon::FromResource(LPBYTE hg,DWORD bits,int cx,int cy){
		return Attach(::CreateIconFromResourceEx(hg,bits,_IsIcon,0x00030000,cx,cy,LR_DEFAULTCOLOR));
	}
	bool Icon::FromBitmap(HBITMAP hBmp,COLORREF cMask,int cx,int cy,int x,int y,int hotX,int hotY){
		if(!hBmp) return 0;
		/*ICONINFO ii;
		Bitmap bmp(hBmp);
		HBITMAP color = bmp.GetClip(cx,cy,x,y);
		if(color==0)
		return 0;
		if(cx==0)
		cx = bmp.Size().x;
		if(cy==0)
		cy = bmp.Size().y;
		HBITMAP mask;
		bmp.Detach();
		if((cMask&0xff000000)==0){
		//创建两幅位图,一个为Color,一个为Mask,使用这两幅位图创建图标.
		HDC clDC = ::CreateCompatibleDC(NULL);
		HDC msDC = ::CreateCompatibleDC(NULL);
		mask = ::CreateBitmap(cx,cy,1,1,NULL);
		HBITMAP cOld = (HBITMAP)::SelectObject(clDC,color);
		HBITMAP sOld = (HBITMAP)::SelectObject(msDC,mask);

		::SetBkColor(clDC,cMask);
		::BitBlt(msDC,0,0,cx,cy,clDC,0,0,SRCCOPY);//在msDC(也就是mask)中绘制一幅黑白图,
		//颜色为dwMask的部分为原图背景色(此例为黑色),其它部分为源图颜色(因目的位图为单色图,
		//此例是白色).
		::SetBkColor(clDC,cMask);//背景色为黑色,完全过滤掉dwMask.
		::SetTextColor(clDC,RGB(255,255,255));//前景色为白色,完全透过.
		::BitBlt(clDC,0,0,cx,cy,msDC,0,0,SRCAND);//把mask图绘制到color上,用AND.本来这个操作本身
		//就能把color的背景色变成黑,但这个函数的工作方式是,把AND后的值再和背景色和前景色进行操作.
		//(大概是进行OR操作),所以必须设置合适的前景和背景色.

		::SelectObject(clDC,cOld);
		::SelectObject(msDC,sOld);
		::DeleteDC(clDC);
		::DeleteDC(msDC);		//创建完成,清理DC资源.
		}
		else{
		mask = color;
		}
		ii.fIcon = _IsIcon;
		ii.xHotspot = hotX;
		ii.yHotspot = hotY;
		ii.hbmColor = color;
		ii.hbmMask = mask;
		HICON hi = ::CreateIconIndirect(&ii);
		return Attach(hi,&ii);*/

		Bitmap bmp,bp;
		bp.Attach(hBmp);
		bmp.Attach(bp.GetClip());
		bp.Detach();
		bmp.ToAlpha(cMask);
		ICONINFO ii;
		ii.fIcon = 1;
		ii.xHotspot = hotX;
		ii.yHotspot = hotY;
		ii.hbmColor = bmp;
		ii.hbmMask = bmp;
		return Attach(::CreateIconIndirect(&ii),&ii);
	}
	bool Icon::LoadFile(LPCWSTR lpFileName,int cx,int cy){
		UINT type;
		if(_IsIcon)
			type = IMAGE_ICON;
		else 
			type = IMAGE_CURSOR;
		HICON h = (HICON)::LoadImageW(NULL,lpFileName,type,cx,cy,LR_LOADFROMFILE);
		if(!h)
			return 0;
		return Attach(h);
	}
	bool Icon::LoadIcon(ResID res,int cx,int cy){
		_IsIcon = 1;
		return Attach((HICON)::LoadImageW(res.Instance,res,IMAGE_ICON,cx,cy,LR_DEFAULTCOLOR));
	}
	bool Icon::LoadCursor(ResID res){
		_IsIcon = 0;
		return Attach(::LoadCursorW(res.Instance,res));
	}
	bool Icon::Attach(HICON hIcon){
		ICONINFO ici;
		BITMAP bm;
		if(!::GetIconInfo(hIcon,&ici)){
			Delete();
			return 0;
		}
		HBITMAP hbm = 0;
		if(ici.hbmColor) hbm = ici.hbmColor;
		else hbm = ici.hbmMask;
		int r = ::GetObject(hbm,sizeof(BITMAP),&bm);
		::DeleteObject(ici.hbmColor);
		::DeleteObject(ici.hbmMask);
		Delete();
		if(r!=sizeof(BITMAP)) return 0;
		_HotX = ici.xHotspot;
		_HotY = ici.yHotspot;
		_IsIcon = ici.fIcon!=0;
		_Size.x = bm.bmWidth;
		_Size.y = bm.bmHeight;
		if(bm.bmBitsPixel==1) _Size.y /= 2;
		_Handle = hIcon;
		return 1;
	}
	bool Icon::Attach(HICON hIcon,PICONINFO pii){
		BITMAP bm;
		HBITMAP hbm;
		if(pii->hbmColor)
			hbm = pii->hbmColor;
		else
			hbm = pii->hbmMask;
		int r = ::GetObject(hbm,sizeof(BITMAP),&bm);
		::DeleteObject(pii->hbmColor);
		::DeleteObject(pii->hbmMask);
		Delete();
		if(r!=sizeof(BITMAP))
			return 0;
		_HotX = pii->xHotspot;
		_HotY = pii->yHotspot;
		_IsIcon = pii->fIcon!=0;
		_Size.x = bm.bmWidth;
		_Size.y = bm.bmHeight;
		if(bm.bmBitsPixel==1) 
			_Size.y /= 2;
		_Handle = hIcon;
		return 1;
	}
	HICON Icon::Detach(){
		_Size.SetValue(0,0);
		_HotX = _HotY = 0;
		HICON handle = _Handle;
		_Handle = 0;
		return handle;
	}
	HICON Icon::GetCopy(){
		return ::CopyIcon(_Handle);
	}
	bool Icon::Delete(){
		if(!_Handle) return 0;
		if(_IsIcon)
			::DestroyIcon(_Handle);//DestroyIcon和DestroyCursor呼叫的是同一个函数,在过去的MSDN中特别指出这两者不能通用,但在MSDN2003中,已没有这样的特别说明.
		else
			::DestroyCursor(_Handle);
		_HotX = _HotY = 0;
		this->_Size.SetValue(0,0);
		return 1;
	}
	bool Icon::Draw(HDC _DC,int x,int y,int cx,int cy){
		if(cx<=0)
			cx = _Size.x;
		if(cy<=0)
			cy = _Size.y;
		return ::DrawIconEx(_DC,x,y,_Handle,cx,cy,NULL,NULL,DI_NORMAL)!=0;
	}
	bool Icon::Draw(HDC hdc,Rect* rect,bool stretch){
		Twin size = rect->Size();
		int cx,cy,x,y;
		if(stretch){
			x = rect->left;
			y = rect->top;
			cx = size.x;
			cy = size.y;
		}
		else{
			x = rect->left + (size.x - _Size.x)/2;
			y = rect->top + (size.y - _Size.y)/2;
			cx = _Size.x;
			cy = _Size.y;
		}
		return ::DrawIconEx(hdc,x,y,_Handle,cx,cy,NULL,NULL,DI_NORMAL)!=0;
	}
	//gdi function
	bool IconToBlackWhite(Icon* icon){
		Bitmap bmp;
		bmp.FromIcon(icon->Handle());
		Memory<unsigned char> data;
		bmp.GetPixelData(&data,32);
		for(uint i=0;i<data.Length();i+=4){
			data[i] = data[i+1] = data[i+2] = (BYTE)(((unsigned int)data[i]+(unsigned int)data[i+1]+(unsigned int)data[i+2])/3);
		}
		bmp.SetPixelData(&data,32);
		ICONINFO ii;
		ii.fIcon = icon->IsIcon();
		ii.xHotspot = icon->HotX();
		ii.yHotspot = icon->HotY();
		ii.hbmColor = bmp.Detach();
		bmp.FromIcon(icon->Handle(),0);
		ii.hbmMask = bmp.Detach();
		return icon->Attach(::CreateIconIndirect(&ii),&ii);
	}
	bool BitmapToBlackWhite(Bitmap& bmp){
		Memory<unsigned char> data;
		if(0>=bmp.GetPixelData(&data,32)) return 0;
		for(uint i=0;i<data.Length();i+=4){
			data[i] = data[i+1] = data[i+2] = (BYTE)(((unsigned int)data[i]+(unsigned int)data[i+1]+(unsigned int)data[i+2])/3);
		}
		bmp.SetPixelData(&data,32);
		return 1;
	}
	HBITMAP CreateBitmap(HDC hdc,int cx,int cy){
		BITMAPINFOHEADER bmih;
		memset(&bmih, 0, sizeof(BITMAPINFOHEADER));
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biBitCount = 32;
		bmih.biCompression = BI_RGB;
		bmih.biPlanes = 1;
		bmih.biWidth = cx;
		bmih.biHeight = cy;
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(BITMAPINFO));
		bmi.bmiHeader = bmih;
		void* p;
		//CreateDIBSection函数使用虚拟内存.
		HBITMAP bmp = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
		return bmp;
	}
	SIZE CalcTextSize(HDC dc,String& text,HFONT font){
		Rect rc(0,0,0,0);
		HDC hdc = dc;
		if(dc==0) hdc = ::GetDC(0);
		HGDIOBJ oldf = 0;
		if(font) oldf = ::SelectObject(hdc,font);
		DrawText(hdc,text,text.Length(),rc,DT_CALCRECT);
		if(font) ::SelectObject(hdc,oldf);
		if(dc==0) ::ReleaseDC(0,hdc);
		return *(SIZE*)&rc.right;
	}
	HBITMAP BitmapList::Add(LPCWSTR file,uint index){
		Image img;
		if(!img.Create(file)) return 0;
		HBITMAP bmp = img.GetBitmap();
		Add(bmp,index);
		return bmp;
	}
	HBITMAP BitmapList::Add(ResID res,ResID type,uint index){
		DWORD size = 0;
		void* p = GetResource(res,type,&size);
		if(!p) return 0;
		Image img;
		if(!img.Create(p,size)) return 0;
		HBITMAP bmp = img.GetBitmap();
		Add(bmp,index);
		return bmp;
	}
}