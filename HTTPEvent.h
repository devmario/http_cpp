/**
 @file HTTPEvent.h
 @brief HTTP요청을 보낼수 있고 받을수 있는 이벤트핸들러
 @author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 @version 1.0
 @section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */
#ifndef HTTPEvent_h
#define HTTPEvent_h

#include "HTTPConfig.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HTTPClient;

/**
 @class HTTPEvent
 @brief 이클래스는 HTTP요청을 보낼수 있고 받을수 있는 이벤트핸들러입니다.
 
 이 클래스를 상속받아 HTTPEvent::Error , HTTPEvent::Receive , HTTPEvent::Progress 를 구현하고
 HTTPEvent::Send 메서드를 사용하면 요청 및 응답처리가 가능합니다.
 */
class HTTPEvent {
private:
	/**
	 @brief 요청을 처리해야할 리스트를 담고있는 std::list 입니다.
	 
	 HTTPEvent 는 여러 요청들을 태그를 달아 보낼수 있습니다.응답을 받을시에는 그 태그를 이용하여 어떤요청에 대한 응답인지를 확인하게 됩니다.
	 그러므로 HTTPEvent 는 그 요청들을 리스트형태로 가지고 있어야 합니다.
	 이 멤버는 여러요청(응답처리를 해야하는)들이 담긴 리스트입니다.
	 */
	std::list<HTTPClient*> client_list;
	
	/**
	 @brief 요청에 대한 태그값으로 해당 요청을 찾는 함수입니다.
	 @return 지정된 태그와 매치하는 요청
	 
	 HTTPEvent::Pause , HTTPEvent::Resume , HTTPEvent::Stop 에서는 HTTPEvent::Send 시에 지정한 태그값으로 일시정지, 활성화, 정지가 가능합니다.
	 이 함수는 지정된 태그에 매치하는 요청을 검색합니다. 같은 태그가 여러개 있을경우 가장먼저 발송한 요청을 반환합니다.
	 */
	HTTPClient* FindTagClient(const std::string _tag);
	
public:
	~HTTPEvent();
	
	//접근하지 마세요
	void AddClient(HTTPClient* _client);
	void RemoveClient(HTTPClient* _client);
	
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
