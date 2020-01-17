/*
*	Simple Task Manager Application
*	Version 1.0
*	simple task manager console application that uses the Win32 API to find
*	and open processes, and suspend and resume threads.  Uses CreateToolHelp32SnapShot
*   All h files are pre-complied in pch.h
*	prefixes : dw->DWORD h->HANDLE, a->array, cb->count_of_bytes, fn->function, sz->Zero Terminted String
*/

#include "pch.h"

using namespace std;

//function prototypes
void processName(DWORD x);
BOOL ListProcessThreads(DWORD x);
HANDLE openThread(DWORD x);
BOOL suspendThread(HANDLE x);
BOOL fnResumeThread(HANDLE x);
void getProcessInfo(DWORD x);
void getHelp();

int main()
{
	string action;    //list_processes //open_process //suspend_thread //resume_thread //exit
	string resumeThread;
	int processId = 0;
	DWORD dwThreadId;
	HANDLE hThread;
	DWORD dwResumeThreadId;
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	//actionType activeType;

	//Get List of process as data type enum. 
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		return 1;
	}

	int flag = 0;

	//Main Program Loop
	while (flag == 0) {

		cout << "What would you like to do? (Use help)" << endl;
		cin >> action;

		if (action == "list_processes") {
			cout << "Running Process List" << endl;
			cProcesses = cbNeeded / sizeof(DWORD);
			for (DWORD i = 0; i < cProcesses; i++) {
				processName(aProcesses[i]);
			}
		}
		else if (action == "open_process") {
			cout << "Enter a process ID to open: " << endl;
			cin >> processId;
			ListProcessThreads(processId);
		}
		else if (action == "suspend_thread") {
			cout << "Enter the thread ID to suspend: " << endl;
			cin >> dwThreadId;
			hThread = openThread(dwThreadId);
			if (!suspendThread(hThread)) {
				cout << "Failed to suspend thread ID " << dwThreadId << " " << endl;
			}
			else {
				cout << "Succesfully suspended thread ID " << dwThreadId << " " << endl;
			}
			CloseHandle(hThread);
		}
		else if (action == "resume_thread") {
			cout << "Enter the thread ID to resume" << endl;
			cin >> dwResumeThreadId;

			hThread = openThread(dwResumeThreadId);
			if (!fnResumeThread(hThread)) {
				cout << "Failed to resume thread ID " << dwResumeThreadId << " " << endl;
			}
			cout << "Successfully resumed thread ID " << dwResumeThreadId << " " << endl;
		}
		else if (action == "get_process_mem") {
			cout << "Enter a process ID to see the memory usage" << endl;
			cin >> processId;
			getProcessInfo(processId);
		}
		else if (action == "help") {
			getHelp();
		}
		else if (action == "exit") {
			flag = 1;
		}
		else {
			cout << "Please enter a valid action type" << endl;
		}
	}
	system("pause");
	return 0;
}

void processName(DWORD processId) {
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ, false, processId);

	if (NULL != hProcess) {
		HMODULE hMod;  //
		DWORD cbNeeded; //count of bytes 

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}
	}
	_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processId);
	CloseHandle(hProcess);
}

BOOL ListProcessThreads(DWORD processId) {
	//Define a th32 object;
	THREADENTRY32 th32;
	//declare a default const;
	HANDLE hThreadSnapShot = INVALID_HANDLE_VALUE;
	//get the snap shot of all threads for a process
	hThreadSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	//Ensure the snapshot doesnt fail
	if (hThreadSnapShot == INVALID_HANDLE_VALUE) {
		return(FALSE);
	}
	//Set size of the data structure
	th32.dwSize = sizeof(THREADENTRY32);
	//Check first thread before looping over list
	if (!Thread32First(hThreadSnapShot, &th32))
	{
		CloseHandle(hThreadSnapShot);          // clean the snapshot object
		return(FALSE);
	}
	do
	{
		//Output all of the threads for the process.  
		if (th32.th32OwnerProcessID == processId)
		{
			_tprintf(TEXT("\n\n     THREAD ID      = %d"), th32.th32ThreadID);
			_tprintf(TEXT("\n     Base priority  = %d"), th32.tpBasePri);
			_tprintf(TEXT("\n"));
		}
	} while (Thread32Next(hThreadSnapShot, &th32));
	CloseHandle(hThreadSnapShot); //clean the snapshot object
	return TRUE;
}

HANDLE openThread(DWORD threadId) {
	HANDLE hThread;
	hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadId);
	return hThread;
}

BOOL suspendThread(HANDLE hThread) {
	return ((int)SuspendThread(hThread) >= 0);
}

BOOL fnResumeThread(HANDLE hThread) {
	return ((int)ResumeThread(hThread) >= 0);
}

void getHelp() {
	cout << "list_processes - get a list of all running proceses" << endl;
	cout << "open_process - opens a process object and returns its running threads" << endl;
	cout << "suspend_thread - suspends a thread by the handle to the thread object" << endl;
	cout << "resume_thread - resumes a thread by the handle to the thread object" << endl;
	cout << "get_process_mem - get data points about a processes memory information" << endl;
	cout << "exit - exits program" << endl;
}

void getProcessInfo(DWORD processId) {
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ, false, processId);

	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
		_tprintf(TEXT("\n\n     PageFaultCount: = %d"), pmc.PageFaultCount);
		_tprintf(TEXT("\n\n     PeakWorkingSetSize: = %d"), pmc.WorkingSetSize);
		_tprintf(TEXT("\n\n     QuotaPeakPagedPoolUsage: = %d"), pmc.QuotaPeakPagedPoolUsage);
		_tprintf(TEXT("\n\n     QuotaPagedPoolUsage: = %d"), pmc.QuotaPagedPoolUsage);
		_tprintf(TEXT("\n\n     QuotaPeakNonPagedPoolUsage: = %d"), pmc.QuotaPeakNonPagedPoolUsage);
		_tprintf(TEXT("\n\n     QuotaNonPagedPoolUsage: = %d"), pmc.QuotaNonPagedPoolUsage);
		_tprintf(TEXT("\n\n     PagefileUsage: = %d"), pmc.PagefileUsage);
		_tprintf(TEXT("\n\n     PeakPagefileUsage: = %d"), pmc.PeakPagefileUsage);
		_tprintf(TEXT("\n"));
	}
	CloseHandle(hProcess);
}