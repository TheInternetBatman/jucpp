#include "../juwnd.h"
#pragma usexpstyle

#include "QR_Encode.h"
struct QRParam {
public:
	ju::String text, savefile;
	int error, radius, border,
		color, background;
	byte alpha;
	void load() {
		ju::Json cfg;
		cfg.LoadFromFile(L"qr.json");
		text = cfg.PropertyStr(L"text", L"http://www.jsuse.com");
		savefile = cfg.PropertyStr(L"savefile", L"qr.jpg");
		error = cfg.PropertyInt32(L"error", 2);
		radius = cfg.PropertyInt32(L"radius", 10);
		border = cfg.PropertyInt32(L"border", 4);
		color = cfg.PropertyInt32(L"color", 0);
		alpha = cfg.PropertyInt32(L"alpha", 255);
		background = cfg.PropertyInt32(L"background", 0xffffff);
	}
};
inline void setPixel(byte* pixels, int index, byte a, byte r, byte g, byte b) {
	pixels[index] = r;
	pixels[index + 1] = g;
	pixels[index + 2] = b;
	pixels[index + 3] = a;
}
bool createQRImage(BYTE data[177][177], int width, QRParam& p) {
	int c = p.color | 0xff000000;
	int b = p.background | (p.alpha << 24);
	ju::Bitmap img;
	int w = width + p.border * 2;
	int r = width + p.border;
	img.CreateCompatibleBitmap(w, w);
	ju::Memory<byte> pixels;
	img.GetPixelData(&pixels, 32);
	for(int i = 0; i<w; i++) {
		for(int j = 0; j<w; j++) {
			int index = j * 4 + w*(w - i - 1) * 4;
			if(i<p.border || i >= r || j<p.border || j >= r) {
				*(int*)(pixels + index) = b;
			} else {
				//这个函数可以来旋转和镜像图像，但是镜像二维码是不能正确读取的。
				//所以虽然绘图的方式有很多（8种），但是仅有一半能读取，正视图就只有这一种。
				*(int*)(pixels + index) = data[j - p.border][i - p.border] ? c : b;
			}
		}
	}
	img.SetPixelData(&pixels, 32);
	img.Resize(w*p.radius, w*p.radius);
	return img.Save(p.savefile);
}
WINMAIN{
	QRParam p;
	p.load();
	ju::Memory<char> str;
	uint len = p.text.ToMultiByte(str, CP_UTF8);
	str[len] = 0;
	CQR_Encode qre;
	BOOL rst = qre.EncodeData(0, 1, 1, 1, str.Handle());
	if(!rst) return 0;
	return createQRImage(qre.m_byModuleData, qre.m_nSymbleSize, p);
};
/*配置文件格式
{
"text": "http://www.jsuse.com",	//编码文本
"error": 2,	//容错等级: 0、1、2、3
"radius": 10,	//每个点的像素宽度
"border": 4,	//边框宽度（单位点）
"color": 0,		//前景色，默认黑色
"background": 16777215,	//背景色，默认白色，二维码的背景色必须比前景色亮
"savefile": "qr.png",	//要保存的文件名，可以是 jpg、png、gif、bmp等。
"alpha": 255	//背景透明度，0 - 255，这个值仅当保存为 png 图像有效
}*/