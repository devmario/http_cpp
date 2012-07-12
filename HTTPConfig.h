#ifndef HTTPConfig_h
#define HTTPConfig_h

/**
 @file HTTPConfig.h
 @brief HTTP관련 환경 설정및 매크로
 
 환경마다 다를수 있는 include나 directory설정, 최대 연결갯수는 이 파일에서 설정합니다.
 @author 
 <table>
 <tr><th>Developer</th><th>Role</th><th>Email</th><th>Mobile No.</th></tr>
 <tr><td>장원희</td><td>Programmer</td><td>mario@vanillabreeze.com</td><td>82-(0)10-4443-0711</td></tr>
 </table>
 @version 1.0
 @section Lisence
 Copyright 2012 vanilla breeze. All rights reserved.
 */

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

/// @brief HTTP관련 임시파일을 저장할 경로를 지정함
#define HTTP_MANAGER_DIRECTORY std::string("/Users/mario/Library/Developer/Xcode/DerivedData/Test-ewxxhqydzviwutcttpisefmegkew/Build/Products/Debug")

/// @brief HTTP관련 임시파일을 저장할 경로를 숨기기 위한 폴더명
///
/// 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY에 저장됨
#define HTTP_MANAGER_DIRECTORY_TEMPORARY std::string("temporary")

/// @brief HTTP관련 캐쉬파일을 저장할 경로를 숨기기 위한 폴더명
///
/// 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE에 저장됨
#define HTTP_MANAGER_DIRECTORY_CACHE std::string("cache")

/// @brief HTTP응답의 캐쉬를 관리하기위한 sqlite3 DB파일
///
/// 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE + "/" + HTTP_MANAGER_CACHE_DB 에 저장됨
#define HTTP_MANAGER_CACHE_DB std::string("db.sql3")

/// @brief 임시,캐쉬 헤더파일들을 숨기기 위한 폴더명
///
///임시헤더파일들은 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY + "/" + HTTP_MANAGER_DIRECTORY_HEADER에 저장됨
///캐쉬헤더파일들은 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE + "/" + HTTP_MANAGER_DIRECTORY_HEADER에 저장됨
#define HTTP_MANAGER_DIRECTORY_HEADER std::string("header")

/// @brief 임시,캐쉬 바디파일들을 숨기기 위한 폴더명
///
/// 임시바디파일들은 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_TEMPORARY + "/" + HTTP_MANAGER_DIRECTORY_BODY에 저장됨
/// 캐쉬바디파일들은 실제로 HTTP_MANAGER_DIRECTORY + "/" + HTTP_MANAGER_DIRECTORY_CACHE + "/" + HTTP_MANAGER_DIRECTORY_BODY에 저장됨
#define HTTP_MANAGER_DIRECTORY_BODY std::string("body")

/// @brief 디버그 매크로
///
/// 함수의 내용을 FLAG;로 바꾸면 프로그램을 중단하지 않음
/// 릴리즈시 필요하다면 매크로를 변경(매크로 내용 삭제)
#define HTTP_DEBUG(FLAG, STRING)\
if(FLAG) {\
	std::cout << "HTTP LOG: " << STRING << "\n";\
	assert(false);\
}

/// @brief 최대 연결 갯수
#define HTTP_MAX_CONNECTIONS 0xF

#endif
