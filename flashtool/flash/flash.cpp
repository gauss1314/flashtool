#include "flash.h"
using std::wstring;

int FlashImg(wchar_t *filePath, pf pShowMessage)
{
	if (filePath == NULL)
	{
		return 0;
	}
	wstring fastbootPath = ReadFromIni(L"exeFilePath");

	/* adb reboot bootloader */
	int backslashIndex = fastbootPath.find_last_of(L"\\");
	wstring adbPath = fastbootPath.substr(0, backslashIndex + 1);
	adbPath += L"adb.exe reboot bootloader";
	RunProccessWaitOver(adbPath, pShowMessage);

	/* 解锁 */
	wstring cmd = fastbootPath + L" oem unlock";
	RunProccessWaitOver(cmd, pShowMessage);

	/* 输入recovery.img */
	wstring recoveryPath = fastbootPath + L" flash recovery ..\\recovery.img";
	RunProccessWaitOver(recoveryPath, pShowMessage);

	/* 解压 */
	HZIP hz = OpenZip((void*)filePath, 0, ZIP_FILENAME);
	if (hz == NULL)
	{
		CloseZip(hz);
		return -1;
	}

	ZIPENTRYW ze;
	GetZipItem(hz, -1, &ze);
	int nNum = ze.index;
	bool bOK = TRUE;

	for (int zi = 0; zi < nNum; zi++)
	{
		GetZipItem(hz, zi, &ze);
		if (IsImg(ze.name) && (wcscmp(ze.name, L"recovery.img") != 0))
		{
			if (UnzipItem(hz, zi, ze.name, 0, ZIP_FILENAME) == 0)
			{
				/* 执行命令 */
				wstring imgName(ze.name);
				wstring flashcmd = fastbootPath + L" flash ";
				wstring partitionName = imgName.substr(0, imgName.size() - 4);
				RunProccessWaitOver(flashcmd + partitionName + L" " + imgName, pShowMessage);
				SetFileAttributes(imgName.c_str(), GetFileAttributes(imgName.c_str()) & ~FILE_ATTRIBUTE_READONLY);
				DeleteFile(imgName.c_str());
			}
			else
			{
				bOK = false;
				break;
			}
		}
	}

	if (!bOK)
	{
		return -1;
	}

	CloseZip(hz);

	/* fastboot reboot */
	wstring reboot = fastbootPath + L" reboot";
	RunProccessWaitOver(reboot, pShowMessage);

	return 0;
}

bool IsImg(wstring str)
{
	int dotIndex;
	dotIndex = str.find_last_of('.');
	if (dotIndex != -1 && str.substr(dotIndex + 1) == L"img")
		return true;
	else
		return false;
}

int RunProccessWaitOver(std::wstring cmdline, pf pShowMessage)
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	HANDLE hReadPipe, hWritePipe;

	if (CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 20000) != TRUE)
	{
		return false;
	}

	if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0))
	{
		return false;
	}

	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	GetStartupInfo(&si);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = hWritePipe;
	si.hStdInput = hReadPipe;
	si.hStdError = hWritePipe;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	if (!CreateProcess(NULL, (LPWSTR)cmdline.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		return -1;
	}

	CloseHandle(hWritePipe);

	wchar_t Buffer[1000];
	ZeroMemory(Buffer, 1000);
	DWORD NumberOfBytesRead = 0;
	while (ReadFile(hReadPipe, Buffer, 999, &NumberOfBytesRead, NULL))
	{
		pShowMessage(Buffer);
	}
	DWORD ExitCode;
	BOOL flag = GetExitCodeProcess(pi.hProcess, &ExitCode);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return flag;
}

void WriteToIni(wstring key, wstring name)
{
	wstring iniFilePath = L".\\config.ini";
	if ((_access("config.ini", 0)) == -1)
	{
		HANDLE hIniFile = CreateFile(iniFilePath.c_str(), GENERIC_ALL, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(hIniFile);
	}
	WritePrivateProfileString(L"section", key.c_str(), name.c_str(), iniFilePath.c_str());
}

wstring ReadFromIni(wstring key)
{
	wstring iniFilePath = L".\\config.ini";
	wchar_t lpFilePath[MAX_PATH];
	GetPrivateProfileString(L"section", key.c_str(), NULL, lpFilePath, MAX_PATH, iniFilePath.c_str());
	wstring value(lpFilePath);
	return value;
}