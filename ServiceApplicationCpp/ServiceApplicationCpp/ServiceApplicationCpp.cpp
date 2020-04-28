#include "ServiceApplicationCpp.h"

FILE *fp = NULL;
SYSTEMTIME time_info;
TIME time_structure;

int main(int argc, CHAR *argv[])
{
	BOOL bServiceCtrlDispatcher = FALSE;

	if (lstrcmpiA(argv[1], "install") == 0)
	{
		if (!ServiceInstall())
		{
			printf("ServiceInstall function failed\n");
			exit(1);
		}
	}
	else if (lstrcmpiA(argv[1], "Start") == 0)
	{
		if (!ServiceStart())
		{
			printf("ServiceStart function failed\n");
			exit(1);
		}
	}
	else if (lstrcmpiA(argv[1], "Stop") == 0)
	{
		if (!ServiceStop())
		{
			printf("ServiceStop function failed\n");
			exit(1);
		}
	}
	else if (lstrcmpiA(argv[1], "delete") == 0)
	{
		if (!ServiceDelete())
		{
			printf("ServiceDelete function failed\n");
			exit(1);
		}
	}
	else
	{
		SERVICE_TABLE_ENTRY  DispatchTable[] =
				{
					{(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
					{NULL, NULL}
				};

		bServiceCtrlDispatcher = StartServiceCtrlDispatcher(DispatchTable);

		if (FALSE == bServiceCtrlDispatcher)
		{
			printf("StartServiceCtrlDispatcher failed\n");
		}
	}
	exit(0);
}

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpArgv)
{
	BOOL bServicesStatus = FALSE;
	hServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceControlHandler);

	if (NULL == hServiceStatusHandle)
	{
		printf("RegisterServiceCtrlHandler failed\n");
	}

	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwServiceSpecificExitCode = 0;

	ServiceReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	bServicesStatus = SetServiceStatus(hServiceStatusHandle, &ServiceStatus);

	if (FALSE == bServicesStatus)
	{
		printf("SetServiceStatus failed\n");
	}

	ServiceInit(dwArgc, lpArgv);

}

void WINAPI ServiceControlHandler(DWORD dwControl)
{
	switch (dwControl)
	{
		case SERVICE_CONTROL_STOP:
		{	
			ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
			break;
		}

		default:
			break;
	}
}

void ServiceReportStatus(
	DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	BOOL bSetServiceStatus = FALSE;

	ServiceStatus.dwCurrentState = dwCurrentState;
	ServiceStatus.dwServiceSpecificExitCode = dwWin32ExitCode;
	ServiceStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
	{
		ServiceStatus.dwControlsAccepted = 0;
	}
	else
	{
		ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	}

	if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
	{
		ServiceStatus.dwCheckPoint = 0;
	}
	else
	{
		ServiceStatus.dwCheckPoint = dwCheckPoint++;
	}

	bSetServiceStatus = SetServiceStatus(hServiceStatusHandle, &ServiceStatus);

	if (FALSE == bSetServiceStatus)
	{
		printf("SetServiceStatus failed\n");
	}

}

void ServiceInit(DWORD gwArgc, LPTSTR *lpArgv)
{
	hServiceEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL);

	if (NULL == hServiceEvent)
	{
		ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
	}
	else
	{
		ServiceReportStatus(SERVICE_RUNNING, NO_ERROR, 0);
	}

	while (1)
	{
		WaitForSingleObject(hServiceEvent, INFINITE);
		ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0); 
	}
}


/* @author: Subhash 
   @function: This function install the service to the Service control manager database
   @return: Returns boolean true if fuction succeed or false when fails
*/
bool ServiceInstall(void)
{
	SC_HANDLE  hScOpenSCManger = NULL;
	SC_HANDLE hScCreateService = NULL;
	DWORD dwGetModuleFileName = 0;
	TCHAR szPath[MAX_PATH];

	dwGetModuleFileName = GetModuleFileName(NULL, szPath, MAX_PATH);

	if (FALSE == dwGetModuleFileName)
	{
		printf("Service installation failed\n");
		return FALSE;
	}

	hScOpenSCManger = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hScOpenSCManger)
	{
		printf("OpenSCManager failed\n");
		return FALSE;
	}

	hScCreateService = CreateService(
		hScOpenSCManger,
		SERVICE_NAME,
		SERVICE_NAME,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		szPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (NULL == hScCreateService)
	{
		CloseServiceHandle(hScOpenSCManger);
		printf("CreateService failed\n");
		return FALSE;
	}

	CloseServiceHandle(hScCreateService);
	CloseServiceHandle(hScOpenSCManger);
	return TRUE;
}

