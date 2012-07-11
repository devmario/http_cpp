#include "HTTPClient.h"
#include "HTTPManager.h"
#include "HTTP.h"

//임시파일경로에 붙을 유니크 아이디값을 위한 값
unsigned long __http_client_identity = 0;

int HTTPClient::SQLMatchURLCallback(void* _reference, int _field_length, char** _field, char** _field_name) {
	HTTPClient* _client = (HTTPClient*)_reference;
	for(int _i = 0; _i < _field_length; _i++) {
		if(strcmp(_field_name[_i], "id") == 0) {
			if(_field[_i]) {
				_client->db_id = std::string(_field[_i]);
				_client->cache_header_file_path = HTTPManager::Share()->_GetCacheHeaderPath() + "/" + _client->db_id;
				_client->cache_body_file_path = HTTPManager::Share()->_GetCacheBodyPath() + "/" + _client->db_id;
			}
		} else if(strcmp(_field_name[_i], "Expires") == 0) {
			if(_field[_i])
				_client->db_expires = HTTP::QueryToTime(_field[_i]);
		} else if(strcmp(_field_name[_i], "Last_Modified") == 0) {
			if(_field[_i])
				_client->db_last_modified = HTTP::QueryToTime(_field[_i]);
		}
	}
    return 0;
};

HTTPClient::HTTPClient(HTTPEvent* _event, 
						 const std::string _tag,
						 const HTTPRequest _request) {
	cache_type = HTTPResponse::CacheType_None;
	
	last_modified = 0;
	expires = 0;
	
	db_last_modified = 0;
	db_expires = 0;
	
	tag = _tag;
	paused = false;
	
	event = _event;
	request = _request;
	
	header_chunk = NULL;
	http_post = NULL;
	last_post = NULL;
	
	//디비에서 URL이랑 매치되는 필드가져오기
	sqlite3* _db = NULL;
	char* _db_message = NULL;
	HTTP_DEBUG((sqlite3_open(HTTPManager::Share()->_GetDBPath().c_str(), &_db)),
			   "sqlite 디비 열기에 실패하였습니다.");
	std::string _query = "SELECT * FROM request WHERE url='" + request.url + "' LIMIT 1";
	HTTP_DEBUG((sqlite3_exec(_db, _query.c_str(), SQLMatchURLCallback, this, &_db_message)), 
			   "sqllite 디비 검색에 실패하였습니다." << "\nError Message:" << _db_message);
	HTTP_DEBUG((sqlite3_close(_db)), 
			   "sqlite 디비 닫기에 실패하였습니다.");
	
	if(db_id.length()) {
		if(db_expires > HTTP::CurrentTime()) {
			//저장된것 사용
			cache_type = HTTPResponse::CacheType_Expires;
		} else {
			//연결(이후 Last-Modified로 캐쉬 체크)
			cache_type = HTTPResponse::CacheType_Last_Modified;
		}
	} else {
		//처음 다운
		cache_type = HTTPResponse::CacheType_None;
	}
	
	if(!request.use_cache)
		cache_type = HTTPResponse::CacheType_None;
	
	//파일 준비
	ReadyFile();
	
	//연결이 필요한경우는 curl생성
	if(cache_type != HTTPResponse::CacheType_Expires)
		curl = curl_easy_init();
	
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, request.GetURL().c_str());
		
		ReadyHeader();
		ReadyBody();
		
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HTTPClient::ReadBody);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, ReadHeader);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, this);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, HTTPClient::Progress);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
	}
	
	event->_AddClient(this);
	HTTPManager::Share()->_AddClient(this);
}

HTTPClient::~HTTPClient() {
	HTTPManager::Share()->_RemoveClient(this);
	if(curl)
		curl_easy_cleanup(curl);
	if(header_chunk)
		curl_slist_free_all(header_chunk);
	if(http_post)
		curl_formfree(http_post);
	
	if(body.is_open())
		body.close();
	if(header.is_open())
		header.close();
	
//	remove(body_file_path.c_str());
//	remove(header_file_path.c_str());
	
	event->_RemoveClient(this);
}

