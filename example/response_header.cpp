#include "HTTP.h"

bool ended_connection = false;

//콜백받는 클래스
class Sender : public HTTPEvent {
public:
    virtual void Receive(const std::string _tag, 
                         HTTPResponse _response) {
		std::cout << "Receive " << _tag << "\n\n";
		
		//헤더를 파싱합니다.
		HTTPResponseHeaderField field = _response.ParseHeader();
		//필드검색
		std::cout << field.SearchField("Content-Type") << "\n";
		//스테이터스
		std::cout << field.status << "\n";
		//스테이터스코드
		std::cout << field.status_code << "\n";
		//모든필드 출력
		for(int i = 0; i < field.chunk.size(); i++) {
			std::cout << field.chunk[i].name << ":" << field.chunk[i].content << "\n";
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