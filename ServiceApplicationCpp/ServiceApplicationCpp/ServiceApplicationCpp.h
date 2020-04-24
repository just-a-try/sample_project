#pragma once
#pragma once
#include<Windows.h>
#include <iostream>
#pragma warning(disable : 4996)

#define NMEB 1

typedef struct time
{
	WORD hour;
	WORD minute;
}TIME;

SERVICE_STATUS        g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler(DWORD);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);
BOOL Start(VOID);
BOOL Stop(VOID);

#define SERVICE_NAME  TEXT("Current time Saving Service") 