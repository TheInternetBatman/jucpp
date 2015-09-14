#pragma once
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Urlmon.lib")
namespace cs{
	//把字串处理成UTF8编码，的字串表示。
	CONCISE_API void EncodeURI(String& str,DWORD codePage = CP_UTF8);
	//把UTF8编码的字串还原。
	CONCISE_API void DecodeURI(String& str,DWORD codePage = CP_UTF8);
	//IP是网络地址的结构in_addr。
	typedef struct CONCISE_API IP : public _struct
	{
		union S_UN{
			struct { UCHAR s_b1,s_b2,s_b3,s_b4; } S_un_b;
			struct { USHORT s_w1,s_w2; } S_un_w;
			ULONG S_addr;
		}S_un;
		inline bool IsNull()//-1
		{
			return S_un.S_addr==INADDR_NONE;
		}
		inline IP()
		{
			S_un.S_addr = INADDR_NONE;
		}
		inline IP(const IP& ia)
		{
			S_un.S_addr = ia.S_un.S_addr;
		}
		inline IP(in_addr ia)
		{
			S_un.S_addr = ia.S_un.S_addr;
		}
		inline IP(ulong ip)
		{
			S_un.S_addr = ip;
		}
		inline IP(const char* ip)
		{
			S_un.S_addr = inet_addr(ip);
		}
		inline void operator = (const IP& ia)
		{
			S_un.S_addr = ia.S_un.S_addr;
		}
		inline void operator = (in_addr ia)
		{
			S_un.S_addr = ia.S_un.S_addr;
		}
		inline void operator = (ulong ip)
		{
			S_un.S_addr = ip;
		}
		inline void operator = (const char* ip)
		{
			S_un.S_addr = inet_addr(ip);
		}
		void operator = (const wchar_t* ip);
		inline IP(const wchar_t* ip)
		{
			*this = ip;
		}
		inline operator in_addr ()
		{
			return Handle();
		}
		inline in_addr Handle()
		{
			return *(in_addr*)this;
		}
		inline operator ulong ()
		{
			return S_un.S_addr;
		}
		inline ulong ToLong()
		{
			return S_un.S_addr;
		}
		inline operator char* ()
		{
			return inet_ntoa(Handle());
		}
		inline bool operator == (IP& ip)
		{
			return ip.S_un.S_addr==S_un.S_addr;
		}
		inline char* ToString()
		{
			return inet_ntoa(Handle());
		}
	}IP;
	/*sockaddr_in 表示一个网络通信结构
	struct sockaddr_in {
	short   sin_family;			地址类型，实际上几乎只有AF_INET一种被使用。
	u_short sin_port;			使用的端口，每个通信都要使用一个端口。
	struct  in_addr sin_addr;	是一个占用4个字节的IP地址。
	char    sin_zero[8];		未使用，置零。
	};*/
	//SOCKADDR结构集成sockeaddr_in结构的常用函数，它就是一个sockaddr_in结构。
	class CONCISE_API  SOCKADDR : public _struct
	{
		sockaddr_in _Handle;
	public:
		SOCKADDR()
		{
			RtlZeroMemory(this,sizeof(SOCKADDR));
			_Handle.sin_family = AF_INET;
		}
		//返回这个实例的地址指针。
		inline operator struct sockaddr_in* ()
		{
			return (struct sockaddr_in*)&_Handle;
		}
		//返回这个实例的地址指针。
		inline operator struct sockaddr* ()
		{
			return (struct sockaddr*)&_Handle;
		}
		//设置使用的端口。一般来说要做一个转换，两个自己高低位换一下。这种转换的存在是因为使用的标准不同造成的遗留问题。
		inline void SetPort(unsigned short port)
		{
			_Handle.sin_port = htons(port);
		}
		//返回端口号,同样也需要进行一个转换。
		inline unsigned short GetPort()
		{
			return ntohs(_Handle.sin_port);
		}
		//设置IP地址，字串表示，in_addr表示，整数表示。
		inline void SetIp(IP addr)
		{
			_Handle.sin_addr = addr;
		}
		inline IP GetIp()
		{
			return _Handle.sin_addr;
		}
		//设置地址家族，实际上使用最多的是AF_INET。
		inline void SetFamily(short family)
		{
			_Handle.sin_family = family;
		}
		//返回地址的家族类型。
		inline short Family()
		{
			return _Handle.sin_family;
		}
		//返回这个结构的大小，有的函数需要使用这个值。
		inline int Size()
		{
			return (int)sizeof(_Handle);
		}
	};
	/*
	typedef struct hostent {
	char FAR* h_name;				//主机的学名。
	char FAR  FAR** h_aliases;		//主机的别名列表（一个主机可以有多个别名）。
	short h_addrtype;				//主机的地址类型，AF_INET 或者 AF_INET6等。
	short h_length;					//主机的IP地址长度,一般是4,没有别的值
	char FAR  FAR** h_addr_list;	//主机的IP地址字串.
	}*/
	//Host表示一个主机结构，
	class CONCISE_API  Host : public _class
	{
		hostent* _Hostent;
		WORD _aliasesCount,_addrCount;
	public:
		Host();
		~Host();
		bool IsNull(){return _Hostent==0;}
		//获取本机的地址信息.
		bool Create();
		//通过整数形式的网络地址获取.
		bool Create(u_long addr);
		//通过主机名获取主机的信息，name既可以是一个域名网络地址:www.google.com(不要加http://，事实上这个函数只是访问DNS解析服务器，并不访问网络主机本身 ),
		//也可以是一个字串形式的IP地址:192.168.1.1,也可以是一个局域网主机的名字:server.
		bool Create(const char* name);
		//通过主机名获取主机的信息，name既可以是一个域名网络地址:www.google.com,也可以是一个字串形式的IP地址:192.168.1.1,也可以是一个主机的名字:server.
		bool Create(const wchar_t* name);
		//主机的学名
		char* GetOfficial(){return _Hostent->h_name;}
		//主机的别名,一个主机可能有多个别名
		char* GetAliases(int index = 0);
		//取得主机别名的个数.
		int GetAliasesCount(){return _aliasesCount;}
		//取得主机IP地址的个数.
		int GetIpCount(){return _addrCount;}
		//获取主机的IP地址,超出索引返回 0 .
		IP GetIp(int index = 0);
		//获取主机的IP地址类型,AF_INET 或者 AF_INET6等.
		int GetIpType(){return _Hostent->h_addrtype;}
		//获取主机的IP地址长度,一般是4,没有别的值.
		int GetIpLength(){return _Hostent->h_length;}
	};
	//封装 Win Sock 函数
	class CONCISE_API  Socket : public HandleType<SOCKET>
	{
	public:
		Socket();
		~Socket();
		Socket(SOCKET s);
		void operator = (SOCKET s){_Handle = s;}
		//实例绑定的是一个无效的SOCKET
		bool IsNull(){return INVALID_SOCKET==_Handle;}
		//实例绑定的是一个有效的SOCKET
		bool NotNull(){return INVALID_SOCKET!=_Handle;}
		//绑定一个有效的SOCKET,如果s=INVALID_SOCKET,返回0。
		bool Attach(SOCKET s);
		//解除已经绑定的SOCKET.
		SOCKET Detach();
		//生成一个SOCKET资源.
		bool Create(int protocol = IPPROTO_TCP,int type = SOCK_STREAM);
		//释放绑定的SOCKET资源.
		bool Close();


