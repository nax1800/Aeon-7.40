#pragma once
#include "framework.h"

namespace HookingManager
{
	MH_STATUS CreateMinHook(uintptr_t Address, LPVOID pDetour, LPVOID* ppOriginal = nullptr)
	{
		MH_CreateHook((void*)Address, pDetour, ppOriginal);
		return MH_EnableHook((void*)Address);
	}

	void VirtualHook(void* Object, int Index, void* Detour, void** OG = nullptr)
	{
		auto vft = *(void***)Object;
		if (!vft || !vft[Index])
		{
			return;
		}

		if (OG)
			*OG = vft[Index];

		DWORD dwOld;
		VirtualProtect(&vft[Index], 8, PAGE_EXECUTE_READWRITE, &dwOld);

		vft[Index] = Detour;

		DWORD dwTemp;
		VirtualProtect(&vft[Index], 8, dwOld, &dwTemp);
	}
}