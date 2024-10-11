#include "Mylog.h"
#include <process.h>
#include <Windows.h>

#define THREAD_NUM 6

//资源变量
int g_num = 0;

unsigned int __stdcall func(void* pPM)
{
	LOG_INFO((char*)"enter");
	Sleep(50);
	g_num++;
	LOG_INFO((char*)"g_num = %d", g_num);

	LOG_INFO((char*)"exit");
	return 0;
}

int main()
{
	MYLOG* logger = MYLOG::getInstance();
	HANDLE handle[THREAD_NUM];

	int threadNum = 0;
	while (threadNum < THREAD_NUM)
	{
		handle[threadNum] = (HANDLE)_beginthreadex(NULL, 0, func, NULL, 0, NULL);
		
		threadNum++;
	}
	//保证子线程已全部运行结束
	WaitForMultipleObjects(THREAD_NUM, handle, TRUE, INFINITE);
	return 0;
}