#include "Mylog.h"

MYLOG*				MYLOG::mylog			= NULL;
string				MYLOG::logBuffer		= "";
HANDLE				MYLOG::mFileHandle		= INVALID_HANDLE_VALUE;
int					MYLOG::writtenSize		= 0;
mutex				MYLOG::log_mutex;
CRITICAL_SECTION	MYLOG::criticalSection;

MYLOG::MYLOG()
{
	init(LOG_LEVEL_NONE, LOG_TARGET_FILE);
}

void MYLOG::init(LOGLEVEL loglevel, LOGTARGET logtarget)
{
	setLogLevel(loglevel);
	setLogTarget(logtarget);
	//初始化临界区
	InitializeCriticalSection(&criticalSection);
	createFile();
}

void MYLOG::uninit()
{
	if (INVALID_HANDLE_VALUE != mFileHandle)
	{
		CloseHandle(mFileHandle);
	}
	//释放临界区
	DeleteCriticalSection(&criticalSection);

}

MYLOG* MYLOG::getInstance()
{
	if (NULL == mylog)
	{

		log_mutex.lock();
		if (NULL == mylog)
		{
			mylog = new MYLOG();
		}
		log_mutex.unlock();
	}
	return mylog;
}

LOGLEVEL MYLOG::getLogLevel()
{
	return this->logLevel;
}

void MYLOG::setLogLevel(LOGLEVEL loglevel)
{
	this->logLevel = loglevel;
}

LOGTARGET MYLOG::getLogTarget()
{
	return this->logTarget;
}

void MYLOG::setLogTarget(LOGTARGET logtarget)
{
	this->logTarget = logtarget;
}

int MYLOG::createFile()
{
	TCHAR fileDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, fileDirectory);

	//log文件的路径
	TCHAR logFileDirectory[256];
	_stprintf_s(logFileDirectory, _T("%s\\Log\\"), fileDirectory);

	//文件夹不存在创建文件夹
	if (_taccess(logFileDirectory, 0) == -1)
	{
		_tmkdir(logFileDirectory);
	}

	
	WCHAR moduleFileName[MAX_PATH];
	//获取当前模块所在路径
	GetModuleFileName(NULL, moduleFileName, MAX_PATH);
	//获取文件的字符
	PWCHAR p = wcsrchr(moduleFileName, _T('\\'));
	p++;
	//去除后缀名
	for (int i = _tcslen(p); i > 0; --i)
	{
		if (p[i] == _T('.'))
		{
			p[i] = _T('\0');
			break;
		}
	}
	WCHAR pszLogFileName[MAX_PATH];
	_stprintf_s(pszLogFileName, _T("%s%s.log"), logFileDirectory, p);

	//创建模块log文件
	mFileHandle = CreateFile(
		pszLogFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == mFileHandle)
	{
		return -1;
	}
	return 0;
}

static int printfToBuffer(char* buffer, int size, char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int ret = vsnprintf(buffer, 100, fmt, ap);
	va_end(ap);
	return ret;
}

static int getSystemTime(char* timeBuffer)
{
	if (!timeBuffer)
	{
		return -1;
	}
		
	SYSTEMTIME localTime;

	GetLocalTime(&localTime);
	char fmt[] = "[%04d-%02d-%02d %02d:%02d:%02d.%03d]";
	int ret = printfToBuffer(timeBuffer, 100, fmt,
		localTime.wYear,
		localTime.wMonth,
		localTime.wDay,
		localTime.wHour,
		localTime.wMinute,
		localTime.wSecond,
		localTime.wMilliseconds);
	return ret;
}

int MYLOG::writeLog(
	LOGLEVEL loglevel,
	unsigned char* fileName,
	unsigned char* function,
	int lineNum,
	char* fmt,
	...)
{
	int ret = 0;
	//进入临界区
	EnterCriticalSection(&criticalSection);
	//获取日期和时间
	char timeBuffer[100];
	ret = getSystemTime(timeBuffer);
	logBuffer += string(timeBuffer);

	char* logLevel=NULL;
	if (loglevel == LOG_LEVEL_DEBUG) {
		logLevel = (char*)"DEBUG";
	}
	else if (loglevel == LOG_LEVEL_INFO) {
		logLevel = (char*)"INFO";
	}
	else if (loglevel == LOG_LEVEL_WARNING) {
		logLevel = (char*)"WARNING";
	}
	else if (loglevel == LOG_LEVEL_ERROR) {
		logLevel = (char*)"ERROR";
	}

	char logInfo[100];
	char fmt2[] = "[PID:%4d][TID:%4d][%s][%-s][%s:%4d]";
	ret = printfToBuffer(logInfo, 100, fmt2,
		GetCurrentProcessId(),
		GetCurrentThreadId(),
		logLevel,
		fileName,
		function,
		lineNum);
	logBuffer += string(logInfo);

	char logInfo2[256];
	va_list ap;
	va_start(ap, fmt);
	ret = vsnprintf(logInfo2, 256, fmt, ap);
	va_end(ap);

	logBuffer += string(logInfo2);
	logBuffer += string("\n");
	writtenSize += logBuffer.length();

	outputToTarget();
	//推出临界区
	LeaveCriticalSection(&criticalSection);

	return 0;
}

void MYLOG::outputToTarget()
{
	if (MYLOG::getInstance()->getLogTarget() & LOG_TARGET_FILE)
	{
		SetFilePointer(mFileHandle, 0, NULL, FILE_END);
		DWORD dwBytesWritten = 0;
		WriteFile(mFileHandle, logBuffer.c_str(), logBuffer.length(), &dwBytesWritten, NULL);
		FlushFileBuffers(mFileHandle);
	}
	if (MYLOG::getInstance()->getLogTarget() & LOG_TARGET_CONSOLE)
	{
		printf("%s", logBuffer.c_str());
	}
	//清除Buffer
	logBuffer.clear();
}