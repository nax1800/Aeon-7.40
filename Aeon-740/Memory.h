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
}