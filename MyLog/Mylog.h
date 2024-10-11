#pragma once

#include <mutex>
#include <iostream>
#include <stdio.h>
#include <TCHAR.H>
#include <Windows.h>

using namespace std;

enum LOGLEVEL {
	LOG_LEVEL_NONE,
	LOG_LEVEL_ERROR,		//error
	LOG_LEVEL_WARNING,		//warning
	LOG_LEVEL_DEBUG,		//debug
	LOG_LEVEL_INFO			//info
};


enum LOGTARGET{
	LOG_TARGET_NONE		= 0x00,
	LOG_TARGET_CONSOLE	= 0x01,	//在终端打印
	LOG_TARGET_FILE		= 0x10  //保存在文件
};

#define FILENAME(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x
#define LOG_INFO(...) MYLOG::writeLog(LOG_LEVEL_INFO, (unsigned char*)(FILENAME(__FILE__)), (unsigned char *)(__FUNCTION__),(int)(__LINE__),__VA_ARGS__)
#define LOG_DEBUG(...) MYLOG::writeLog(LOG_LEVEL_DEBUG, (unsigned char*)(FILENAME(__FILE__)), (unsigned char *)(__FUNCTION__),(int)(__LINE__),__VA_ARGS__)
#define LOG_WARNING(...) MYLOG::writeLog(LOG_LEVEL_WARNING, (unsigned char*)(FILENAME(__FILE__)), (unsigned char *)(__FUNCTION__),(int)(__LINE__),__VA_ARGS__)
#define LOG_ERROR(...) MYLOG::writeLog(LOG_LEVEL_ERROR, (unsigned char*)(FILENAME(__FILE__)), (unsigned char *)(__FUNCTION__),(int)(__LINE__),__VA_ARGS__)

#define ENTER()		LOG_INFO("enter")
#define EXIT()		LOG_INFO("exit")
#define FAIL()		LOG_ERROR("fail")

#define MAX_SIZE	2*1024*1024 //单个日志文件的最大存储为2MB

class MYLOG
{
public:
	//初始化日志等级和日志输出位置
	void init(LOGLEVEL loglevel, LOGTARGET logtarget);
	void uninit();
	//创建文件
	int createFile();
	//单例模式
	static MYLOG* getInstance();
	//获取当前日志等级
	LOGLEVEL getLogLevel();
	//设置当前日志等级
	void setLogLevel(LOGLEVEL loglevel);
	//获取日志输出位置
	LOGTARGET getLogTarget();
	//设置日志输出位置
	void setLogTarget(LOGTARGET logtarget);
	//写Log
	static int writeLog(
		LOGLEVEL loglevel,
		unsigned char* fileName,
		unsigned char* function,
		int lineNum,
		char* fmt,
		...);
	//输出Log
	static void outputToTarget();
private:
	MYLOG();
	~MYLOG();

	static MYLOG* mylog;
	//互斥锁
	static mutex log_mutex;
	//临界区
	static CRITICAL_SECTION criticalSection;
	//存储Log的Buffer
	static string logBuffer;
	//已写Log的长度
	static int writtenSize;
	//Log级别
	LOGLEVEL logLevel;
	//Log输出位置
	LOGTARGET logTarget;
	//文件句柄
	static HANDLE mFileHandle;
};

