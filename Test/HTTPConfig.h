#ifndef HTTPConfig_h
#define HTTPConfig_h

//시스템 별로 헤더가 다를경우 변경
#include <string>
#include <list>
#include <curl/curl.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sqlite3.h>

//HTTP관련 임시파일을 저장할 경로를 지정함
#define HTTP_MANAGER_DIRECTORY std::string("/Users/mario/Library/Developer/Xcode/DerivedData/Test-ewxxhqydzviwutcttpisefmegkew/Build/Products/Debug")

//HTTP관련 임시파일을 저장할 경로를 숨기기 위한 폴더명
//실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY에 저장됨
#define HTTP_MANAGER_DIRECTORY_TEMPORARY std::string("temporary")

//HTTP관련 캐쉬파일을 저장할 경로를 숨기기 위한 폴더명
//실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE에 저장됨
#define HTTP_MANAGER_DIRECTORY_CACHE std::string("cache")

//HTTP응답의 캐쉬를 관리하기위한 sqlite3 DB파일
//실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE + "/" + HTTP_MANAGER_CACHE_DB 에 저장됨
#define HTTP_MANAGER_CACHE_DB std::string("db.sql3")

//임시,캐쉬 헤더파일들을 숨기기 위한 폴더명
//임시헤더파일들은 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY + "/" + HTTP_MANAGER_DIRECTORY_HEADER에 저장됨
//캐쉬헤더파일들은 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE + "/" + HTTP_MANAGER_DIRECTORY_HEADER에 저장됨
#define HTTP_MANAGER_DIRECTORY_HEADER std::string("header")

//임시,캐쉬 바디파일들을 숨기기 위한 폴더명
//임시바디파일들은 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY + "/" + HTTP_MANAGER_DIRECTORY_BODY에 저장됨
//캐쉬바디파일들은 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE + "/" + HTTP_MANAGER_DIRECTORY_BODY에 저장됨
#define HTTP_MANAGER_DIRECTORY_BODY std::string("body")

//디버그 매크로
//함수의 내용을 FLAG;로 바꾸면 프로그램을 중단하지 않음
//릴리즈시 필요하다면 매크로를 변경
#define HTTP_DEBUG(FLAG, STRING)\
if(FLAG) {\
	std::cout << "HTTP LOG: " << STRING << "\n";\
	assert(false);\
}

//최대 연결 갯수
#define HTTP_MAX_CONNECTIONS 0xF

#endif