		inline bool Shutdown(int how = SD_BOTH){return shutdown(_Handle,how)==0;}
		//Accept
		SOCKET Accept(sockaddr* srv = 0);
		//监听，backlog是可以同时监听的客户端数量。
		inline bool Listen(int backlog = SOMAXCONN){return 0==listen(_Handle,backlog);}
		//绑定，一个通信结构。Bind用于监听Socket，这个Socket就不能用于Connect或者Send了。
		inline bool Bind(const sockaddr* srv){return 0==bind(_Handle,srv,sizeof(sockaddr_in));}


		//链接服务端
		inline bool Connect(const sockaddr* addr){return 0==connect(_Handle,addr,sizeof(sockaddr_in));}
		//发送数据，send函数把buf中的数据成功copy到s的发送缓冲的剩余空间里后它就返回了，但是此时这些数据并不一定马上被传到连接的另一端
		inline int Send(const char* buf,int len,int flags = 0){return send(_Handle,buf,len,flags);}
		//接收数据，接收成功返回接收字节数，正常关闭返回 0 ，否则 返回 -1 。
		inline int Receive(char* buf,int len,int flags = 0){return recv(_Handle,buf,len,flags);}
		//无连接传输发送数据，如果len==0，这个函数会让RecvFrom返回一次。
		inline int SendTo(const char* buf,int len,int flags,const struct sockaddr* to,int tolen = sizeof(sockaddr_in)){return sendto(_Handle,buf,len,flags,to,tolen);}
		//无连接传输接受数据
		inline int RecvFrom(char* buf,int len,int flags,struct sockaddr* from,int* fromlen){return recvfrom(_Handle,buf,len,flags,from,fromlen);}
		//设置属性。
		inline int SetSockOpt(int level,int opt,const char* val,int len){return setsockopt(_Handle,level,opt,val,len);}
		inline int GetSockOpt(int level,int opt,char* val,int* len){return getsockopt(_Handle,level,opt,val,len);}
		//取得一个已连接 Socket 对方的 SOCKADDR 信息。
		inline bool GetPeerAddr(SOCKADDR& si){
			int len = sizeof(si);
			return getpeername(_Handle,si,&len)==0;
		}
		//取得本地 Scoket 的地址信息。
		inline bool GetSockAddr(SOCKADDR& si){
			int len = sizeof(si);
			return getsockname(_Handle,si,&len)==0;
		}
	};
	//用于监听，建立来着客户端和服务器的连接。即使这个监听关闭，已经建立的连接仍然可用。监听 Scoket 只负责创建连接，但是它本身即不连接也不传送数据。
	class CONCISE_API  TCPServer : public _class
	{
	protected:
		Socket	_Handle;
		Thread _thread;

