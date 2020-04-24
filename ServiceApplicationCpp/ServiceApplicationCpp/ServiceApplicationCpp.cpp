// ConsoleApplication4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include"ServiceApplicationCpp.h"

FILE *fp = NULL;
SYSTEMTIME time_info;
TIME t;
DWORD sleep_time = 65000;

int main(int argc, TCHAR *argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
	{
		return GetLastError();
	}

	return 0;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
	DWORD Status = E_FAIL;

	// Register our service control handler with the SCM
	g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

	if (g_StatusHandle == NULL)
	{
		return;
	}

	// Tell the service controller we are starting
	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugString(TEXT(
			"My Sample Service: ServiceMain: SetServiceStatus returned error"));
	}

	/*
	 * Perform tasks necessary to start the service here
	 */
	if (Start())
	{
		OutputDebugString(TEXT(
			"Service Start function failed"));
	}

	// Create a service stop event to wait on later
	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_ServiceStopEvent == NULL)
	{
		// Error creating event
		// Tell service controller we are stopped and exit
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;

		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		{
			OutputDebugString(TEXT(
				"My Sample Service: ServiceMain: SetServiceStatus returned error"));
		}
		return;
	}

	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugString(TEXT(
			"My Sample Service: ServiceMain: SetServiceStatus returned error"));
	}

	// Start a thread that will perform the main task of the service
	HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	// Wait until our worker thread exits signaling that the service needs to stop
	WaitForSingleObject(hThread, INFINITE);


	/*
	 * Perform any cleanup tasks
	 */

	CloseHandle(g_ServiceStopEvent);

	// Tell the service controller we are stopped
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 3;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugString(TEXT(
			"My Sample Service: ServiceMain: SetServiceStatus returned error"));
	}

	return;
}

VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:

		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;

		/*
		 * Perform tasks necessary to stop the service here
		 */
		if (Stop())
		{
			OutputDebugString(TEXT(
				"Service Stop function failed"));
		}

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 4;

		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		{
			OutputDebugString(TEXT(
				"My Sample Service: ServiceCtrlHandler: SetServiceStatus returned error"));
		}

		// This will signal the worker thread to start shutting down
		SetEvent(g_ServiceStopEvent);

		break;

	default:
		break;
	}
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
	while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
	{
		while (1)
		{
			Sleep(sleep_time);
			GetLocalTime(&time_info);
			t.hour = time_info.wHour;
			t.minute = time_info.wMinute;

			if (!(fp = fopen("time.txt", "ab")))
			{
				OutputDebugString(TEXT(
					"file not opened properly"));
				break;
			}
			else if (!(fp = fopen("time.txt", "wb")))
			{
				OutputDebugString(TEXT(
					"file not opened properly"));
				break;
			}

			if (fwrite(&t, 4, NMEB, fp) == EOF)
			{
				OutputDebugString(TEXT(
					"fwrite is not excecuted successfully"));
				break;
			}

			if (fp)
			{
				if (fclose(fp) == EOF)
				{
					OutputDebugString(TEXT(
						"The file is not closed successfully"));
				}
			}
		}

		if (fp)
		{
			if (fclose(fp) == EOF)
			{
				OutputDebugString(TEXT(
					"The file is not closed successfully"));
			}
		}
	}

	return ERROR_SUCCESS;
}

