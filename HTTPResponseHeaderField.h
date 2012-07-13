/*!
 \file HTTPResponseHeaderField.h
 \brief HTTP요청에 대한 응답헤더를 나타내는 클래스입니다.
 \author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 \version 1.0
 \section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */

#ifndef HTTPResponseHeaderField_h
#define HTTPResponseHeaderField_h

#include "HTTPConfig.h"

/*!
 \class HTTPResponseHeaderField
 \brief HTTP요청에 대한 응답헤더을 나타내는 클래스입니다.
 
 이 클래스는 응답헤더에대한 정보를 살펴볼수 있습니다.
 
 \par Example
 다음의 예제는 naver의 헤더를 파싱하는 예제입니다.이후프로그램은 종료됩니다.Mac OSX에서 작성되었습니다.
 \include response_header.cpp
 */
class HTTPResponseHeaderField {
private:
	/*!
	 \brief 생성자
	 
	 private 입니다. HTTPResponse::ParseHeader() 로만 생성할수 있습니다.
	 */
	HTTPResponseHeaderField(const std::string _header_file_path);
	
public:
	
	/*!
	 \brief Header맨 처음에 나오는 라인(응답상태를 나타냄)
	 */
	std::string status;
	
	/*!
	 \brief 헤더 첫라인의 코드(숫자)
	 */
	int status_code;
	
	/*!
	 \brief 응답헤더필드에대한 구조체
	 
	 \ref 응답헤더필드에 관한 상세 설명 http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
	 */
	typedef struct Chunk {
		std::string name;		/*!< 헤더필드의 이름 */
		std::string content;	/*!< 헤더필드의 내용 */
	} Chunk;
	
	/*!
	 \brief 헤더의 내용이 담겨져 있는 배열
	 */
	std::vector<Chunk> chunk;
	
	/*!
	 \brief 응답헤더에 해당 필드의 정보가 있는지 검사하고 내용을 리턴
	 \param 검색할 필드명
	 \return 검색된 필드내용
	 
	 \ref 응답헤더필드에 관한 상세 설명 http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
	 */
	const std::string SearchField(const std::string _field_name);
	
	/*!
	 \brief friend class 는 HTTPResponse 입니다.
	 HTTPResponseHeaderField 는 HTTPResponse::ParseHeader() 로만 생성할수 있습니다.
	 */
	friend class HTTPResponse;
};

#endif
