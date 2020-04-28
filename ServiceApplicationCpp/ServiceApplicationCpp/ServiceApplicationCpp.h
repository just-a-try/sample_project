#include <Windows.h>
#include <iostream>
#pragma warning(disable : 4996)
#define SLEEP_TIME 5000
#define NMEB 1
#define TIME_STRUCT_SIZE 5
#pragma pack(1)
typedef struct time
{
	WORD hour;
	char colon = ':';
	WORD minute;
}TIME;

#define SERVICE_NAME TEXT("Current Time Saving Service")
SERVICE_STATUS  ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE hServiceStatusHandle = NULL;
HANDLE hServiceEvent = NULL;

DWORD WINAPI ServiceWorkerThread(void);
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpArgv);
void WINAPI ServiceControlHandler(DWORD dwControl);
void ServiceReportStatus(
	DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint);
void ServiceInit(DWORD dwArgc, LPTSTR *lpArgv);
bool ServiceInstall(void);
bool ServiceDelete(void);
bool ServiceStart(void);
bool ServiceStop(void);
