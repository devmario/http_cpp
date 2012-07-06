#include "HTTP.h"
#include <sqlite3.h>

class TestUnit : public HTTPEvent {
public:
	virtual void Receive(const std::string _tag, 
						 HTTPResponse _response) {
		std::cout << "Cache Type:" << _response.cache_type << "\n";
		std::cout << "Receive " << _tag;
		std::cout << "\n" << _response.GetHeaderFilePath();
		std::cout << "\n" << _response.GetBodyFilePath();
//		
//		std::cout << "\n\n";
//		std::cout << _response.GetHeaderContent();
//		std::cout << "\n\n";
//		std::cout << _response.GetBodyContent();
//		std::cout << "\n\n";
//		
//		std::cout << "Location field search:" << _response.SearchHeaderField("Location") << "\n";
//		std::cout << "Content-Length field search:" << _response.SearchHeaderField("Content-Length") << "\n\n";
//		
//		HTTPResponseHeaderField _field = _response.ParseHeader();
//		std::cout << "Status:" << _field.status << "\n";
//		std::cout << "Status Code:" << _field.status_code << "\n";
//		for(int i = 0; i < _field.chunk.size(); i++) {
//			std::cout << _field.chunk[i].name << " field chunk:" << _field.chunk[i].content << "\n";
//		}
	}
	
	virtual void Error(const std::string _tag, 
					   CURLcode _error_code) {
//		std::cout << "Error " << _error_code << " " << _tag << "\n";
//		std::cout << "\n";
	}
	
	virtual void Progress(const std::string _tag,
						  double _download_total, 
						  double _download_now, 
						  double _upload_total, 
						  double _upload_now) {
//		if(_download_total != 0) {
//			std::cout << "Progress " << _tag << ": " << _download_now << "/" << _download_total << "," << _upload_now << "/" << _upload_total << "\n";
//		}
	}
};

int main(void) {
	TestUnit unit;
	
	HTTPRequest request;
	request.SetURL("http://forum.falinux.com/zbxe/files/attach/images/563067/ff_title_height_30.png");
	unit.Send("Tag", request);
	
	while (true) {
		HTTPManager::Share()->Update();
	}
}
