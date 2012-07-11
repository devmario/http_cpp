#include "HTTP.h"

#pragma mark URL 인코딩 디코딩

const std::string HTTP::URLDecode(const std::string _string) {
	return curl_unescape(_string.c_str(), _string.length());
}

const std::string HTTP::URLEncode(const std::string _string) {
	return curl_escape(_string.c_str(), _string.length());
}

#pragma mark -
#pragma mark Header time 파싱

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

#pragma mark -
#pragma mark sqlite 쿼리 time 파싱

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

#pragma mark -
#pragma mark 현재시간(그리니치)

time_t HTTP::CurrentTime() {
	time_t _time = time(NULL);
	struct tm _gmt;
	memset(&_gmt, 0, sizeof(struct tm));
	gmtime_r(&_time, &_gmt);
	time_t _gmt_time = mktime(&_gmt);
	return _gmt_time;
}
