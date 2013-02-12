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
	
	bool RemoveDirectory(std::string path);
	
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
	static int SQLMaxAge(void* _reference, int _field_length, char** _field_content, char** _field_name);
	
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
	
	CURLM* GetCURLMulti();
	
	/**
	 @brief 생성자
	 
	 HTTPManager 는 싱글턴 클래스이므로 생성자는 private입니다.
	 */
	HTTPManager(std::string _HTTP_MANAGER_DIRECTORY,
				std::string _HTTP_MANAGER_DIRECTORY_TEMPORARY,
				std::string _HTTP_MANAGER_DIRECTORY_CACHE,
				std::string _HTTP_MANAGER_CACHE_DB,
				std::string _HTTP_MANAGER_DIRECTORY_HEADER,
				std::string _HTTP_MANAGER_DIRECTORY_BODY);
	
	/**
	 @brief 소멸자
	 
	 HTTPManager 는 싱글턴 클래스이므로 소멸자는 private입니다.
	 */
	virtual ~HTTPManager();
	
public:
	
	std::string HTTP_MANAGER_DIRECTORY;
	std::string HTTP_MANAGER_DIRECTORY_TEMPORARY;
	std::string HTTP_MANAGER_DIRECTORY_CACHE;
	std::string HTTP_MANAGER_CACHE_DB;
	std::string HTTP_MANAGER_DIRECTORY_HEADER;
	std::string HTTP_MANAGER_DIRECTORY_BODY;
	
	static void* UpdatePrev(void*);
	static void* UpdateNext(void*);
	
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
	static HTTPManager* Share(std::string _HTTP_MANAGER_DIRECTORY,
							  std::string _HTTP_MANAGER_DIRECTORY_TEMPORARY,
							  std::string _HTTP_MANAGER_DIRECTORY_CACHE,
							  std::string _HTTP_MANAGER_CACHE_DB,
							  std::string _HTTP_MANAGER_DIRECTORY_HEADER,
							  std::string _HTTP_MANAGER_DIRECTORY_BODY);
	
	static HTTPManager* Share();
	
	/**
	 
	 @brief 응답을 기다리고 있는 요청이 몇개인지 검사합니다.
	 @return 응답을 기다리고있는 요청의 갯수를 리턴합니다.
	 
	 \par Example
	 다음의 예제는 naver.com으로 10개의 요청을 보내고 응답을 기다리고 있는 요청의 갯수를 보여주는 예제입니다.
	 \include running_http.cpp
	 */
	int GetRunningHTTP();
	
	/*!
	 \brief 모든 임시파일및 캐싱을 초기화합니다.데이터베이스또한 초기화됩니다.
	 \return 연결이 되있는게 하나라도 있으면 false를 리턴하면서 초기화 되지않습니다. 정상적으로 초기화 되면 true를 리턴합니다.
	 
	 만약 연결이 있는 상태라면 초기화 할수 없습니다.캐쉬파일의 용량과 초기화의 좋은 작동은 다음버전에 업데이트 됩니다.
	 */
	bool CleanCache();
	
	friend class HTTPClient;
};

#endif
