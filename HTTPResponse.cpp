#include "HTTPResponse.h"

HTTPResponse::HTTPResponse(const std::string _header_file_path, 
						   const std::string _body_file_path,
						   CacheType _cache_type) {
	header_file_path = _header_file_path;
	body_file_path = _body_file_path;
	cache_type = _cache_type;
}

HTTPResponse::~HTTPResponse() {
	
}

static const std::string __ReadFileToString(std::string _file_path) {
	std::string _content;
	std::ifstream _stream;
	_stream.open(_file_path.c_str());
	if(_stream.is_open()) {
		char _buffer[0xFF] = {NULL,};
		while(_stream.good()) {
			_stream.read(_buffer, 0xFF - 1);
			_content.append(_buffer);
			memset(_buffer, NULL, 0xFF);
		}
		_content.append(_buffer);
		
		_stream.close();
	}
	return _content;
}

const std::string HTTPResponse::GetBodyContent() {
	return __ReadFileToString(body_file_path);
}

const std::string HTTPResponse::GetHeaderContent() {
	return __ReadFileToString(header_file_path);
}

const std::string HTTPResponse::SearchHeaderField(const std::string _field_name) {
	std::string _field;
	
	std::string _header = GetHeaderContent();
	
	char _buffer = NULL;
	int _start = _header.find(_field_name.c_str());
	if(_start < 0)
		return _field;
	
	int _end = _header.length();
	int _i = _start + _field_name.length() + std::string(": ").length();
	
	while(_i > 0 && _i < _end) {
		_buffer = _header.at(_i);
		if((_buffer == '\n') ||
		   (_buffer == '\r') ||
		   (_buffer == '\0') ||
		   (_buffer == ';')) {
			break;
		} else {
			_field.append(sizeof(char), _buffer);
		}
		_i++;
	}
	return _field;
}

const HTTPResponseHeaderField HTTPResponse::ParseHeader() {
	HTTPResponseHeaderField _field(header_file_path);
	return _field;
}
