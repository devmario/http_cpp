#include "HTTPResponseHeaderField.h"

static bool __IsNewLine(char _buffer) {
	return ((_buffer == '\n') || (_buffer == '\r'));
}

static bool __IsSeperator(char _buffer) {
	return _buffer == ':';
}

HTTPResponseHeaderField::HTTPResponseHeaderField(const std::string _header_file_path) {
	bool _parse_status = false;
	bool _parse_field = false;
	
	std::ifstream _stream;
	_stream.open(_header_file_path.c_str());
	std::string _name;
	std::string _content;
	if(_stream.is_open()) {
		char _buffer = '\0';
		while(_stream.good()) {
			_stream.read(&_buffer, sizeof(char));
			if(!_parse_status) {
				if(__IsNewLine(_buffer)) {
					_parse_status = true;
				} else {
					status.append(sizeof(char), _buffer);
				}
			} else {
				if(__IsNewLine(_buffer)) {
					_parse_field = false;
					if(_name.size() || _content.size()) {
						char _content_buffer = _content.at(0);
						while(_content_buffer == ' ') {
							_content.replace(0, 1, "");
							_content_buffer = _content.at(0);
						}
						
						chunk.push_back((HTTPResponseHeaderField::Chunk){_name, _content});
					}
					_name.clear();
					_content.clear();
				} else if(__IsSeperator(_buffer)) {
					_parse_field = true;
				} else {
					if(!_parse_field) {
						_name.append(sizeof(char), _buffer);
					} else {
						_content.append(sizeof(char), _buffer);
					}
				}
			}
			_buffer = '\0';
		}
		
		_stream.close();
	}
	
	std::string _status_code;
	bool _find_space = false;
	for(int _i = 0; _i < status.length(); _i++) {
		char _buffer = status.at(_i);
		if(!_find_space) {
			if(_buffer == ' ') {
				_find_space = true;
			}
		} else {
			if(_buffer == ' ') {
				break;
			} else {
				_status_code.append(sizeof(char), _buffer);
			}
		}
	}
	status_code = atoi(_status_code.c_str());
}

const std::string HTTPResponseHeaderField::SearchField(const std::string _field_name) {
	for(int _i = 0; _i < chunk.size(); _i++) {
		if(chunk[_i].name == _field_name)
			return chunk[_i].content;
	}
	return "";
}