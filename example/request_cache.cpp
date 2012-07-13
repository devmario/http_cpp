#include "HTTP.h"

//응답받았는지의 여부
bool is_connected = true;

//카운터
int count = 0;

//콜백 클래스
class Sender : public HTTPEvent {
public:
	//응답성공
	virtual void Receive(const std::string _tag, 
						 HTTPResponse _response) {
		system("clear\n");
		std::cout << "다운수 " << count << " Receive " << _tag << std::endl;
		
		count++;
		is_connected = false;
	}
	
	//에러
	virtual void Error(const std::string _tag, 
					   CURLcode _error_code) {
		system("clear\n");
		std::cout << "다운수 " << count << " Error " << _error_code << " " << _tag << std::endl;
		
		count++;
		is_connected = false;
	}
	
	//진행율
	virtual void Progress(const std::string _tag,
						  double _download_total, 
						  double _download_now, 
						  double _upload_total, 
						  double _upload_now) {
		if(_download_now) {
			system("clear\n");
			//다운로드 상태를 출력합니다.
			std::cout << "다운수 " << count << " Progress " << (int)((_download_now / _download_total) * 100) << "%" << "\n";
		}
	}
};

int main(void) {
	std::cout << "Ready\n";
	
	Sender sender;
	HTTPRequest request;
	
	//사진 주소
	request.SetURL("http://24.media.tumblr.com/tumblr_m70ncaTGhd1qa42jro1_1280.jpg");
	
	//캐쉬를 사용할것이라고 설정
	request.SetUseCache(true);
	
	//전송
	sender.Send("Tag", request);
	
	while(true) {
		HTTPManager::Share()->Update();
		
		//응답을 받았으면 다시요청
		if(!is_connected) {
			sender.Send("Tag", request);
			is_connected = true;
		}
	}
}