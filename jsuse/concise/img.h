#pragma once
namespace cs{
	CONCISE_API bool GetImageEncoderClsid(const wchar_t* type,CLSID* pClsid);
	//ImageItem 是 Gdiplus 的 PropertyItem 类.	
	class ImageItem{
	public:
		PROPID  id;                 // ID of this property
		ULONG   length;             // Length of the property value, in bytes
		WORD    type;               // Type of the value, as one of TAG_TYPE_XXX
		VOID*   value;              // property value
	};
	//对应Gdiplus::BitmapData类型，Data参数就是像素数据。
	class BitmapData{
	public:
		UINT Width;
		UINT Height;
		INT Stride;
		UINT PixelFormat;
		BYTE* Data;
		UINT_PTR Reserved;
	};

	//Image是另一种图像资源,目前唯一的用途就是加载和保存不同类型的图像文件.用Image资源绘图效率是非常低的.
	class CONCISE_API Image : public _class{
	protected:
		void* _image;
	public:
		Image();
		//从位图资源生成, 需要手动删除bmp资源。
		bool Create(HBITMAP bmp);
		//从文件加载;
		bool Create(const wchar_t* file);
		//从内存加载，这个函数的mem参数是file读取后的数据.len是数据长度。
		bool Create(void* mem,int len);
		//设置当前帧，当前帧是保存和绘图使用的帧。
		bool SetActiveFrame(int index);
		//保存为文件;quality是保存质量，100是最大值，0是最小值。
		bool Save(const wchar_t* file,long quality = 60);
		//保存到内存,type是包含点号的类型名，比如“.jpg”等，quality是保存质量，100是最大值，0是最小值。
		int Save(Memory<char>& mem,LPCWSTR type,long quality = 60);
		//绘图;
		bool Draw(HDC hdc,int x,int y,int cx = 0,int cy = 0);
		//宽
		int Width();
		//高
		int Height();
		//获取srImg的缩略图，如果srImg为0,它获取自身的Thumbnail.
		bool GetThumbnail(int cx,int cy,Image* srImg = 0);
		//复制
		void operator = (Image& img);
		//关闭，资源被清理。
		void Close();
		//复合图像(gif,jpg)等, 有维和帧两个概念, 用某维某帧图像来索引图像, 但是维一般返回值是 1, 目前图像只用是帧来区分.
		//uint DimensionsCount();
		//返回第一维图像的帧数
		uint FrameCount();
		//图像的像素格式，对应类型 Gdiplus::ImagePixelFormat 类型
		uint PixelFormat();
		//图像帧信息, 返回值是帧数, 信息存在tms里面.
		uint GetPropertyItem(Memory<ImageItem>& tms);
		//这个函数返回当前帧图像的一个 HBITMAP 位图资源,用户必须自己负责释放.
		HBITMAP GetBitmap();
		/*//获取图像当前帧指定位置的像素颜色，如果大量操作像素数据使用LockPixelData函数
		int GetPixel(int x,int y);
		//设置图像当前帧指定位置的图像颜色，如果大量操作像素数据使用LockPixelData函数
		bool SetPixel(int x,int y,int color);
		//获取像素数据，对数据进行操作后，调用UnlockPixelData函数，数据会被刷新到图像中。
		//rect = 0, 表示获取整个图像的数据；pixelFormat 指定需要的像素格式，0 表示32位 ARGB 格式。
		BitmapData* LockPixelData(LPRECT rect = 0,int pixelFormat = 0);
		//调用LockPixelData后，必须调用UnlockPixelData来解除锁定，并且释放资源。
		void UnlockPixelData(BitmapData* pbd);*/
		//析构。
		~Image();
	};
};