		void _Accept();
	public:
		//当客户端有链接请求时做出响应的回调函数。
		Function<void,SOCKET> OnAccept;			

		~TCPServer();
		//返回绑定的 Socket 指针。
		Socket* GetSocket(){return &_Handle;}
		//同步启动监听服务，直到Close被调用，这个函数不会返回。如果要接收任何IP的链接，就设置ip为INADDR_ANY。
		HRESULT Startup(ushort port,IP ip = INADDR_ANY);
		//通过一个新线程异步启动监听服务，如果要接收任何IP的链接，就设置ip为INADDR_ANY。
		HRESULT StartupAsyn(ushort port,IP ip = INADDR_ANY);
		//关闭服务端。这个函数并不清除有它建立的链接。
		void Close();
	};
	//TCPTransfer发送的数据包含4个字节的数据头，必须用TCPTransfer来接收数据
	/*class CONCISE_API  TCPTransfer : public _class{
	protected:
		Socket				_handle;
		Memory<char>		_sendBuf;
		void*				_userParam;
		Thread				_recvThread,_sendThread;
		bool				_connected;
		Event				_sendBlock;

		void _connect(SOCKADDR sa,bool startRecv,bool asyn);
		void _send();
	public:
		Function<void,void*> OnSend;
		Function<void,void*,int,void*> OnRecv;//call back function of recv data
		Function<void,void*> OnClose;			//call back function be called when a socket be closed and have startup a recv thread,this function be call in a Asynchronous thread. 
		Function<void,bool,void*> OnConnect;

		TCPTransfer();
		~TCPTransfer();
		//返回绑定的 Socket 指针。
		Socket* GetSocket(){return &_handle;}
		//attach a socket,cn is indicate this socket whether connected.
		void Attach(SOCKET s,bool cn);
		//detach a socket from this instance
		SOCKET Detach();
		//accept a socket from server accept call back function,this is equal to Attach(s,true).
		void Accept(SOCKET s);
		//close this transfer,this don't call in any call back function(OnClose,OnRecv,OnConnect).
		void Close();
		//在此实例的任何回调函数中如果调用Close函数，会形成死锁，如果这种情况下需要关闭此Socket，调用CloseSocket仅仅关闭Socket句柄。
		void CloseSocket(){_handle.Close();}
		//set user data,the last param of OnClose and OnRecv.
		void SetUserParam(void* param);
		//connect a server,startRecv indicate whether auto starup recv thread when connect is success.
		bool Connect(IP ip,WORD port,bool startRecv = 0);
		//connect a server,startRecv indicate whether auto starup recv thread when connect is success.
		bool ConnectAsyn(IP ip,WORD port,bool startRecv = 0);
		//start recv thread,some socket is only use for send data,so not nessary call this function.
		bool StartRecv();
		//start recv thread,some socket is only use for send data,so not nessary call this function.
		bool StartRecvAsyn();
		//equal with Attatch(s,false).
		void operator = (SOCKET s);
		//add data to send buffer
		bool AddSendData(const void* data,int len,UINT offset = -1); 
		//send data in send buffer,if no data in buffer,remote point always receive a call of datalen is zero.
		//zip is indicate whether compress data with zip before send data.
		//yes or no this function success,sendbuf always be cleared.
		int Send(bool zip = 0);
		//启动异步发送线程。
		bool StartSendAsyn();
	};*/

