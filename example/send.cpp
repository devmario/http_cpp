#include "HTTP.h"

class Sender : public HTTPEvent {
public:
	virtual void Receive(const std::string _tag, 
						 HTTPResponse _response) {
		std::cout << "Receive " << _tag << std::endl << std::endl;
		std::cout << _response.GetHeaderContent() << std::endl;
		std::cout << _response.GetBodyContent() << std::endl;
	}
	
	virtual void Error(const std::string _tag, 
					   CURLcode _error_code) {
		std::cout << "Error " << _error_code << " " << _tag << std::endl;
	}
	
	virtual void Progress(const std::string _tag,
						  double _download_total, 
						  double _download_now, 
						  double _upload_total, 
						  double _upload_now) {
	}
};

int main(void) {
	Sender sender;
	HTTPRequest request;
	request.SetURL("http://www.google.com");
	sender.Send("Tag", request);
	
	while(true) {
		HTTPManager::Share()->Update();
	}
}