#include "SavingCurrentTime.h"
using namespace std;

void fileOperation(void * a)
{
	FILE *fp = NULL;
	SYSTEMTIME time_info;
	TIME t;
	DWORD sleep_time = 65000;

	while (1)
	{
		Sleep(sleep_time);
		GetLocalTime(&time_info);
		t.hour = time_info.wHour;
		t.minute = time_info.wMinute;

		if (!(fp = fopen("time.txt", "wb")))
		{
			cout << "file not opened properly" << endl;
			break;
		}

		if (fwrite(&t, 4, NMEB, fp) == EOF)
		{
			cout << "fwrite is not excecuted successfully" << endl;
			break;
		}

	}
	
		if (fp)
		{
			if (fclose(fp) == EOF)
			{
				cout << "The file is not closed successfully" << endl;
			}
		}
		return;
}

int main()
{
	HANDLE Thread;

		BOOL StartServiceCtrlDispatcher(
			const SERVICE_TABLE_ENTRYA *lpServiceStartTable
		);

		Thread = (HANDLE)_beginthread(&fileOperation, 0, 0);

		if (!Thread)
		{
			cout << "Thread is not created successfully" << endl;
			exit(1);
		}

	exit(0);
}