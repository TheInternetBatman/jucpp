#include "../juwnd.h"
#pragma usexpstyle

WINMAIN{
	ju::FileStream fs;
	fs.OpenExist(L"D:/Develop/Wamp/www/html/public/wifi.html");
	ju::String str;
	fs.ReadString(str, 0, CP_UTF8);
	fs.Close();

	ju::String ns;
	int start = 0;
	while(true) {
		int pos = str.Find(L"\r\n",start);
		if(pos == -1) {
			ns += str.GetSub(start,-1);
			break;
		}
		ns += str.GetSub(start, pos-start);
		ns += L"\\r\\n\\\n";
		start = pos + 2;
	}
	fs.Create(L"D:/Develop/Arduino/MCHardware/data/index2.html");
	fs.SetLength(0);
	fs.WriteString(ns, 0, 0, CP_UTF8);
	return 1;

	char buf[256];
	char data[256] = "今日，扎克伯格晒出了自己的衣柜，让人大跌眼镜，里面真的只有黑、灰两款衣服！他在Facebook发问： “陪产假后第一天上班，穿哪件好呢？”";
	ju::Aes aes;
	aes.SetKey("kljasldfasd",-1);
	char iv[] = "扎克伯格";
	aes.SetIV(iv,sizeof(iv));
	aes.SetMode(true);
	int len = strlen(data);
	//aes.EncryptCfb(buf,data,len);
	aes.EncryptEcb(buf,data);
	aes.SetMode(false);
	::ZeroMemory(data, sizeof(data));
	//aes.SetIV(iv, sizeof(iv));
	aes.DecryptEcb(data, buf);
	//aes.DecryptCfb(data,buf,len);
	return 1;
}
