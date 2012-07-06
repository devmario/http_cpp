#include "HTTPManager.h"
#include "HTTP.h"

HTTPManager* __http_manager = NULL;
pthread_t __http_thread = NULL;

HTTPManager* HTTPManager::Share() {
	if(__http_manager == NULL) {
		__http_thread = pthread_self();
		__http_manager = new HTTPManager();
	}
	return __http_manager;
}

const std::string HTTPManager::_GetTemporaryHeaderPath() {
	return temporary_header_path;
}

const std::string HTTPManager::_GetTemporaryBodyPath() {
	return temporary_body_path;
}

const std::string HTTPManager::_GetCacheHeaderPath() {
	return cache_header_path;
}

const std::string HTTPManager::_GetCacheBodyPath() {
	return cache_body_path;
}

const std::string HTTPManager::_GetDBPath() {
	return db_path;
}

void HTTPManager::ReadyDirectory() {
	HTTP_DEBUG((access(HTTP_MANAGER_DIRECTORY.c_str(), X_OK | W_OK | R_OK) != 0), "임시파일을 저장할 공간에 권한이 없습니다.");
	
	//임시폴더 Reset
	temporary_path = HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY;
	if(access(temporary_path.c_str(), F_OK) == 0) {
		system(std::string("rm -rf " + temporary_path).c_str());
	}
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
			std::string _header_path = _manager->_GetCacheHeaderPath() + "/" + _field_content[_i];
			remove(_header_path.c_str());
			
			std::string _body_path = _manager->_GetCacheBodyPath() + "/" + _field_content[_i];
			remove(_body_path.c_str());
		}
	}
	return 0;
}

void HTTPManager::ReadyDB() {
	//디비 연결하고 테이블 생성(테이블 없을시)
	sqlite3* _db = NULL;
	char* _db_message = NULL;
	HTTP_DEBUG((sqlite3_open(_GetDBPath().c_str(), &_db)), 
			   "sqllite 시동에 실패하였습니다.");
	HTTP_DEBUG((sqlite3_exec(_db, "CREATE TABLE IF NOT EXISTS request (id integer primary key, url TEXT, Expires DATETIME, Last_Modified DATETIME)", NULL, this, &_db_message)),
			   "sqllite 테이블 생성에 실패하였습니다." << "\nError Message:" << _db_message);
	
	std::string _current_time_query = HTTP::TimeToQuery(HTTP::CurrentTime());
	
	HTTP_DEBUG((sqlite3_exec(_db, std::string("SELECT * FROM request WHERE Expires != NULL AND Expires <= '" + _current_time_query + "'").c_str(), HTTPManager::SQLExpires, this, &_db_message)),
			   "Expires만료기간이 지난 컬럼을 가져오는데에 문제가 발생하였습니다." << "\nError Message:" << _db_message);
	HTTP_DEBUG((sqlite3_exec(_db, std::string("DELETE FROM request WHERE Expires <= '" + _current_time_query + "'").c_str(), NULL, NULL, &_db_message)),
			   "Expires만료기간이 지난 컬럼을 삭제하는데에 문제가 발생하였습니다." << "\nError Message:" << _db_message);
	HTTP_DEBUG((sqlite3_close(_db)), 
			   "sqlite를 close하는데 실패하였습니다.");
}

HTTPManager::HTTPManager() {
	HTTP_DEBUG((__http_manager), "이미 생성된 매니져가 있습니다.");
	HTTP_DEBUG((__http_thread != pthread_self()), "한쓰레드에서 호출해주세요.");
	
	ReadyDirectory();
	ReadyDB();
	
	still_running = 0;
	messages_left = 0;
	
	curl_global_init(CURL_GLOBAL_ALL);
	multi_handle = curl_multi_init();
	curl_multi_setopt(multi_handle, CURLMOPT_MAXCONNECTS, HTTP_MAX_CONNECTIONS);
}

HTTPManager::~HTTPManager() {
	HTTP_DEBUG((__http_thread != pthread_self()), "한쓰레드에서 호출해주세요.");
	client_list.clear();
}

void HTTPManager::_AddClient(_HTTPClient* _client) {
	HTTP_DEBUG((__http_thread != pthread_self()), "한쓰레드에서 호출해주세요.");
	client_list.push_back(_client);
	if(_client->curl)
		curl_multi_add_handle(multi_handle, _client->curl);
}

void HTTPManager::_RemoveClient(_HTTPClient* _client) {
	HTTP_DEBUG((__http_thread != pthread_self()), "한쓰레드에서 호출해주세요.");
	client_list.remove(_client);
	if(_client->curl)
		curl_multi_remove_handle(multi_handle, _client->curl);
}

void HTTPManager::Update() {
	HTTP_DEBUG((__http_thread != pthread_self()), "한쓰레드에서 호출해주세요.");
	
	std::list<_HTTPClient*>::iterator _iterator = client_list.begin();
	while(_iterator != client_list.end()) {
		_HTTPClient* _client = *_iterator;
		_iterator++;
		if(_client->PrevUpdateAndGetNeedDelete())
			delete _client;
	}
	
	curl_multi_perform(multi_handle, 
					   &still_running);
	
	//받은 응답 메세지 큐에서 확인하고 이벤트 전달, 이후 list에서 제거
	CURLMsg* _message = curl_multi_info_read(multi_handle, &messages_left);
	while(_message) {
		if(_message->msg == CURLMSG_DONE) {
			_HTTPClient* _client = NULL;
			
			for(_iterator = client_list.begin(); _iterator != client_list.end(); _iterator++) {
				if(_message->easy_handle == (*_iterator)->curl) {
					CURLcode _result = _message->data.result;
					_client = *_iterator;
					_client->MessageReciever(_result);
					break;
				}
			}
			
			if(_client) {
				delete _client;
			}
		}
		_message = curl_multi_info_read(multi_handle, &messages_left);
	}
}

int HTTPManager::GetRunningHTTP() {
	return still_running;
}