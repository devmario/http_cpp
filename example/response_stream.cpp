#include "HTTP.h"

bool ended_connection = false;

//콜백받는 클래스
class Sender : public HTTPEvent {
public:
    virtual void Receive(const std::string _tag, 
                         HTTPResponse _response) {
		std::cout << "Receive " << _tag << "\n\n";
		
		//바디내용을 출력합니다.
		std::ifstream _stream;
		_stream.open(_response.GetBodyFilePath().c_str());
		if(_stream.is_open()) {
			char _buffer[0xFF] = {NULL,};
			while(_stream.good()) {
				_stream.read(_buffer, 0xFF);
				std::cout << _buffer;
				memset(_buffer, NULL, 0xFF);
			}
			_stream.close();
		}
		
		ended_connection = true;
    }
	
    virtual void Error(const std::string _tag, 
                       CURLcode _error_code) {
		//에러코드를 출력합니다.
		std::cout << "Error " << _tag << _error_code;
		
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
	request.SetURL("http://www.naver.com");
	sender.Send("네이버", request);
	std::cout << "Ready\n";
	
    while(true) {
		HTTPManager::Share()->Update();
		
		usleep(10000);
		
		//연결이 종료됬는지 확인합니다.
		if(ended_connection)
			break;
    }
}