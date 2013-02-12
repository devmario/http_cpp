#include "HTTPManager.h"
#include "HTTP.h"
#include <ftw.h>

static int
rmdir_helper(const char *fpath, const struct stat *sb,
			 int tflag, struct FTW *ftwbuf)
{
	switch ( tflag )
	{
		case FTW_D:
		case FTW_DP:
			if ( rmdir(fpath) == -1 )
				perror("unlink");
			break;
			
		case FTW_F:
		case FTW_SL:
		case FTW_SLN:
			if ( unlink(fpath) == -1 )
				perror("unlink");
			break;
			
        default:
			puts("do nothing");
	}
	return 0;
}

bool HTTPManager::RemoveDirectory(std::string path) {
	int flags = 0;
	flags |= FTW_DEPTH; // post-order traverse
	
	if (nftw(path.c_str(), rmdir_helper, 10, flags) == -1) {
		return false;
	}
	return true;
}

HTTPManager* __http_manager = NULL;


HTTPManager* HTTPManager::Share(std::string _HTTP_MANAGER_DIRECTORY,
								std::string _HTTP_MANAGER_DIRECTORY_TEMPORARY,
								std::string _HTTP_MANAGER_DIRECTORY_CACHE,
								std::string _HTTP_MANAGER_CACHE_DB,
								std::string _HTTP_MANAGER_DIRECTORY_HEADER,
								std::string _HTTP_MANAGER_DIRECTORY_BODY) {
	if(__http_manager == NULL) {
		__http_manager = new HTTPManager(_HTTP_MANAGER_DIRECTORY,
										 _HTTP_MANAGER_DIRECTORY_TEMPORARY,
										 _HTTP_MANAGER_DIRECTORY_CACHE,
										 _HTTP_MANAGER_CACHE_DB,
										 _HTTP_MANAGER_DIRECTORY_HEADER,
										 _HTTP_MANAGER_DIRECTORY_BODY);
	}
	
	return __http_manager;
}

HTTPManager* HTTPManager::Share() {
	return __http_manager;
}

const std::string HTTPManager::GetTemporaryHeaderPath() {
	return temporary_header_path;
}

const std::string HTTPManager::GetTemporaryBodyPath() {
	return temporary_body_path;
}

const std::string HTTPManager::GetCacheHeaderPath() {
	return cache_header_path;
}

const std::string HTTPManager::GetCacheBodyPath() {
	return cache_body_path;
}

const std::string HTTPManager::GetDBPath() {
	return db_path;
}

void HTTPManager::ReadyDirectory() {
	HTTP_DEBUG((access(HTTP_MANAGER_DIRECTORY.c_str(), X_OK | W_OK | R_OK) != 0), "임시파일을 저장할 공간에 권한이 없습니다.");
	
	//임시폴더 Reset
	temporary_path = HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY;
	RemoveDirectory(temporary_path);
	mkdir(temporary_path.c_str(), 0777);
	
	//임시헤더파일
	temporary_header_path = temporary_path + "/" + HTTP_MANAGER_DIRECTORY_HEADER;
	mkdir(temporary_header_path.c_str(), 0777);
	
	//임시바디파일 Reset
	temporary_body_path = temporary_path + "/" + HTTP_MANAGER_DIRECTORY_BODY;
	mkdir(temporary_body_path.c_str(), 0777);
	
	//캐쉬폴더
	cache_path = HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE;
	if(access(cache_path.c_str(), F_OK) != 0) {
		mkdir(cache_path.c_str(), 0777);
	}
	
	//DB file path
	db_path = cache_path + "/" + HTTP_MANAGER_CACHE_DB;
	
	//헤더캐쉬파일저장소 없으면 만들기
	cache_header_path = cache_path + "/" + HTTP_MANAGER_DIRECTORY_HEADER;
	if(access(cache_header_path.c_str(), F_OK) != 0) {
		mkdir(cache_header_path.c_str(), 0777);
	}
	
	//바디캐쉬파일저장소 없으면 만들기
	cache_body_path = cache_path + "/" + HTTP_MANAGER_DIRECTORY_BODY;
	if(access(cache_body_path.c_str(), F_OK) != 0) {
		mkdir(cache_body_path.c_str(), 0777);
	}
}

