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
	
	//example.com으로 주소설정
	request.SetURL("http://www.example.com");
	
	//POST로 보낼것을 지정
	request.SetMethod(HTTPRequest::HTTPMethod_POST);
	
	//변수 name을 Vanilla Breeze로 설정
	std::stringstream stream;
	stream << "Vanilla Breeze";
	request.AppendBody("name", stream.str());
	
	//변수 content를 Game으로 설정
	stream.str("");
	stream << "Game";
	request.AppendBody("content", stream.str());
	
	//변수 number를 100으로 설정
	stream.str("");
	stream << 100;
	request.AppendBody("number", stream.str());
	
	//전송
	sender.Send("Tag", request);
	
	while(true) {
		HTTPManager::Share()->Update();
		
		//응답을 받았으면 종료
		if(!is_connected)
			break;
	}
}