/**
 @file HTTPClient.h
 @brief 요청 및 응답을 처리하는 private 클래스
 @author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 @version 1.0
 @section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */

#ifndef HTTPClient_h
#define HTTPClient_h

#include "HTTPConfig.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HTTPEvent.h"

/**
 @class HTTPClient
 @brief 내부적인 Request + Response 유닛이며 실제로 HTTP_cpp 를 이용하는 개발자가 이클래스를 사용할일은 없습니다.
 
 HTTPClient 는 libcurl과 파일스트림을 이용하여 요청에 대한 응답처리를 합니다.
 
 또한 캐싱을 할건지 말건지에대한 판단또한 합니다.
 
 HTTP_cpp 에서는 캐쉬의 판단에 있어서 Expires와 Last-Modified만 지원합니다.
 
 @ref Expires,Last-Modified에 대한 참고링크 http://mytory.co.kr/archives/1232
 */
class HTTPClient {
private:
	
	/**
	 @brief 보내려는 요청값과 같은 이전요청(같은 URL)이 있나를 검색하는 sqlite콜백
	 
	 값을 찾았을 경우 HTTPClient 의 멤버변수인 db_id , db_expires , db_last_modified 를 설정한다.
	 
	 @ref 이콜백에 대한 참고링크 http://six605.tistory.com/300
	 */
	static int SQLMatchURLCallback(void* _reference, 
								   int _field_length, 
								   char** _field, 
								   char** _field_name);
	
	/**
	 @brief 요청에 대한 응답의 바디를 저장하는 콜백함수
	 
	 libcurl은 이콜백함수를 이용하여 std::ofstream 형태의 멤버변수인 body 에 요청에대한 응답의 바디를 저장합니다.
	 
	 @ref 이 콜백에 대한 참고링크 http://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
	 */
	static size_t ReadBody(char* _stream, 
						   size_t _size, 
						   size_t _count, 
						   void* _pointer);
	
	/**
	 @brief 요청에 대한 응답의 헤더를 저장하는 콜백함수
	 
	 libcurl은 이콜백함수를 이용하여 std::ofstream 형태의 멤버변수인 header 에 요청에대한 응답의 헤더를 저장합니다.
	 
	 @ref 이 콜백에 대한 참고링크 http://blog.naver.com/PostView.nhn?blogId=ycw002&logNo=20062219296
	 */
	static size_t ReadHeader(char* _stream, 
							 size_t _size, 
							 size_t _count, 
							 void* _pointer);
	
	/**
	 @brief 요청 또는 응답의 진행상태를 가져오는 콜백함수
	 
	 이함수는 2가지 일을합니다.
	 
	 첫번째로는 libcurl은 이콜백함수를 이용하여 요청의 전송상태와 응답의 전송상태를 확인합니다.이 함수는 내부적으로 HTTPEvent::Progress 에 값이 전달됩니다.
	 
	 두번째로는 HTTPClient 가 일시중지 상태인지 연결상태인지를 판단해 libcurl에 알맞게 작동되도록 libcurl의 함수인 curl_easy_pause를 실행합니다.(@ref curl_easy_pause에 대한 링크: http://curl.haxx.se/libcurl/c/curl_easy_pause.html )
	 
	 @ref 이 콜백에 대한 참고링크 http://stackoverflow.com/questions/1637587/c-libcurl-console-progress-bar
	 */
	static int Progress(void* _client_pointer, 
						double _download_total, 
						double _download_now, 
						double _upload_total, 
						double _upload_now);
	
	/**
	 @brief 캐쉬의 타입을 저장하는 멤버
	 
	 HTTPResponse::CacheType_None : 요청시 보낼 URL이 처음 요청이거나, Expires가 만료되었거나
	 
	 HTTPResponse::CacheType_Last_Modified : 요청시 보낼 URL이 이미 보냈었던 요청이고, Expires가 만료되었을때
	 
	 HTTPResponse::CacheType_Expires : 요청시 보낼 URL이 이미 보냈었던 요청이고, Expires가 만료되지 않았을때
	 */
	HTTPResponse::CacheType cache_type;
	
	/**
	 @brief sqlite 데이터베이스에서의 유니크아이디
	 
	 캐쉬파일의 파일이름으로도 사용된다.
	 */
	std::string db_id;
	
