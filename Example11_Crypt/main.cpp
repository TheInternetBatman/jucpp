#include "../juwnd.h"
#pragma usexpstyle

WINMAIN{
	ju::File file;
	file.Create(L"abc.txt");
	ju::FileSystem fs;
	fs.Initialize(L"abc.txt");
	bool r = fs.Delete();
	ju::Print(L"%d",r);
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
