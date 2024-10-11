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
	LOG_TARGET_CONSOLE	= 0x01,	//���ն˴�ӡ
	LOG_TARGET_FILE		= 0x10  //�������ļ�
};

#define FILENAME(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x
#define LOG_INFO(...) MYLOG::writeLog(LOG_LEVEL_INFO, (unsigned char*)(FILENAME(__FILE__)), (unsigned char *)(__FUNCTION__),(int)(__LINE__),__VA_ARGS__)
#define LOG_DEBUG(...) MYLOG::writeLog(LOG_LEVEL_DEBUG, (unsigned char*)(FILENAME(__FILE__)), (unsigned char *)(__FUNCTION__),(int)(__LINE__),__VA_ARGS__)
#define LOG_WARNING(...) MYLOG::writeLog(LOG_LEVEL_WARNING, (unsigned char*)(FILENAME(__FILE__)), (unsigned char *)(__FUNCTION__),(int)(__LINE__),__VA_ARGS__)
#define LOG_ERROR(...) MYLOG::writeLog(LOG_LEVEL_ERROR, (unsigned char*)(FILENAME(__FILE__)), (unsigned char *)(__FUNCTION__),(int)(__LINE__),__VA_ARGS__)

#define ENTER()		LOG_INFO("enter")
#define EXIT()		LOG_INFO("exit")
#define FAIL()		LOG_ERROR("fail")

#define MAX_SIZE	2*1024*1024 //������־�ļ������洢Ϊ2MB

class MYLOG
{
public:
	//��ʼ����־�ȼ�����־���λ��
	void init(LOGLEVEL loglevel, LOGTARGET logtarget);
	void uninit();
	//�����ļ�
	int createFile();
	//����ģʽ
	static MYLOG* getInstance();
	//��ȡ��ǰ��־�ȼ�
	LOGLEVEL getLogLevel();
	//���õ�ǰ��־�ȼ�
	void setLogLevel(LOGLEVEL loglevel);
	//��ȡ��־���λ��
	LOGTARGET getLogTarget();
	//������־���λ��
	void setLogTarget(LOGTARGET logtarget);
	//дLog
	static int writeLog(
		LOGLEVEL loglevel,
		unsigned char* fileName,
		unsigned char* function,
		int lineNum,
		char* fmt,
		...);
	//���Log
	static void outputToTarget();
private:
	MYLOG();
	~MYLOG();

	static MYLOG* mylog;
	//������
	static mutex log_mutex;
	//�ٽ���
	static CRITICAL_SECTION criticalSection;
	//�洢Log��Buffer
	static string logBuffer;
	//��дLog�ĳ���
	static int writtenSize;
	//Log����
	LOGLEVEL logLevel;
	//Log���λ��
	LOGTARGET logTarget;
	//�ļ����
	static HANDLE mFileHandle;
};