	/**
	 @brief sqlite 데이터베이스에서의 이전요청(같은 URL)에대한 응답헤더에 담긴 Expires를 time_t 스트럭트값으로 변경한 값
	 
	 이 값은 인스턴스 초기화때 설정됩니다.
	 이기간이 만료되기 전까지는 요청도 보내지 않고(서버에서 확인하지 않고) 바로 캐쉬파일을 이용하여 응답을 임의로 발생시킵니다.
	 */
	time_t db_expires;
	
	/**
	 @brief sqlite 데이터베이스에서의 이전요청(같은 URL)에대한 응답헤더에 담긴 Last-Modified를 time_t 스트럭트값으로 변경한 값
	 
	 이 값은 인스턴스 초기화때 설정됩니다.
	 Expires가 만료되었거나 Expires가 존재하지 않는경우 캐싱판단에 이 값이 사용됩니다. 
	 요청에대한 서버의 응답을 확인하는중에 헤더를 파싱하고 파싱된값중에 Last-Modified가 이전에 받은 응답(sqlite 데이터베이스에저장)인 이 멤버변수와 비교과정을 거쳐 응답바디를 full response할 것인지 캐쉬파일을 사용할것인지 결정합니다.
	 */
	time_t db_last_modified;
	
	/**
	 @brief 이 인스턴스가 처리중인 응답에대한 헤더의 Expires값을 time_t 구조체로 변경한값
	 
	 이 값은 요청에대한 응답을처리할때(헤더를 읽어들일때) 설정됩니다.
	 이값은 이후 다음번 캐싱에대한 판단을 위해 sqlite 데이터베이스에 저장됩니다.
	 다음요청(같은 URL)부터는 HTTPClient 의 멤버변수인 db_expires 로 읽혀지고 캐싱판단에 사용됩니다.
	 */
	time_t expires;
	
	/**
	 @brief 이 인스턴스가 처리중인 응답에대한 헤더의 Last-Modified값을 time_t 구조체로 변경한값
	 
	 이 값은 요청에대한 응답을처리할때(헤더를 읽어들일때) 설정됩니다.
	 이값은 이후 다음번 캐싱에대한 판단을 위해 sqlite 데이터베이스에 저장됩니다.
	 다음요청(같은 URL)부터는 HTTPClient 의 멤버변수인 db_last_modified 로 읽혀지고 캐싱판단에 사용됩니다.
	 */
	time_t last_modified;
	
	/**
	 @brief 요청처리에대한 일시정지 FLAG
	 
	 HTTP_cpp 는 요청에대한 응답처리를 일시정지할수 있습니다.
	 그와 반대로 일시정지 상태를 다시 응답처리상태로 활성화 할 수도 있습니다.
	 그에대한 FLAG값입니다.
	 */
	bool paused;
	
	/**
	 @brief 요청에대한 TAG값
	 
	 이 값은 요청에대한 응답이 개발자에게 갔을때 그 응답이 어떤요청에 대한 응답인지 쉽게 판단할수 있도록 하게하기위한 std::string 값 태그입니다.
	 이값은 요청에대한 응답이 처리되고 난뒤에 HTTPEvent::Receive , HTTPEvent::Error , HTTPEvent::Progress 등으로 전달됩니다.
	 */
	std::string tag;
	
	/**
	 @brief 요청을 처리할수 있는 curl 인스턴스
	 
	 HTTP_cpp 는 내부적으로 요청처리에 libcurl을 사용합니다.
	 멤버변수 curl 은 이를위한 curl 기본 인스턴스 입니다.
	 */
	CURL* curl;
	
	/**
	 @brief 요청헤더를 만드는 curl 인스턴스
	 
	 HTTP_cpp 는 내부적으로 요청처리에 libcurl을 사용합니다.
	 멤버변수 header_chunk 는 이를위한 curl 인스턴스 입니다.
	 */
	struct curl_slist* header_chunk;
	
	/**
	 @brief 요청바디를 만드는 curl 인스턴스
	 
	 HTTP_cpp 는 내부적으로 요청처리에 libcurl을 사용합니다.
	 멤버변수 http_post 는 이를위한 curl 인스턴스 입니다.
	 멤버변수 http_post 는 last_post 와 함께 요청바디를 만듭니다.
	 */
	struct curl_httppost* http_post;
	
