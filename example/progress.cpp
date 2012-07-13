#include "HTTP.h"

bool ended_connection = false;

//콜백받는 클래스
class Sender : public HTTPEvent {
public:
    virtual void Receive(const std::string _tag, 
                         HTTPResponse _response) {
		//내용을 출력합니다.
		system("clear\n");
		std::cout << "Receive " << _tag << "\n";
		
		ended_connection = true;
    }
	
    virtual void Error(const std::string _tag, 
                       CURLcode _error_code) {
		//에러코드를 출력합니다.
		system("clear\n");
		std::cout << "Error " << _tag << _error_code << "\n";
		
		ended_connection = true;
    }
	
    virtual void Progress(const std::string _tag,
                          double _download_total, 
                          double _download_now, 
                          double _upload_total, 
                          double _upload_now) {
		if(_download_now) {
			system("clear\n");
			//다운로드 상태를 출력합니다.
			std::cout << "Progress " << (int)((_download_now / _download_total) * 100) << "%" << "\n";
		}
    }
};

int main(void) {
	Sender sender;
	
	HTTPRequest request;
	request.SetURL("http://24.media.tumblr.com/tumblr_m70ni1c9gx1qa42jro1_1280.jpg");
	sender.Send("테리리차드슨엄마", request);
	std::cout << "Ready\n";
	
	while(true) {
		HTTPManager::Share()->Update();
		usleep(10000);
		//연결완료시 프로그램 종료
		if(ended_connection)
			break;
	}
}