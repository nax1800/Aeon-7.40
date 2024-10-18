#pragma once
#include "framework.h"

namespace SDKUtils
{
	UObject* (*StaticLoadObject)(UObject*, UObject*, const TCHAR*, const TCHAR*, uint32_t, void*, bool) = decltype(StaticLoadObject)(Memory::GetAddress(GOffsets::StaticLoadObject));
	UObject* (*oStaticFindObject)(UClass* Class, void* InOuter, const wchar_t* Name, bool ExactClass) = decltype(oStaticFindObject)(Memory::GetAddress(GOffsets::StaticFindObject));

	template <typename T>
	T* FindObject(string ObjectName, UClass* ObjectClass = UObject::StaticClass())
	{
		return (T*)oStaticFindObject(ObjectClass, nullptr, wstring(ObjectName.begin(), ObjectName.end()).c_str(), false);
	}

	template <typename T>
	static T* LoadObject(string ObjectName, UClass* ObjectClass = UObject::StaticClass())
	{
		return reinterpret_cast<T*>(StaticLoadObject(ObjectClass, 0, wstring(ObjectName.begin(), ObjectName.end()).c_str(), nullptr, 0, nullptr, false));
	}

	bool IsValidLowLevel(UObject* Object)
	{
		if (!Object || IsBadReadPtr(Object, 8))
			return false;

		if (!Object->Class || IsBadReadPtr(Object->Class, 8))
			return false;

		if (UObject::GObjects->GetByIndex(Object->Index) != Object)
			return false;

		return true;
	}
}