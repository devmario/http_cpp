#include "HTTPRequest.h"
#include "HTTP.h"

HTTPRequest::HTTPRequest() {
	method = HTTPRequest::HTTPMethod_GET;
	use_cache = false;
}

HTTPRequest::~HTTPRequest() {
}

HTTPRequest::HTTPMethod HTTPRequest::GetMethod() {
	return method;
}

void HTTPRequest::SetMethod(HTTPMethod _method) {
	method = _method;
}

const std::string HTTPRequest::GetURL() {
	return url;
}

void HTTPRequest::SetURL(const std::string _url) {
	url = _url;
}

void HTTPRequest::SetUseCache(bool _use_cache) {
	use_cache = _use_cache;
}

bool HTTPRequest::GetUseCache() {
	return use_cache;
}

void HTTPRequest::AppendHeader(const std::string _name, 
							   const std::string _content) {
	header_vector.push_back((HeaderField){_name, _content});
}

void HTTPRequest::ClearHeader() {
	header_vector.clear();
}

void HTTPRequest::AppendBody(const std::string _name, 
							 const std::string _content) {
	body_vector.push_back((BodyField){_name, _content, std::string("")});
}

void HTTPRequest::AppendBodyWithFile(const std::string _name, 
									 const std::string _file_path) {
	HTTP_DEBUG((access(_file_path.c_str(), F_OK) != 0), "HTTPRequest로 보내실 파일이 존재하지 않습니다.");
	body_vector.push_back((BodyField){_name, std::string(""), _file_path});
}

void HTTPRequest::ClearBody() {
	body_vector.clear();
}
