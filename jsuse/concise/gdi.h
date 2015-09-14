#pragma once
#include "res.h"
namespace cs{
	//GDI对象在选入一个DC时,是不能被释放和销毁的,所以如果成功执行了InDC没有执行OutDC函数,资源就不能正确的释放.
	//如果一个GdiObject实例已经绑定了一个系统GDI对象,这时执行生成和绑定一个新的GDI对象时,实例会自动去释放或销毁
	//已经绑定的对象,但是当这个对象已经选入一个DC的情况下,是不能释放的,这种情况下,函数在DEBUG模式下会发出一个警告,
	//但是函数并不失败.
	template<typename T> class CONCISETL_API GdiObject : public HandleType<T>
	{
	public:
		GdiObject()
		{
		}
		//自动删除绑定的GDI资源,如果执行了InDC而没有执行OutDC这个清理工作可能会失败.
		~GdiObject()
		{
			if(_Handle) ::DeleteObject(_Handle);
		}
		template<typename R>bool GetObject(R* pr)
		{
			return ::GetObject(_Handle,sizeof(R),pr)!=0;
		}
		//把绑定的GDI资源选进DC.执行这个函数后,正常情况下,程序应该成对的执行这个函数.
		//当一个GDI对象选入DC时，这时实例的句柄绑定的不是原GDI对象，hdc是一个非0无效DC时会导致资源泄漏。
		inline void Select(HDC hdc)
		{
			if(hdc) _Handle = (T)::SelectObject(hdc,_Handle);
		}
		//如果这个实例不为空,则绑定的前一个GDI资源会被删除,但即使没有正确删除上一个资源,例如当这个资源正在被选进
		//一个DC的时候,函数仍然会完成绑定新的对象.只有当要绑定的对象是一个NULL指针时,函数才返回0.
		bool Attach(T t){
#ifdef _DEBUG
			if(!t) PrintD(L"%s",L"GdiObject.Attatch:绑定的对象为一个空(NULL)对象");
			//_ASSERT_EXPR(0,L"GdiObject.Attatch:绑定的对象为一个空(NULL)对象");
			//SystemMessageBox();
#endif
			if(_Handle){
				BOOL r = ::DeleteObject(_Handle);
				if(!r) PrintD(L"%s",L"GdiObject.Attatch:原对象没有正确释放");
			}
			_Handle = t;
			return 1;
		}
		//如果当前资源被选进一个DC,则函数解除绑定失败.
		T Detach(){
			T h = _Handle;
			_Handle = 0;
			return h;
		}
		//删除绑定的GDI资源,实例的资源句柄置为0,如果当前资源被选进一个DC,则函数失败.
		bool Delete()
		{
			if(!::DeleteObject(_Handle))	return 0;
			_Handle = 0;
			return 1;
		}
		void FromStock(int stock)
		{
			Attach((T)::GetStockObject(stock));
		}
	};
	//颜色结构.
	typedef union CONCISE_API Color{
	public:
		Color();
		Color(int value);
		Color(BYTE r,BYTE g,BYTE b,BYTE a = 0);
		void SetValue(BYTE r,BYTE g,BYTE b,BYTE a = 0);
		void operator = (COLORREF color);
		operator COLORREF ();
		struct{BYTE red,green,blue,alpha;};
		void* operator new(size_t size){
			return MemoryAlloc((int)size);
		}
		void operator delete(void* p){
			MemoryFree(p);
		}
	private:
		struct{UINT Value;};
	}Color;
	//区域.
	class CONCISE_API Rgn : public GdiObject<HRGN>{
	public:
		enum{
			And = 1,	//相当于交集.
			Or = 2,		//相当于并集.
			Xor = 3,	//交集之外的部分(对等集).
			Diff = 4,	//减去rgn(包含的部分).
			Copy = 5	//Copy rgn;
		};
		//生成一个区域对象,矩形大小为0.
		bool Create();
		//生成一个矩形区域.
		bool CreateRect(int x1,int y1,int x2,int y2);
		//生成一个椭圆区域.
		bool CreateEllipse(int left,int top,int right,int bottom){return Attach(CreateEllipticRgn(left,top,right,bottom));}
		//生成一个椭圆区域.
		bool CreateEllipse(const RECT* r){return Attach(CreateEllipticRgnIndirect(r));}
		//生成一个多边形区域,ps,pn分别是点的坐标和占的个数,mode选1时,区域会被娄空,而2时总填充.
		bool CreatePolygon(LPPOINT ps,int pn,int mode = 1/*or 2*/){return Attach(CreatePolygonRgn(ps,pn,mode));}// ALTERNATE or WINDING
		//生成一个圆角矩形区域,后两个参数是圆角的大小.
		bool CreateRoundRect(int left,int top,int right,int bottom,int width,int height){return Attach(CreateRoundRectRgn(left,top,right,bottom,width,height));}
		//组合区域.参考这个类的枚举类型.
		int Combine(HRGN rgn,DWORD cbMode = Or);
		//平移一个区域.
		bool Offset(int dx,int dy)		{return 0!=::OffsetRgn(_Handle,dx,dy);}
		//平移一个区域.
		bool Offset(Twin dim)	{return 0!=::OffsetRgn(_Handle,dim.x,dim.y);}
		//两个区域是否一样.
		inline bool operator == (HRGN rgn){return ::EqualRgn(_Handle,rgn)!=0;}
		//把一个RGN的值赋给另一个,如果为0,则删除绑定的RGN.
		void operator = (HRGN rgn);
		//把一个RGN的值赋给另一个,如果为0,则删除绑定的RGN.
		void operator = (Rgn& rgn);
		//绘制区域.
		inline bool Paint(HDC hdc){return ::PaintRgn(hdc,_Handle)!=0;}
		//返回包含区域的最小矩形.
		inline bool GetRectBox(LPRECT pr){return 0!=::GetRgnBox(_Handle,pr);}
		//点是否在区域内.
		inline bool PtInRgn(Twin pt){return ::PtInRegion(_Handle,pt.x,pt.y)!=0;}
	};
	class CONCISE_API Pen : public GdiObject<HPEN>
	{
	public:
		//生成一个PEN对象.
		bool Create(COLORREF color,int style = 0,int width = 1);
		//绑定系统的DC_PEN对象.
		bool CreateDCPen(){return Attach((HPEN)::GetStockObject(DC_PEN));}
		//设置画笔颜色.必须调用CreateDCPen或者手动绑定DC_PEN.
		COLORREF SetDCColor(HDC hdc,COLORREF color);
		//获取画笔颜色.必须调用CreateDCPen或者手动绑定DC_PEN.
		COLORREF GetDCColor(HDC hdc);
		//返回画笔信息.
		bool GetLogPen(LOGPEN* lp){return GetObject(lp);}
		//返回画笔颜色.
		COLORREF GetColor();
	};
	class CONCISE_API Brush : public GdiObject<HBRUSH>
	{
	public:
		enum {Black = 4,DarkGray = 3,Gray = 2,Null = 5,White = 0,DC = 18};//系统内置刷子.
		//生成.
		bool Create(COLORREF color,UINT style = BS_SOLID,DWORD hatch = 0);
		//绑定系统内置的DC_BRUSH对象.
		bool CreateDCBrush(){return Attach((HBRUSH)::GetStockObject(DC_BRUSH));}
		//set the DC_BRUSH brush color,it can be select in dc later;
		COLORREF SetDCColor(HDC hdc,COLORREF color){return ::SetDCBrushColor(hdc,color);}
		//get the DC_BRUSH brush color
		COLORREF GetDCColor(HDC hdc){return ::GetDCBrushColor(hdc);}
		//生成.
		bool CreatePattern(HBITMAP bmp){return Attach(::CreatePatternBrush(bmp));}
		//返回系统内置颜色.
		bool GetSysColorBrush(int sysIndex){return Attach(::GetSysColorBrush(sysIndex));}
		//设置DC的位置.
		bool SetOrigin(HDC hdc,int x,int y){return 0!=::SetBrushOrgEx(hdc,x,y,0);}
		//取得起始位置.
		bool GetOrigin(HDC hdc,POINT* pt){return 0!=GetBrushOrgEx(hdc,pt);}
		//返回刷子信息.
		bool GetLogBrush(LOGBRUSH* lb){return GetObject(lb);}
		//返回颜色.
		COLORREF GetColor();
	};
	//Font的具体参数可以从LogFont()返回的指针来操作.如果没有调用Create函数,实际上并没有字体资源生成
	//这个类此时仍可以做为一个LOGFONT结构使用,很多字体的操作实际上只要求对LOGFONT进行,而无须生成字体.
	class CONCISE_API Font : public GdiObject<HFONT>{
	protected:
		Object<LOGFONT> _LogFont;
	public:
		Font(void){_LogFont.New();Reset();}
		//生成字体,使用_LogFont的参数.
		bool Create();		
		//设置字体为缺省字体.生效需要重新调用Create函数.
		void Reset();	
		//设置字体的名称.生效需要重新调用Create函数.
		void SetName(LPCWSTR fontName){WcsCopy(_LogFont->lfFaceName,fontName,31);}
		//设置字体的大小.生效需要重新调用Create函数.
		void SetSize(LONG size){_LogFont->lfHeight = size;}
		//返回用以生成字体的LOGFONT结构指针.
		LPLOGFONT LogFont(){return _LogFont.Handle();}
		//用一个已知的LOGFONT初始化用以生成字体的LOGFONT结构指针.
		void SetLogFont(LPLOGFONT logfont){memcpy(_LogFont.Handle(),logfont,sizeof(LOGFONT));}
		//返回字体信息, 结果存储在logfont成员.
		bool GetLogFont(){return GetObject(_LogFont.Handle());}
	};
	//DC必须手动释放资源，GetDC和GetWindowDC得到的句柄使用ReleaseDC释放，Create得到的句柄，使用DeleteDC释放。
	class CONCISE_API DC : public HandleType<HDC>{
	public:
		void operator = (HDC hdc){
			_Handle = hdc;
		}
		bool CreateCompatible(HDC hdc = NULL);
		template<typename T> T SelectObject(T hGdi){
			return (T)::SelectObject(_Handle,hGdi);
		}
		//绑定整个窗口的DC包括标题栏和边界,滚动条区域.
		bool GetWindowDC(HWND hWnd);
		//绑定一个窗口的客户区DC，0 表示前台窗口的 DC .
		bool GetDC(HWND hWnd = 0);
		//删除或释放绑定的DC资源.
		bool DeleteDC();
		bool ReleaseDC(HWND hWnd = 0);
		//此函数容易因系统内核内存不足而失败.
		inline HBITMAP CreateCompatibleBitmap(int cx = 0,int cy = 0){return ::CreateCompatibleBitmap(_Handle,cx,cy);}
		inline bool MoveTo(int x,int y,LPPOINT pt = 0){return 0!=::MoveToEx(_Handle,x,y,pt);}
		inline bool LineTo(int x,int y){return 0!=::LineTo(_Handle,x,y);}
		inline int GetClipRect(LPRECT clip){return ::GetClipBox(_Handle,clip);}
		inline int GetDeviceCaps(int index){return ::GetDeviceCaps(_Handle,index);}//HORZRES for screen width,VERTRES for height,BITSPIXEL for color depth.
		inline UINT SetTextAlign(int align){return ::SetTextAlign(_Handle,align);}
		inline COLORREF SetTextColor(COLORREF color){return ::SetTextColor(_Handle,color);}
		inline COLORREF SetBkColor(COLORREF bk){return ::SetBkColor(_Handle,bk);}
		inline bool SetBkTransparent(bool transparent = true){return ::SetBkMode(_Handle,transparent?TRANSPARENT:OPAQUE)!=0;}
		inline int DrawText(LPCWSTR str,LPRECT rect,UINT flags){return ::DrawTextW(_Handle,str,(int)wcslen(str),rect,flags);}
		inline HPALETTE SelectPalette(HPALETTE hpl,bool force = 0){return ::SelectPalette(_Handle,hpl,force);}
		inline int SetStretchBltMode(int stretchMode){return ::SetStretchBltMode(_Handle,stretchMode);}
		inline UINT RealizePalette(){return ::RealizePalette(_Handle);}
		inline COLORREF SetDCPenColor(COLORREF color){return ::SetDCPenColor(_Handle,color);}
		inline COLORREF SetDCBrushColor(COLORREF color){return ::SetDCBrushColor(_Handle,color);}
		inline COLORREF GetDCPenColor(){return ::GetDCPenColor(_Handle);}
		inline COLORREF GetDCBrushColor(){return ::GetDCBrushColor(_Handle);}
		inline HBRUSH GetDCBrush(){return (HBRUSH)::GetStockObject(DC_BRUSH);}
		inline HPEN GetDCPen(){return (HPEN)::GetStockObject(DC_PEN);}
		inline HPEN SelectDCPen(){return SelectObject(GetDCPen());}
		inline HPEN ReleaseDCPen(HPEN old){return SelectObject(old);}
		inline HBRUSH SelectDCBrush(){return SelectObject(GetDCBrush());}
		inline HBRUSH ReleaseDCBrush(HBRUSH old){return SelectObject(old);}
	};
	/*如果调色板数据为0,DibOffset和PixelOffset是两个固定数值14和54,24和32位图像.每行的像素数据有一个4字节对齐的问题,
	一行的像素数据字节数一定是4的倍数.
	DibOffset             PixelOffset                       像素数据  
	|___________________|_____________________|________________________________________________________________________________|*/
	class CONCISE_API Bitmap : public GdiObject<HBITMAP>{
	public:
		inline Bitmap(){}
		//用一个位图句柄初始化.
		Bitmap(HBITMAP bmp){Attach(bmp);}
		//加载一个图像资源或者一个BMP文件.resInst = 0表示加载一个位图文件.cx,cy是指定的位图尺寸,默认表示按原大小加载.
		bool Load(ResID res,int cx = 0,int cy = 0);//resInst = 0 indicate load from file
		//加载图像文件，使用gdi+，支持各种文件格式。
		bool Load(LPCWSTR file);
		//加载图像数据，使用gid+，数据必须是文件格式。
		bool Load(void* data,int len);
		//dib就是一个bmp文件不包括文件头的部分。
		int GetDib(Memory<unsigned char>& dib,WORD bits = 0);
		//保存为Bmp图像文件，直接操作数据，没有使用gid+，BitsPixel是文件的色深,0表示为屏幕的色深.
		bool SaveBmp(LPCWSTR lpFileName,WORD BitsPixel = 0);
		//保存图像到文件，文件类型由文件名的后缀决定。quality是图像质量，数值从 0——100
		bool Save(LPCWSTR file,long quality = 60);
		//保存图像到内存，数据类型由type决定（比如“.jpg”保存为jpg类型的数据）。
		int Save(Memory<char>& mem,LPCTSTR type,long quality = 60);
		//从图像数据生成图像（数据以一个BITMAPINFOHEADER开头）.如果数据是一个bmp文件，则file传1.
		bool CreateFromMemory(LPVOID data,bool file = 0);
		//生成一个屏幕色深的位图。
		bool CreateCompatibleBitmap(int cx,int cy);
		//生成一个2位的位图.
		static HBITMAP CreateMonochroic(int cx = 0,int cy = 0);
		//从图标获取图像,因为图标包含两个图像资源(彩色和MASK图),color参数设置获取哪一个.
		bool FromIcon(HICON hIcon,bool color = 1);
		//这个函数复制图像,hBmp对应的资源不会释放.
		void operator = (HBITMAP hBmp);//copy the bitmap
		//从bitmap复制图像.
		void operator = (Bitmap& bitmap);