const std::string HeaderLineFindField(const std::string _chunk_name, char* _stream, size_t _length, bool* _finded) {
	std::string _field;
	_field.append(_stream, _length);
	
	std::string _chunk = _chunk_name + ": ";
	bool _match = true;
	int _n = 0;
	for(int _i = 0; _i < _chunk.length(); _i++) {
		if(_chunk.at(_i) != _field.at(_i)) {
			_match = false;
			break;
		}
		_n = _i;
	}
	_n++;
	if(_match) {
		_field.replace(0, _n, "");
		int _i = 0;
		char _buffer = _field.at(_i);
		while(true) {
			if(_buffer == '\n' || _buffer == '\r') {
				_field.replace(_i, 1, "");
			} else {
				_i++;
			}
			if(_i < _field.length())
				_buffer = _field.at(_i);
			else
				break;
		}
	}
	
	*_finded = _match;
	return _field;
}

size_t HTTPClient::ReadHeader(char* _stream, size_t _size, size_t _count, void* _pointer) {
	HTTPClient* _client = (HTTPClient*)_pointer;
	size_t _length = _size * _count;
	
	bool _finded;
	std::string _content;
	
	if(_client->last_modified == 0) {
		_finded = false;
		_content = HeaderLineFindField("Last-Modified", _stream, _length, &_finded);
		if(_finded) {
			_client->last_modified = HTTP::HeaderToTime(_content);
		}
	}
	
	if(_client->expires == 0) {
		_finded = false;
		_content = HeaderLineFindField("Expires", _stream, _length, &_finded);
		if(_finded) {
			_client->expires = HTTP::HeaderToTime(_content);
		}
	}
	
	_client->header.write(_stream, _length);
	return _length;
}

size_t HTTPClient::ReadBody(char* _stream, size_t _size, size_t _count, void* _pointer) {
	HTTPClient* _client = (HTTPClient*)_pointer;
	size_t _length = _size * _count;
	_client->body.write(_stream, _length);
	return _length;
}

int HTTPClient::Progress(void* _client_pointer, 
						  double _download_total, 
						  double _download_now, 
						  double _upload_total, 
						  double _upload_now) {
	HTTPClient* _client = (HTTPClient*)_client_pointer;
	
	if(_client->paused) {
		curl_easy_pause(_client->curl, CURLPAUSE_ALL);
	} else {
		curl_easy_pause(_client->curl, CURLPAUSE_CONT);
	}
	
	_client->event->Progress(_client->tag,
							 _download_total, 
							 _download_now, 
							 _upload_total, 
							 _upload_now);
	
	return 0;
}

void HTTPClient::ReadyFile() {
	std::stringstream _header_path_stream;
	_header_path_stream << HTTPManager::Share()->_GetTemporaryHeaderPath() + "/";
	_header_path_stream << __http_client_identity;
	header_file_path = _header_path_stream.str();
	
	header.open(header_file_path.c_str());
	HTTP_DEBUG(!header.is_open(), std::string("응답헤더를 저장할 파일이 열리지 않습니다.HTTPConfig.h의 HTTP_MANAGER_DIRECTORY, HTTP_RESPONSE_HEADER_FILE_PREFIX의 경로를 확인해주세요.권한문제 혹은 잘못된 경로일 수 있습니다.\n경로: ") + header_file_path + "\n");
	
	std::stringstream _body_path_stream;
	_body_path_stream << HTTPManager::Share()->_GetTemporaryBodyPath() + "/";
	_body_path_stream << __http_client_identity;
	body_file_path = _body_path_stream.str();
	
	body.open(body_file_path.c_str());
	HTTP_DEBUG(!body.is_open(), std::string("응답바디를 저장할 파일이 열리지 않습니다.HTTPConfig.h의 HTTP_MANAGER_DIRECTORY, HTTP_RESPONSE_BODY_FILE_PREFIX의 경로를 확인해주세요.권한문제 혹은 잘못된 경로일 수 있습니다.\n경로: ") + body_file_path + "\n");
	
	__http_client_identity++;
}

void HTTPClient::ReadyHeader() {
	for(int i = 0; i < request.header_vector.size(); i++) {
		HTTPRequest::HeaderField _field = request.header_vector[i];
		header_chunk = curl_slist_append(header_chunk,
										 std::string(_field.name + ": " + _field.content).c_str());
	}
	
	if(header_chunk)
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_chunk);
}

void HTTPClient::ReadyBody() {
	switch(request.GetMethod()) {
		case HTTPRequest::HTTPMethod_POST:
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
			for(int i = 0; i < request.body_vector.size(); i++) {
				HTTPRequest::BodyField _field = request.body_vector[i];
				
				CURLformoption _option;
				std::string _content;
				
				if(_field.content.length()) {
					_option = CURLFORM_COPYCONTENTS;
					_content = _field.content;
				} else if(_field.file_path.length()) {
					_option = CURLFORM_FILE;
					_content = _field.file_path;
				}
				
				curl_formadd(&http_post, &last_post,
							 CURLFORM_COPYNAME, _field.name.c_str(),
							 _option, _content.c_str(),
							 CURLFORM_END);
			}
			if(http_post)
				curl_easy_setopt(curl, CURLOPT_HTTPPOST, http_post);
			
			break;
			
		default:
			break;
	}
}

