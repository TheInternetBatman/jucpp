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