/* @author: Subhash 
   @function: This function delete the service from the Service control manager database
   @return: Returns boolean true if fuction succeed or false when fails
*/
bool ServiceDelete(void)
{
	SC_HANDLE  hScOpenSCManger = NULL;
	SC_HANDLE hScOpenService = NULL;
	BOOL bDeleteService = FALSE;

	hScOpenSCManger = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hScOpenSCManger)
	{
		printf("OpenSCManager failed\n");
		return FALSE;
	}

	hScOpenService = OpenService(
		hScOpenSCManger,
		SERVICE_NAME,
		SERVICE_ALL_ACCESS);

	if (NULL == hScOpenService)
	{
		printf("OpenService failed failed\n");
		return FALSE;
	}
	
	bDeleteService = DeleteService(hScOpenService);

	if (FALSE == bDeleteService)
	{
		printf("DeleteService failed\n");
		return FALSE;
	}

	CloseServiceHandle(hScOpenService);
	CloseServiceHandle(hScOpenSCManger);
	return TRUE;
}

/* @author: Subhash 
   @function: This function starts the service 
   @return: Returns boolean true if fuction succeed or false when fails
*/
bool ServiceStart(void)
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
		printf("OpenSCManager failed\n");
		return FALSE;
	}

	hOpenService = OpenService(
		hOpenSCManager,
		SERVICE_NAME,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hOpenService)
	{
		printf("OpenService failed\n");
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
		printf("QuerryService failed\n");
		return FALSE;
	}

	if ((SvcStatusProcess.dwCurrentState != SERVICE_STOPPED) &&
		(SvcStatusProcess.dwCurrentState != SERVICE_STOP_PENDING))
	{
		printf("Service already running\n");
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
			printf("QuerryService failed\n");
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
	}

	bQueryServiceStatus = QueryServiceStatusEx(
		hOpenService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&SvcStatusProcess,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded);

	if (FALSE == bQueryServiceStatus)
	{
		printf("QuerryService failed\n");
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
		return FALSE;
	}

	HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	if (SvcStatusProcess.dwCurrentState != SERVICE_RUNNING)
	{
		CloseServiceHandle(hOpenService);
		CloseServiceHandle(hOpenSCManager);
	}

	CloseServiceHandle(hOpenService);
	CloseServiceHandle(hOpenSCManager);
	return TRUE;

}

/* @author: Subhash 
   @function: This function stops the service 
   @return: Returns boolean true if fuction succeed or false when fails
*/
bool ServiceStop(void)
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
		printf("OpenSCManager failed\n");
		return FALSE;
	}

	hOpenService = OpenService(
		hOpenSCManager,
		SERVICE_NAME,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hOpenService)
	{
		printf("OpenService failed\n");
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
		printf("QueryServiceStatusEx failed\n");
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
		printf("ControlService failed\n");
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
			printf("QueryServiceStatusEx failed\n");
			CloseServiceHandle(hOpenService);
			CloseServiceHandle(hOpenSCManager);
			return FALSE;
		}

		if (SvcStatusProcess.dwCurrentState == SERVICE_STOPPED)
		{
			printf("Service stopped failed\n");
			break;
		}
	}

	CloseServiceHandle(hOpenService);
	CloseServiceHandle(hOpenSCManager);
	return TRUE;
}

/* @author: Subhash 
   @function: This function starts the service 
   @param: void pointer for thread
   @return: Returns the error status of the function
*/
DWORD WINAPI ServiceWorkerThread(void)
{
	while (WaitForSingleObject(hServiceEvent, 0) != WAIT_OBJECT_0)
	{
		while (1)
		{
			Sleep(SLEEP_TIME);
			GetLocalTime(&time_info);
			time_structure.hour = time_info.wHour;
			time_structure.minute = time_info.wMinute;

			if (!(fp = fopen("time.txt", "wb")))
			{
				printf("file not opened properly in write mode\n");
				break;
			}

			if (fwrite(&time_structure, TIME_STRUCT_SIZE, NMEB, fp) == EOF)
			{
				printf("fwrite is not excecuted successfully\n");
				break;
			}

			if (fp)
			{
				if (fclose(fp) == EOF)
				{
					printf("The file is not closed successfully\n");
					break;
				}
			}
		}

		if (fp)
		{
			if (fclose(fp) == EOF)
			{
				printf("The file is not closed successfully\n");
			}
		}
	}

	return ERROR_SUCCESS;
}
