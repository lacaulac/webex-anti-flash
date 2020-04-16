#include "pch.h"
#include <Windows.h>
#include "Hook.h"

typedef BOOL(__stdcall* oFlashWindowEx)(PFLASHWINFO);

oFlashWindowEx origflashWindowEx;

BOOL __stdcall hkFlashWindowEx(PFLASHWINFO par1);

Hook* hookFlashWindowEx;

void AntiFlashThread()
{
	origflashWindowEx = (oFlashWindowEx)GetProcAddress(GetModuleHandleA("user32.dll"), "FlashWindowEx");
	hookFlashWindowEx = new Hook((FARPROC)origflashWindowEx, (FARPROC)& hkFlashWindowEx);
	hookFlashWindowEx->enable();
	//MessageBoxA(NULL, "Webex shouldn't flash anymore!", "Webex Anti-flash", 0);
}

BOOL __stdcall hkFlashWindowEx(PFLASHWINFO par1)
{
	return FALSE;
}
