#include "HTTPEvent.h"
#include "HTTPManager.h"

HTTPEvent::~HTTPEvent() {
	std::list<_HTTPClient*>::iterator _iterator = client_list.begin();
	while(_iterator != client_list.end()) {
		_HTTPClient* _client = *_iterator;
		_iterator++;
		delete _client;
	}
	client_list.clear();
}

void HTTPEvent::Send(const std::string _tag, const HTTPRequest _request) {
	new _HTTPClient(this, _tag, _request);
}

void HTTPEvent::_AddClient(_HTTPClient* _client) {
	client_list.push_back(_client);
}

void HTTPEvent::_RemoveClient(_HTTPClient* _client) {
	client_list.remove(_client);
}

_HTTPClient* HTTPEvent::FindTagClient(const std::string _tag) {
	std::list<_HTTPClient*>::iterator _iterator;
	for(_iterator = client_list.begin(); _iterator != client_list.end(); _iterator++) {
		_HTTPClient* _client = *_iterator;
		if(_client->tag == _tag) {
			return _client;
		}
	}
	return NULL;
}

bool HTTPEvent::Pause(const std::string _tag) {
	_HTTPClient* _client = FindTagClient(_tag);
	if(_client)
		return _client->Pause();
	return false;
}

bool HTTPEvent::Resume(const std::string _tag) {
	_HTTPClient* _client = FindTagClient(_tag);
	if(_client)
		return _client->Resume();
	return false;
}

bool HTTPEvent::Stop(const std::string _tag) {
	_HTTPClient* _client = FindTagClient(_tag);
	if(_client) {
		delete _client;
		return true;
	}
	return false;
}

void HTTPEvent::Clean() {
	std::vector<_HTTPClient*> _vector;
	std::list<_HTTPClient*>::iterator _iterator = client_list.begin();
	while(_iterator != client_list.end()) {
		_HTTPClient* _client = *_iterator;
		_vector.push_back(_client);
		_iterator++;
	}
	for(int i = 0; i < _vector.size(); i++)
		delete _vector[i];
	client_list.clear();
}