	//广播方式发送数据，或者无连接发送数据，他也要使用一个端口，但是这个端口可以和另一个监听共用一个端口（如果两个监听共用一个端口，第二个调用会失败）。
	class CONCISE_API  UDPSender : public _class
	{
	protected:
		Socket _Handle;
	public:
		~UDPSender(){_Handle.Close();}
		bool Close(){return _Handle.Close();}
		//生成广播组件。
		bool Create();
		//发送广播数据，ip是指定发送的IP，如果设为INADDR_BROADCAST，则对所有主机广播，port是要使用的端口。
		int SendTo(char* data,int dLen,ushort port,IP ip = INADDR_BROADCAST);
		//这个函数仅仅是把本地 UDP Socket 的目标地址和端口设置为指定的值，之后可以调用Send函数发送数据，不
		//用每次都指定目标地址。
		bool SetDest(ushort port,IP ip = INADDR_BROADCAST);
		//必须已经调用SetDest函数，才可以调用这个函数。
		int Send(char* data,int len);
		//返回socket
		Socket* GetSocket(){return &_Handle;}
	};
	//接收广播数据，或者无连接数据，端口可以使用已经使用的端口。有连接和无连接是两个不同的体系，它们的端口也是各自使用的。
	class CONCISE_API  UDPReceiver : public _class
	{
	protected:
		Socket _Handle;
		Thread _thread;

		void _Receive();
	public:
		//接收回调函数，参数:数据，数据长度，包含客户端信息的sockaddr_in指针。
		Function<void,char*,int,SOCKADDR&> OnRecv;

