#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <sys/stat.h>
#define WINDOWS_MEAN_AND_LEAN

/*
          _   _             _     __  __                                 
     /\  | | | |           | |   |  \/  |                                
    /  \ | |_| |_ __ _  ___| |__ | \  / | ___ _ __ ___   ___  _ __ _   _ 
   / /\ \| __| __/ _` |/ __| '_ \| |\/| |/ _ \ '_ ` _ \ / _ \| '__| | | |
  / ____ \ |_| || (_| | (__| | | | |  | |  __/ | | | | | (_) | |  | |_| |
 /_/    \_\__|\__\__,_|\___|_| |_|_|  |_|\___|_| |_| |_|\___/|_|   \__, |
                                                                    __/ |
                                                                   |___/ 
	AttachMemory 1.29 C++ API Source Code
	Written By Senator

	- Contact -
	Github Page : http://v2cash.github.io/AttachMemory/
	Github :      https://github.com/v2cash
	Steam :       http://steamcommunity.com/id/2cash/

	A Simple Usage of AttachMemory:

	char* IS_INJECTED = AttachMem::InsertMemory(AttachMem::GetProcessID("steam"), "C:/ThisDLL.dll");
	if (IS_INJECTED != "ATTACHMEMORY_RESULT_OK")
		cout << "Error, Reason: " << IS_INJECTED;
*/

namespace AttachMem
{

	// Grab the Process ID 
	DWORD GetProcessID(char* procName)
	{
		BOOL pFound;
		PROCESSENTRY32 pe;
		HANDLE ProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pe.dwSize = sizeof(pe);
		pFound = Process32First(ProcSnap, &pe);
		while (pFound) {
			if (strstr(pe.szExeFile, procName)) {
				return pe.th32ProcessID;
			}

			pe.dwSize = sizeof(pe);
			pFound = Process32Next(ProcSnap, &pe);
		}
		return 0;
	}

	// Check if File Exists
	bool fileExists(const std::string& file) {
		struct stat buf;
		return (stat(file.c_str(), &buf) == 0);
	}

	// Inserts Mmeory Into The Process
	char* InsertMemory(DWORD procID, char *File, char *ProcName)
	{
		if (GetProcessID(ProcName) == 0) return "ATTACHMEMORY_RESULT_PROCESS_NOT_FOUND";
		if (!fileExists(File)) return "ATTACHMEMORY_RESULT_FILE_NOT_FOUND";

		HMODULE hLocKernel32 = GetModuleHandle("Kernel32");
		FARPROC hLocLoadLibrary = GetProcAddress(hLocKernel32, "LoadLibraryA");
		HANDLE hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, procID);
		LPVOID hRemoteMem = VirtualAllocEx(hProc, NULL, strlen(File) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(hProc, hRemoteMem, File, strlen(File) + 1, 0);
		HANDLE hRemoteThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)hLocLoadLibrary, hRemoteMem, 0, NULL);
		CloseHandle(hProc);
		return "ATTACHMEMORY_RESULT_OK";
	}
}