#include "windows.h"
#include "memory.h"
#include <stdlib.h>

#define BUFFER_SIZE 4096

BOOL Memory::Backup(SIZE_T size) {
	if (size > BACKUP_MAX_SIZE) return FALSE;

	backupBytesSize = size;
	return ReadProcessMemory(hProcess, address[0], backupBytes, size, NULL);
}

Memory::Memory() : hProcess(NULL), address(),
	addressSize(0), backupBytes(), backupBytesSize(0) {}

Memory::Memory(HANDLE hProcess) : Memory() {
	Memory::hProcess = hProcess;
}

Memory::Memory(HANDLE hProcess, LPVOID address) : Memory(hProcess) {
	Memory::address[0] = address;
	addressSize = 1;
}

Memory::Memory(HANDLE hProcess, PBYTE pattern, SIZE_T patternLen, ULONG start, ULONG end,
	INT inc, INT protection) : Memory(hProcess) {
	addressSize = AOBScan(pattern, patternLen, start, end, inc, protection);
}

VOID Memory::SetProcess(HANDLE hProcess) {
	Memory::hProcess = hProcess;
}

HANDLE Memory::GetProcess() {
	return hProcess;
}

LPVOID Memory::GetAddress() {
	return address[0];
}

SIZE_T Memory::GetAddressSize() {
	return addressSize;
}

DWORD Memory::AOBScan(PBYTE pattern, SIZE_T patternLen, ULONG start, ULONG end, INT inc, INT protection) {
	MEMORY_BASIC_INFORMATION memoryInfo{};
	ULONG regionAddr = start;

	BYTE buffer[BUFFER_SIZE];
	PBYTE memoryBuffer = buffer;

	INT resultCount = 0;

	while (regionAddr < end) {
		if (!VirtualQueryEx(hProcess, (LPCVOID)regionAddr, &memoryInfo, sizeof(MEMORY_BASIC_INFORMATION)))
			return -1;

		if ((memoryInfo.Protect & protection) != 0) {
			ULONG patternAddr = regionAddr;

			while (patternAddr < regionAddr + memoryInfo.RegionSize) {
				if (!ReadProcessMemory(hProcess, (LPVOID)patternAddr, memoryBuffer, BUFFER_SIZE, NULL)) {
					break;
				}

				for (SIZE_T i = 0; i < BUFFER_SIZE; i += inc) {
					BOOL patternBreak = FALSE;

					for (SIZE_T k = 0; k < patternLen; k++) {
						if (pattern[k] != memoryBuffer[k]) {
							patternBreak = TRUE;
							break;
						}
					}

					if (!patternBreak) {
						address[resultCount] = (LPVOID)patternAddr;
						resultCount++;
						if (resultCount >= MAX_HIT_COUNT) return resultCount;
					}
					else {
						patternAddr += inc; memoryBuffer += inc;
					}
				}
				memoryBuffer -= BUFFER_SIZE;
			}
		}
		regionAddr += memoryInfo.RegionSize;
	}
	return resultCount;
}

BOOL Memory::Nop(SIZE_T len) {
	Backup(len);

	BYTE *nop = new BYTE[len]();
	for (SIZE_T i = 0; i < len; i++)
		nop[i] = (BYTE)0x90;

	BOOL ret = WriteProcessMemory(hProcess, address[0], nop, len, NULL);
	delete[] nop;

	return ret;
}

BOOL Memory::Write(LPCVOID lpBuffer, SIZE_T nSize) {
	Backup(nSize);

	return WriteProcessMemory(hProcess, address[0], lpBuffer, nSize, NULL);
}

BOOL Memory::Restore() {
	if (hProcess != NULL)
		return WriteProcessMemory(hProcess, address[0], backupBytes, backupBytesSize, NULL);
	else return  FALSE;
}
