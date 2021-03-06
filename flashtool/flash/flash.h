﻿#include <iostream>
#include <vector>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <strsafe.h>
#include "XUnzip\XUnzip.h"
#include <io.h>

// 判断是否是img格式的文件
bool IsImg(std::wstring str);

// 函数指针
typedef void (*pf)(wchar_t *Buf);

// 创建进程执行并等待返回
int RunProccessWaitOver(std::wstring cmdline, pf pShowMessage);

// 烧写img文件
int FlashImg(wchar_t *filePath, pf pShowMessage);

// 写入ini文件
void WriteToIni(std::wstring key, std::wstring name);

// 从ini文件读取
std::wstring ReadFromIni(std::wstring key);