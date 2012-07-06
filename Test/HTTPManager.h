#ifndef HTTPManager_h
#define HTTPManager_h

#include "HTTPConfig.h"
#include "_HTTPClient.h"

class HTTPManager {
private:
	std::string temporary_path;
	std::string cache_path;
	
	std::string db_path;
	
	std::string temporary_header_path;
	std::string temporary_body_path;
	
	std::string cache_header_path;
	std::string cache_body_path;
	
	CURLM* multi_handle;
	int still_running;
	
	int messages_left;
	
	std::list<_HTTPClient*> client_list;
	std::string temporary_directory;
	
	static int SQLExpires(void* _reference, int _field_length, char** _field_content, char** _field_name);
	
	void ReadyDirectory();
	void ReadyDB();
	
	HTTPManager();
	~HTTPManager();
	
public:
	const std::string _GetTemporaryHeaderPath();
	const std::string _GetTemporaryBodyPath();
	const std::string _GetCacheHeaderPath();
	const std::string _GetCacheBodyPath();
	const std::string _GetDBPath();
	
	static HTTPManager* Share();
	
	void _AddClient(_HTTPClient* _client);
	void _RemoveClient(_HTTPClient* _client);
	
	void Update();
	
	int GetRunningHTTP();
};

#endif
