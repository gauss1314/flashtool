#include <iostream>
#include <vector>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <strsafe.h>
#include "XUnzip\XUnzip.h"

const int BUFSIZE = 4096;
// �ж��Ƿ���img��ʽ���ļ�
bool IsImg(std::wstring str);

typedef void (*pf)(TCHAR *Buf);

// ��������ִ�в��ȴ�����
int RunProccessWaitOver(std::wstring cmdline, pf abc);

// ��дimg�ļ�
int FlashImg(std::wstring filePath, pf f1);

void WriteToPipe(void);
void ReadFromPipe(void);
void ErrorExit(PTSTR);