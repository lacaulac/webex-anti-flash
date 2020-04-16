#pragma once
#include <Windows.h>

#define TRAMPOLINE_SIZE 7

//This class allows easy creation of (WinAPI) hooks through a trampoline/detour hooking system
class Hook
{
public:
	Hook(FARPROC originalAddress, FARPROC hookAddress); //Creates the hook object from the original function entrypoint and hooked function address
	Hook(FARPROC originalAddress, FARPROC hookAddress, bool allowOriginalCodeUsage);

	//The following 3 functions aren't being used
	[[deprecated]]
	void allowOrigExec();
	[[deprecated]]
	void disallowOrigExec();
	template<class T> T getOrig();

	//Enables the hook. Returns true when successful
	bool enable();
	//Disables the hook. Returns true when successful
	bool disable();
	//Checks if the code present at the original function entry point corresponds to the state (enabled/disabled) of the Hook. Returns true if everything's good
	bool isCurrentCodeOk();

	//Compares two binary blocks of defined length and returns true if they're equal.
	static bool isSame(BYTE* par1, BYTE* par2, size_t length);
private:
	FARPROC m_originalAddress, m_hookAddress;
	bool isEnabled;
	BYTE trampoline[TRAMPOLINE_SIZE] = { //The "trampoline" code allowing ourselves to place a hook.
		0xB8, //mov eax, ...
		0x44, 0x33, 0x22, 0x11, //... ..., 0x11223344
		0xFF, 0xE0 //jmp eax
	};
	BYTE originalCode[TRAMPOLINE_SIZE]; //Original bytes present at the function entrypoint upon hook creation
	DWORD oldProtec; //Used by VirtualProtect
};

template<class T>
inline T Hook::getOrig()
{
	return (T)((BYTE*)originalCode);
}
