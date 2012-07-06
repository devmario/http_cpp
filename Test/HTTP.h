#ifndef HTTP_h
#define HTTP_h

#include "HTTPConfig.h"
#include "HTTPManager.h"
#include "HTTPEvent.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HTTPResponseHeaderField.h"

class HTTP {
public:
	static const std::string URLEncode(const std::string _string);
	
	//그리니치 천문대 시간
	
	static time_t HeaderToTime(const std::string _header_time);
	static time_t QueryToTime(const std::string _query_time);
	
	static const std::string TimeToHeader(time_t _time);
	static const std::string TimeToQuery(time_t _time);
	
	static time_t CurrentTime();
};

#endif
