#include "HTTP.h"

//응답받았는지의 여부
bool is_connected = true;

//콜백 클래스
class Sender : public HTTPEvent {
public:
	//응답성공
	virtual void Receive(const std::string _tag, 
						 HTTPResponse _response) {
		std::cout << "Receive " << _tag << std::endl;
		std::cout << _response.GetHeaderContent() << std::endl;
		std::cout << _response.GetBodyContent() << std::endl;
		
		is_connected = false;
	}
	
	//에러
	virtual void Error(const std::string _tag, 
					   CURLcode _error_code) {
		std::cout << "Error " << _error_code << " " << _tag << std::endl;
		
		is_connected = false;
	}
	
	//진행율
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
	
	//주소설정
	request.SetURL("http://www.example.com");
	//모질라브라우저를 사용한다고 헤더에 설정
	request.AppendHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/12.0");
	
	//전송
	sender.Send("Tag", request);
	
	while(true) {
		HTTPManager::Share()->Update();
		
		//응답을 받았으면 종료
		if(!is_connected)
			break;
	}
}