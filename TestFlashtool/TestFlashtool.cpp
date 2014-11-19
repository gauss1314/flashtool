#include <windows.h>
#include <iostream>

using std::wcout;
using std::endl;

int wmain(int argc, wchar_t *argv[])
{
	for (int i = 0; i < argc; i++)
	{
		wcout << argv[i] << " ";
	}
	wcout << endl;
	return 0;
}