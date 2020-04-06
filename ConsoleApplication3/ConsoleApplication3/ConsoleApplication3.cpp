#include "ConsoleApplication3.h"
using namespace std;

void fileOperation(void * a)
{
	FILE *fp;
	SYSTEMTIME time_info;
	TIME t;
	DWORD sleep_time = 3000;

	Sleep(sleep_time);
	GetLocalTime(&time_info);
	t.hour = time_info.wHour;
	t.minute = time_info.wMinute;

	if (!(fp = fopen("time.txt", "wb")))
	{
		printf("file not opened properly\n");
	}

	if (fwrite(&t, 4, NMEB, fp) == EOF)
	{
		printf("fwrite is not excecuted successfully\n");
	}

	if (fclose(fp) == EOF)
	{
		printf("The file is not closed successfully\n");
	}
}

int main()
{
	HANDLE Thread;

	while (1)
	{
		Thread = (HANDLE)_beginthread(&fileOperation, 0, 0);
	}

	return 0;
}
