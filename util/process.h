#pragma once

class Process {
private:
	HANDLE hProcess;
	DWORD processId;
	LPCWSTR errorMessage;

	BOOL OpenProcessId();

public:
	Process();
	Process(LPCWSTR processName);

	LPCWSTR GetError();
	HANDLE GetHandle();

	BOOL RunProgramPath(LPCWSTR programPath);
	BOOL OpenProcessName(LPCWSTR processName);

	LPVOID GetModuleAddress(LPCWSTR moduleName);
};
