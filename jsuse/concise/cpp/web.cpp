#include "../stdafx.h"
#include "../../concise.h"

namespace cs{
	void DecodeURI(String& str,DWORD codePage)
	{
		String num;
		Memory<char> buf;
		int len = str.Length(),j = 0;
		buf.SetLength(len);
		for(int i=0;i<len;)
		{
			if(str[i]=='%')
			{
				num = L"";
				if((i+3)>len) break;
				num.CopyFrom(str+i+1,2);
				i += 3;
				buf[j] = num.ToInt32(16);
				j++;
			}
			else
			{
				buf[j] = (char)str[i];
				j++;
				i++;
			}
		}
		str.FromMultiByte(buf,j,codePage);
	}
	void EncodeURI(String& str,DWORD codePage)
	{
		Memory<char> buf;
		int n = str.ToMultiByte(&buf,codePage);
		String num;
		for(int i=0,j=0;i<n;i++)
		{
			if(buf[i]==32)
			{
				str.CopyFrom(L"%20",3,j);
				j += 3;
			}
			else if(buf[i]>0)
			{
				wchar_t ch = buf[i];
				str.CopyFrom(&ch,1,j);
				j++;
			}
			else
			{
				num.Format(L"%X",(uint)buf[i]);
				num.Tail(-2);
				num.Upper();
				num.Insert('%',0);
				str.CopyFrom(num,3,j);
				j += 3;
			}
		}
	}
	//_socket
	void StartupWSA()
	{
		WSADATA wsd;
		::WSAStartup(MAKEWORD(2,2),&wsd);
	}
	void CleanupWSA()
	{
		::WSACleanup();
	}
	//IP
	void IP::operator =(const wchar_t *ip)
	{
		String wip = ip;
		if(wip.IsNull()){
			S_un.S_addr = INADDR_NONE;
			return;
		}
		Memory<char> mip;
		int len = wip.ToMultiByte(&mip);
		mip[len] = 0;
		*this = mip;
	}
	//Host
	Host::Host():_Hostent(0)
	{
		StartupWSA();
	}
	Host::~Host()
	{
		CleanupWSA();
	}
	bool Host::Create()
	{
		Memory<char> buf;
		buf.SetLength(256);
		if(::gethostname(buf,256)!=0) return 0;
		return Create(buf);
	}
	bool Host::Create(u_long ip)
	{
		_Hostent = ::gethostbyaddr((char*)&ip,4,AF_INET);
		if(_Hostent==0) return 0;
		_aliasesCount = 0;
		while(_Hostent->h_aliases[_aliasesCount]) _aliasesCount++;
		_addrCount = 0;
		while(_Hostent->h_addr_list[_addrCount]) _addrCount++;
		return 1;
	}
	bool Host::Create(const char* name)
	{
		_Hostent = ::gethostbyname(name);
		if(_Hostent==0) return 0;
		_aliasesCount = 0;
		while(_Hostent->h_aliases[_aliasesCount]) _aliasesCount++;
		_addrCount = 0;
		while(_Hostent->h_addr_list[_addrCount]) _addrCount++;
		return 1;
	}
	bool Host::Create(const wchar_t* name)
	{
		Memory<char> buf;
		String n = name;
		int len = n.ToMultiByte(&buf);
		if(buf.IsNull())
			return Create("");
		buf[len] = 0;
		return Create(buf);
	}
	char* Host::GetAliases(int index)
	{
		if(_Hostent)
		{
			if((unsigned int)_aliasesCount<=(unsigned int)index) return 0;
			return _Hostent->h_aliases[index];
		}
		return 0;
	}
	IP Host::GetIp(int index)
	{
		IP ip;
		if(_Hostent&&((uint)_addrCount>(uint)index))
		{
			ip = *((u_long*)_Hostent->h_addr_list[index]);
		}
		return ip;
	}
	//Socket Ò»°ãÀ´ËµWin Sockº¯Êý´íÎóÊ±·µ»Ø-1£¬ÍøÂçÖÐ¶Ï·µ»Ø0.
	Socket::Socket()
	{
		_Handle = INVALID_SOCKET;
		StartupWSA();
	}
	Socket::Socket(SOCKET s)
	{
		_Handle = s;
		StartupWSA();
	}
	Socket::~Socket()
	{
		::closesocket(_Handle);
		CleanupWSA();
	}
	bool Socket::Create(int protocol,int type)
	{
		if(_Handle!=INVALID_SOCKET) return 0;
		_Handle = ::socket(AF_INET,type,protocol);
		return _Handle!=INVALID_SOCKET;
	}
	bool Socket::Close()
	{
		if(_Handle==INVALID_SOCKET) return 0;
		::closesocket(_Handle);
		_Handle = INVALID_SOCKET;
		return 1;
	}
	SOCKET Socket::Accept(sockaddr* srv)
	{
		int slen;
		slen = sizeof(sockaddr_in);
		SOCKADDR saddr;
		if(srv==0) srv = saddr; 
		return accept(_Handle,srv,&slen);
	}
	bool Socket::Attach(SOCKET s)
	{
		if(s==INVALID_SOCKET)
			return 0;
		_Handle = s;
		return 1;
	}
	SOCKET Socket::Detach()
	{
		SOCKET s = _Handle;
		_Handle = INVALID_SOCKET;
		return s;
	}
	//TCPServer
	TCPServer::~TCPServer()
	{
		_Handle.Close();
		_thread.Wait();
	}
	void TCPServer::Close(){
		_Handle.Close();
		_thread.Wait();
		_thread.Close();
	}
	HRESULT TCPServer::Startup(ushort port,IP ip){
		if(!_Handle.Create()) return 0;
		SOCKADDR addr;
		addr.SetIp(ip);
		addr.SetPort(port);
		if(!_Handle.Bind(addr)){
			int rst = WSAGetLastError();
			_Handle.Close();
			return rst;
		}
		if(!_Handle.Listen()){
			int rst = WSAGetLastError();
			_Handle.Close();
			return rst;
		}
		_Accept();
		return 0;
	}
	HRESULT TCPServer::StartupAsyn(ushort port,IP ip){
		if(!_Handle.Create()) return 0;
		SOCKADDR addr;
		addr.SetIp(ip);
		addr.SetPort(port);
		if(!_Handle.Bind(addr)){
			int rst = WSAGetLastError();
			_Handle.Close();
			return rst;
		}
		if(!_Handle.Listen()){
			int rst = WSAGetLastError();
			_Handle.Close();
			return rst;
		}
		_thread.Close();
		if(!_thread.Call<void>(this,&TCPServer::_Accept)){
			int rst = WSAGetLastError();
			_Handle.Close();
			return rst;
		}
		return 0;
	}
	void TCPServer::_Accept(){
		while(1){
			SOCKET cs = _Handle.Accept();
			if((cs==-1)||(cs==0)){
				break;
			}
			OnAccept(cs);
		}
	}

