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
	char data[256] = "���գ����˲���ɹ�����Լ����¹����˴���۾����������ֻ�кڡ��������·�������Facebook���ʣ� ������ٺ��һ���ϰ࣬���ļ����أ���";
	ju::Aes aes;
	aes.SetKey("kljasldfasd",-1);
	char iv[] = "���˲���";
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