	/**
	 @brief 요청바디를 만드는 curl 인스턴스
	 
	 HTTP_cpp 는 내부적으로 요청처리에 libcurl을 사용합니다.
	 멤버변수 last_post 는 이를위한 curl 인스턴스 입니다.
	 멤버변수 last_post 는 http_post 와 함께 요청바디를 만듭니다.
	 */
	struct curl_httppost* last_post;
	
	/**
	 @brief 요청에 대한 응답의 헤더를 저장할 임시파일의 경로입니다.
	 
	 이경로로 생성된 파일은 응답처리후 삭제됩니다.
	 */
	std::string header_file_path;
	
	/**
	 @brief 요청에 대한 응답의 바디를 저장할 임시파일의 경로입니다.
	 
	 이경로로 생성된 파일은 응답처리후 삭제됩니다.
	 */
	std::string body_file_path;
	
	/**
	 @brief 요청에 대한 응답헤더를 캐쉬할 파일의 경로입니다.
	 
	 이경로로 생성된 파일은 삭제되지 않습니다.삭제는 HTTPManager::CleanCache 를 이용하여야 합니다.
	 */
	std::string cache_header_file_path;
	
	/**
	 @brief 요청에 대한 응답바디를 캐쉬할 파일의 경로입니다.
	 
	 이경로로 생성된 파일은 삭제되지 않습니다.삭제는 HTTPManager::CleanCache 를 이용하여야 합니다.
	 */
	std::string cache_body_file_path;
	
	/**
	 @brief 요청에 대한 응답헤더를 쓸 파일스트림입니다.
	 
	 이 스트림은 header_file_path 로 저장되며 응답처리가 끝난후 캐싱을 할 필요가 있다면 cache_header_file_path 로 경로가 변경됩니다.
	 */
	std::ofstream header;
	
	/**
	 @brief 요청에 대한 응답바디를 쓸 파일스트림입니다.
	 
	 이 스트림은 body_file_path 로 저장되며 응답처리가 끝난후 캐싱을 할 필요가 있다면 cache_body_file_path 로 경로가 변경됩니다.
	 */
	std::ofstream body;
	
	/**
	 @brief HTTP_cpp 를 사용하는 개발자에게 Event를 보내기 위한 주소값입니다.
	 
	 개발자가 HTTPEvent::Send 함수호출시 사용된 HTTPEvent 메모리주소는 이 멤버변수에 저장됩니다.
	 */
	HTTPEvent* event;
	
	/**
	 @brief 요청을 저장하기 위한 멤버변수 입니다.
	 
	 개발자가 HTTPEvent::Send 함수호출시 사용된 HTTPRequest 변수는 이 멤버변수에 저장됩니다.
	 */
	HTTPRequest request;
	
	/**
	 @brief 인스턴스를 생성할때 호출되는 파일스트림 준비함수 입니다.
	 
	 디버그 모드에서는 HTTPConfig.h 에 지정한 경로가 권한이 없는경우 프로그램이 중단됩니다.
	 이를 막기위해서는 HTTPConfig.h 의 HTTP_DEBUG 매크로를 변경하세요
	 */
	void ReadyFile();
	
	/**
	 @brief 인스턴스를 생성할때 호출되는 요청해더 준비함수 입니다.
	 
	 libcurl의 함수를 이용하여 요청해더를 만들어냅니다.
	 
	 @ref libcurl의 요청헤더 만들기 함수링크 http://curl.haxx.se/libcurl/c/curl_slist_append.html
	 */
	void ReadyHeader();
	
	/**
	 @brief 인스턴스를 생성할때 호출되는 요청바디 준비함수 입니다.
	 
	 libcurl의 함수를 이용하여 요청바디를 만들어냅니다.
	 
	 @ref libcurl의 요청바디 만들기 함수링크 http://curl.haxx.se/libcurl/c/curl_formadd.html
	 */
	void ReadyBody();
	
