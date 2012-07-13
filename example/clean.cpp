#include "HTTP.h"
#include <term.h>  
#include <termios.h>

#define NUM_CONNECTION 10

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
bool is_clean = false;

//출력을 위한 스티림
std::stringstream progress[NUM_CONNECTION];
std::stringstream status[NUM_CONNECTION];

//입력대기 쓰래드
static void* wait_input(void* _reference) {
	while(true) {
		int code = getch();
		if(code == 'c') {
			if(!is_clean) {
				for(int i = 0; i < NUM_CONNECTION; i++) {
					status[i].str("");
					status[i] << "Clean";
					progress[i].str("");
				}
				is_clean = true;
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
		int num = atoi(_tag.c_str());
		status[num].str("");
		status[num] << "Receive";
		
		progress[num].str("");
    }
	
    virtual void Error(const std::string _tag, 
                       CURLcode _error_code) {
		int num = atoi(_tag.c_str());
		status[num].str("");
		status[num] << "Error";
		
		progress[num].str("");
    }
	
    virtual void Progress(const std::string _tag,
                          double _download_total, 
                          double _download_now, 
                          double _upload_total, 
                          double _upload_now) {
		int num = atoi(_tag.c_str());
		if(_download_now) {
			status[num].str("");
			status[num] << "Progress";
			
			progress[num].str("");
			progress[num] << (int)((_download_now / _download_total) * 100) << "%";
		}
    }
};

int main(void) {
	//입력대기쓰래드
	pthread_t input_thread;
	pthread_create(&input_thread, NULL, wait_input, NULL);
	
    Sender sender;
	HTTPRequest request;
	request.SetURL("http://www.universetoday.com/wp-content/uploads/2010/10/spiral-galaxy.jpg");
	for(int i = 0; i < NUM_CONNECTION; i++) {
		status[i] << "Ready";
		std::stringstream stream;
		stream << i;
		sender.Send(stream.str(), request);
	}
	
    while(true) {
		//화면지우기(MacOSX)
		system("clear\n");
		if(!is_clean)
			std::cout << "소문자 c을 누르시면 다운을 종료합니다.\n";
		std::cout << "소문자 e를 누르시면 프로그램이 종료됩니다.\n";
		if(!is_clean) {
			for(int i = 0; i < NUM_CONNECTION; i++) {
				std::cout << "Image " << i << " " << status[i].str() << " " << progress[i].str() << "\n";
			}
		}
		
		//연결을 삭제 합니다.
		if(is_clean)
			sender.Clean();
        
		HTTPManager::Share()->Update();
		
		usleep(10000);
    }
}