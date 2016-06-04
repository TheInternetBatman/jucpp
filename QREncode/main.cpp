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
				//���������������ת�;���ͼ�񣬵��Ǿ����ά���ǲ�����ȷ��ȡ�ġ�
				//������Ȼ��ͼ�ķ�ʽ�кܶࣨ8�֣������ǽ���һ���ܶ�ȡ������ͼ��ֻ����һ�֡�
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
/*�����ļ���ʽ
{
"text": "http://www.jsuse.com",	//�����ı�
"error": 2,	//�ݴ�ȼ�: 0��1��2��3
"radius": 10,	//ÿ��������ؿ��
"border": 4,	//�߿��ȣ���λ�㣩
"color": 0,		//ǰ��ɫ��Ĭ�Ϻ�ɫ
"background": 16777215,	//����ɫ��Ĭ�ϰ�ɫ����ά��ı���ɫ�����ǰ��ɫ��
"savefile": "qr.png",	//Ҫ������ļ����������� jpg��png��gif��bmp�ȡ�
"alpha": 255	//����͸���ȣ�0 - 255�����ֵ��������Ϊ png ͼ����Ч
}*/