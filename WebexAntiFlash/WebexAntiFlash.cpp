#include "pch.h"
#include <Windows.h>
#include "Hook.h"

typedef BOOL(__stdcall* oFlashWindowEx)(PFLASHWINFO);
typedef BOOL(__stdcall* oFlashWindow)(HWND, BOOL);

oFlashWindowEx origflashWindowEx;
oFlashWindow origFlashWindow;

BOOL __stdcall hkFlashWindowEx(PFLASHWINFO par1);
BOOL __stdcall hkFlashWindow(HWND par1, BOOL par2);

Hook* hookFlashWindowEx;
Hook* hookFlashWindow;

void AntiFlashThread()
{
	origflashWindowEx = (oFlashWindowEx)GetProcAddress(GetModuleHandleA("user32.dll"), "FlashWindowEx");
	origFlashWindow = (oFlashWindow)GetProcAddress(GetModuleHandleA("user32.dll"), "FlashWindow");

	hookFlashWindowEx = new Hook((FARPROC)origflashWindowEx, (FARPROC)& hkFlashWindowEx);
	hookFlashWindow = new Hook((FARPROC)origFlashWindow, (FARPROC)& hkFlashWindow);

	hookFlashWindowEx->enable();
	hookFlashWindow->enable();
	//MessageBoxA(NULL, "Webex shouldn't flash anymore!", "Webex Anti-flash", 0);
}

BOOL __stdcall hkFlashWindowEx(PFLASHWINFO par1)
{
	return FALSE;
}

BOOL __stdcall hkFlashWindow(HWND par1, BOOL par2)
{
	return FALSE;
}
