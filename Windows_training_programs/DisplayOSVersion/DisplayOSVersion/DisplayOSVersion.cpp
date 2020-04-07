#include "DisplayOSVersion.h"

using namespace std;

/* @author: Subhash
   @function: to display os version of the system
   @return: returns true if the version is dispayed or false if not displayed
*/

bool display_os_info()
{
	bool ret = true;
	NTSTATUS(WINAPI *RtlGetVersion)(LPOSVERSIONINFOEXW);
	OSVERSIONINFOEXW osInfo;
	*(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

	if (NULL != RtlGetVersion)
	{
		osInfo.dwOSVersionInfoSize = sizeof(osInfo);
		RtlGetVersion(&osInfo);
		cout << "Windows version:" << osInfo.dwMajorVersion;
	}
	else
	{
		ret = false;
	}

	return ret;
}
int main()
{
	if (!display_os_info())
	{
		cout << "display os version function not excecuted successfully";
	}
	return 0;
}