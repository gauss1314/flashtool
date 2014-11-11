#include <iostream>
#include <vector>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <strsafe.h>
#include "XUnzip\XUnzip.h"
#include <io.h>

const int BUFSIZE = 4096;
// 判断是否是img格式的文件
bool IsImg(std::wstring str);

typedef void (*pf)(TCHAR *Buf);

// 创建进程执行并等待返回
int RunProccessWaitOver(std::wstring cmdline, pf abc);

// 烧写img文件
int FlashImg(std::wstring filePath, pf f1);

// 写入ini文件
void iniFile(std::wstring testtool);