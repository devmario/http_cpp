#include "HTTP.h"

const std::string HTTP::URLEncode(const std::string _string) {
	std::string result("");
	
	int i, j;
	unsigned char c;
	char buffer[3];
	for(i = j = 0; _string.data()[i]; i++) {
		c = (unsigned char)_string.data()[i];
		if((c >= '0') && (c <= '9'))
			result.append(sizeof(char), c);
		else if((c >= 'A') && (c <= 'Z'))
			result.append(sizeof(char), c);
		else if((c >= 'a') && (c <= 'z'))
			result.append(sizeof(char), c);
		else if((c == '@') || (c == '.') || (c == '/') || (c == '\\') || (c == '-') || (c == '_') || (c == ':'))
			result.append(sizeof(char), c);
		else {
			sprintf(buffer, "%02x", c);
			result.append(sizeof(char), '%');
			result.append(sizeof(char), buffer[0]);
			result.append(sizeof(char), buffer[1]);
		}
	}
	
	return result;
}

time_t HTTP::HeaderToTime(const std::string _header_time) {
	struct tm _tm;
	memset(&_tm, 0, sizeof(struct tm));
	strptime(_header_time.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &_tm);
	return mktime(&_tm);
}

const std::string HTTP::TimeToHeader(time_t _time) {
	struct tm* _tm = (struct tm*)calloc(1, sizeof(struct tm));
	localtime_r(&_time, _tm);
	char _buffer[0xFF] = {NULL,};
	strftime(_buffer, sizeof _buffer, "%a, %d %b %Y %H:%M:%S GMT", _tm);
	free(_tm);
	return std::string(_buffer);
}

time_t HTTP::QueryToTime(const std::string _query_time) {
	struct tm _tm; 
	memset(&_tm, 0, sizeof(struct tm));
	strptime(_query_time.c_str(), "%Y-%m-%d %H:%M:%S", &_tm);
	return mktime(&_tm);
}

const std::string HTTP::TimeToQuery(time_t _time) {
	struct tm* _tm = (struct tm*)calloc(1, sizeof(struct tm));
	localtime_r(&_time, _tm);
	char _buffer[0xFF] = {NULL,};
	strftime(_buffer, sizeof _buffer, "%Y-%m-%d %H:%M:%S", _tm);
	free(_tm);
	return std::string(_buffer);
}

time_t HTTP::CurrentTime() {
	time_t _time = time(NULL);
	struct tm _gmt;
	memset(&_gmt, 0, sizeof(struct tm));
	gmtime_r(&_time, &_gmt);
	time_t _gmt_time = mktime(&_gmt);
	return _gmt_time;
}
