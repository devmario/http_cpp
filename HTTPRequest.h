/*!
 \file HTTPRequest.h
 \brief HTTP요청에 대한 정보를 담고있는 클래스
 \author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 \version 1.0
 \section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */
#ifndef HTTPRequest_h
#define HTTPRequest_h

#include "HTTPConfig.h"
#include "HTTPResponse.h"

/*!
 \class HTTPRequest
 \brief HTTP요청에 대한 정보를 담고있는 클래스입니다.
 
 이 클래스는 요청에대한 정보들을 만들수 있으며 연결에대한 작동을 결정합니다.캐시사용의 여부를 결정할수 있으며, GET, POST를 지원합니다.요청 헤더와 바디를 생성할수도 있습니다.
 */
class HTTPRequest {
public:
	/*!
	 \brief 요청의 타입을 나타내는 FLAG입니다.
	 */
	typedef enum {
        HTTPMethod_GET = 0, /*!< GET형태의 요청임을 나타냅니다. */
        HTTPMethod_POST,	/*!< POST형태의 요청임을 나타냅니다. */
		/*
		HTTPMethod_OPTIONS,
		HTTPMethod_HEAD,
		HTTPMethod_PUT,
		HTTPMethod_DELETE,
		HTTPMethod_TRACE,
		HTTPMethod_CONNECT,
		HTTPMethod_PATCH
		 */
    } HTTPMethod;

private:
	/*!
	 \brief 요청의 타입을 나타냅니다.
	 
	 default값은 HTTPRequest::HTTPMethod_GET 입니다.
	 */
	HTTPMethod method;
	
	/*!
	 \brief 요청을 보내려는 주소입니다.
	 */
	std::string url;
	
	/*!
	 \brief 요청해더에 추가해줄 필드들을 나타내는 스트럭트입니다.
	 
	 \ref Request Header Field에 관한 링크 http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
	 */
	typedef struct HeaderField {
		std::string name;		/*!< 필드의 이름을 나타냅니다. */
		std::string content;	/*!< 필드의 내용을 나타냅니다. */
	} HeaderField;
	
	/*!
	 \brief 요청헤더에 추가되는 필드들의 배열입니다.
	 
	 이 배열데이터는 요청을 할때 요청헤더에 포함됩니다.
	 */
	std::vector<HeaderField> header_vector;
	
	/*!
	 \brief 요청바디에 추가해줄 필드들을 나타내는 스트럭트입니다.
	 
	 서버에 변수혹은 파일을 보낼때 사용되어집니다.
	 name은 항상 셋팅되어야 하지만 content와 file_path는 작동에 따라서 한개만 선택적으로 사용되어 집니다.
	 */
	typedef struct BodyField {
		std::string name;		/*!< 필드의 이름을 나타냅니다. */
		std::string content;	/*!< 필드의 내용을 나타냅니다. */
		std::string file_path;	/*!< 필드의 파일을 나타냅니다.(파일업로드시 이용되어 집니다.) */
	} BodyField;
	
	/*!
	 \brief 요청바디에 추가되는 필드들의 배열입니다.
	 
	 이 배열데이터는 요청을 할때 요청바디에 포함됩니다.서버에 변수들을 보낼때 사용되어집니다.
	 */
	std::vector<BodyField> body_vector;
	
	/*!
	 \brief 해당 요청에대해 캐싱을 수행할지에 대해 결정하는 FLAG입니다.
	 
	 default값은 false입니다.
	 */
	bool use_cache;

public:
	/*!
	 \brief 생성자
	 */
	HTTPRequest();
	
	/*!
	 \brief 소멸자
	 */
	~HTTPRequest();
	
	/*!
	 \brief 요청하려는 메소드타입을 가져옵니다.
	 \return 메소드 타입을 리턴합니다.
	 
	 default값은 HTTPRequest::HTTPMethod_GET 입니다.
	 */
	HTTPMethod GetMethod();
	
	/*!
	 \brief 요청하려는 메소드타입을 설정합니다.
	 \param _method 설정하려는 메소드 타입입니다.
	 
	 default값은 HTTPRequest::HTTPMethod_GET 입니다.
	 
	 \par Example
	 다음의 예제는 POST로 서버에 변수를 전송하고 응답받은후 프로그램을 종료하는 예제입니다.
	 Mac OSX 에서 작성되었습니다.
	 \include request_post.cpp
	 */
	void SetMethod(HTTPMethod _method);
	
