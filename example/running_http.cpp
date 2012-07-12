#include "HTTP.h"

int recive_count = 0;
int send_count = 10; 

class Sender : public HTTPEvent {
public:
	virtual void Receive(const std::string _tag, 
						 HTTPResponse _response) {
		std::cout << "Receive " << _tag << std::endl;
		recive_count++;
	}
	
	virtual void Error(const std::string _tag, 
					   CURLcode _error_code) {
		std::cout << "Error " << _error_code << " " << _tag << std::endl;
		recive_count++;
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
	
	for(int i = 0; i < send_count; i++) {
		HTTPRequest request;
		request.SetURL("http://www.naver.com");
		sender.Send("Tag", request);
	}
	
	while(recive_count < send_count) {
		std::cout << HTTPManager::Share()->GetRunningHTTP() << std::endl;
		HTTPManager::Share()->Update();
		usleep(10000);
	}
}