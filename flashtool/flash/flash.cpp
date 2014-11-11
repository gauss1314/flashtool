#include "flash.h"

int FlashImg(std::wstring filePath, pf f1)
{
	if (filePath.empty())
	{
		return 0;
	}

	// 解压
	std::wstring iniFilePath = L".\\config.ini";
	wchar_t lpExeFile[MAX_PATH];
	GetPrivateProfileString(L"section", L"zipFilePath", NULL, lpExeFile, MAX_PATH, iniFilePath.c_str());
	std::wstring FastbootPath(lpExeFile);
	std::vector<std::wstring>  imgVec;

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
				imgVec.push_back((std::wstring)ze.name);
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

	/* 解锁 */
	std::wstring cmd = FastbootPath + L" oem unlock";
	RunProccessWaitOver(cmd, f1);

	/* 创建进程执行命令 */
	std::wstring flashcmd = FastbootPath + L" flash ";

	for (std::vector<std::wstring>::iterator ite = imgVec.begin(); ite != imgVec.end(); ite++)
	{
		std::wstring partitionName = ite->substr(0, ite->length() - 4);
		RunProccessWaitOver(flashcmd + partitionName + L" " + *ite, f1);
		std::wstring tmpFile(*ite);
		SetFileAttributes(tmpFile.c_str(), GetFileAttributes(tmpFile.c_str()) & ~FILE_ATTRIBUTE_READONLY);
		DeleteFile(tmpFile.c_str());
		DWORD result = GetLastError();
	}
	return 0;
}

bool IsImg(std::wstring str)
{
	int dotIndex;
	dotIndex = str.find_last_of('.');
	if (dotIndex != -1 && str.substr(dotIndex + 1) == L"img")
		return true;
	else
		return false;
}

int RunProccessWaitOver(std::wstring cmdline, pf abc)
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

	WCHAR* tmpcmdline = new WCHAR[cmdline.length() + 1];
	ZeroMemory(tmpcmdline, cmdline.length() + 1);
	_tcscpy_s(tmpcmdline, cmdline.length() + 1, cmdline.c_str());
	if (!CreateProcess(NULL, tmpcmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
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
	std::string ReturnMsg;
	while (ReadFile(hReadPipe, Buffer, 999, &NumberOfBytesRead, NULL))
	{
		abc((TCHAR *)Buffer);
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return flag;
}

void iniFile(std::wstring testtool)
{
	std::wstring iniFilePath = L".\\config.ini";
	if ((_access("config.ini", 0)) == -1)
	{
		HANDLE hIniFile = CreateFile(iniFilePath.c_str(), GENERIC_ALL, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(hIniFile);
	}
	WritePrivateProfileString(L"section", L"zipFilePath", testtool.c_str(), iniFilePath.c_str());
}