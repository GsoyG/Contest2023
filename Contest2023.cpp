#include <iostream>
#include <windows.h>
#include "util/memory.h"
#include "util/process.h"

int main() {
	std::cout << "2023腾讯游戏安全竞赛 解题程序\n";

	Process process(L"contest.exe");
	if (process.GetHandle() == NULL) {
		std::cout << "打开进程失败\n";
		system("pause");
		return 1;
	}

	LPVOID address_base64Hook = (LPVOID)((INT64)process.GetModuleAddress(L"contest.exe") + 0xBA28);
	LPVOID address_fileNameHook1 = (LPVOID)((INT64)process.GetModuleAddress(L"contest.exe") + 0xC8F3);
	LPVOID address_fileNameHook2 = (LPVOID)((INT64)process.GetModuleAddress(L"contest.exe") + 0xC5C6);
	LPVOID address_fileNameHook3 = (LPVOID)((INT64)process.GetModuleAddress(L"contest.exe") + 0x772FA);
	LPVOID address_fileNameHook4 = (LPVOID)((INT64)process.GetModuleAddress(L"contest.exe") + 0x772E9);
	Memory base64Hook(process.GetHandle(), address_base64Hook);
	Memory fileNameHook1(process.GetHandle(), address_fileNameHook1);
	Memory fileNameHook2(process.GetHandle(), address_fileNameHook2);
	Memory fileNameHook3(process.GetHandle(), address_fileNameHook3);
	Memory fileNameHook4(process.GetHandle(), address_fileNameHook4);

	if (base64Hook.GetAddressSize() <= 0) {
		std::cout << "初始化失败\n";
		system("pause");
		return 1;
	}

	std::cout << "初始化成功，请输入功能对应的编号：\n";

	bool isEnable_base64Hook = false;
	bool isEnable_fileNameHook = false;

	while (true) {
		std::cout << "[1] 写入明文信息";
		if (isEnable_base64Hook)
			std::cout << " [已开启]";
		std::cout << std::endl;
		std::cout << "[2] 更改文件名称";
		if (isEnable_fileNameHook)
			std::cout << " [已开启]";
		std::cout << std::endl;

		char input = 0;
		std::cin >> input;

		switch (input) {
		case '1': {
			if (isEnable_base64Hook) {
				base64Hook.Restore();
				isEnable_base64Hook = false;
			}
			else {
				BYTE data[] = {
					0x0F, 0xB6, 0x19, 0x41, 0x88, 0x1E, 0xEB, 0xDD
				};
				base64Hook.Write(data, sizeof(data));
				isEnable_base64Hook = true;
			}
			break;
		}
		case '2': {
			if (isEnable_fileNameHook) {
				fileNameHook1.Restore();
				fileNameHook2.Restore();
				fileNameHook3.Write("contest.txt", 12);
				isEnable_fileNameHook = false;
			}
			else {
				std::cout << "请输入文件名称：\n";
				char fileName[FILENAME_MAX];
				std::cin >> fileName;

				fileNameHook1.Nop(4);
				fileNameHook2.Nop(5);
				fileNameHook2.Nop(5);
				fileNameHook3.Write(fileName, strlen(fileName) + 1);
				fileNameHook4.Write("catchmeifyoucan", 16);
				isEnable_fileNameHook = true;
			}
			break;
		}
		default:
			break;
		}
	}

	return 0;
}