		//重新设置大小.
		bool Resize(int cx,int cy);
		//返回一个原来的图像片断资源.要自己处理资源的释放.
		HBITMAP GetClip(int cx = 0,int cy = 0,int x = 0,int y = 0);
		//剪切图像,并保存在这个实例中.
		bool ClipBitmap(int cx = 0,int cy = 0,int x = 0,int y = 0);
		//取得位图的像素数据,数据保存在data中,长度既是data的大小.BitsPixel = 0表示数据为屏幕色深.返回图像的高度.
		//掃描是從左下角開始從左向右，從下向上的順序。每個32位圖像的像素為4個字節，順序為：藍，绿，红，alpha。
		int GetPixelData(Memory<unsigned char>* p,WORD bits = 0);
		//设置位图的像素数据,数据保存在data中.返回图像的行数(高度).
		int SetPixelData(Memory<unsigned char>* p,WORD bits = 0);
		//设置位图的像素数据,数据保存在data中.返回图像的行数(高度).
		int SetPixelData(const unsigned char* p,uint len,WORD bits = 0);
		//返回位图的尺寸.
		Twin Size();
		//把图像的特定颜色部分转换成透明。
		bool ToAlpha(Color c1,Color c2);
		//把图像的特定颜色部分转换成透明。
		bool ToAlpha(Color c);
		//StretchBlt,绘制图像,hdc,是要绘制的DC,x,y是起始位置,cx,cy是绘制区域的尺寸,sx,sy是源图像的起始位置.
		//scx,scy是要绘制源图像的大小,不能超出实际大小.raster是绘图方式.
		bool Draw(HDC hDC,int x = 0,int y = 0,int cx = 0,int cy = 0,int sx = 0,int sy = 0,int scx = 0,int scy = 0,DWORD raster = SRCCOPY);
		//这个函数会考虑alpha通道的数据,所以只用于32位的位图(实际上如果屏幕是32位的，所有加载的位图都是32位的)
		//其它参数和Draw是相同的.alpha是整体透明度,0-255之间取值.
		bool DrawAlpha(HDC hdc,BYTE alpha = 255,bool alphaChannel = 0,int x = 0,int y = 0,int cx = 0,int cy = 0,int sx = 0,int sy = 0,int scx = 0,int scy = 0);
	};
	//这个类型用于绘制位图,他的类型是一个HDC而非HBITMAP.
	/*class CONCISE_API DrawBitmap : public HandleType<HDC>
	{
	protected:
		HBITMAP _Old;//DC原来的位图，仅仅用做保存资源句柄。
	public:
		//构造函数
		DrawBitmap();
		//析构.
		~DrawBitmap();
		//生成内部 DC 对象，这个函数会自动释放原来的 DC 对象。
		bool Create();
		//释放 DC 资源和位图资源（如果已经绑定位图）。
		bool Close();
		//绑定一个位图资源, 并且删除原来绑定的位图资源。hBmp 位图资源将由这个对象负责释放。
		bool AttachBitmap(HBITMAP hBmp);
		//解除绑定的位图资源，这一般用于不希望位图被释放的情况。
		HBITMAP DetachBitmap();
		//返回绑定位图的一个副本。
		HBITMAP GetBitmap();
		//位图的大小
		Twin Size();
		//重新设置位图的大小。
		bool Resize(int cx,int cy);
		//删除资源.
		bool Delete();
		//绘制图像,hdc,是要绘制的DC,x,y是起始位置,cx,cy是绘制区域的尺寸,sx,sy是源图像的起始位置.
		//scx,scy是要绘制源图像的大小,不能超出实际大小.raster是绘图方式.
		bool Draw(HDC hDC,int x = 0,int y = 0,int cx = 0,int cy = 0,int sx = 0,int sy = 0,int scx = 0,int scy = 0,DWORD raster = SRCCOPY);
		//这个函数会考虑alpha通道的数据,所以只用于32位的位图(实际上如果屏幕是32位的，所有加载的位图都是32位的)
		//其它参数和Draw是相同的.alpha是整体透明度,0-255之间取值.
		bool DrawAlpha(HDC hdc,BYTE alpha = 255,bool alphaChannel = 0,int x = 0,int y = 0,int cx = 0,int cy = 0,int sx = 0,int sy = 0,int scx = 0,int scy = 0);
	};*/
	//Icon 与 Cursor 在内部应该是相同的.但它们有各自的标识.而在资源和文件中它们的区别却较大(又是微软的败笔,
	//做为GDI资源,它们没有任何区别),但确要使用各自的函数.
	class CONCISE_API Icon : public HandleType<HICON>
	{
	public:
		Icon(void);
		Icon(HICON icon){Attach(icon);}
		Icon(ResID res){LoadIcon(res);}
		~Icon(void);

