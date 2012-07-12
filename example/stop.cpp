#include "HTTP.h"
#include <term.h>  
#include <termios.h>

//입력테스팅하기(엔터없어도 체크)
static int getch(void) {  
	int ch;  
	struct termios buf;  
	struct termios save;  
	
	tcgetattr(0, &save);  
	buf = save;  
	buf.c_lflag &= ~(ICANON|ECHO);  
	buf.c_cc[VMIN] = 1;  
	buf.c_cc[VTIME] = 0;  
	tcsetattr(0, TCSAFLUSH, &buf);  
	ch = getchar();  
	tcsetattr(0, TCSAFLUSH, &save);  
	return ch;  
}

//플래그
bool is_stop = false;
bool ended_connection = false;

//출력을 위한 스티림
std::stringstream progress;
std::stringstream status;

//입력대기 쓰래드
static void* wait_input(void* _reference) {
	while(true) {
		int code = getch();
		if(code == 's') {
			if(!is_stop) {
				status.str("");
				status << "Stop";
				progress.str("");
				is_stop = true;
			}
		} else if(code == 'e') {
			exit(0);
		}
	}
} 

//콜백받는 클래스(출력스트림을 설정한다)
class Sender : public HTTPEvent {
public:
    virtual void Receive(const std::string _tag, 
                         HTTPResponse _response) {
		status.str("");
		status << "Receive";
		
		progress.str("");
		
		ended_connection = true;
    }
	
    virtual void Error(const std::string _tag, 
                       CURLcode _error_code) {
		status.str("");
		status << "Error";
		
		progress.str("");
		
		ended_connection = true;
    }
	
    virtual void Progress(const std::string _tag,
                          double _download_total, 
                          double _download_now, 
                          double _upload_total, 
                          double _upload_now) {
		if(_download_now) {
			status.str("");
			status << "Progress";
			
			progress.str("");
			progress << (int)((_download_now / _download_total) * 100) << "%";
		}
    }
};

int main(void) {
	//입력대기쓰래드
	pthread_t input_thread;
	pthread_create(&input_thread, NULL, wait_input, NULL);
	
	status << "Ready";
	
    Sender sender;
	HTTPRequest request;
	request.SetURL("http://www.universetoday.com/wp-content/uploads/2010/10/spiral-galaxy.jpg");
	sender.Send("big_image", request);
	
    while(true) {
		//화면지우기(MacOSX)
		system("clear\n");
		if(!is_stop)
			std::cout << "소문자 s을 누르시면 다운을 종료합니다.\n";
		std::cout << "소문자 e를 누르시면 프로그램이 종료됩니다.\n";
		if(!is_stop)
			std::cout << status.str() << " " << progress.str() << "\n";
		
		//일시중지 합니다.
		if(is_stop)
			sender.Stop("big_image");
        
		HTTPManager::Share()->Update();
		
		//일시중지가 되면 연결이 사라져 ended_connection을 true로 설정해주는 Event로 진입을 할수
		//없으므로 다음 if문은 실행되지 않습니다.
		if(ended_connection) {
			sender.Send("big_image", request);
			ended_connection = false;
		}
		
		usleep(10000);
    }
}