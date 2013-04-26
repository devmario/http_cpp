/*!
 \file HTTPResponse.h
 \brief HTTP요청에 대한 응답을 나타내는 클래스입니다.
 \author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 \version 1.0
 \section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */
#ifndef HTTPResponse_h
#define HTTPResponse_h

#include "HTTPConfig.h"
#include "HTTPResponseHeaderField.h"

/*!
 \class HTTPResponse
 \brief HTTP요청에 대한 응답을 나타내는 클래스입니다.
 
 이 클래스는 응답에대한 정보를 처리하고 살펴볼수 있습니다.
 */
class HTTPResponse {
public:
	static std::string ReadFileToString(std::string _file_path);
	/**
	 \brief 응답받을때의 캐쉬의 타입을 나타냅니다.
	 */
	typedef enum CacheType {
		CacheType_Empty = 0,		//아직 설정안됨
		CacheType_None,		 		/*!< 요청시 보낼 URL이 처음 요청이거나, Expires가 만료되었거나 */
		CacheType_Last_Modified,	/*!< 요청시 보낼 URL이 이미 보냈었던 요청이고, Expires가 만료되었을때 */
		CacheType_Expires			/*!< 요청시 보낼 URL이 이미 보냈었던 요청이고, Expires가 만료되지 않았을때 */
	} CacheType;
	
private:
	/*!
	 \brief 응답받은 헤더에 접근할수있는 파일의 경로입니다.
	 
	 응답은 파일로 디스크에 저장됩니다.
	 */
	std::string header_file_path;
	
	/*!
	 \brief 응답받은 바디에 접근할수있는 파일의 경로입니다.
	 
	 응답은 파일로 디스크에 저장됩니다.
	 */
	std::string body_file_path;
	
	/*!
	 받은 응답의 캐쉬타입을 나타냅니다.
	 */
	CacheType cache_type;
	
public:
	HTTPResponse();
	
	/*!
	 \brief 생성자
	 
	 HTTPResponse 의 생성자는 private이며 friend class인 HTTPClient만이 초기화 할수 있습니다.
	 */
	HTTPResponse(const std::string _header_file_path,
				 const std::string _body_file_path,
				 CacheType _cache_type);
	
	/*!
	 \brief 소멸자
	 */
	~HTTPResponse();
	
	/*!
	 \brief 응답받은 헤더에 접근합니다.
	 \return 응답받은 헤더가 적혀있는 파일의 경로
	 
	 응답은 File IO(디스크에 쓰여짐)로 받습니다.
	 이 경로를 이용하여 스트림을 읽어주세요.이 파일은 HTTPEvent::Recieve 가 종료될때 삭제됩니다.
	 */
	const std::string GetHeaderFilePath() {
		return header_file_path;
	}
	
	/*!
	 \brief 응답받은 바디에 접근합니다.
	 \return 응답받은 바디가 적혀있는 파일의 경로
	 
	 응답은 File IO(디스크에 쓰여짐)로 받습니다.
	 이 경로를 이용하여 스트림을 읽어주세요.이 파일은 HTTPEvent::Recieve 가 종료될때 삭제됩니다.
	 
	 \par Example
	 다음의 예제는 naver로 요청하여 내용을 파일스트림으로 읽어들이며 출력하는 예제입니다.이후 프로그램은 종료됩니다.
	 Mac OSX에서 작성되었습니다.
	 \include response_stream.cpp
	 */
	const std::string GetBodyFilePath() {
		return body_file_path;
	}
	
	/*!
	 \brief 응답의 캐시여부를 판단합니다.
	 \return 어떤 형태의 캐시인가를 반환합니다.
	 */
	CacheType GetCacheType() {
		return cache_type;
	}
	
	/*!
	 \brief 헤더의 내용을 가져옵니다.
	 \return 헤더의 내용이 담긴 문자열
	 
	 이함수는 내부적으로 HTTPResponse::GetHeaderFilePath() 에서 파일경로를 가져와 스트림을 읽어들여 문자열을 만들어냅니다.
	 */
	const std::string GetHeaderContent();
	
	/*!
	 \brief 바디의 내용을 가져옵니다.
	 \return 바디의 내용이 담긴 문자열
	 
	 이함수는 내부적으로 HTTPResponse::GetBodyFilePath() 에서 파일경로를 가져와 스트림을 읽어들여 문자열을 만들어냅니다.
	 */
	const std::string GetBodyContent();
	
	/*!
	 \brief 헤더를 검색하여 인자로 보낸 이름으로된 필드명의 값 하나 가져오기
	 \param _field_name 검색할 필드이름
	 \return 검색결과 필드값
	 
	 이함수는 메모리에 잠시 헤더내용이 올라갑니다.
	 
	 std::string::find가 사용되므로 내부적으로는 std::search 가 수행됩니다.
	 
	 다음은 std::search의 SGI레퍼런스에 나와있는 설명입니다.
	 \note Worst case behavior is quadratic: at most (last1 – first1) * (last2 – first2) comparisons. This worst case, however, is rare. Average complexity is linear.
	 
	 native search이며 컴플렉시티 = O(m*n)입니다. 
	 
	 검색해야할 항목이 많거나 헤더의 내용이 길경우는 HTTPResponse::ParseHeader()를 사용하세요
	 */
	const std::string SearchHeaderField(const std::string _field_name);
	
	/*!
	 \brief 헤더필드를 가져옵니다.
	 \return 헤더필드의 내용이 담긴 클래스를 반환합니다.
	 
	 이함수는 응답받은 파일스트림을 파싱후 파싱된 결과를 리턴합니다.
	 
	 \par Example
	 다음의 예제는 naver의 헤더를 파싱하는 예제입니다.이후프로그램은 종료됩니다.Mac OSX에서 작성되었습니다.
	 \include response_header.cpp
	 */
	const HTTPResponseHeaderField ParseHeader();
	
	/*!
	 이클래스는 HTTPClient 와 friend입니다.
	 */
	friend class HTTPClient;
};

#endif
