#include "../stdafx.h"

#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库

#include "../../concise.h"

namespace cs{
	LPCWSTR HttpRequest::ContentTypeUrlEncoded = L"Content-Type: application/x-www-form-urlencoded";
	LPCWSTR HttpRequest::ContentTypeTextXml = L"Content-Type: text/xml";

	void HttpRequest::SetHttpsMode(bool https){
		if(https){
			Port = INTERNET_DEFAULT_HTTPS_PORT;
			_connectFlag |= INTERNET_FLAG_SECURE;
		}else{
			Port = INTERNET_DEFAULT_HTTP_PORT;
			_connectFlag |= INTERNET_FLAG_SECURE;
			_connectFlag -= INTERNET_FLAG_SECURE;
		}
	}
	bool HttpRequest::GetHttpsMode(){
		return (_connectFlag&INTERNET_FLAG_SECURE)==INTERNET_FLAG_SECURE;
	}
	bool HttpRequest::Create(){
		if(hInternet) return 1;
		hInternet = ::InternetOpen(Agent,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
		return hInternet!=NULL;
	}
	bool HttpRequest::Connect(){
		if(hConnect||hRequest) return 0;
		hConnect = ::InternetConnect(hInternet,Host,Port,NULL,NULL,INTERNET_SERVICE_HTTP,_connectFlag,0);
		if(hConnect==0){
			return 0;
		}
		hRequest = ::HttpOpenRequest(hConnect,Method,(LPCWSTR)Page,HTTP_VERSION,NULL,0,_connectFlag|Flags,0);
		if(!hRequest){
			InternetCloseHandle(hConnect);
			hConnect = NULL;
		}
		return hRequest!=0;
	}
	void HttpRequest::SetPostMode(){
		Method = L"POST";
	}
	void HttpRequest::SetGetMode(){
		Method = L"GET";
	}
	bool HttpRequest::SetHeader(LPCWSTR header){
		if(!hRequest) return 0;
		return 0!=HttpAddRequestHeaders(hRequest,header,-1,HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE); 
	}
	int HttpRequest::Request(const void* data,int len,Memory<char>& html,String* retHead){
		if(!hRequest) return -1;
		if(!::HttpSendRequest(hRequest,NULL,0,(LPVOID)data,len)) return -1;

		DWORD dwSize = 0;
		if(retHead){
			HttpQueryInfo(hRequest,HTTP_QUERY_RAW_HEADERS_CRLF, NULL, &dwSize, NULL);
			if(dwSize!=0){
				retHead->SetCubage(dwSize/2);
				if(HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)retHead->Handle(), &dwSize, NULL)){
					//安全起见，实际上数据已经是一个NULL结尾的字串。
					(*retHead)[dwSize/2-1] = 0;
					retHead->Realize();
				}
			}
		}

		int offset = 0;
		for(;;){
			char szData[1024];
			DWORD dwByteRead = 0;
			::InternetReadFile(hRequest,szData,sizeof(szData),&dwByteRead);
			if(!dwByteRead) break;
			if(html.Length()<dwByteRead+offset+1){
				if(!html.SetLength(dwByteRead+offset+1)){
					Print(L"html alloc failed");
					break;
				}
			}
			html.CopyFrom(szData,dwByteRead,offset);
			offset += dwByteRead;
		}
		if(offset!=0) html[offset] = 0;
		::InternetCloseHandle(hConnect);
		hConnect = 0;
		::InternetCloseHandle(hRequest);
		hRequest = 0;
		return offset;
	}
	bool HttpRequest::ParseUrl(LPCWSTR url){
		String str = url;
		if(WcsIncludeNoCase(url,L"https://")){
			SetHttpsMode(true);
			str.Tail(8);
		}else if(WcsIncludeNoCase(url,L"http://")){
			SetHttpsMode(false);
			str.Tail(7);
		}else return 0;
		int pos = str.Find('/');
		if(pos==-1){
			Host = str;
			return 1;
		}else{
			Host = L"";
			Host.CopyFrom(str,pos);
			str.Tail(pos);
		}
		Page = str;
		return 1;
	}
	HttpRequest::HttpRequest(){
		_connectFlag = 0;
		Flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
		Agent = L"jsuser http object 1.0";
		Method = L"GET";
		hInternet = 0;
		hConnect = 0;
		hRequest = 0;
		Port = INTERNET_DEFAULT_HTTP_PORT;
	}
	HttpRequest::~HttpRequest(){
		if(hInternet) ::InternetCloseHandle(hInternet);
		if(hConnect) ::InternetCloseHandle(hConnect);
		if(hRequest) ::InternetCloseHandle(hRequest);
	}
	void HttpRequest::Close(){
		if(hInternet){
			::InternetCloseHandle(hInternet);
			hInternet = NULL;
		}
		if(hConnect){
			::InternetCloseHandle(hConnect);
			hConnect = NULL;
		}
		if(hRequest){
			::InternetCloseHandle(hRequest);
			hRequest = NULL;
		}
	}
}