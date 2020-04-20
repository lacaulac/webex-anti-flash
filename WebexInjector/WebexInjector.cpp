// WebexInjector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdio>
#include <string>

bool inject(char* path, unsigned long pid);
bool getPIDByName(LPCWSTR procName, unsigned long* pid);


int main(int argc, char** argv)
{
	unsigned long targetPID = 0;
	wchar_t convText[255]; //If we need to convert to LPCWSTR from argv[2]


	if (argc == 3)
	{
		if (strcmp(argv[1], "pid") == 0)
		{
			targetPID = strtoul(argv[1], NULL, 10);
		}
		else if (strcmp(argv[1], "name") == 0)
		{
			mbstowcs_s(NULL, convText, argv[2], 254);
			targetPID = getPIDByName(convText, &targetPID);
		}
		else
		{
			std::cout << "Usage: " << argv[0] << " pid/name <PID/Name>" << std::endl;
			return -3;
		}
	}
	else
	{
		//We try to find atmgr.exe on our own
		getPIDByName(L"atmgr.exe", &targetPID);
	}

	if (targetPID == 0)
	{
		//std::cout << "Couldn't find the Webex process. Have you tried starting the program as an Administrator?" << std::endl;
		MessageBoxA(NULL, "Couldn't find the Webex process. Have you tried starting WebexInjector.exe as an Administrator?\nImpossible de trouver le processus Webex. Essayez de lancer WebexInjector.exe en tant qu'administrateur", "WebexInjector error", MB_ICONERROR);
		return -1;
	}
	if (inject((char*)"WebexAntiFlash.dll", targetPID))
	{
		//std::cout << "Successful injection" << std::endl;
		MessageBoxA(NULL, "The injection was successful.\nL'injection n'a rencontré aucun problème", "WebexInjector", MB_ICONINFORMATION);
		return 0;
	}
	else
	{
		//std::cout << "An error occurred while injecting into Webex. Have you tried starting the program as an Administrator?" << std::endl;
		MessageBoxA(NULL, "An error occurred while injecting into Webex. Have you tried starting WebexInjector.exe as an Administrator?\nUne erreur s'est produite lors de l'injection. Essayez de lancer WebexInjector.exe en tant qu'administrateur", "WebexInjector error", MB_ICONERROR);
		return -2;
	}
	
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

bool inject(char* path, unsigned long pid)
{
	HANDLE procHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, false, pid);
	if (procHandle == NULL)
		return false;

	printf("[INJECTION] Got a handle to %d\n", pid);

	PVOID dllNameAdr = VirtualAllocEx(procHandle, NULL, strlen(path) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (dllNameAdr == NULL)
		return false;

	printf("[INJECTION] Got %d bytes into %d\n", strlen(path) + 1, pid);

	if (WriteProcessMemory(procHandle, dllNameAdr, path, strlen(path) + 1, NULL) == NULL)
		return false;

	printf("[INJECTION] Wrote %d bytes into %d\n", strlen(path) + 1, pid);

	HANDLE tHandle = CreateRemoteThread(procHandle, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), dllNameAdr, 0, 0);
	if (tHandle == NULL)
		return false;

	printf("[INJECTION] Started LoadLibraryA into %d\n", pid);


	return true;
}

bool getPIDByName(LPCWSTR procName, unsigned long* pid)
{
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE procsSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(procsSnapshot, &pEntry) == TRUE)
	{
		do
		{
			if (lstrcmpW(pEntry.szExeFile, procName) == 0)
			{
				//std::cout << "Found at PID " << pEntry.th32ProcessID << std::endl;
				*pid = pEntry.th32ProcessID;
				break;
			}
		} while (Process32Next(procsSnapshot, &pEntry) == TRUE);
	}
	return false;
}
