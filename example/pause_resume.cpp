#include "HTTP.h"
#include <term.h>  
#include <termios.h>

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

bool is_pause = false;
bool ended_connection = false;

std::stringstream progress;
std::stringstream status;

static void* wait_input(void* _reference) {
	while(true) {
		int code = getch();
		if(code == 'p') {
			if(!is_pause) {
				status.str("");
				status << "Pause";
				progress.str("");
				is_pause = true;
			}
		} else if(code == 'r') {
			if(is_pause) {
				status.str("");
				status << "Resume";
				progress.str("");
				is_pause = false;
			}
		} else if(code == 'e') {
			exit(0);
		}
	}
}

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
	pthread_t input_thread;
	pthread_create(&input_thread, NULL, wait_input, NULL);
	
	status << "Ready";
	
    Sender sender;
	HTTPRequest request;
	request.SetURL("http://www.universetoday.com/wp-content/uploads/2010/10/spiral-galaxy.jpg");
	sender.Send("big_image", request);
	
    while(true) {
		system("clear\n");
		if(is_pause)
			std::cout << "소문자 r을 누르시면 활성화 됩니다.\n";
		else
			std::cout << "소문자 p를 누르시면 일시정지 됩니다.\n";
		std::cout << "소문자 e를 누르시면 프로그램이 종료됩니다.\n";
		std::cout << status.str() << " " << progress.str() << "\n";
		
		if(is_pause)
			sender.Pause("big_image");
		else
			sender.Resume("big_image");
        
		HTTPManager::Share()->Update();
		
		if(ended_connection) {
			sender.Send("big_image", request);
			ended_connection = false;
		}
		
		usleep(10000);
    }
}
