
#include "ProcessStatusCheck.h"

using namespace std;

/* @author: Subhash
   @function: to display the given process running status and its memory usage
   @param const wchar_t *processName: wide char pointer array of the process name
   @return: true if given process is present or false if not present
*/
bool IsProcessRunning(const wchar_t *processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	entry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(snapshot, &entry))
	{
		while (Process32Next(snapshot, &entry))
		{
			if (!_wcsicmp(entry.szExeFile, processName))
			{
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
				GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
				cout << "Given process is running" << endl;
				cout << "Process ID " << entry.th32ProcessID << endl;
				cout << "Memory usage of the given process is " << pmc.WorkingSetSize << endl;
				exists = true;
			}
		}
	}
	CloseHandle(snapshot);
	return exists;
}

int main()
{
	wchar_t *process_name = (wchar_t*)malloc(10 * sizeof(wchar_t));
	cout << "Enter the process name : ";
	wcin >> process_name;
	if (!IsProcessRunning(process_name))
	{
		cout << "Given process is not running" << endl;
	}
	return 0;
}
