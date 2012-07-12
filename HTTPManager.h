/**
 @file HTTPManager.h
 @brief HTTPEvent들을 관리하는 Manager
 @author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 @version 1.0
 @section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */

#ifndef HTTPManager_h
#define HTTPManager_h

#include "HTTPConfig.h"
#include "HTTPClient.h"

/**
 @class HTTPManager
 @brief HTTPEvent 들을 관리하는 Singleton Class
 */
class HTTPManager {
private:
	/**
	 @brief 임시파일들이 담길 폴더경로문자열
	 */
	std::string temporary_path;
	
	/**
	 @brief 캐쉬파일들이 담길 폴더경로문자열
	 */
	std::string cache_path;
	
	/**
	 @brief sqlite 디비파일의 경로문자열
	 */
	std::string db_path;
	
	/**
	 @brief 임시헤더파일들이 담길 폴더경로문자열
	 */
	std::string temporary_header_path;
	
	/**
	 @brief 임시바디파일들이 담길 폴더경로문자열
	 */
	std::string temporary_body_path;
	
	/**
	 @brief 캐쉬헤더파일들이 담길 폴더경로문자열
	 */
	std::string cache_header_path;
	
	/**
	 @brief 캐쉬바디파일들이 담길 폴더경로문자열
	 */
	std::string cache_body_path;
	
	/**
	 @brief libcurl에서 다중 요청,응답처리를 위한 핸들러주소
	 
	 @ref 멀티핸들에 대한 참고 링크 http://www.onlineaspect.com/2009/01/26/how-to-use-curl_multi-without-blocking/
	 */
	CURLM* multi_handle;
	
	/**
	 @brief 실행중인 요청의 갯수
	 */
	int still_running;
	
	/**
	 @brief 처리해야할 남은 메세지 갯수
	 */
	int messages_left;
	
	/**
	 @brief 응답대기상태에 있는 처리해야할 요청의 리스트
	 */
	std::list<HTTPClient*> client_list;
	
	/**
	 @brief Expires가 지난 요청캐쉬를 삭제하기위한 sqlite 쿼리처리 콜백
	 
	 값을 찾았을경우 필드를 삭제하고 캐쉬파일을 삭제함
	 
	 @ref 이콜백에 대한 참고링크 http://six605.tistory.com/300
	 */
	static int SQLExpires(void* _reference, int _field_length, char** _field_content, char** _field_name);
	
	/**
	 @brief 임시파일, 캐쉬파일, 디비파일들을 저장할 디렉토리를 준비합니다.
	 
	 프로그램 실행후 처음으로 HTTPManager Singleton Instance( HTTPManager::Share() )를 부를때 호출됩니다.
	 */
	void ReadyDirectory();
	
	/**
	 @brief sqlite 데이터베이스를 준비합니다.
	 
	 데이터베이스가 존재하지 않으면 만들고, 만료기간(Expires)가 지난 필드와 캐쉬파일들을 삭제합니다.
	 
	 프로그램 실행후 처음으로 HTTPManager Singleton Instance( HTTPManager::Share() )를 부를때 호출됩니다.
	 */
	void ReadyDB();
	
	/**
	 @brief 생성자
	 
	 HTTPManager 는 싱글턴 클래스이므로 생성자는 private입니다.
	 */
	HTTPManager();
	
	/**
	 @brief 소멸자
	 
	 HTTPManager 는 싱글턴 클래스이므로 소멸자는 private입니다.
	 */
	~HTTPManager();
	
public:
	/**
	 @brief 임시헤더파일들이 담긴 폴더의 경로를 가져옵니다.
	 @return 임시헤더폴더
	 
	 HTTP_cpp에서는 보낸요청에대하여 파일스트림으로 응답을 받고 event가 완료된 후에는 파일스트림을 삭제합니다.(캐쉬가 필요할경우는 삭제하지 않음)
	 HTTP_cpp에서는 이를 임시파일이라고하고 이 경로는 응답중(헤더와 바디로이루어진) 헤더를 받을 경로입니다.
	 
	 <b>Example</b><br>다음의 예제는 임시헤더파일들이 담길 폴더의 경로를 가져오고 출력하는 예제입니다.
	 \code{.cpp}
	 HTTPManager* _manager = HTTPManager::Share();
	 std::cout << _manager->GetTemporaryHeaderPath() << std::endl;
	 \endcode
	 */
	const std::string GetTemporaryHeaderPath();
	
	/**
	 @brief 임시바디파일들이 담긴 폴더의 경로를 가져옵니다.
	 @return 임시바디폴더
	 
	 HTTP_cpp에서는 보낸요청에대하여 파일스트림으로 응답을 받고 event가 완료된 후에는 파일스트림을 삭제합니다.(캐쉬가 필요할경우는 삭제하지 않음)
	 HTTP_cpp에서는 이를 임시파일이라고하고 이 경로는 응답중(헤더와 바디로이루어진) 바디를 받을 경로입니다.
	 
	 <b>Example</b><br>다음의 예제는 임시바디파일들이 담길 폴더의 경로를 가져오고 출력하는 예제입니다.
	 \code{.cpp}
	 HTTPManager* _manager = HTTPManager::Share();
	 std::cout << _manager->GetTemporaryBodyPath() << std::endl;
	 \endcode
	 */
	const std::string GetTemporaryBodyPath();
	