BOOL Start(VOID)
{
	BOOL bStartService = FALSE;
	SERVICE_STATUS_PROCESS SvcStatusProcess;
	SC_HANDLE hOpenSCManager = NULL;
	SC_HANDLE hOpenService = NULL;
	BOOL bQueryServiceStatus = FALSE;
	DWORD dwBytesNeeded;

	hOpenSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hOpenSCManager)
	{
		OutputDebugString(TEXT(
			"OpenSCManager failed"));
		return FALSE;
	}

	hOpenService = OpenService(
		hOpenSCManager,
		SERVICE_NAME,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hOpenService)
	{
		OutputDebugString(TEXT(
			"OpenService failed"));
		CloseServiceHandle(hOpenSCManager);
		return FALSE;
	}

	bQueryServiceStatus = QueryServiceStatusEx(
		hOpenService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&SvcStatusProcess,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded);

	if (FALSE == bQueryServiceStatus)
	{
		OutputDebugString(TEXT(
			"QuerryService failed"));
		return FALSE;
	}

	if ((SvcStatusProcess.dwCurrentState != SERVICE_STOPPED) &&
		(SvcStatusProcess.dwCurrentState != SERVICE_STOP_PENDING))
	{
		OutputDebugString(TEXT(
			"Service already running"));
	}

	while (SvcStatusProcess.dwCurrentState == SERVICE_STOP_PENDING)
	{
		bQueryServiceStatus = QueryServiceStatusEx(
			hOpenService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&SvcStatusProcess,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded);

		if (FALSE == bQueryServiceStatus)
		{
			OutputDebugString(TEXT(
				"QuerryService failed"));
			CloseServiceHandle(hOpenService);
			CloseServiceHandle(hOpenSCManager);
			return FALSE;
		}
	}

	bStartService = StartService(
		hOpenService,
		NULL,
		NULL);

	if (FALSE == bStartService)
	{
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
		return FALSE;
	}

	bQueryServiceStatus = QueryServiceStatusEx(
		hOpenService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&SvcStatusProcess,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded);

	if (FALSE == bQueryServiceStatus)
	{
		OutputDebugString(TEXT(
			"QuerryService failed"));
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
		return FALSE;
	}

	if (SvcStatusProcess.dwCurrentState != SERVICE_RUNNING)
	{
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
		return FALSE;
	}
	CloseServiceHandle(hOpenService);
	CloseServiceHandle(hOpenSCManager);
	return TRUE;
}

BOOL Stop(VOID)
{
	SERVICE_STATUS_PROCESS SvcStatusProcess;
	SC_HANDLE hOpenSCManager = NULL;
	SC_HANDLE hOpenService = NULL;
	BOOL bControlService = TRUE;
	BOOL bQueryServiceStatus = TRUE;
	DWORD dwBytesNeeded;

	hOpenSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hOpenSCManager)
	{
		OutputDebugString(TEXT(
			"OpenSCManager failed"));
		return FALSE;
	}

	hOpenService = OpenService(
		hOpenSCManager,
		SERVICE_NAME,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hOpenService)
	{
		OutputDebugString(TEXT(
			"OpenService failed"));
		CloseServiceHandle(hOpenSCManager);
		return FALSE;
	}

	bQueryServiceStatus = QueryServiceStatusEx(
		hOpenService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&SvcStatusProcess,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded);

	if (FALSE == bQueryServiceStatus)
	{
		OutputDebugString(TEXT(
			"QuerryService failed"));
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
		return FALSE;
	}

	bControlService = ControlService(
		hOpenService,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&SvcStatusProcess);

	if (FALSE == bControlService)
	{
		OutputDebugString(TEXT(
			"Control Service failed"));
		return FALSE;
	}

	while (SvcStatusProcess.dwCurrentState != SERVICE_STOPPED)
	{
		bQueryServiceStatus = QueryServiceStatusEx(
			hOpenService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&SvcStatusProcess,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded);

		if (FALSE == bQueryServiceStatus)
		{
			OutputDebugString(TEXT(
				"QuerryService failed"));
			CloseServiceHandle(hOpenService);
			CloseServiceHandle(hOpenSCManager);
			return FALSE;
		}

		if (SvcStatusProcess.dwCurrentState == SERVICE_STOPPED)
		{
			OutputDebugString(TEXT(
				"Service stopped"));
			break;
		}
	}

	CloseServiceHandle(hOpenService);
	CloseServiceHandle(hOpenSCManager);
	return TRUE;
}