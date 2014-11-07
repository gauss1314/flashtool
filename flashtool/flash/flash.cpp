#include "flash.h"

//HANDLE g_hPipe_get = NULL;
//HANDLE g_tmp = NULL;

int FlashImg(std::wstring filePath, pf f1)
{
	if (filePath.empty())
	{
		std::wcout << "GlassX 刷机工具 0.0.1" << std::endl;
		std::wcout << "输入格式：flashtool path/to/xxx.zip" << std::endl;
		return 0;
	}
	// 解压
	std::wstring FastbootPath = L"FastbootTest.exe";
	std::vector<std::wstring>  imgVec;

	HZIP hz = OpenZip((void*)filePath.c_str(), 0, ZIP_FILENAME);
	if (hz == NULL)
	{
		CloseZip(hz);
		std::wcout << "读取zip文件失败" << std::endl;
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
				std::wcout << "已经得到解压文件" << ze.name << std::endl;
			}
			else
			{
				std::wcout << "解压" << ze.name << "失败! 程序即将退出" << std::endl;
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
		DWORD dd = GetFileAttributes(tmpFile.c_str());
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
		std::wcout << "创建程失败!" << GetLastError() << std::endl;
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

/*int RunProccessWaitOver(std::wstring cmdline)
{

PROCESS_INFORMATION pi;
ZeroMemory(&pi, sizeof(pi));

WCHAR* tmpcmdline = new WCHAR[cmdline.length() + 1];
ZeroMemory(tmpcmdline, cmdline.length() + 1);
_tcscpy_s(tmpcmdline, cmdline.length() + 1, cmdline.c_str());

SECURITY_ATTRIBUTES saAttr;
saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
saAttr.bInheritHandle = TRUE;
saAttr.lpSecurityDescriptor = NULL;

if (!CreatePipe(&g_hPipe_get, &g_tmp, &saAttr, 0))
ErrorExit(TEXT("StdoutRd CreatePipe"));

if (!SetHandleInformation(g_hPipe_get, HANDLE_FLAG_INHERIT, 0))
ErrorExit(TEXT("Stdout SetHandleInformation"));

STARTUPINFO si;
si.cb = sizeof(si);
ZeroMemory(&si, sizeof(si));

//si.hStdInput = g_hPipe_get;
si.hStdError = g_tmp;
si.hStdOutput = g_tmp;
si.dwFlags |= STARTF_USESTDHANDLES;

if (!CreateProcess(NULL, tmpcmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
{
std::wcout << "创建程失败!" << GetLastError() << std::endl;
return -1;
}

ReadFromPipe();
WaitForSingleObject(pi.hProcess, INFINITE);

// 获取子进程返回值
DWORD ExitCode;
BOOL flag = GetExitCodeProcess(pi.hProcess, &ExitCode);

CloseHandle(pi.hProcess);
CloseHandle(pi.hThread);

return flag;
}

void ErrorExit(PTSTR lpszFunction)
{
LPVOID lpMsgBuf;
LPVOID lpDisplayBuf;
DWORD dw = GetLastError();

FormatMessage(
FORMAT_MESSAGE_ALLOCATE_BUFFER |
FORMAT_MESSAGE_FROM_SYSTEM |
FORMAT_MESSAGE_IGNORE_INSERTS,
NULL,
dw,
MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
(LPTSTR)&lpMsgBuf,
0, NULL);

lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40)*sizeof(TCHAR));
StringCchPrintf((LPTSTR)lpDisplayBuf,
LocalSize(lpDisplayBuf) / sizeof(TCHAR),
TEXT("%s failed with error %d: %s"),
lpszFunction, dw, lpMsgBuf);
MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

LocalFree(lpMsgBuf);
LocalFree(lpDisplayBuf);
ExitProcess(1);
}

void ReadFromPipe(void)
{
DWORD dwRead;
CHAR chBuf[BUFSIZE] = { 0 };
BOOL bSuccess = FALSE;

for (;;)
{
bSuccess = ReadFile(g_hPipe_get, chBuf, BUFSIZE, &dwRead, NULL);
if (!bSuccess || dwRead == 0)
break;
}
}*/