/*!
 \file HTTPEvent.h
 \brief HTTP요청을 보낼수 있고 받을수 있는 이벤트핸들러
 \author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 \version 1.0
 \section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */
#ifndef HTTPEvent_h
#define HTTPEvent_h

#include "HTTPConfig.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HTTPClient;

/*!
 \class HTTPEvent
 \brief 이클래스는 HTTP요청을 보낼수 있고 받을수 있는 이벤트핸들러입니다.
 
 이 클래스를 상속받아 HTTPEvent::Error , HTTPEvent::Receive , HTTPEvent::Progress 를 구현하고
 HTTPEvent::Send 메서드를 사용하면 요청 및 응답처리가 가능합니다.
 */
class HTTPEvent {
private:
	/*!
	 \brief 요청을 처리해야할 리스트를 담고있는 std::list 입니다.
	 
	 HTTPEvent 는 여러 요청들을 태그를 달아 보낼수 있습니다.응답을 받을시에는 그 태그를 이용하여 어떤요청에 대한 응답인지를 확인하게 됩니다.
	 그러므로 HTTPEvent 는 그 요청들을 리스트형태로 가지고 있어야 합니다.
	 이 멤버는 여러요청(응답처리를 해야하는)들이 담긴 리스트입니다.
	 */
	std::list<HTTPClient*> client_list;
	
	/*!
	 \brief 요청에 대한 태그값으로 해당 요청을 찾는 함수입니다.
	 \return 지정된 태그와 매치하는 요청
	 
	 \link #Pause HTTPEvent::Pause \endlink , \link #Resume HTTPEvent::Resume \endlink , \link #Stop HTTPEvent::Stop \endlink 에서는 HTTPEvent::Send 시에 지정한 태그값으로 일시정지, 활성화, 정지가 가능합니다.
	 이 함수는 지정된 태그에 매치하는 요청을 검색합니다. 같은 태그가 여러개 있을경우 가장먼저 발송한 요청을 반환합니다.
	 */
	HTTPClient* FindTagClient(const std::string _tag);
	
	/*!
	 \brief 요청,응답 리스트 등록하기
	 
	 이 함수는 내부적으로 사용됩니다.실제로 개발자에게 이함수가 필요할 일은 없습니다.
	 
	 HTTPClient 는 하나의 \link #Send HTTPEvent::Send \endlink 에서 발생하는 인스턴스 입니다.
	 HTTPClient 는 생성시 태그와 함께 HTTPManager 에게 등록되고 요청에 대한 응답을 처리합니다.
	 HTTPManager 에서는 모든 HTTPClient 가 등록되며,
	 HTTPEvent 에서는 해당 HTTPEvent 에서 발생한 HTTPClient 리스트만을 가지고 있습니다.
	 이때 HTTPEvent 에 해당 HTTPClient 를 등록하는 함수입니다.
	 */
	void AddClient(HTTPClient* _client);
	
	/*!
	 \brief 요청,응답 리스트에서 삭제하기
	 
	 이 함수는 내부적으로 사용됩니다.실제로 개발자에게 이함수가 필요할 일은 없습니다.
	 
	 이함수는 HTTPClient 가 더이상 필요하지않을때(더이상 이벤트를 발생할 필요할 필요가 없을때), 즉 HTTPClient 가 소멸될때
	 호출됩니다.
	 */
	void RemoveClient(HTTPClient* _client);
	
public:
	/*!
	 \brief 생성자
	 
	 HTTPEvent 는 생성시에 아무일도 하지않습니다.
	 */
	HTTPEvent() {
		
	};
	
	/*!
	 \brief 소멸자
	 
	 HTTPEvent 는 소멸될때 응답대기중인 모든 연결을 끊습니다.
	 */
	~HTTPEvent();
	
	/*!
	 \brief 해당 태그로 자정된 요청을보냅니다.(연결 만들기)
	 \param _tag 보낼연결에 지정할 태그값 입니다.
	 \param _request 연결을 시작할 요청데이터 입니다.
	 
	 이함수는 해당 태그로 지정된 요청을 보냅니다.
	 이후 HTTPManager::Update 가 호출될때 데이터가 오고가며, 응답 콜백들이 작동됩니다.
	 이함수를 이용하여 하나의 HTTPEvent 에서 여러개의 요청을 할수 있으며 응답콜백들이 작동될때 그 여러개요청의 구분은 태그를 사용하여 판단합니다.
	 \par Example
	 다음은 HTTPEvent의 Send를 이용하여 구글로 요청을 보내는 예제입니다.
	 \include send.cpp
	 */
	void Send(const std::string _tag, 
			  const HTTPRequest _request);
	
	/*!
	 \brief 해당 태그로 지정된 연결을 일시정지정지 합니다.
	 \param _tag 중지할 연결의 태그입니다.
	 \return 일시중지가 성공적으로 수행되면 true를 반환하며, 이미 일시중지 상태이면 false를 반환합니다.
	 
	 \par Example
	 다음의 예제는 이미지를 다운받고 다운이 다되면 다시다운 받습니다.
	 또한 키 p를 누르면 다운받다가 일시정지되며 r을 누르면 다시 다운받습니다.
	 e를 누르면 프로그램이 종료됩니다.
	 Mac OSX 시스템에서 작성되었습니다.
	 \include pause_resume.cpp
	 */
	bool Pause(const std::string _tag);
	