	/**
	 @brief sqlite 데이터베이스를 업데이트하는 함수입니다.
	 
	 이 함수는 응답처리후 이후 캐쉬를 위해 정보를 데이터베이스에 저장합니다.(INSERT OR REPLACE: @ref sqlite 쿼리인 INSERT OR REPLACE에 대한 링크 http://forums.fedoraforum.org/showthread.php?t=213737 )
	 
	 이함수가 호출되는 시점은 요청에대한 응답을 처리하기 바로 직전입니다.
	 
	 @return 이함수가 만약 데이터베이스에 데이터를 업데이트하거나 추가할경우 true가 반환되며 아닐경우 false를 리턴합니다. 업데이트할지 추가할지, 아니면 아무것도 안할지의 판단은 이 함수내에 캐싱에대한 판단에 따릅니다.
	 */
	bool UpdateDB();
	
	
public:
	
	/**
	 @brief 생성자
	 
	 HTTPClient 를 만들기위한 생성자 입니다. 
	 HTTP_cpp 는 HTTP_cpp 를 사용하는 개발자가 HTTPEvent::Send 함수를 호출하였을때 이 생생자를 이용하여 HTTPClient 를 생성합니다.
	 libcurl 인스턴스(요청처리를 위함) 및 파일스트림(응답처리 및 캐쉬를 위함), sqlite 엔트리(캐쉬를 위함), sqlite 쿼리(캐쉬를 위함) 등이 준비되며,
	 이때 첫번째로는 HTTPManager::_AddClient 가 호출되어 HTTPManager 에 요청이 등록됩니다.
	 두번째로는 HTTPEvent::_AddClient 가 호출되어 이벤트리스트에 등록됩니다.
	 
	 @param _event HTTP_cpp 를 사용하는 개발자에게 event( HTTPEvent::Recieve , HTTPEvent::Error , HTTPEvent::Progress )를 보내기 위한 HTTPEvent 인스턴스 주소값
	 @param _tag HTTPEvent 에서 Event가 발생하였을때 어떤요청에 대한 Event인지를 판단하기위한 태그값
	 @param _request 요청에대한 정보
	 */
	HTTPClient(HTTPEvent* _event, 
				const std::string _tag,
				const HTTPRequest _request);
	
	/**
	 @brief 소멸자
	 
	 HTTPManager 에서 요청에 대한 응답처리가 끝나면 이 소멸자를 호출합니다.
	 또한 이소멸자는 호출되며 첫번째로는 HTTPManager::_RemoveClient 를 호출하여 요청을 처리할 리스트에서 삭제됩니다.
	 두번째로는 HTTPEvent::_RemoveClient 가 호출되어 이벤트리스트에서도 삭제됩니다.
	 
	 */
	~HTTPClient();
	
	/**
	 @brief libcurl 인스턴스를 가져오는 함수입니다.
	 */
	CURL* GetCURL();
	
	/**
	 @brief 어떤요청에 대한 응답인가를 구분하기위한 태그를 가져오기위한 함수입니다.
	 */
	const std::string GetTag();
	
	/**
	 @brief 연결을 일시중지 합니다.
	 @param 이미 일시중지가 되어있다면 false를 리턴합니다.
	 */
	bool Pause();
	
	/**
	 @brief 연결을 일시중지상태에서 빠져나옵니다.
	 @param 일시중지상태가 아니었다면 false를 리턴합니다.
	 */
	bool Resume();
	
	/**
	 @brief 이함수는 HTTPManager::Update 때 필요한 HTTPClient 의 작동을 위한 함수입니다.
	 
	 이함수는 캐싱처리를 위한 함수입니다.
	 
	 HTTPManager::Update 함수가 호출될때 작동됩니다.
	 
	 캐싱처리를 할때는 더이상 연결이 필요하지 않음으로 libcurl에서 메세지가 오지않아도 이함수에서 바로 HTTPEvent 로 응답처리결과를 알려주며,
	 처리가 끝난 HTTPClient 는 더이상 필요하지 않기때문에 삭제를 해야할지 말아야할지를 HTTPManager 에게 돌려줍니다.
	 이후 HTTPManager::Update 에서 소멸자를 호출하여 인스턴스를 삭제합니다.
	 */
	bool PrevUpdateAndGetNeedDelete();
	
	/**
	 @brief 응답이 처리된 결과를 HTTPEvent (개발자가 생성한 클래스) 에게 전달하는 함수입니다.
	 
	 HTTPManager::Update 함수가 호출될때 작동됩니다.
	 
	 libcurl에서 응답처리 메세지가 호출되었을때 작동하며 
	 이후 HTTPClient 인스턴스는 삭제됩니다.
	 */
	void MessageReciever(CURLcode _code);
};

#endif