		bool Attach(HICON hIcon);
		bool Attach(HICON hIcon,PICONINFO pii);
		HICON Detach();

		bool LoadIcon(ResID res,int cx = 0,int cy = 0);
		bool LoadCursor(ResID res);
		bool LoadFile(LPCWSTR file,int cx = 0,int cy = 0);

		bool FromResource(LPBYTE hg,DWORD bits,int cx,int cy);
		//hBmp：位图，alpha：透明通道，cx：宽度，cy：长度，x：起始位置x坐标，y：起始位置y坐标，hotX：热点x位置，hotY：热点y位置。
		bool FromBitmap(HBITMAP hBmp,COLORREF alpha = -1,int cx = 0,int cy = 0,int x = 0,int y = 0,int hotX = 0,int hotY = 0);

		bool Draw(HDC hDC,int x,int y,int cx = 0,int cy = 0);
		bool Draw(HDC hdc,Rect* rect,bool stretch = 0);
		HICON GetCopy();

		bool Delete();

		void operator = (HICON hIcon);
		void operator = (Icon& icon);
		inline bool IsIcon() const {return _IsIcon;}

		inline int HotX() const {return _HotX;}
		inline int HotY() const {return _HotY;}
		inline Twin Size() const {return _Size;}
	protected:
		Twin _Size;
		DWORD _HotX,_HotY;
	private:
		bool _IsIcon;
	};
	inline WORD PaletteSize(WORD color)
	{
		if(color>8) return 0;
		return (1<<color)*sizeof(RGBQUAD);
	}
	CONCISE_API bool IconToBlackWhite(Icon* icon);
	CONCISE_API bool BitmapToBlackWhite(Bitmap& bmp);
	CONCISE_API HBITMAP CreateBitmap(HDC hdc,int cx,int cy);
	class CONCISE_API GdiSelector : public _class{
	protected:
		HGDIOBJ _old;
		HDC		_dc;
	public:
		GdiSelector():_old(0),_dc(0){}
		GdiSelector(HDC hdc,HGDIOBJ obj){
			_old = ::SelectObject(hdc,obj);
			_dc = hdc;
		}
		void Select(HDC hdc,HGDIOBJ obj){
			_old = ::SelectObject(hdc,obj);
			_dc = hdc;
		}
		void Close(){
			if(_dc){
				::SelectObject(_dc,_old);
			}
		}
		~GdiSelector(){
			if(_dc){
				::SelectObject(_dc,_old);
			}
		}
	};
	CONCISE_API SIZE CalcTextSize(HDC hdc,String& text,HFONT font = 0);
	class CONCISE_API BitmapList : public _class{
	protected:
		HDC _dc;
		IntegerDiction<HBITMAP> _bmpList;
	public:
		BitmapList():_dc(0){
		}
		uint Count(){
			return _bmpList.Count();
		}
		HBITMAP GetBitmap(uint index){
			return _bmpList.Element(index);
		}
		bool Delete(uint index){
			HBITMAP bmp = _bmpList.Element(index);
			if(!bmp) return 0;
			::DeleteObject(bmp);
			return 1;
		}
		bool DeleteByValue(HBITMAP bmp){
			if(_bmpList.DeleteValue(bmp)){
				::DeleteObject(bmp);
				return 1;
			}
			return 0;
		}
		bool Add(HBITMAP bmp,uint index = -1){
			bool exist = 0;
			_bmpList.Add(bmp,index,&exist);
			return exist;
		}
		//从文件添加图像，成功返回生成图像的句柄
		HBITMAP Add(LPCWSTR file,uint index = -1);
		//从资源加载图像，成功返回生成图像的句柄
		HBITMAP Add(ResID res,ResID type,uint index = -1);
		HBITMAP Remove(uint index){
			HBITMAP bmp = _bmpList.Element(index);
			if(!bmp) return 0;
			_bmpList.Delete(index);
			return bmp;
		}
		uint IndexOf(HBITMAP bmp,uint start = 0){
			return _bmpList.IndexOf(bmp,start);
		}
		void Clear(){
			for(uint i=0;i<_bmpList.Count();i++){
				::DeleteObject(_bmpList[i]);
			}
			_bmpList.Clear();
		}
		~BitmapList(){
			if(_dc) ::DeleteDC(_dc);
		}
		bool Draw(HBITMAP bmp,HDC hdc,uchar alpha = 255,bool alphaChanel = false,int x = 0,int y = 0,int cx = 0,int cy = 0,int sx = 0,int sy = 0,int scx = 0,int scy = 0){
			if(!bmp) return 0;
			if(!_dc) _dc = ::CreateCompatibleDC(NULL);
			if(!_dc) return 0;

			AutoStruct(BITMAP,bp);
			if(sizeof(BITMAP)!=::GetObject(bmp,sizeof(BITMAP),&bp)) return 0;

			BLENDFUNCTION bf;
			bf.AlphaFormat = alphaChanel;
			bf.BlendFlags = 0;
			bf.BlendOp = 0;//AC_SRC_OVER = 0;
			bf.SourceConstantAlpha = alpha;
			if(cx<=0) cx = bp.bmWidth;
			if(cy<=0) cy = bp.bmHeight;
			if((sx<0)||(sy<0))
				return 0;
			if(scx==0) scx = bp.bmWidth - sx;
			if(scy==0) scy = bp.bmHeight - sy;
			if(((scx+sx)>bp.bmWidth)||((scy+sy)>bp.bmHeight))
				return 0;
			GdiSelector gs(_dc,bmp);
			return ::AlphaBlend(hdc,x,y,cx,cy,_dc,sx,sy,scx,scy,bf)!=0;
			//this function is most slowly,when alpha is not 255 and source rect width destination rect
			// is not equal in size,especially last reason.
		}
		bool Draw(int index,HDC hdc,uchar alpha = 255,bool alphaChanel = false,int x = 0,int y = 0,int cx = 0,int cy = 0,int sx = 0,int sy = 0,int scx = 0,int scy = 0){
			HBITMAP bmp = _bmpList.Element(index);
			return Draw(bmp,hdc,alpha,alphaChanel,x,y,cx,cy,sx,sy,scx,scy);
		}
	};
};