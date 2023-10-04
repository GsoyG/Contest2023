#pragma once

#define MAX_HIT_COUNT 1
#define BACKUP_MAX_SIZE 512

class Memory {
private:
	HANDLE hProcess;
	LPVOID address[MAX_HIT_COUNT];
	SIZE_T addressSize;
	LPBYTE backupBytes[BACKUP_MAX_SIZE];
	SIZE_T backupBytesSize;

	BOOL Backup(SIZE_T size);

public:
	Memory();
	Memory(HANDLE hProcess);
	Memory(HANDLE hProcess, LPVOID address);
	Memory(HANDLE hProcess, PBYTE pattern, SIZE_T patternLen, ULONG start, ULONG end,
		INT inc, INT protection);

	VOID SetProcess(HANDLE hProcess);
	HANDLE GetProcess();

	LPVOID GetAddress();
	SIZE_T GetAddressSize();

	DWORD AOBScan(PBYTE pattern, SIZE_T patternLen, ULONG start, ULONG end, INT inc, INT protection);

	//BOOL Modify();
	BOOL Nop(SIZE_T len);
	BOOL Write(LPCVOID lpBuffer, SIZE_T nSize);
	BOOL Restore();
};
