#include "pch.h"
#include "Hook.h"

Hook::Hook(FARPROC originalAddress, FARPROC hookAddress) : Hook(originalAddress, hookAddress, false)
{
	//Just calls the other constructor with a default parameter
}

Hook::Hook(FARPROC originalAddress, FARPROC hookAddress, bool allowOriginalCodeUsage)
{
	memcpy(originalCode, (void*)originalAddress, TRAMPOLINE_SIZE); //Makes a backup of the original assembly code
	memcpy((void*)(trampoline + 1), &hookAddress, 4); //Filling the hooked function's address into the trampoline's code

	//Initialising the fields
	isEnabled = false;
	m_hookAddress = hookAddress;
	m_originalAddress = originalAddress;
	oldProtec = NULL;
	/*
	if (allowOriginalCodeUsage) //Not useful, just introducing bugs everywhere when set to true :'(
		allowOrigExec(); //Let's not use that, shall we?
	//*/
}

void Hook::allowOrigExec()
{
	VirtualProtect(originalCode, TRAMPOLINE_SIZE, PAGE_EXECUTE_READ, &oldProtec); //Gotta remove this someday
}

void Hook::disallowOrigExec()
{
	VirtualProtect(originalCode, TRAMPOLINE_SIZE, oldProtec, NULL); //That bit too
}


bool Hook::enable()
{
	if (isEnabled && isCurrentCodeOk()) //If the hook is already enabled, then we've got nothing to do
		return true;
	DWORD oldProtect;
	if(VirtualProtect(m_originalAddress, TRAMPOLINE_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect)) //Unprotect the beginning of the function's area
	{
		memcpy((void*)m_originalAddress, trampoline, TRAMPOLINE_SIZE); //Copying our trampoline where the function's supposed to start
		VirtualProtect(m_originalAddress, TRAMPOLINE_SIZE, oldProtect, NULL); //Putting back on the old VM protection
		isEnabled = true; //Setting the correct value to the isEnabled boolean
		return true; //We succeeded
	}
	else //If we couldn't unprotect the area, then we failed :'(
	{
		return false;
	}
}

bool Hook::disable()
{
	if (!isEnabled) //If the hook is already disabled, then we've got nothing to do
		return true;
	DWORD oldProtect;
	if (VirtualProtect(m_originalAddress, TRAMPOLINE_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect)) //Unprotect the beginning of the function's area
	{
		memcpy((void*)m_originalAddress, originalCode, TRAMPOLINE_SIZE); //Copying the original first bytes of the function where the function's supposed to start
		VirtualProtect(m_originalAddress, TRAMPOLINE_SIZE, oldProtect, NULL); //Putting back on the old VM protection
		isEnabled = false; //Setting the correct value to the isEnabled boolean
		return true; //We succeeded
	}
	else //If we couldn't unprotect the area, then we failed :'(
	{
		return false;
	}
}

bool Hook::isCurrentCodeOk()
{
	if (isEnabled) //Since the hook should be enabled, we're comparing the code at the function's address with our trampoline.
		return Hook::isSame((BYTE*)m_originalAddress, trampoline, TRAMPOLINE_SIZE);
	else //Since the hook should be disabled, we're comparing the code at the function's address with the original function's code.
		return Hook::isSame((BYTE*)m_originalAddress, originalCode, TRAMPOLINE_SIZE);
}

bool Hook::isSame(BYTE* par1, BYTE* par2, size_t length)
{
	for (size_t i = 0; i < length; i++) //For each byte in the sequence
	{
		if (par1[i] != par2[i])
			return false; //If only one of the bytes in one sequence isn't equal to another byte in the same spot in the other sequence, then the sequences aren't equal
	}
	return true; //We didn't see any differences so it's gotta be equal! :)
}
