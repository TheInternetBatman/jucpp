#include "../juwnd.h"
#pragma usexpstyle
#include <stdio.h>

<<<<<<< .mineint _length = 0;
char _buf[5120];
void parseRecv() {
	int offset = 0;
	for(int i = 0; i<_length; i++) {
		char ch = _buf[i];
		if(ch == '\n') {
			_buf[i - 1] = 0;
			//_onRecvLine(_buf + offset);
			offset = i + 1;
		}
	}
	if(offset<_length) {
		memcpy(_buf, _buf + offset, _length - offset);
	}
	_length -= offset;
}
void check_calc() {
	ju::File fs;
	fs.OpenExist(L"abc.txt");
	ju::Memory<char> buf;
	int len = fs.Read(&buf);
	buf.CopyTo(_buf, len);
	_length = len;
	parseRecv();
}
=======void convertQuote(ju::String& html) {
	for(int i = 0; i < html.Length(); i++) {
		if(html[i]=='"') html[i] = '\'';
	}
}
void convertHtml() {
	ju::FileStream fs;
	fs.OpenExist(L"D:/Develop/Wamp/www/html/public/ether.html");
	ju::String str;
	fs.ReadString(str, 0, CP_UTF8);
	fs.Close();
	
	convertQuote(str);

	ju::String ns;
	int start = 0;
	while(true) {
		int pos = str.Find(L"\r\n", start);
		if(pos == -1) {
			ns += L"\"";
			ns += str.GetSub(start, -1);
			ns += L"\"";
			break;
		}
		ns += L"\"";
		ns += str.GetSub(start, pos - start);
		ns += L"\\r\\n\"\r\n";
		start = pos + 2;
	}
	fs.Create(L"D:/Develop/Wamp/www/html/public/ether2.html");
	fs.SetLength(0);
	fs.WriteString(ns, 0, 0, CP_UTF8);
}
void __stdcall _recv(ju::Socket* socket) {
	char buf[10240];
	while(1) {
		int len = socket->Receive(buf, sizeof(buf));
		if(len <= 0) {
			ju::Print(L"disconnect");
			break;
		}
		buf[len] = 0;
		ju::Print("%s", buf);
	}
	ju::Print(L"recv exit");
}
void tcpClient() {
	ju::Socket socket;
	socket.Create();
	ju::Thread th;
	ju::SOCKADDR sa;
	sa.SetIp("192.168.1.105");
	sa.SetPort(80);
	if(!socket.Connect(sa)) {
		ju::Print(L"can not connect");
		return;
	}
	const char* req =
"GET / HTTP/1.1\r\n\
Host: 192.168.1.105\r\n\
Connection : keep - alive\r\n\
Cache - Control : max - age = 0\r\n\
Upgrade - Insecure - Requests : 1\r\n\
User - Agent : Mozilla / 5.0 (Windows NT 6.1; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 52.0.2743.116 Safari / 537.36\r\n\
Accept : text / html, application / xhtml + xml, application / xml; q = 0.9, image / webp, */*;q=0.8\r\n\
Accept-Encoding: gzip, deflate, sdch\r\n\
Accept - Language: zh - CN, zh; q = 0.8\r\n\r\n";
	socket.Send(req,strlen(req));
	Sleep(1000);
	th.CallStd<void>(&_recv, &socket);
	/*th.Close();
	socket.Close();
	socket.Create();
	sa.SetIp("192.168.1.105");
	sa.SetPort(80);
	if(!socket.Connect(sa)) {
		ju::Print(L"can not connect");
		return;
	}
	th.CallStd<void>(&_recv, &socket);*/
	ju::Print(L"send http request");
}
ju::Socket* last = 0;
void __stdcall onAccept(SOCKET s) {
	ju::Print(L"connect");
	ju::Socket socket;
	last = &socket;
	socket.Attach(s);
	ju::SOCKADDR si;
	socket.GetPeerAddr(si);
	ju::String ip = si.GetIp();
	ju::Print(ip);

	char buf[2049];
	while(true) {
		int len = socket.Receive(buf,2048);
		if(len <= 0) {
			break;
		}
		buf[len] = 0;
		ju::Print("%s", buf);
	}
	ju::Print(L"disconnect");
}
char* data = "8��15�գ����ӿƼ���������ҳ�T2�Żݴ�����ҳ�棬���硱��5̨T2���ʹ����˻��������Ļ�������Ͽ���ͷ���ĺ��㣬����¶����ȴ�Ǵ���T2��漷ѹ��������ò�ȡ�Ĳ��ԡ�\r\n\
T2���������׶��ٴ��ӿƼ�һֱû��͸¶�������ܿ����������ǣ�T1����һ�꣬��2015��8��ʱ��������255626̨���������100��̨����Ȼ���������Ʒ������������û�дﵽԤ�ڵ�������\r\n\
����۳�100��̨�����Ͽ������ݻ���������Ϊ�Աȣ�����IDC��������ʾ������ڶ����Ȼ�Ϊ�ĳ�������1910�򲿣������ڽ��걥�ܸ����С�׵ڶ����ȵĳ�����Ҳ�ﵽ1050�򲿣�Ҳ����˵���һ��ĳ�������������������һ�����ȵ���ͷ��\r\n\
�������ڽ����ֻ�Ȧ֮ǰ��ӵ���ڶ��ӵ���ߣ����������ԡ��ƺ�����һ����ɢȥ������6��һλ�Գ��������Ʒ�˿���û������������������������թ�����ӿƼ����Ϸ�ͥ��\r\n\
�ڴ��ӿƼ�ǰCTOǮ����ְ֮�󣬴���T3�ķ�������һ�����ϣ���������΢����һֱ������������ҫT3�������ԣ�ֻ�ǲ�Ʒ�ٳٲ����ԣ���ԭ�������ڴ��ķ�˿Ҳ����ɥʧ�����ġ�\r\n\
���ӿƼ����չ����ʱ�����\r\n\
���˵������ʲô���չ����ʱ�����Ȼ�����Ǽ�����ר�����������⼸��һֱ���Ǵ��ӿƼ��Ķ̰塣���Ҫ̸�����Ӫ�����ڹ����ֻ��У����ӿƼ����ϵڶ�������Ҳû�˸��ϵ�һ�ɣ�����ġ�����������õ��ֻ����Ѿ���Ϊ��Ӫ���İ��ľ��䰸����\r\n\
���ӿƼ���һ�����˳��޵ľ�������������Ŷӣ���������ŶӴ�����˹��ܺ����Ĵ���ROM�����ӱ�ǩ��Ӧ�ã���Щ�����һ���̶��ֲ��˴����ֻ���Ӳ�����÷����ȱ�ݡ�\r\n\
���۴��ӿƼ����ļҹ����ֻ������չ���������ŶӶ���һ֧����������ȫ���԰������չ���Щ�������ֻ�ϵͳ��UI������Ż���\r\n\
T3��δ���У������ڴ��ӿƼ���VR����ϢȴԽ��Խ�࣬��������Ϣ�ƴ���VR������T3һ�����С����Զ�������VR�Ѿ���Ϊ�µķ�ڣ�������VR����Ҳ�������˶෬Ŭ����\r\n\
�Ҳ��۴���VR�ܷ���ɹ���������Ŀǰ�����������ư�������¸������ˡ�������չ��Ļ���VR�Ŷ�Ҳ��һ���ܺõ�̸���ʱ���\r\n\
���չ��������ţ�\r\n\
�����ƴ�ǰ�ڽ��ܡ��ƾ����Ĳɷ�ʱ��ʾ��������һ��ʲô���⣬���ӿƼ�Ҳ�Ǳ��չ������ǵ��գ�����������Щһ��Ҫ�Ѳ�Ʒ��Ʒ�����õ�����˵����ʵ���ӿƼ����չ�Ҳûʲô����\r\n\
�����Ƶı�����Ȼ�ǲ�ϣ�����ӱ��չ��������黳���黳����ʵ����ʵ������������֮��������ԭ����ϣ���������������������򿪵�·������ʵȴ���޵��뷨��ԯ���ޡ���ý�屨���������������ҵ��������콢��һ������һ̨����ٷ������̵���������60̨�½���1��2̨��\r\n\
�����ƺ������п����˰���Ͱ������������˵�������ºó�������������Ӧ�ÿ���������վ����ô����²���ֻ�д��ӡ��������Ͱ�Ҫ�����廹�Ǵ����ж�ѡһ�Ļ�������Ȼ�ǲ��Զ����ġ�\r\n\
��������г���û�и��ֻ������Ƕ��ٴ�Ϣ�Ļ��ᣬ����������վ�ڶ��˵�С�ף�����Ҫȥ̸����С�Ĵ��ӡ�������֣����ӳ�������֮�������õķ�ʽ���ǳ��Ź�ֵ���а��Լ����ˡ�\r\n\
��������˵�������Ǵ���ʷ����õ�ʱ�ڡ���������������Ǳ��չ���õ�ʱ�ڰɡ�\r\n\
����������ףԸ��������һ�����õ����졣";
void __stdcall sendData() {
	
	while(true) {
		Sleep(5000);
		if(!last) continue;
		last->Send(data, strlen(data));
	}
}
void tcpServer() {
	ju::TCPServer serv;
	serv.OnAccept.BindStd(&onAccept);
	ju::Thread th;
	th.CallStd<void>(&sendData);
	serv.Startup(3720);
}
>>>>>>> .theirsWINMAIN{
<<<<<<< .mine	check_calc();
	return 0;
	ju::File file;
	file.Create(L"abc.txt");
	ju::FileSystem fs;
	fs.Initialize(L"abc.txt");
	bool r = fs.Delete();
	ju::Print(L"%d",r);
=======	tcpServer();
	//convertHtml();
>>>>>>> .theirs	return 1;
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

