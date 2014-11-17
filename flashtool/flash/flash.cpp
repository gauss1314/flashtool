#include "flash.h"
using std::wstring;

int FlashImg(wstring filePath, pf f1)
{
	if (filePath.empty())
	{
		return 0;
	}
	wstring iniFilePath = L".\\config.ini";
	wchar_t lpExeFile[MAX_PATH];
	GetPrivateProfileString(L"section", L"zipFilePath", NULL, lpExeFile, MAX_PATH, iniFilePath.c_str());
	wstring FastbootPath(lpExeFile);

	/* 解锁 */
	wstring cmd = FastbootPath + L" oem unlock";
	RunProccessWaitOver(cmd, f1);

	/* 解压 */
	HZIP hz = OpenZip((void*)filePath.c_str(), 0, ZIP_FILENAME);
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
		if (IsImg(ze.name))
		{
			if (UnzipItem(hz, zi, ze.name, 0, ZIP_FILENAME) == 0)
			{
				/* 执行命令 */
				wstring imgName(ze.name);
				wstring flashcmd = FastbootPath + L" flash ";
				wstring partitionName = imgName.substr(0, imgName.size() - 4);
				RunProccessWaitOver(flashcmd + partitionName + L" " + imgName, f1);
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

int RunProccessWaitOver(wstring cmdline, pf abc)
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
	WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD ExitCode;
	BOOL flag = GetExitCodeProcess(pi.hProcess, &ExitCode);

	CloseHandle(hWritePipe);

	wchar_t Buffer[1000];
	ZeroMemory(Buffer, 1000);
	DWORD NumberOfBytesRead = 0;
	while (ReadFile(hReadPipe, Buffer, 999, &NumberOfBytesRead, NULL))
	{
		abc(Buffer);
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return flag;
}

void iniFile(wstring testtool)
{
	wstring iniFilePath = L".\\config.ini";
	if ((_access("config.ini", 0)) == -1)
	{
		HANDLE hIniFile = CreateFile(iniFilePath.c_str(), GENERIC_ALL, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(hIniFile);
	}
	WritePrivateProfileString(L"section", L"zipFilePath", testtool.c_str(), iniFilePath.c_str());
}