#ifndef HTTPResponse_h
#define HTTPResponse_h

#include "HTTPConfig.h"
#include "HTTPResponseHeaderField.h"

class HTTPResponse {
private:
	std::string header_file_path;
	std::string body_file_path;
	
public:
	typedef enum CacheType {
		//요청시 보낼 URL이 처음 요청이거나, Expires가 만료되었거니
		CacheType_None = 0,
		//요청시 보낼 URL이 이미 보냈었던 요청이고
		//Expires가 만료되었을때
		CacheType_Last_Modified,
		//요청시 보낼 URL이 이미 보냈었던 요청이고
		//Expires가 만료되지 않았을때
		CacheType_Expires
	} CacheType;
	//캐쉬 타입
	CacheType cache_type;
	
	HTTPResponse(const std::string _header_file_path, 
				 const std::string _body_file_path,
				 CacheType _cache_type);
	~HTTPResponse();
	
	//응답을 파일로 읽기(HTTPEvent::Recieve가 끝난후에는 이파일은 삭제됩니다.)
	//내용이 클경우는 직접 파일에 접근
	const std::string GetHeaderFilePath() {
		return header_file_path;
	}
	const std::string GetBodyFilePath() {
		return body_file_path;
	}
	
	//헤더 가져오기
	const std::string GetHeaderContent();
	
	//내용 가져오기
	const std::string GetBodyContent();
	
	//헤더를 검색하여 인자로 보낸 이름으로된 필드명의 값 하나 가져오기
	//메모리에 잠시 헤더내용이 올라감
	//std::string::find사용
	//내부적으로는 std::search사용
	//이하 std::search의 SGI레퍼런스에 나와있는 설명
	//Worst case behavior is quadratic: at most (last1 – first1) * (last2 – first2) comparisons. This worst case, however, is rare. Average complexity is linear.
	//native search
	//컴플렉시티 = O(m*n)
	const std::string SearchHeaderField(const std::string _field_name);
	
	//스크림으로 파싱후 파싱된 결과리턴
	//스트림으로 파싱하지만 HTTPResponseHeaderField가 헤더내용을 메모리로 가지게 됨
	const HTTPResponseHeaderField ParseHeader();
};

#endif