	/*!
	 \brief 해당 태그로 지정된 연결을 활성화 합니다.
	 \param _tag 활성화할 연결의 태그입니다.
	 \return 활성화가 성공적으로 수행되면 true를 반환하며, 이미 활성화 상태이면 false를 반환합니다.
	 
	 \par Example
	 다음의 예제는 이미지를 다운받고 다운이 다되면 다시다운 받습니다.
	 또한 키 p를 누르면 다운받다가 일시정지되며 r을 누르면 다시 다운받습니다.
	 e를 누르면 프로그램이 종료됩니다.
	 Mac OSX 시스템에서 작성되었습니다.
	 \include pause_resume.cpp
	 */
	bool Resume(const std::string _tag);
	
	/*!
	 \brief 해당 태그로 지정된 연결을 제거 합니다.
	 \param _tag 제거할 연결의 태그입니다.
	 \return 제거가 성공적으로 수행되면 true를 반환하며, 찾을수 없는 연결이면 false를 반환합니다.
	 
	 \par Example
	 다음의 예제는 이미지를 다운받고 다운이 다되면 다시다운 받습니다.
	 또한 키 s를 누르면 다운받다가 연결을 제거하며
	 e를 누르면 프로그램이 종료됩니다.
	 Mac OSX 시스템에서 작성되었습니다.
	 \include stop.cpp
	 */
	bool Stop(const std::string _tag);
	
	/*!
	 \brief 이 HTTPEvent 에서 발생한 모든 연결을 제거합니다.
	 
	 하나의 HTTPEvent 에는 여러개의 연결을 생성할수 있습니다.
	 이함수를 이용하면 모든 연결이 제거됩니다.
	 
	 \par Example
	 다음의 예제는 이미지 10개를 다운받습니다.
	 소문자 c누르면 모든 연결이 제거되며, e를 누르면 프로그램이 종료합니다.
	 Mac OSX 시스템에서 작성되었습니다.
	 \include clear.cpp
	 */
	void Clean();
	
	/*!
	 \brief 응답받기(이후 연결은 제거됩니다.) 추상화 함수입니다.
	 \param _tag 어떤요청에 대한 응답인지를 구분하기위한 태그입니다.
	 \param _response 응답에 대해서 세부정보및 내용들을 접근할수 있는 인자입니다.
	 
	 이함수는 HTTPEvent::Send 가 에대한 처리(응답받기)가 성공적으로 완료되면 호출됩니다.
	 이함수가 호출된 후에는 연결이 삭제됩니다.( HTTPEvent 가 삭제되는 것이 아닌 HTTPEvent::Send 로 생성된 내부 연결)
	 
	 \par Example
	 다음의 예제는 네이버의 html을 출력합니다.
	 출력이이 끝나면 프로그램이 종료됩니다.
	 Mac OSX 시스템에서 작성되었습니다.
	 \include recieve.cpp
	 */
	virtual void Receive(const std::string _tag, 
						 HTTPResponse _response) = 0;
	
	/*!
	 \brief 응답 에러(이후 연결은 제거 됩니다.) 추상화 함수입니다.
	 \param _tag 어떤요청에 대한 응답인지를 구분하기위한 태그입니다.
	 \param _error_code \ref CURLcode에 대한링크 http://curl.haxx.se/libcurl/c/libcurl-errors.html
	 
	 이함수는 HTTPEvent::Send 에서 생성된 연결에 문제가 생겼을때 호출되며 에러코드를 넘겨줍니다.
	 이함수가 호출된 후에는 연결이 삭제됩니다.( HTTPEvent 가 삭제되는 것이 아닌 HTTPEvent::Send 로 생성된 내부 연결)
	 
	 \par Example
	 다음의 예제는 존재하지않는 주소로 요청을 하며, 에러코드를 출력후 프로그램이 종료됩니다.
	 Mac OSX 시스템에서 작성되었습니다.
	 \include error.cpp
	 */
	virtual void Error(const std::string _tag, 
					   CURLcode _error_code) = 0;
	
	/*!
	 \brief 응답받거나 요청보낸 데이터의 진행률을 가져옵니다.
	 \param _tag 어떤요청에 대한 응답인지를 구분하기위한 태그입니다.
	 \param _download_total 다운받아야할 전체 컨텐츠 길이
	 \param _download_now 다운받은 컨텐츠 길이
	 \param _upload_total 업로드해야할 전체 컨텐츠 길이
	 \param _upload_now 업로드한 컨텐츠 길이
	 
	 이함수는 HTTPEvent 에서 발생되는 연결의 진행률을 표시합니다. 연결후 서버로부터 스트림을 읽어들일때 마다 호출됩니다. 
	 
	 \par Example
	 다음의 예제는 이미지를 1개 다운받습니다.
	 다운받으면서 다운진행률을 표시하고 모두 다운이 끝나면 프로그램이 종료됩니다.
	 Mac OSX 시스템에서 작성되었습니다.
	 \include progress.cpp
	 */
	virtual void Progress(const std::string _tag,
						  double _download_total, 
						  double _download_now, 
						  double _upload_total, 
						  double _upload_now) = 0;
	
	/*!
	 \brief 내부적인 연결 유닛인 HTTPClient 는 HTTPEvent 와 friend 입니다.
	 */
	friend class HTTPClient;
};

#endif
