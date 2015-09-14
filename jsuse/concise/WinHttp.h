#pragma once
#include <Iphlpapi.h>
#include <WinInet.h>
namespace cs{
	class HttpRequest : public _class{
	protected:
		HINTERNET hInternet,hConnect,hRequest;
		DWORD _connectFlag;
	public:
		static LPCWSTR ContentTypeUrlEncoded,//要想让$_POST等接收到数据，需要这个头
			ContentTypeTextXml;//上传数据使用二进制方式接收，$GLOBALS['HTTP_RAW_POST_DATA']，设置此头
		DWORD	Flags;
		WORD	Port;
		String Agent,
			Host,
			Page,	//
			Method;	//"POST" OR "GET"
		//这个函数会改变端口为http或https的默认端口，如果使用其它端口，需要重新设置 Port 的值。
		void SetHttpsMode(bool https);
		bool GetHttpsMode();
		void SetPostMode();
		void SetGetMode();
		bool Create();
		//如果上一个请求还没有完成，这个函数会失败，强制结束调用 Close 函数，只有 Request 函数返回后，才可以重新连接，发起请求。
		bool Connect();
		//如果头是新的，可以一次添加多个头，用回车换行来分隔，如果要改变头，一次只能设置一个。
		bool SetHeader(LPCWSTR header);
		//发送请求，并且返回下载的数据，如果失败返回值是 -1，因为网页存在下载数据长度为 0 的情况，此时函数并没有失败.
		//不要用于请求大数据，因为数据是一次取回的，缓冲区大小会有限制。
		int Request(const void* data,int len,Memory<char>& html,String* retHead = 0);
		//分析一个完整的链接，自动获取http获取https模式，自动获取host，自动获取页面，url必须是一个以http或https开头的完整的链接地址。
		bool ParseUrl(LPCWSTR url);
		void Close();
		HttpRequest();
		~HttpRequest();
	};
}