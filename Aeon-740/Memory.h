#pragma once
#include "framework.h"

namespace Memory
{
	uintptr_t GetBaseAddress()
	{
		return (uintptr_t)GetModuleHandle(0);
	}

	uintptr_t GetAddress(uintptr_t Offset)
	{
		return GetBaseAddress() + Offset;
	}

	void NullFunction(uintptr_t OffsetFunc)
	{
		uintptr_t func = Memory::GetAddress(OffsetFunc);
		DWORD dwProtection;
		VirtualProtect((PVOID)func, 1, PAGE_EXECUTE_READWRITE, &dwProtection);

		*(uint8_t*)func = 0xC3;

		DWORD dwTemp;
		VirtualProtect((PVOID)func, 1, dwProtection, &dwTemp);
	}

	void PatchByte(uintptr_t ByteOffset)
	{
		auto ByteToPatch = (uint8_t*)(uint8_t*)(Memory::GetAddress(ByteOffset));
		DWORD dwProtection;
		VirtualProtect((PVOID)ByteToPatch, 1, PAGE_EXECUTE_READWRITE, &dwProtection);
		*ByteToPatch = 0x85;
		DWORD dwTemp;
		VirtualProtect((PVOID)ByteToPatch, 1, dwProtection, &dwTemp);
	}
}