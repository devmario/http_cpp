/**
 @file HTTPManager.h
 @brief HTTPEvent들을 관리하는 Manager
 @author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 @version 1.0
 @section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */

#ifndef HTTPManager_h
#define HTTPManager_h

#include "HTTPConfig.h"
#include "HTTPClient.h"

/**
 @class HTTPManager
 @brief HTTPEvent 들을 관리하는 Singleton Class
 */
class HTTPManager {
private:
	std::string temporary_path;
	std::string cache_path;
	
	std::string db_path;
	
	std::string temporary_header_path;
	std::string temporary_body_path;
	
	std::string cache_header_path;
	std::string cache_body_path;
	
	CURLM* multi_handle;
	int still_running;
	
	int messages_left;
	
	std::list<HTTPClient*> client_list;
	std::string temporary_directory;
	
	static int SQLExpires(void* _reference, int _field_length, char** _field_content, char** _field_name);
	
	void ReadyDirectory();
	void ReadyDB();
	
	HTTPManager();
	~HTTPManager();
	
public:
	/// @brief private 함수이며 임시헤더파일들의 경로를 가져옵니다.
	const std::string _GetTemporaryHeaderPath();
	/// @brief private 함수이며 임시바디파일들의 경로를 가져옵니다.
	const std::string _GetTemporaryBodyPath();
	/// @brief private 함수이며 캐쉬해더파일들의 경로를 가져옵니다.
	const std::string _GetCacheHeaderPath();
	/// @brief private 함수이며 캐쉬바디파일들의 경로를 가져옵니다.
	const std::string _GetCacheBodyPath();
	/// @brief private 함수이며 sqlite 데이터베이스의 경로를 가져옵니다.
	const std::string _GetDBPath();
	
	/// @brief Singleton HTTPManager Class의 포인터를 반환합니다.
	///
	/// HTTPManager instance는 이 메소드 호출시 초기화되며 이메소드의 접근은 항상 같은 쓰레드에서 하여야 합니다.
	///
	/// HTTPConfig.h 에 명시되어 있는 HTTP_DEBUG 메크로의 assert 문을 주석처리하지 않으면 다른쓰레드에서 접근시 혹은 에러 발생시 프로그램은 중단됩니다.
	/// @return Singleton HTTPManager Class의 포인터를 반환합니다.
	///
	/// <b>Example</b>
	/// <p style="font-family:Menlo,Consolas,monospace;font-size:12px;line-height:14px;">
	/// <span style="color:#3f6e74;">HTTPManager</span><span style="color:#000000;">*&nbsp;_manager&nbsp;=&nbsp;</span><span style="color:#3f6e74;">HTTPManager</span><span style="color:#000000;">::</span><span style="color:#26474b;">Share</span><span style="color:#000000;">();</span>
    /// </p>
	static HTTPManager* Share();
	
	/// @brief private 함수이며 요청과 응답을 처리할 private class 인 HTTPClient 를 작동시키기 위해 추가하는 함수입니다.
	void _AddClient(HTTPClient* _client);
	/// @brief private 함수이며 요청과 응답을 처리할 private class 인 HTTPClient 의 작동을 멈추기 위해 제거하는 함수입니다.
	void _RemoveClient(HTTPClient* _client);
	
	/// @brief HTTPEvent 요청에 대한 응답을 기다리고 응답이 있으면 처리합니다.
	///
	/// HTTPEvent 의 추상화함수들은 모두 응답에 관한 함수들이며, 응답이 있을시 그 함수들이 이 Update 함수 안에서 호출됩니다.
	///
	/// 메인쓰레드(drawing 쓰레드)에서 매 프레임마다 호출해주세요.
	///
	/// <b>Example</b>
    /// <p style="font-family:Menlo,Consolas,monospace;font-size:12px;line-height:14px;">
	/// <span style="color:#3f6e74;">HTTPManager</span><span style="color:#000000;">::</span><span style="color:#26474b;">Share</span><span style="color:#000000;">()-&gt;</span><span style="color:#26474b;">Update</span><span style="color:#000000;">();</span>
    /// </p>
	void Update();
	
	/// @brief 응답을 기다리고 있는 요청이 몇개인지 검사합니다.
	/// @return 응답을 기다리고있는 요청의 갯수를 리턴합니다.
	
	/**
	 
	 
	 다음의 예제는 naver.com으로 10개의 요청을 보내고 응답을 기다리고 있는 요청의 갯수를 보여주는 예제입니다.
	 
	\code{.cpp}
	#include "HTTP.h"
	
	int recive_count = 0;
	int send_count = 10; 
	
	class Sender : public HTTPEvent {
	public:
		virtual void Receive(const std::string _tag, 
							 HTTPResponse _response) {
			std::cout << "Receive " << _tag << std::endl;
			recive_count++;
		}
		
		virtual void Error(const std::string _tag, 
						   CURLcode _error_code) {
			std::cout << "Error " << _error_code << " " << _tag << std::endl;
			recive_count++;
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
		
		for(int i = 0; i < send_count; i++) {
			HTTPRequest request;
			request.SetURL("http://www.naver.com");
			sender.Send("Tag", request);
		}
		
		while(recive_count < send_count) {
			std::cout << HTTPManager::Share()->GetRunningHTTP() << std::endl;
			HTTPManager::Share()->Update();
			usleep(10000);
		}
	}
	\endcode
	 */
	int GetRunningHTTP();
	
	void CleanCache();
};

#endif
