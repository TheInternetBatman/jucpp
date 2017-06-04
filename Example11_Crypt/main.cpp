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
char* data = "8月15日，锤子科技在其官网挂出T2优惠促销的页面，诸如”买5台T2就送大疆无人机“这样的活动，表面上看噱头做的很足，但暴露出的却是锤子T2库存挤压过多而不得采取的策略。\r\n\
T2的销量到底多少锤子科技一直没有透露，现在能看到的数据是，T1上市一年，到2015年8月时的销量是255626台，坚果不到100万台。显然，这三款产品的整体销量都没有达到预期的销量。\r\n\
坚果售出100万台表面上看来数据还不错，但作为对比，根据IDC的数据显示，今年第二季度华为的出货量是1910万部，即便在今年饱受负面的小米第二季度的出货量也达到1050万部，也就是说坚果一年的出货量还不如其他厂商一个季度的零头。\r\n\
罗永浩在进入手机圈之前曾拥有众多的拥趸者，但这个”光辉“似乎正在一步步散去。今年6月一位自称是罗永浩粉丝的用户，以涉嫌虚假宣传和销售欺诈将锤子科技告上法庭。\r\n\
在锤子科技前CTO钱晨离职之后，锤子T3的发布日期一拖再拖，罗永浩在微博上一直不虞遗力的炫耀T3的新特性，只是产品迟迟不面试，让原本满怀期待的粉丝也慢慢丧失了耐心。\r\n\
锤子科技被收购的资本何在\r\n\
如果说锤子有什么被收购的资本，显然不会是技术、专利或渠道，这几块一直都是锤子科技的短板。如果要谈及设计营销，在国产手机中，锤子科技敢认第二，估计也没人敢认第一吧？当年的”东半球最好用的手机“已经成为了营销文案的经典案例。\r\n\
锤子科技另一个被人称赞的就是其软件开发团队，就是这个团队打造出了广受好评的锤子ROM、锤子便签等应用，这些软件在一定程度弥补了锤子手机在硬件配置方面的缺陷。\r\n\
无论锤子科技被哪家国产手机厂商收购，其软件团队都是一支生力军，完全可以帮助被收购那些厂商做手机系统、UI方面的优化。\r\n\
T3尚未面市，但关于锤子科技做VR的消息却越来越多，甚至有消息称锤子VR将会先T3一步上市。不言而喻的是VR已经成为新的风口，锤子在VR领域也是做出了多番努力。\r\n\
且不论锤子VR能否大获成功，至少在目前看来，罗永浩把这个故事给讲好了。如果被收购的话，VR团队也是一个很好的谈判资本。\r\n\
被收购还是死撑？\r\n\
罗永浩此前在接受《财经》的采访时表示“就算万一有什么问题，锤子科技也是被收购而不是倒闭，对于我们这些一定要把产品和品牌做好的人来说，其实锤子科技被收购也没什么。“\r\n\
罗永浩的本意自然是不希望锤子被收购，但是情怀归情怀，现实归现实。在引入苏宁之后，罗永浩原本寄希望借助于苏宁的渠道来打开道路，但事实却与罗的想法南辕北辙。据媒体报道锤子线下销量惨淡，苏宁旗舰店一月难卖一台，其官方代理商单日销量从60台下降至1、2台。\r\n\
罗永浩后来又倚靠上了阿里巴巴这个大树，虽说大树底下好乘凉，但罗永浩应该看到的是在站在这棵大树下并非只有锤子。如果阿里巴巴要从魅族还是锤子中二选一的话，答案显然是不言而喻的。\r\n\
这个冷酷的市场并没有给手机厂商们多少喘息的机会，即便是曾经站在顶端的小米，更不要去谈在弱小的锤子。面对困局，锤子除了死撑之外或许最好的方式就是趁着估值还行把自己卖了。\r\n\
罗永浩曾说”现在是锤子史上最好的时期“，好在哪里？或许是被收购最好的时期吧。\r\n\
不管怎样，祝愿锤子能有一个灿烂的明天。";
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

