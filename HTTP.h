/**
 @mainpage HTTP_cpp 소개
 @section intro 소개
 <p>HTTP_cpp 는 c++로 작성된 Client용 HTTPEvent 를 사용하기위한 Class묶음입니다.</p>
 <p>@ref github https://github.com/devmario/http_cpp</p>
 <p>다음과 같은 기능을 지원합니다.</p>
 <ul>
 <li>멀티 HTTPEvent 지원</li>
 <li>HTTPEvent 에 관한 Thread 관리</li>
 <li>puase, resume</li>
 <li>컨텐츠 캐싱</li>
 <li>헤더 파싱</li>
 <li>POST, GET</li>
 <li>파일 IO를 이용한 요청,응답</li>
 </ul>
 
 <p>또한 내부적으로는 다음과 같은 라이브러리가 사용됩니다.</p>
 <ul>
 <li>@ref libcurl http://curl.haxx.se/libcurl/</li>
 <li>@ref sqlite3 http://www.sqlite.org/</li>
 </ul>
 
 <p>HTTPEvent Class작성 원칙</p>
 <ul>
 <li>사용시 HTTPEvent 를 상속받아 추상화된 함수를 반드시 이용: 혹시라도 모를 처리하지 않은 이벤트를 사전에 없애기 위한 방침</li>
 <li>더 많은 메모리 풀을 위하여 파일로 응답에 대한 처리를 하기를 권장</li>
 <li>한쓰레드 에서만 호출가능: 디버그 과정에서 혹시라도 모를 쓰레드에 의한 오류를 막기위한 방침</li>
 </ul>
 
 <p>준비</p>
 <ul>
 <li>curl 및 sqlite필요</li>
 <li>@ref libcurl http://curl.haxx.se/libcurl/</li>
 <li>@ref sqlite3 http://www.sqlite.org/</li>
 <li>iOS와 Android용 curl의 경우 @ref cocos2dx http://www.cocos2d-x.org/ 의 소스안에 빌드되어 있음</li>
 <li>iOS는 기본적으로 sqlite를 지원하며</li>
 <li>Android는 sqlite관련 헤더(.h)와 소스파일(.c)가 필요함</li>
 <li>HTTPConfig.h 의 매크로들을 해당 플랫폼에 맞게 수정해야함</li>
 </ul>
 
 @section  CREATEINFO      작성정보
 - 작성자 :   
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 - 작성일 : 2012/07/10
 - Version : 1.0
 </ul>
 */

/*!
 \example response_header.cpp
 \example response_stream.cpp
 \example stop.cpp
 \example send.cpp
 \example pause_resume.cpp
 \example running_http.cpp
 \example update.cpp
 \example clean.cpp
 \example error.cpp
 \example progress.cpp
 \example recieve.cpp
 \example request_body.cpp
 \example request_cache.cpp
 \example request_get.cpp
 \example request_header.cpp
 \example request_uploader.cpp
 */

/**
 @file HTTP.h
 @brief HTTP_cpp 관련 Util 함수들과 Include.
 @author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 @version 1.0
 @section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */

#ifndef HTTP_h
#define HTTP_h

#include "HTTPConfig.h"
#include "HTTPManager.h"
#include "HTTPEvent.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HTTPResponseHeaderField.h"

/**
 @class HTTP
 @brief HTTP와 관련된 정적 Util함수들을 호출하기위한 Class
 
 캐싱을 위한 Expires, Last-Modified 컨버터함수와 URL인코딩관련 함수들이 있습니다.
 */
class HTTP {
public:
	/**
	 @brief 인자로 넘겨진 문자열에 대해 URL디코딩을 수행하고 그결과를 문자열로 반환합니다.
	 @param _string URL디코딩을 수행할 문자열
	 @return URL디코딩이 된 문자열
	 
	 <b>Example</b><br>다음의 예제는 URL인코딩된 문자(네이버)를 URL디코딩하여 그 결과를 출력하는 예제입니다.
	 \code{.cpp}
	 std::cout << HTTP::URLDecode("%EB%84%A4%EC%9D%B4%EB%B2%84") << std::endl;
	 \endcode
	 */
	static const std::string URLDecode(const std::string _string);
	
	/**
	@brief 인자로 넘겨진 문자열에대하여 URL인코딩을 수행하고 인코딩된 값을 문자열로 반환합니다.
	@param _string URL인코딩을 수행할 문자열
	@return URL인코딩이 된 문자열
	
	<b>Example</b><br>다음의 예제는 문자(네이버)를 URL 인코딩한 결과를 출력하는 예제입니다.
	\code{.cpp}
	std::cout << HTTP::URLEncode("네이버") << std::endl;
	\endcode
	 */
	static const std::string URLEncode(const std::string _string);
	
