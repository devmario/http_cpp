#include "HTTP.h"

bool ended_connection = false;

//콜백받는 클래스
class Sender : public HTTPEvent {
public:
    virtual void Receive(const std::string _tag, 
                         HTTPResponse _response) {
		ended_connection = true;
    }
	
    virtual void Error(const std::string _tag, 
                       CURLcode _error_code) {
		//에러코드를 출력합니다.
		std::cout << "Error " << _tag << "의 에러코드 : " << _error_code << "\n";
		ended_connection = true;
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
	request.SetURL("http://www.jangwunheui.com");
	sender.Send("없는주소", request);
	std::cout << "Ready\n";
	
    while(true) {
		HTTPManager::Share()->Update();
		
		usleep(10000);
		
		//연결이 종료됬는지 확인합니다.
		if(ended_connection)
			break;
    }
}