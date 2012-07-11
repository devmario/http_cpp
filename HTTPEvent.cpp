#include "HTTPEvent.h"
#include "HTTPManager.h"

HTTPEvent::~HTTPEvent() {
	std::list<HTTPClient*>::iterator _iterator = client_list.begin();
	while(_iterator != client_list.end()) {
		HTTPClient* _client = *_iterator;
		_iterator++;
		delete _client;
	}
	client_list.clear();
}

void HTTPEvent::Send(const std::string _tag, const HTTPRequest _request) {
	new HTTPClient(this, _tag, _request);
}

void HTTPEvent::AddClient(HTTPClient* _client) {
	client_list.push_back(_client);
}

void HTTPEvent::RemoveClient(HTTPClient* _client) {
	client_list.remove(_client);
}

HTTPClient* HTTPEvent::FindTagClient(const std::string _tag) {
	std::list<HTTPClient*>::iterator _iterator;
	for(_iterator = client_list.begin(); _iterator != client_list.end(); _iterator++) {
		HTTPClient* _client = *_iterator;
		if(_client->GetTag() == _tag) {
			return _client;
		}
	}
	return NULL;
}

bool HTTPEvent::Pause(const std::string _tag) {
	HTTPClient* _client = FindTagClient(_tag);
	if(_client)
		return _client->Pause();
	return false;
}

bool HTTPEvent::Resume(const std::string _tag) {
	HTTPClient* _client = FindTagClient(_tag);
	if(_client)
		return _client->Resume();
	return false;
}

bool HTTPEvent::Stop(const std::string _tag) {
	HTTPClient* _client = FindTagClient(_tag);
	if(_client) {
		delete _client;
		return true;
	}
	return false;
}

void HTTPEvent::Clean() {
	std::vector<HTTPClient*> _vector;
	std::list<HTTPClient*>::iterator _iterator = client_list.begin();
	while(_iterator != client_list.end()) {
		HTTPClient* _client = *_iterator;
		_vector.push_back(_client);
		_iterator++;
	}
	for(int i = 0; i < _vector.size(); i++)
		delete _vector[i];
	client_list.clear();
}