		~UDPReceiver();
		//开始接收指定端口的广播。
		bool Start(ushort port);
		//异步开始接收指定端口的广播。
		bool StartAsyn(ushort port);
		//关闭接收。
		void Close();
		//返回socket
		Socket* GetSocket(){return &_Handle;}
	};
	//用于监听，建立来着客户端和服务器的连接。即使这个监听关闭，已经建立的连接仍然可用。监听 Scoket 只负责创建连接，但是它本身即不连接也不传送数据。
	/*class CONCISE_API  TCPServer : public _class{
	protected:
		Socket	_Handle;
		Thread _thread;
		void _Accept();
	public:
		//当客户端有链接请求时做出响应的回调函数。
		Function<void,SOCKET> OnAccept;			

		~TCPServer();
		//返回绑定的 Socket 指针。
		Socket* GetSocket(){return &_Handle;}
		//生成服务端监听功能实例，如果要接收任何IP的链接，就设置ip为INADDR_ANY。
		bool Starup(ushort port,IP ip = INADDR_ANY);
		//关闭服务端。这个函数并不清除有它建立的链接，直到这个实例销毁，它的所有连接也会销毁。
		void Close();
	};
	//TCPTransfer发送的数据包含4个字节的数据头，必须用TCPTransfer来接收数据
	class CONCISE_API  TCPTransfer : public _class{
	protected:
		Socket		handle;
		Memory<char>	sendBuf;
		void*				userParam;
		Thread		recvThread;
		bool				connected;

		void onRecv();
		void connect(SOCKADDR sa,bool startRecv);
	public:
		Function<void,void*,int,void*> OnRecv;//call back function of recv data
		Function<void,void*> OnClose;			//call back function be called when a socket be closed and have startup a recv thread,this function be call in a Asynchronous thread. 
		Function<void,bool,void*> OnConnect;
		TCPTransfer();
		~TCPTransfer();
		//返回绑定的 Socket 指针。
		Socket* GetSocket(){return &handle;}
		//attach a socket,cn is indicate this socket whether connected.
		void Attach(SOCKET s,bool cn);
		//detach a socket from this instance
		SOCKET Detach();
		//accept a socket from server accept call back function,this is equal to Attach(s,true).
		void Accept(SOCKET s);
		//close this transfer,this don't call in any call back function(OnClose,OnRecv,OnConnect).
		void Close();
		//在此实例的任何回调函数中如果调用Close函数，会形成死锁，如果这种情况下需要关闭此Socket，调用CloseSocket仅仅关闭Socket句柄。
		void CloseSocket(){handle.Close();}
		//set user data,the last param of OnClose and OnRecv.
		void SetUserParam(void* param);
		//connect a server,startRecv indicate whether auto starup recv thread when connect is success.
		bool Connect(IP ip,WORD port,bool startRecv = 0,bool asyn = 0);
		//start recv thread,some socket is only use for send data,so not nessary call this function.
		bool StartRecv();
		//equal with Attatch(s,false).
		void operator = (SOCKET s);
		//add data to send buffer
		bool AddSendData(const void* data,int len,UINT offset = -1); 
		//send data in send buffer,if no data in buffer,remote point always receive a call of datalen is zero.
		//zip is indicate whether compress data with zip before send data.
		//yes or no this function success,sendbuf always be cleared.
		int Send(bool zip = 0);
	};

	//广播方式发送数据，或者无连接发送数据，他也要使用一个端口，但是这个端口可以和另一个监听共用一个端口（如果两个监听共用一个端口，第二个调用会失败）。
	class CONCISE_API  UDPSender : public _class
	{
	protected:
		Socket _Handle;
	public:
		~UDPSender(){_Handle.Close();}
		bool Close(){return _Handle.Close();}
		//生成广播组件。
		bool Create();
		//发送广播数据，ip是指定发送的IP，如果设为INADDR_BROADCAST，则对所有主机广播，port是要使用的端口。
		int SendTo(char* data,int dLen,ushort port,IP ip = INADDR_BROADCAST);
		//这个函数仅仅是把本地 UDP Socket 的目标地址和端口设置为指定的值，之后可以调用Send函数发送数据，不
		//用每次都指定目标地址。
		bool SetDest(ushort port,IP ip = INADDR_BROADCAST);
		//必须已经调用SetDest函数，才可以调用这个函数。
		int Send(char* data,int len);
		//返回socket
		Socket* GetSocket(){return &_Handle;}
	};
	//接收广播数据，或者无连接数据，端口可以使用已经使用的端口。有连接和无连接是两个不同的体系，它们的端口也是各自使用的。
	class CONCISE_API  UDPReceiver : public _class
	{
	protected:
		Socket _Handle;
		Thread _thread;
		void _Receive();
	public:
		//接收回调函数，参数:数据，数据长度，包含客户端信息的sockaddr_in指针。
		Function<void,char*,int,SOCKADDR&> OnRecv;

		~UDPReceiver();
		//开始接收指定端口的广播。
		bool Start(ushort port);
		//关闭接收。
		void Close();
		//返回socket
		Socket* GetSocket(){return &_Handle;}
	};*/
}