int HTTPManager::SQLExpires(void* _reference, int _field_length, char** _field_content, char** _field_name){
	HTTPManager* _manager = (HTTPManager*)_reference;
	for(int _i = 0; _i < _field_length; _i++) {
		if(strcmp(_field_name[_i], "id") == 0) {
			std::string _header_path = _manager->GetCacheHeaderPath() + "/" + _field_content[_i];
			remove(_header_path.c_str());
			
			std::string _body_path = _manager->GetCacheBodyPath() + "/" + _field_content[_i];
			remove(_body_path.c_str());
		}
	}
	return 0;
}

int HTTPManager::SQLMaxAge(void* _reference, int _field_length, char** _field_content, char** _field_name){
	std::string id;
	int Max_Age;
	time_t Last_Updated;
	std::string url;
	for(int _i = 0; _i < _field_length; _i++) {
		if(strcmp(_field_name[_i], "id") == 0) {
			id = _field_content[_i];
		} else if(strcmp(_field_name[_i], "Max_Age") == 0) {
			Max_Age = atoi(_field_content[_i]);
		} else if(strcmp(_field_name[_i], "Last_Updated") == 0) {
			Last_Updated = atol(_field_content[_i]);
		} else if(strcmp(_field_name[_i], "url") == 0) {
			url = _field_content[_i];
		}
	}
	std::cout << "delete max_age expires\n"
	<< Max_Age << "," << HTTP::CurrentTime() << "," << Last_Updated << "\n"
	<< (Max_Age - (int)(HTTP::CurrentTime() - Last_Updated)) << "\n";
	return 0;
}

void HTTPManager::ReadyDB() {
	//디비 연결하고 테이블 생성(테이블 없을시)
	sqlite3* _db = NULL;
	char* _db_message = NULL;
	HTTP_DEBUG((sqlite3_open(GetDBPath().c_str(), &_db)), 
			   "sqllite 시동에 실패하였습니다." << "\nError Message:" << sqlite3_errmsg(_db) << "\n");
	HTTP_DEBUG((sqlite3_exec(_db, "BEGIN; CREATE TABLE IF NOT EXISTS request (id integer primary key unique, url TEXT, Expires integer, Last_Modified integer, Last_Updated integer, Max_Age integer); CREATE INDEX IF NOT EXISTS request_url ON request (url); COMMIT;", NULL, this, &_db_message)),
			   "sqllite 테이블 생성에 실패하였습니다." << "\nError Message:" << _db_message);
	
	std::stringstream _current_time_query;
	_current_time_query << HTTP::CurrentTime();
	
	HTTP_DEBUG((sqlite3_exec(_db, ("SELECT * FROM request WHERE Expires != 0 AND Expires <= " + _current_time_query.str()).c_str(), HTTPManager::SQLExpires, this, &_db_message)),
			   "Expires만료기간이 지난 컬럼을 가져오는데에 문제가 발생하였습니다." << "\nError Message:" << _db_message);
	HTTP_DEBUG((sqlite3_exec(_db, ("DELETE FROM request WHERE Expires != 0 AND Expires <= " + _current_time_query.str()).c_str(), NULL, NULL, &_db_message)),
			   "Expires만료기간이 지난 컬럼을 삭제하는데에 문제가 발생하였습니다." << "\nError Message:" << _db_message);

#ifdef DEBUG
	HTTP_DEBUG((sqlite3_exec(_db, ("SELECT * FROM request WHERE (" + _current_time_query.str() + " - Last_Updated) > Max_Age;").c_str(), HTTPManager::SQLMaxAge, NULL, &_db_message)),
			   "Max_Age를 가진 컬럼을 가져오는데에 문제가 발생하였습니다." << "\nError Message:" << _db_message);
#endif

	HTTP_DEBUG((sqlite3_exec(_db, ("DELETE FROM request WHERE (" + _current_time_query.str() + " - Last_Updated) > Max_Age;").c_str(), HTTPManager::SQLMaxAge, NULL, &_db_message)),
			   "Max_Age를 가진 컬럼을 가져오는데에 문제가 발생하였습니다." << "\nError Message:" << _db_message);
	
	HTTP_DEBUG((sqlite3_close(_db)),
			   "sqlite를 close하는데 실패하였습니다." << "\nError Message:" << sqlite3_errmsg(_db) << "\n");
}

