#ifndef HTTPResponseHeaderField_h
#define HTTPResponseHeaderField_h

#include "HTTPConfig.h"

//필드에 관한 상세 설명
//http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html

class HTTPResponseHeaderField {
public:
	HTTPResponseHeaderField(const std::string _header_file_path);
	
	//Header맨 처음에 나오는 라인
	std::string status;
	
	//첫라인의 코드
	int status_code;
	
	typedef struct Chunk {
		//필드이름
		std::string name;
		//필드내용
		std::string content;
	} Chunk;
	
	//헤더의 내용이 담겨져 있는 배열
	std::vector<Chunk> chunk;
	
	//헤더에 _field_name의 정보가 있는지 검사하고 내용을 리턴
	const std::string SearchField(const std::string _field_name);
};

#endif