	/**
	@brief 헤더에 있는 시간을 시간구조체인 time_t 로 변경하기위한 함수입니다. 
	인자로 넘겨지는 값은 반드시 <pre>"%a, %d %b %Y %H:%M:%S GMT"</pre>형태의 그리니치 천문대의 표준시간이어야 합니다.
	<pre>포매팅 예: Tue, 10 Jul 2012 05:38:12 GMT</pre>
	
	이함수는 내부적으로 HTTP 응답에 대하여 캐쉬를 해야할지 말아야 할지 판단하는데 사용됩니다.헤더에 포함된 Expires, Last-Modified를 파싱하는데 사용되며, 각나라마다 다른 시차차이 때문에 캐쉬에대한 정보를 그리니치천문대의 표준시간으로 저장하게 됩니다.
	
	@ref Expires와 Last-Modified에 대한 위키링크 http://en.wikipedia.org/wiki/List_of_HTTP_header_fields#Expires
	
	HTTP_cpp 에서는 캐쉬의 판단에 있어서 Expires와 Last-Modified만 지원합니다.
	
	@ref Expires,Last-Modified에 대한 참고링크 http://mytory.co.kr/archives/1232
	
	@param _header_time time_t 구조체로 변환할 "%a, %d %b %Y %H:%M:%S GMT"형태의 그리니치 천문대 표준시간 문자열
	@return 인자로 넘어온 _header_time 문자열이 변환된 time_t구조체
	
	<b>Example</b><br>다음의 예제는 Tue, 10 Jul 2012 05:38:12 GMT 를 time_t(long)로 출력하는 예제입니다.
	\code{.cpp}
	time_t _time = HTTP::HeaderToTime("Tue, 10 Jul 2012 05:38:12 GMT");
	std::cout << _time << std::endl;
	\endcode
	 */
	static time_t HeaderToTime(const std::string _header_time);
	
	/**
	@brief sqlite의 datetime필드의 문자열을 time_t 로 변경하기위한 함수 입니다.
	인자로 넘겨지는 값은 반드시 <pre>"%Y-%m-%d %H:%M:%S"</pre>형태의 그리니치 천문대의 표준시간이어야 합니다.
	<pre>포매팅 예: 2012-07-10 05:23:22</pre>
	
	이함수는 내부적으로 HTTP 응답에 대하여 캐쉬를 해야할지 말아야 할지 판단하는데 사용됩니다.요청에 대한 첫 응답헤더에 포함된 Expires, Last-Modified를 sqlite 데이터베이스에 저장하고 난뒤 같은 URL로 요청을 다시 보냈을시 sqlite 데이터베이스에서 캐쉬를 이용할건지 다시 응답을 받을건지 결정하며 이때 이 함수를 이용하여 sqlite의 datetime필드를 파싱합니다. 각나라마다 다른 시차차이 때문에 캐쉬에대한 정보를 그리니치천문대의 표준시간으로 저장하게 됩니다.
	
	@ref Expires와 Last-Modified에 대한 위키링크 http://en.wikipedia.org/wiki/List_of_HTTP_header_fields#Expires
	@param _query_time time_t 구조체로 변환할 "%Y-%m-%d %H:%M:%S"형태의 그리니치 천문대 표준시간 문자열
	@return 인자로 넘어온 _query_time 문자열이 변환된 time_t구조체
	
	<b>Example</b><br>다음의 예제는 2012-12-13 10:11:13 를 time_t(long)로 출력하는 예제입니다.
	\code{.cpp}
	std::cout << HTTP::QueryToTime("2012-12-13 10:11:13") << std::endl;
	\endcode
	 */
	static time_t QueryToTime(const std::string _query_time);
	
	/**
	@brief 인자로 넘어간 time_t 구조체를 헤더가 쓰는 시간규격인 "%a, %d %b %Y %H:%M:%S GMT"로 변환합니다.
	
	<pre>포매팅 예: Tue, 10 Jul 2012 05:38:12 GMT</pre>
	이함수는 HTTP::HeaderToTime 에대하여 그 반대를 수행합니다.
	@param _time 변경할 time_t구조체이며 그리니치 천문대 표준시간이어야 합니다.
	@return 변경된 "%a, %d %b %Y %H:%M:%S GMT"형태의 문자열
	
	<b>Example</b><br>다음의 예제는 현제시간의 그리니치천문대 표준시간을 헤더의 시간표현방식으로 출력하는 예제입니다.
	\code{.cpp}
	std::cout << HTTP::TimeToHeader(HTTP::CurrentTime()) << std::endl;
	\endcode 
	 */
	static const std::string TimeToHeader(time_t _time);
	
	/**
	@brief 인자로 넘어간 time_t 구조체를 sqlite쿼리가 쓰는 시간규격인 "%Y-%m-%d %H:%M:%S"로 변환합니다.
	
	<pre>포매팅 예: 2012-07-10 05:23:22</pre>
	이함수는 HTTP::QueryToTime 에대하여 그 반대를 수행합니다.
	@param _time 변경할 time_t구조체이며 그리니치 천문대 표준시간이어야 합니다.
	@return 변경된 "%Y-%m-%d %H:%M:%S"형태의 문자열
	
	<b>Example</b><br>다음의 예제는 현제시간의 그리니치천문대 표준시간을 sqlite쿼리의 시간표현방식으로 출력하는 예제입니다.
	\code{.cpp}
	std::cout << HTTP::TimeToQuery(HTTP::CurrentTime()) << std::endl;
	\endcode
	 */
	static const std::string TimeToQuery(time_t _time);
	
	/**
	@brief 이 함수는 현재 시간을 그리니치 천문대의 표준시간으로 변환한것을 돌려줍니다.
	@return 현재시간을 그리니치천문대 표준시로 변환한 time_t 구조체
	
	<b>Example</b><br>다음의 예제는 현제시간을 time_t(long)로 출력하는 예제입니다.
	\code{.cpp}
	std::cout << HTTP::CurrentTime() << std::endl;
	\endcode
	 */
	static time_t CurrentTime();
};

#endif
