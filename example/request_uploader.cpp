#include "HTTP.h"

//응답받았는지의 여부
bool is_connected = true;

//콜백 클래스
class Sender : public HTTPEvent {
public:
	//응답성공
	virtual void Receive(const std::string _tag, 
						 HTTPResponse _response) {
		system("clear\n");
		std::cout << "Receive " << _tag << std::endl;
		std::cout << _response.GetHeaderContent() << std::endl;
		std::cout << _response.GetBodyContent() << std::endl;
		
		is_connected = false;
	}
	
	//에러
	virtual void Error(const std::string _tag, 
					   CURLcode _error_code) {
		system("clear\n");
		std::cout << "Error " << _error_code << " " << _tag << std::endl;
		
		is_connected = false;
	}
	
	//진행율
	virtual void Progress(const std::string _tag,
						  double _download_total, 
						  double _download_now, 
						  double _upload_total, 
						  double _upload_now) {
		if(_upload_now) {
			system("clear\n");
			//업로드 상태를 출력합니다.
			std::cout << " Progress " << (int)((_upload_now / _upload_total) * 100) << "%" << "\n";
		}
	}
};

int main(void) {
	Sender sender;
	HTTPRequest request;
	
	//업로드 주소설정
	request.SetURL("http://example.com");
	
	//POST
	request.SetMethod(HTTPRequest::HTTPMethod_POST);
	
	//전송할 파일을 추가(2개 전송)
	request.AppendBodyWithFile("file", "red.png");
	request.AppendBodyWithFile("file", "blue.png");
	
	//전송
	sender.Send("Tag", request);
	
	while(true) {
		HTTPManager::Share()->Update();
		
		//응답을 받았으면 종료
		if(!is_connected)
			break;
	}
}