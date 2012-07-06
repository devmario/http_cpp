#ifndef _HTTPClient_h
#define _HTTPClient_h

#include "HTTPConfig.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HTTPEvent.h"

//내부적인 Request + Response 유닛
class _HTTPClient {
public:
	
	HTTPResponse::CacheType cache_type;
	
	static int SQLMatchURLCallback(void* _reference, int _field_length, char** _field, char** _field_name);
	
	std::string db_id;
	time_t db_expires;
	time_t db_last_modified;
	
	time_t expires;
	time_t last_modified;
	
	bool paused;
	
	std::string tag;
	CURL* curl;
	struct curl_slist* header_chunk;
	struct curl_httppost* http_post;
	struct curl_httppost* last_post;
	
	std::string header_file_path;
	std::string body_file_path;
	std::string cache_header_file_path;
	std::string cache_body_file_path;
	
	std::ofstream header;
	std::ofstream body;
	
	std::ofstream cache_header;
	std::ofstream cache_body;
	
	HTTPEvent* event;
	HTTPRequest request;
	
	static size_t ReadBody(char* _stream, size_t _size, size_t _count, void* _pointer);
	static size_t ReadHeader(char* _stream, size_t _size, size_t _count, void* _pointer);
	static int Progress(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);
	
	_HTTPClient(HTTPEvent* _event, 
				const std::string _tag,
				const HTTPRequest _request);
	
	~_HTTPClient();
	
	bool ReadyFile();
	bool ReadyHeader();
	bool ReadyBody();
	
	bool Pause();
	bool Resume();
	
	bool UpdateDB();
	
	bool PrevUpdateAndGetNeedDelete();
	
	void MessageReciever(CURLcode _code);
};

#endif
