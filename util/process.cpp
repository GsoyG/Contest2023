#include "windows.h"
#include "process.h"
#include "tlhelp32.h"
#include "psapi.h"

BOOL Process::OpenProcessId() {
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (hProcess == NULL) {
		errorMessage = L"�򿪽���ʧ��";
		return FALSE;
	}

	return TRUE;
}

Process::Process() {
	hProcess = NULL;
	processId = NULL;
	errorMessage = L"";
}

Process::Process(LPCWSTR processName) {
	Process();
	if (OpenProcessName(processName)) {
		OpenProcessId();
		return;
	}

	if (RunProgramPath(processName)) {
		int waitTime = 0;
		while (!OpenProcessName(processName)) {
			if (waitTime > 10) {
				errorMessage = L"�ȴ�����������ʱ";
				return;
			}

			Sleep(1000);
			waitTime++;
		}

		OpenProcessId();
	}
}

LPCWSTR Process::GetError() {
	return errorMessage;
}

HANDLE Process::GetHandle() {
	return hProcess;
}

BOOL Process::RunProgramPath(LPCWSTR programPath) {
	STARTUPINFO startInfo{};
	PROCESS_INFORMATION procInfo{};

	WCHAR dir[MAX_PATH], fullPath[MAX_PATH];
	GetModuleFileName(NULL, dir, MAX_PATH);
	wcsrchr(dir, '\\')[1] = 0;

	lstrcpy(fullPath, dir);
	lstrcat(fullPath, programPath);

	BOOL result = CreateProcess(fullPath, NULL, NULL, NULL, FALSE,
		CREATE_SUSPENDED, NULL, dir, &startInfo, &procInfo);
	if (result) {
		ResumeThread(procInfo.hThread);

		hProcess = procInfo.hProcess;
		return TRUE; 
	}
	else {
		errorMessage = L"���г���ʧ��";
		return FALSE;
	}
}

BOOL Process::OpenProcessName(LPCWSTR processName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (INVALID_HANDLE_VALUE == hSnapshot) return NULL;

	PROCESSENTRY32 processEntry = { sizeof(processEntry) };
	for (BOOL entryOpened = Process32First(hSnapshot, &processEntry);
		entryOpened;
		entryOpened = Process32Next(hSnapshot, &processEntry)) {
		if (!wcscmp(processEntry.szExeFile, processName)) {
			processId = processEntry.th32ProcessID;
		}
	}
	CloseHandle(hSnapshot);

	if (processId == NULL) {
		errorMessage = L"δ�ҵ�ָ������";
		return FALSE;
	}

	return TRUE;
}

LPVOID Process::GetModuleAddress(LPCWSTR moduleName) {
	//ö����Ϸ��������DLL����·�� �жϽ���·����β�������ַ��� ��ȡDLL��ַ
	HMODULE hModules[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
		for (int i = 0; i < sizeof(hModules); i++) {
			WCHAR filename[MAX_PATH];
			WCHAR *index;
			if (GetModuleFileNameEx(hProcess, hModules[i], filename, MAX_PATH)) {
				index = wcsstr(filename, moduleName);
				if (index) {
					return hModules[i];
				}
			}
		}
	}
}