	/**
	 @brief 캐쉬해더파일들이 담긴 폴더의 경로를 가져옵니다.
	 @return 캐쉬헤더폴더
	 
	 HTTP_cpp에서는 보낸요청에대하여 파일스트림으로 응답을 받고 event가 완료된 후에는 파일스트림을 삭제하거나 캐쉬가 필요할경우는 저장합니다.
	 HTTP_cpp에서는 이 캐쉬되는 파일을 캐쉬파일이라고하고 이 경로는 응답중(헤더와 바디로이루어진) 헤더를 캐쉬할 경로입니다.
	 
	 <b>Example</b><br>다음의 예제는 캐쉬해더파일들이 담길 폴더의 경로를 가져오고 출력하는 예제입니다.
	 \code{.cpp}
	 HTTPManager* _manager = HTTPManager::Share();
	 std::cout << _manager->GetCacheHeaderPath() << std::endl;
	 \endcode
	 */
	const std::string GetCacheHeaderPath();
	
	/**
	 @brief 캐쉬바디파일들이 담긴 폴더의 경로를 가져옵니다.
	 @return 캐시바디폴더
	 
	 HTTP_cpp에서는 보낸요청에대하여 파일스트림으로 응답을 받고 event가 완료된 후에는 파일스트림을 삭제하거나 캐쉬가 필요할경우는 저장합니다.
	 HTTP_cpp에서는 이 캐쉬되는 파일을 캐쉬파일이라고하고 이 경로는 응답중(헤더와 바디로이루어진) 바디를 캐쉬할 경로입니다.
	 
	 <b>Example</b><br>다음의 예제는 캐쉬바디파일들이 담길 폴더의 경로를 가져오고 출력하는 예제입니다.
	 \code{.cpp}
	 HTTPManager* _manager = HTTPManager::Share();
	 std::cout << _manager->GetCacheBodyPath() << std::endl;
	 \endcode
	 */
	const std::string GetCacheBodyPath();
	
	/**
	 @brief sqlite 데이터베이스의 경로를 가져옵니다.
	 @return sqlite 데이터베이스 경로
	 
	 <b>Example</b><br>다음의 예제는 sqlite 데이터베이스의 경로를 가져오고 출력하는 예제입니다.
	 \code{.cpp}
	 HTTPManager* _manager = HTTPManager::Share();
	 std::cout << _manager->GetDBPath() << std::endl;
	 \endcode
	 */
	const std::string GetDBPath();
	
	/**
	 @brief Singleton HTTPManager Class의 포인터를 반환합니다.
	 @return Singleton HTTPManager Class의 포인터를 반환합니다.
	 
	 HTTPManager instance는 이 메소드 호출시 초기화되며 이메소드의 접근은 항상 같은 쓰레드에서 하여야 합니다.
	 
	 HTTPConfig.h 에 명시되어 있는 HTTP_DEBUG 메크로의 assert 문을 주석처리하지 않으면 다른쓰레드에서 접근시 혹은 에러 발생시 프로그램은 중단됩니다.
	 
	 <b>Example</b><br>다음의 예제는 싱클턴 HTTPManager 인스턴스를 가져오는 예제입니다.
	 \code{.cpp}
	 HTTPManager* _manager = HTTPManager::Share();
	 \endcode
	 */
	static HTTPManager* Share();
	
	/**
	 @brief 요청과 응답을 처리할 class 인 HTTPClient 를 작동시키기 위해 추가하는 함수입니다.
	 
	 이 함수는 HTTPClient 가 생성될때 자동으로 호출되어 HTTPManager 의 멤버인 client_list 에 자동으로 추가됩니다.
	 */
	void AddClient(HTTPClient* _client);
	
	/**
	 @brief 요청과 응답을 처리할 class 인 HTTPClient 의 작동을 멈추기 위해 제거하는 함수입니다.
	 
	 이 함수는 HTTPClient 가 소멸될때 자동으로 호출되어 HTTPManager 의 멤버인 client_list 에 자동으로 제거됩니다.
	 */
	void RemoveClient(HTTPClient* _client);
	
	/**
	 @brief HTTPEvent 요청에 대한 응답을 기다리고 응답이 있으면 처리합니다.
	 
	 HTTPEvent 의 추상화함수들은 모두 응답에 관한 함수들이며, 응답이 있을시 그 함수들이 이 Update 함수 안에서 호출됩니다.
	 메인쓰레드(drawing 쓰레드)에서 매 프레임마다 호출해주세요.
	 
	 \par Example
	 다음의 예제는 네이버로 요청을 보내고 요청을 기다리는 예제입니다.
	 \include running_http.cpp
	 */

	void Update();
	
	/**
	 
	 @brief 응답을 기다리고 있는 요청이 몇개인지 검사합니다.
	 @return 응답을 기다리고있는 요청의 갯수를 리턴합니다.
	 
	 \par Example
	 다음의 예제는 naver.com으로 10개의 요청을 보내고 응답을 기다리고 있는 요청의 갯수를 보여주는 예제입니다.
	 \include running_http.cpp
	 */
	int GetRunningHTTP();
	
	void CleanCache();
};

#endif