	/*
	bool TCPTransfer::StartRecv(){
		if(!_connected) return 0;
		const int			maxTcpBufSize = 0x2000;
		Memory<char>		_recvBuf(maxTcpBufSize);
		int					recvLen = 0,offset = 0,dataLen = 0;
		while(1){
			if((recvLen+maxTcpBufSize)>(int)_recvBuf.Length()){
				if(!_recvBuf.SetLength(recvLen+maxTcpBufSize)){
					Print(L"socket receive: can't alloc enough memory");
					_handle.Close();
					_connected = 0;
					OnClose(_userParam);
					break;
				}
			}
			int rLen = _handle.Receive(_recvBuf.Handle()+recvLen,maxTcpBufSize);
			//Print(L"%d",rLen);
			if(rLen<=0){
				PrintLastError(WSAGetLastError());
				_handle.Close();
				_connected = 0;
				OnClose(_userParam);
				break;
			}
			recvLen += rLen;

			while(1){
				if(recvLen<(offset+4)) break;
				if(dataLen==0){
					dataLen = *(int*)(_recvBuf.Handle()+offset);
					//Print(L"recv datalen:%d",dataLen);
					offset += 4;
				}
				bool zip;
				if(dataLen<0){
					dataLen = dataLen&0x7fffffff;
					zip = 1;
				}else{
					zip = 0;
				}
				if(dataLen>(recvLen-offset)){
					break;
				}
				if(zip){
					Memory<char> buf;
					UINT len = DeCompress(_recvBuf.Handle()+offset,dataLen,&buf);
					OnRecv(buf.Handle(),len,_userParam);
				}else{
					OnRecv(_recvBuf.Handle()+offset,dataLen,_userParam);
				}
				offset += dataLen;
				dataLen = 0;
			}
			recvLen -= offset;
			_recvBuf.Move(0,offset,recvLen);
			offset = 0;
		}
		return 1;
	}
	TCPTransfer::TCPTransfer(){
		_connected = 0;
		_userParam = 0;
		_sendBuf.SetUseSysMemory(true);
		_sendBuf.SetLength(0x2000);
		*(int*)_sendBuf.Handle() = 0;
	}
	TCPTransfer::~TCPTransfer(){
		Close();
	}
	void TCPTransfer::Attach(SOCKET s,bool cn){
		_connected = cn;
		_handle.Attach(s);
	}
	SOCKET TCPTransfer::Detach(){
		_connected = 0;
		return _handle.Detach();
	}
	void TCPTransfer::Accept(SOCKET s){
		_connected = 1;
		_handle.Attach(s);
	}
	void TCPTransfer::Close(){
		_handle.Close();
		_connected = 0;
		_recvThread.Wait();
		_recvThread.Close();
	}
	void TCPTransfer::SetUserParam(void* param){
		_userParam = param;
	}
	void TCPTransfer::_connect(SOCKADDR sa,bool startRecv,bool asyn){
		_connected = _handle.Connect(sa);
		OnConnect(_connected,_userParam);
		if(!_connected)
			PrintLastError(WSAGetLastError());
		if(!_connected||!startRecv) return;
		if(asyn){
			StartRecvAsyn();
		}else{
			StartRecv();
		}
	}
	bool TCPTransfer::Connect(IP ip,WORD port,bool startRecv){
		SOCKADDR sa;
		sa.SetIp(ip);
		sa.SetPort(port);
		if(_connected) return 0;
		if(_handle.IsNull())
			_handle.Create();
		_connect(sa,startRecv,0);
		return _connected;
	}
	bool TCPTransfer::ConnectAsyn(IP ip,WORD port,bool startRecv){
		SOCKADDR sa;
		sa.SetIp(ip);
		sa.SetPort(port);
		if(_connected) return 0;
		if(_handle.IsNull())
			_handle.Create();
		return _recvThread.Call<void>(this,&TCPTransfer::_connect,sa,startRecv,true);
	}
	bool TCPTransfer::StartRecvAsyn(){
		if(!_connected) return 0;
		_recvThread.Close();
		return _recvThread.Call<bool>(this,&TCPTransfer::StartRecv);
	}
	void TCPTransfer::operator = (SOCKET s){
		_handle.Attach(s);
	}
	bool TCPTransfer::AddSendData(const void* data,int len,UINT offset){
		if(len<0) return 0;
		int dataLen = *(int*)_sendBuf.Handle();
		int sumLen = dataLen + len + sizeof(int);
		if(sumLen<=0) return 0;
		if(sumLen>(int)_sendBuf.Length()){
			if(!_sendBuf.SetLength(sumLen))
				return 0;
		}
		if(offset>(UINT)dataLen) offset = dataLen;
		_sendBuf.Move(offset+sizeof(int)+len,offset+sizeof(int),dataLen-offset);
		_sendBuf.CopyFrom((const char*)data,len,offset+sizeof(int));
		dataLen += len;
		*(int*)_sendBuf.Handle() = dataLen;
		return 1;
	}
	int TCPTransfer::Send(bool zip){
		int dataLen = *(int*)_sendBuf.Handle(),len;
		//Print(L"TCPTransfer::Send,dataLen:%d",dataLen);
		if(zip){
			UINT bufLen = GetMaxCompressLen(dataLen);
			Memory<char> buf(bufLen+4);
			bufLen = Compress(_sendBuf.Handle()+4,dataLen,buf.Handle()+4,bufLen);
			*(int*)buf.Handle() = bufLen|0x80000000;
			len = _handle.Send(buf.Handle(),bufLen+4);
		}else{
			len = _handle.Send(_sendBuf.Handle(),dataLen+sizeof(int));
		}
		*(int*)_sendBuf.Handle() = 0;
		return len;
	}

	void TCPTransfer::_send(){
		while(true){
			_sendBlock.Wait();
			int dataLen = *(int*)_sendBuf.Handle(),len;
			//Print(L"TCPTransfer::Send,dataLen:%d",dataLen);
			if(zip){
				UINT bufLen = GetMaxCompressLen(dataLen);
				Memory<char> buf(bufLen+4);
				bufLen = Compress(_sendBuf.Handle()+4,dataLen,buf.Handle()+4,bufLen);
				*(int*)buf.Handle() = bufLen|0x80000000;
				len = _handle.Send(buf.Handle(),bufLen+4);
			}else{
				len = _handle.Send(_sendBuf.Handle(),dataLen+sizeof(int));
			}
			*(int*)_sendBuf.Handle() = 0;
		}
	}
	bool TCPTransfer::StartSendAsyn(){
		if(!_connected) return 0;
		_sendThread.Close();
		_sendBlock.Create();
		return _sendThread.Call<bool>(this,&TCPTransfer::_send);
	}
	*/
	//UDPSender
	bool UDPSender::Create()
	{
		if(!_Handle.Create(IPPROTO_UDP,SOCK_DGRAM)) return 0;
		BOOL fBroadcast = TRUE;
		_Handle.SetSockOpt(SOL_SOCKET,SO_BROADCAST,(char*)&fBroadcast,sizeof(BOOL));
		return 1;
	}
	int UDPSender::SendTo(char* data,int dLen,ushort port,IP ip)
	{
		SOCKADDR sServ;
		sServ.SetIp(ip);
		sServ.SetPort(port);
		return _Handle.SendTo(data,dLen,0,sServ);
	}
	bool UDPSender::SetDest(ushort port, IP ip)
	{
		SOCKADDR sa;
		sa.SetPort(port);
		sa.SetIp(ip);
		return _Handle.Connect(sa);
	}
	int UDPSender::Send(char *data, int len)
	{
		return _Handle.Send(data,len);
	}
	//UDPReceiver
	void UDPReceiver::_Receive()
	{
		SOCKADDR sa;
		int size = sa.Size();
		Memory<char> buf;
		buf.SetLength(0x10000);
		while(1){
			int rLen = _Handle.RecvFrom(buf.Handle(),0x10000,0,sa,&size);
			if(rLen<0){
				_Handle.Close();
				break;
			}
			OnRecv(buf.Handle(),rLen,sa);
		}
	}
	UDPReceiver::~UDPReceiver(){
		_Handle.Close();
		_thread.Wait();
	}
	void UDPReceiver::Close(){
		_Handle.Close();
		_thread.Close();
	}
	bool UDPReceiver::StartAsyn(ushort port){
		if(!_Handle.Create(IPPROTO_UDP,SOCK_DGRAM)) return 0;
		SOCKADDR sin;
		sin.SetPort(port);
		sin.SetIp(INADDR_ANY);
		if(!_Handle.Bind(sin)){
			_Handle.Close();
			Print(L"°ó¶¨Ê§°Ü£ºUDPReceiver::Tune£¨ushort port£©");
			return 0;
		}
		return _thread.Call<void>(this,&UDPReceiver::_Receive);
	}
	bool UDPReceiver::Start(ushort port){
		if(!_Handle.Create(IPPROTO_UDP,SOCK_DGRAM)) return 0;
		SOCKADDR sin;
		sin.SetPort(port);
		sin.SetIp(INADDR_ANY);
		if(!_Handle.Bind(sin)){
			_Handle.Close();
			Print(L"°ó¶¨Ê§°Ü£ºUDPReceiver::Tune£¨ushort port£©");
			return 0;
		}
		_Receive();
		return 1;
	}
	//TCPServer
	/*TCPServer::~TCPServer()
	{
		_Handle.Close();
		_thread.Wait();
	}
	void TCPServer::Close(){
		_Handle.Close();
		_thread.Wait();
		_thread.Close();
	}
	bool TCPServer::Starup(ushort port,IP ip)
	{
		if(!_Handle.Create()) return 0;
		SOCKADDR addr;
		addr.SetIp(ip);
		addr.SetPort(port);
		if(!_Handle.Bind(addr))
		{
			_Handle.Close();
			return 0;
		}
		if(!_Handle.Listen()) 
		{
			_Handle.Close();
			return 0;
		}
		_thread.Close();
		if(!_thread.Call<void>(this,&TCPServer::_Accept))
		{
			_Handle.Close();
			return 0;
		}
		return 1;
	}
	void TCPServer::_Accept()
	{
		while(1)
		{
			SOCKET cs = _Handle.Accept();
			if((cs==-1)||(cs==0))
			{
				break;
			}
			OnAccept(cs);
		}
	}

	void TCPTransfer::onRecv(){
		const int			maxTcpBufSize = 0x2000;
		Memory<char>	recvBuf(maxTcpBufSize);
		int					recvLen = 0,offset = 0,dataLen = 0;
		while(1){
			if((recvLen+maxTcpBufSize)>(int)recvBuf.Length()){
				if(!recvBuf.SetLength(recvLen+maxTcpBufSize)){
					Print(L"socket receive: can't alloc enough memory");
					handle.Close();
					connected = 0;
					OnClose(userParam);
					break;
				}
			}
			int rLen = handle.Receive(recvBuf.Handle()+recvLen,maxTcpBufSize);
			//Print(L"%d",rLen);
			if(rLen<=0){
				PrintLastError(WSAGetLastError());
				handle.Close();
				connected = 0;
				OnClose(userParam);
				break;
			}
			recvLen += rLen;

			while(1){
				if(recvLen<(offset+4)) break;
				if(dataLen==0){
					dataLen = *(int*)(recvBuf.Handle()+offset);
					//Print(L"recv datalen:%d",dataLen);
					offset += 4;
				}
				bool zip;
				if(dataLen<0){
					dataLen = dataLen&0x7fffffff;
					zip = 1;
				}else{
					zip = 0;
				}
				if(dataLen>(recvLen-offset)){
					break;
				}
				if(zip){
					Memory<char> buf;
					UINT len = DeCompress(recvBuf.Handle()+offset,dataLen,&buf);
					OnRecv(buf.Handle(),len,userParam);
				}else{
					OnRecv(recvBuf.Handle()+offset,dataLen,userParam);
				}
				offset += dataLen;
				dataLen = 0;
			}
			recvLen -= offset;
			recvBuf.Move(0,offset,recvLen);
			offset = 0;
		}
	}
	TCPTransfer::TCPTransfer(){
		connected = 0;
		userParam = 0;
		sendBuf.SetLength(0x2000);
		*(int*)sendBuf.Handle() = 0;
	}
	TCPTransfer::~TCPTransfer(){
		Close();
	}
	void TCPTransfer::Attach(SOCKET s,bool cn){
		connected = cn;
		handle.Attach(s);
	}
	SOCKET TCPTransfer::Detach(){
		connected = 0;
		return handle.Detach();
	}
	void TCPTransfer::Accept(SOCKET s){
		connected = 1;
		handle.Attach(s);
	}
	void TCPTransfer::Close(){
		handle.Close();
		connected = 0;
		recvThread.Wait();
		recvThread.Close();
	}
	void TCPTransfer::SetUserParam(void* param){
		userParam = param;
	}
	void TCPTransfer::connect(SOCKADDR sa,bool startRecv){
		connected = handle.Connect(sa);
		OnConnect(connected,userParam);
		if(!connected)
			PrintLastError(WSAGetLastError());
		recvThread.Close();
		if(connected){
			if(startRecv){
				StartRecv();
			}
		}
	}
	bool TCPTransfer::Connect(IP ip,WORD port,bool startRecv,bool asyn){
		SOCKADDR sa;
		sa.SetIp(ip);
		sa.SetPort(port);
		if(connected) return 0;
		if(handle.IsNull())
			handle.Create();
		if(asyn){
			return recvThread.Call<void>(this,&TCPTransfer::connect,sa,startRecv);
		}else{
			connect(sa,startRecv);
			return connected;
		}
	}
	bool TCPTransfer::StartRecv(){
		if(!connected) return 0;
		return recvThread.Call<void>(this,&TCPTransfer::onRecv);
	}
	void TCPTransfer::operator = (SOCKET s){
		handle.Attach(s);
	}
	bool TCPTransfer::AddSendData(const void* data,int len,UINT offset){
		if(len<0) return 0;
		int dataLen = *(int*)sendBuf.Handle();
		int sumLen = dataLen + len + sizeof(int);
		if(sumLen<=0) return 0;
		if(sumLen>(int)sendBuf.Length()){
			if(!sendBuf.SetLength(sumLen))
				return 0;
		}
		if(offset>(UINT)dataLen) offset = dataLen;
		sendBuf.Move(offset+sizeof(int)+len,offset+sizeof(int),dataLen-offset);
		sendBuf.CopyFrom((const char*)data,len,offset+sizeof(int));
		dataLen += len;
		*(int*)sendBuf.Handle() = dataLen;
		return 1;
	}
	int TCPTransfer::Send(bool zip){
		int dataLen = *(int*)sendBuf.Handle(),len;
		//Print(L"TCPTransfer::Send,dataLen:%d",dataLen);
		if(zip){
			UINT bufLen = GetMaxCompressLen(dataLen);
			Memory<char> buf(bufLen+4);
			bufLen = Compress(sendBuf.Handle()+4,dataLen,buf.Handle()+4,bufLen);
			*(int*)buf.Handle() = bufLen|0x80000000;
			len = handle.Send(buf.Handle(),bufLen+4);
		}else{
			len = handle.Send(sendBuf.Handle(),dataLen+sizeof(int));
		}
		*(int*)sendBuf.Handle() = 0;
		return len;
	}

	//UDPSender
	bool UDPSender::Create()
	{
		if(!_Handle.Create(IPPROTO_UDP,SOCK_DGRAM)) return 0;
		BOOL fBroadcast = TRUE;
		_Handle.SetSockOpt(SOL_SOCKET,SO_BROADCAST,(char*)&fBroadcast,sizeof(BOOL));
		return 1;
	}
	int UDPSender::SendTo(char* data,int dLen,ushort port,IP ip)
	{
		SOCKADDR sServ;
		sServ.SetIp(ip);
		sServ.SetPort(port);
		return _Handle.SendTo(data,dLen,0,sServ);
	}
	bool UDPSender::SetDest(ushort port, IP ip)
	{
		SOCKADDR sa;
		sa.SetPort(port);
		sa.SetIp(ip);
		return _Handle.Connect(sa);
	}
	int UDPSender::Send(char *data, int len)
	{
		return _Handle.Send(data,len);
	}
	//UDPReceiver
	void UDPReceiver::_Receive()
	{
		SOCKADDR sa;
		int size = sa.Size();
		Memory<char> buf(0x10000);
		while(1)
		{
			int rLen = _Handle.RecvFrom(buf.Handle(),0x10000,0,sa,&size);
			if(rLen<0)
			{
				_Handle.Close();
				break;
			}
			OnRecv(buf.Handle(),rLen,sa);
		}
	}
	UDPReceiver::~UDPReceiver()
	{
		_Handle.Close();
		_thread.Wait();
	}
	void UDPReceiver::Close()
	{
		_Handle.Close();
		_thread.Close();
	}
	bool UDPReceiver::Start(ushort port)
	{
		if(!_Handle.Create(IPPROTO_UDP,SOCK_DGRAM)) return 0;
		SOCKADDR sin;
		sin.SetPort(port);
		sin.SetIp(INADDR_ANY);
		if(!_Handle.Bind(sin))
		{
			_Handle.Close();
			Print(L"°ó¶¨Ê§°Ü£ºUDPReceiver::Tune£¨ushort port£©");
			return 0;
		}
		return _thread.Call<void>(this,&UDPReceiver::_Receive);
	}
	*/
}