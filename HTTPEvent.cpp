#include "HTTPEvent.h"
#include "HTTPManager.h"

HTTPEvent::HTTPEvent() : THCEvent() {
	THCManager::Share()->SetCategory("http", 0.1, HTTPManager::Share(), HTTPManager::UpdatePrev, HTTPManager::UpdateNext);
};

HTTPEvent::~HTTPEvent() {
}

void HTTPEvent::Send(const std::string _tag, const HTTPRequest _request) {
	THCEvent::Run("http", _tag, new HTTPClient(this, _tag, _request));
}

bool HTTPEvent::Pause(const std::string _tag) {
	HTTPClient* _client = (HTTPClient*)FindTagCommand(_tag);
	if(_client)
		return _client->Pause();
	return false;
}

bool HTTPEvent::Resume(const std::string _tag) {
	HTTPClient* _client = (HTTPClient*)FindTagCommand(_tag);
	if(_client)
		return _client->Resume();
	return false;
}

bool HTTPEvent::Stop(const std::string _tag) {
	return THCEvent::Stop("http", _tag);
}

void HTTPEvent::CompleteCommand(std::string _category, std::string _tag, THCCommand* _command) {
	HTTPClient* _client = (HTTPClient*)_command;
	if(_client->is_raise_error_multi_handle) {
		ErrorMulti(_tag, _client->code_multi);
	} else {
		if(_client->is_recieve) {
			if(_client->code == CURLE_OK) {
				Receive(_tag, _client->response);
			} else {
				Error(_tag, _client->code);
			}
		} else if(_client->is_use_cache) {
			if(_client->is_use_cache) {
				Receive(_tag, _client->response);
			}
		}
	}
}