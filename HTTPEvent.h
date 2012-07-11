#ifndef HTTPEvent_h
#define HTTPEvent_h

#include "HTTPConfig.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HTTPClient;

class HTTPEvent {
private:
	std::list<HTTPClient*> client_list;
	HTTPClient* FindTagClient(const std::string _tag);
	
public:
	~HTTPEvent();
	
	//접근하지 마세요
	void _AddClient(HTTPClient* _client);
	void _RemoveClient(HTTPClient* _client);
	
	//요청보내기(연결 만들기)
	void Send(const std::string _tag, 
			  const HTTPRequest _request);
	//연결 일시정지
	bool Pause(const std::string _tag);
	//연결 활성화
	bool Resume(const std::string _tag);
	//연결 제거
	bool Stop(const std::string _tag);
	//모든연결 제거
	void Clean();
	
	//응답받기(이후 연결은 제거됩니다.)
	virtual void Receive(const std::string _tag, 
						 HTTPResponse _response) = 0;
	
	//응답 에러(이후 연결은 제거 됩니다.)
	//CURLcode에 대한링크
	//http://curl.haxx.se/libcurl/c/libcurl-errors.html
	virtual void Error(const std::string _tag, 
					   CURLcode _error_code) = 0;
	
	//응답받거나 요청보낸 데이터의 진행률
	virtual void Progress(const std::string _tag,
						  double _download_total, 
						  double _download_now, 
						  double _upload_total, 
						  double _upload_now) = 0;
};

#endif