CURLM* HTTPManager::GetCURLMulti() {
	return multi_handle;
}

HTTPManager::HTTPManager(std::string _HTTP_MANAGER_DIRECTORY,
						 std::string _HTTP_MANAGER_DIRECTORY_TEMPORARY,
						 std::string _HTTP_MANAGER_DIRECTORY_CACHE,
						 std::string _HTTP_MANAGER_CACHE_DB,
						 std::string _HTTP_MANAGER_DIRECTORY_HEADER,
						 std::string _HTTP_MANAGER_DIRECTORY_BODY) {
	HTTP_DEBUG((__http_manager), "이미 생성된 매니져가 있습니다.");
	HTTP_MANAGER_DIRECTORY=_HTTP_MANAGER_DIRECTORY;
	HTTP_MANAGER_DIRECTORY_TEMPORARY=_HTTP_MANAGER_DIRECTORY_TEMPORARY;
	HTTP_MANAGER_DIRECTORY_CACHE=_HTTP_MANAGER_DIRECTORY_CACHE;
	HTTP_MANAGER_CACHE_DB=_HTTP_MANAGER_CACHE_DB;
	HTTP_MANAGER_DIRECTORY_HEADER=_HTTP_MANAGER_DIRECTORY_HEADER;
	HTTP_MANAGER_DIRECTORY_BODY=_HTTP_MANAGER_DIRECTORY_BODY;
	
	ReadyDirectory();
	ReadyDB();
	
	still_running = 0;
	messages_left = 0;
	
	curl_global_init(CURL_GLOBAL_ALL);
	multi_handle = curl_multi_init();
	curl_multi_setopt(multi_handle, CURLMOPT_MAXCONNECTS, HTTP_MAX_CONNECTIONS);
}

HTTPManager::~HTTPManager() {
	client_list.clear();
}

void* HTTPManager::UpdatePrev(void* _argument) {
	HTTPManager* _manager = (HTTPManager*)_argument;
	CURLMcode _code = curl_multi_perform(_manager->multi_handle, &_manager->still_running);
	if(_code == CURLM_OK || _code == CURLM_CALL_MULTI_PERFORM) {
		return NULL;
	} else {
		curl_multi_cleanup(_manager->multi_handle);
		
		_manager->still_running = 0;
		_manager->messages_left = 0;
		
		_manager->multi_handle = curl_multi_init();
		curl_multi_setopt(_manager->multi_handle, CURLMOPT_MAXCONNECTS, HTTP_MAX_CONNECTIONS);
		
		std::cout << "curl_multi_perform error: " << _code << "\n";
		CURLMcode* _code_mem = (CURLMcode*)malloc(sizeof(CURLMcode));
		*_code_mem = _code;
		return _code_mem;
	}
}

void* HTTPManager::UpdateNext(void* _argument) {
	HTTPManager* _manager = (HTTPManager*)_argument;
	
	//받은 응답 메세지 큐에서 확인하고 이벤트 전달
	CURLMsg* _message = curl_multi_info_read(_manager->multi_handle, &_manager->messages_left);
	while(_message) {
		if(_message->msg == CURLMSG_DONE) {
			THCManager::ThreadCategory _category = THCManager::Share()->GetCategoryList("http");
			for(std::list<THCCommand*>::iterator _it = _category.list_command.begin(); _it != _category.list_command.end(); _it++) {
				HTTPClient* _client = (HTTPClient*)*_it;
				if(_message->easy_handle == _client->GetCURL()) {
					CURLcode _result = _message->data.result;
					_client->MessageReciever(_result);
					break;
				}
			}
		}
		_message = curl_multi_info_read(_manager->multi_handle, &_manager->messages_left);
	}
	
	return NULL;
}

int HTTPManager::GetRunningHTTP() {
	return still_running;
}

bool HTTPManager::CleanCache() {
	if(client_list.size() || still_running || messages_left)
		return false;
	
	RemoveDirectory(HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY);
	RemoveDirectory(HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE);
	ReadyDirectory();
	ReadyDB();
	
	return true;
}
