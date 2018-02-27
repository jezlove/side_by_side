/*
	C "SideBySide.c"
	version 0.1
	contact jez@jezlove.com
*/

// compiled with dmc

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShellAPI.h>

#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")

__declspec(dllimport) DWORD WINAPI GetProcessId(HANDLE);

typedef struct {
    DWORD processId;
    HWND hwnd;
} LaunchedApplication;

BOOL __stdcall onWindow(HWND hwnd, LPARAM lParam) {
    LaunchedApplication* la = (LaunchedApplication*)(lParam);
    DWORD windowProcessId;
    GetWindowThreadProcessId(hwnd, &windowProcessId);
    if(windowProcessId == la->processId) {
		la->hwnd = hwnd;
		return FALSE;
	}
    return TRUE;
}

HWND launch(LPCWSTR path) {
	SHELLEXECUTEINFOW sei = {
		sizeof(SHELLEXECUTEINFOW),
		SEE_MASK_NOCLOSEPROCESS,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		SW_SHOWNORMAL,
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		NULL,
	};
	sei.lpFile = path;
	if(ShellExecuteExW(&sei)) {
		LaunchedApplication la = {
			0,
			NULL,
		};
		la.processId = GetProcessId(sei.hProcess);
		WaitForInputIdle(sei.hProcess, INFINITE);
		EnumDesktopWindows(NULL, (WNDENUMPROC)(&onWindow), (LPARAM)(&la));
		return la.hwnd;
	}
	return NULL;
}

int __stdcall WinMain(HINSTANCE ih, HINSTANCE oih, LPSTR a, int sc) {

	int parameterCount;
	LPWSTR* parameters = CommandLineToArgvW(GetCommandLineW(), &parameterCount);
	if(NULL == parameters) { return 0; }

	if(3 == parameterCount) {
		HWND hwnd1 = launch(parameters[1]);
		HWND hwnd2 = launch(parameters[2]);
		if(hwnd1 && hwnd2) {

			HWND hwnds[2];

			hwnds[0] = hwnd1;
			hwnds[1] = hwnd2;

			TileWindows(NULL, MDITILE_VERTICAL, NULL, 2, hwnds);
		}
	}

	LocalFree(parameters);

	return 0;
}