	/*!
	 \brief 요청을 보내려는 주소를 가져옵니다.
	 \return 요청을 보내려는 주소입니다.
	 */
	const std::string GetURL();
	
	/*!
	 \brief 요청을 보내려는 주소를 설정합니다.
	 \param _url 요청을 보내려는 주소입니다.
	 
	 \par Example
	 다음의 예제는 GET으로 서버에 변수를 전송하고 응답받은후 프로그램을 종료하는 예제입니다.
	 Mac OSX 에서 작성되었습니다.
	 \include request_get.cpp
	 */
	void SetURL(const std::string _url);
	
	/*!
	 \brief 요청에 대하여 캐싱을 수행할지 결정합니다.
	 \param _use_cache true이면 캐싱을 수행하며 false이면 수행하지 않습니다.
	 
	 default값은 false입니다.
	 
	 \par Example
	 다음의 예제는 이미지를 계속 다운받으며, 캐쉬를 이용하여 최초 한번만 다운되는것을 나타내는 예제입니다.
	 Mac OSX 에서 작성되었습니다.
	 \include request_cache.cpp
	 */
	void SetUseCache(bool _use_cache);
	
	/*!
	 \brief 요청에대한 캐싱의 여부를 가져옵니다.
	 \return true이면 캐싱을 수행하며 false이면 수행하지 않습니다.
	 
	 default값은 false입니다.
	 */
	bool GetUseCache();
	
	/*!
	 \brief 요청헤더에 필드를 추가합니다.
	 \param _name 추가하려는 필드의 이름입니다.
	 \param _content 추가하려는 필드의 내용입니다.
	 
	 이함수로 요청해더를 만들수 있습니다.
	 
	 \ref Request Header Field에 관한 링크 http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
	 
	 \par Example
	 다음의 예제는 모질라브라우저를 사용하는중이라는 헤더필드를 추가하여 요청을보내고, 응답을받으면 프로그램을 종료하는 예제입니다.
	 Mac OSX 에서 작성되었습니다.
	 \include request_header.cpp
	 */
	void AppendHeader(const std::string _name, 
					  const std::string _content);
	
	/*!
	 \brief 그동안 추가한 요청헤더의 필드를 초기화(삭제) 합니다.
	 */
	void ClearHeader();
	
	/*!
	 \brief 요청바디에 필드를 추가합니다.
	 \param _name 추가하려는 필드의 이름입니다.(서버는 이이름으로 변수를 가져옵니다.)
	 \param _content 추가하려는 필드의 내용입니다.
	 
	 이함수로 요청바디를 만들어 서버에 변수를 보내줄수 있습니다.
	 
	 \par Example
	 다음의 예제는 POST로 서버에 변수를 전송하고 응답받은후 프로그램을 종료하는 예제입니다.
	 Mac OSX 에서 작성되었습니다.
	 \include request_post.cpp
	 */
	void AppendBody(const std::string _name, 
					const std::string _content);
	
	/*!
	 \brief 요청바디에 파일를 추가합니다.
	 \param _name 추가하려는 파일의 이름입니다.(서버는 이이름으로 파일을 가져옵니다.)
	 \param _file_path 추가하려는 파일의 경로입니다.
	 
	 이함수로 서버에 파일을 보내줄수 있습니다.
	 
	 HTTPRequest::AppendBody 와 함께 사용하여 multipart/form-data 형태의 요청을 서버에게 할수 있습니다.
	 
	 \par Example
	 다음의 예제는 서버로 파을들을 업로드하고 응답받은후 프로그램을 종료하는 예제입니다.
	 Mac OSX 에서 작성되었습니다.
	 \include request_uploader.cpp
	 */
	void AppendBodyWithFile(const std::string _name, 
							const std::string _file_path);
	
	/*!
	 \brief 그동안 추가한 요청바디의 내용을 초기화(삭제) 합니다.
	 */
	void ClearBody();
	
	/*!
	 HTTPRequest 클래스는 HTTPClient 클래스와 friend입니다.
	 */
	friend class HTTPClient;
};

#endif