bool HTTPClient::Pause() {
	if(paused)
		return false;
	paused = true;
	return true;
}

CURL* HTTPClient::GetCURL() {
	return curl;
}

const std::string HTTPClient::GetTag() {
	return tag;
}

bool HTTPClient::Resume() {
	if(!paused)
		return false;
	paused = false;
	return true;
}

bool HTTPClient::UpdateDB() {
	if(header.is_open())
		header.close();
	if(body.is_open())
		body.close();
	
	std::string _query;
	bool _need_update = false;
	bool _is_insert = false;
	if(cache_type == HTTPResponse::CacheType_None) {
		
		std::string _expires;
		if(expires)
			_expires = "'" + HTTP::TimeToQuery(expires) + "'";
		else {
			_expires = "NULL";
		}
		
		std::string _last_modified;
		if(last_modified)
			_last_modified = "'" + HTTP::TimeToQuery(last_modified) + "'";
		else
			_last_modified = "NULL";
		
		if(db_id.length()) {
			_query = "INSERT OR REPLACE INTO request (url, Expires, Last_Modified) VALUES ('" + request.url + "', " + _expires + ", " + _last_modified + ")";
			_is_insert = true;
		} else {
			_query = "UPDATE request SET Last_Modified='" + HTTP::TimeToQuery(last_modified) + "', Expires='" + _expires + "' WHERE id='" + db_id + "'";
		}
		_need_update = true;
	
	} else if(cache_type == HTTPResponse::CacheType_Last_Modified) {
		if(last_modified && db_last_modified) {
			if(last_modified != db_last_modified) {
				_query = "UPDATE request SET Last_Modified='" + HTTP::TimeToQuery(last_modified) + "' WHERE id='" + db_id + "'";
				_need_update = true;
			}
		}
	}
	
	if(!_need_update) {
		header_file_path = HTTPManager::Share()->_GetCacheHeaderPath() + "/" + db_id;
		body_file_path = HTTPManager::Share()->_GetCacheBodyPath() + "/" + db_id;
		return false;
	}
	
	sqlite3* _db = NULL;
	char* _db_message = NULL;
	
	HTTP_DEBUG((sqlite3_open(HTTPManager::Share()->_GetDBPath().c_str(), &_db)),
			   "열기실패");
	HTTP_DEBUG((sqlite3_exec(_db, _query.c_str(), NULL, NULL, &_db_message)),
			   "error:" << _db_message);
	
	if(_is_insert) {
		_query = "SELECT * FROM request WHERE url='" + request.url + "' LIMIT 1";
		HTTP_DEBUG((sqlite3_exec(_db, _query.c_str(), SQLMatchURLCallback, this, &_db_message)), 
				   "sqllite 디비 검색에 실패하였습니다." << "\nError Message:" << _db_message);
	}
	
	HTTP_DEBUG((sqlite3_close(_db)),
			   "닫기실패");
	
	rename(header_file_path.c_str(), cache_header_file_path.c_str());
	rename(body_file_path.c_str(), cache_body_file_path.c_str());
	
	return true;
}

bool HTTPClient::PrevUpdateAndGetNeedDelete() {
	if(cache_type == HTTPResponse::CacheType_Expires) {
		event->Receive(tag, HTTPResponse(cache_header_file_path, cache_body_file_path, cache_type));
		return true;
	} else if(cache_type == HTTPResponse::CacheType_Last_Modified) {
		if(db_last_modified && last_modified) {
			if(last_modified == db_last_modified) {
				event->Receive(tag, HTTPResponse(cache_header_file_path, cache_body_file_path, cache_type));
				return true;
			}
		}
	}
	return false;
}

void HTTPClient::MessageReciever(CURLcode _code) {
	if(header.is_open())
		header.close();
	if(body.is_open())
		body.close();
	if(_code == CURLE_OK) {
		UpdateDB();
		event->Receive(tag, HTTPResponse(cache_header_file_path, cache_body_file_path, cache_type));
	} else {
		event->Error(tag, _code);
	}
}
