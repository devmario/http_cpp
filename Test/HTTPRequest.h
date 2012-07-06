#ifndef HTTPRequest_h
#define HTTPRequest_h

#include "HTTPConfig.h"
#include "HTTPResponse.h"

class HTTPRequest {
public:
	typedef enum {
        HTTPMethod_GET = 0,
        HTTPMethod_POST,
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
	
	HTTPMethod method;
	std::string url;
	
	typedef struct HeaderField {
		std::string name;
		std::string content;
	} HeaderField;
	std::vector<HeaderField> header_vector;
	
	typedef struct BodyField {
		std::string name;
		std::string content;
		std::string file_path;
	} BodyField;
	std::vector<BodyField> body_vector;
	
	bool use_cache;
	
	HTTPRequest();
	~HTTPRequest();
	
	//요청 메소드
	HTTPMethod GetMethod();
	void SetMethod(HTTPMethod _method);
	
	//주소
	const std::string GetURL();
	void SetURL(const std::string _url);
	
	//헤더
	//Request Header Field에 관한 링크
	//http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
	void AppendHeader(const std::string _name, 
					  const std::string _content);
	void ClearHeader();
	
	//바디
	void AppendBody(const std::string _name, 
					const std::string _content);
	void AppendBodyWithFile(const std::string _name, 
							const std::string _file_path);
	void ClearBody();
};

#endif
