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

	void sinCos(float* ScalarSin, float* ScalarCos, float Value)
	{
		float quotient = (0.31830988618f * 0.5f) * Value;
		if (Value >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}
		float y = Value - (2.0f * 3.1415926535897932f) * quotient;
		float sign;
		if (y > 1.57079632679f)
		{
			y = 3.1415926535897932f - y;
			sign = -1.0f;
		}
		else if (y < -1.57079632679f)
		{
			y = -3.1415926535897932f - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}
		float y2 = y * y;
		*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*ScalarCos = sign * p;
	}

	FQuat FRotToQuat(FRotator Rot)
	{
		const float DEG_TO_RAD = 3.1415926535897932f / (180.f);
		const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
		float SP, SY, SR;
		float CP, CY, CR;

		sinCos(&SP, &CP, Rot.Pitch * DIVIDE_BY_2);
		sinCos(&SY, &CY, Rot.Yaw * DIVIDE_BY_2);
		sinCos(&SR, &CR, Rot.Roll * DIVIDE_BY_2);

		FQuat RotationQuat;
		RotationQuat.X = CR * SP * SY - SR * CP * CY;
		RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
		RotationQuat.Z = CR * CP * SY - SR * SP * CY;
		RotationQuat.W = CR * CP * CY + SR * SP * SY;

		return RotationQuat;
	}

	template<typename T = AActor>
	T* SpawnActor(UClass* ActorClass, FVector Location = {}, FRotator Rotation = {}, AActor* Owner = nullptr)
	{
		FTransform Transform;

		Transform.Translation = Location;
		Transform.Scale3D = { 1,1,1 };
		Transform.Rotation = FRotToQuat(Rotation);

		AActor* Actor = UGameplayStatics::BeginDeferredActorSpawnFromClass(Globals::GetWorld(), ActorClass, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner);
		UGameplayStatics::FinishSpawningActor(Actor, Transform);

		return static_cast<T*>(Actor);
	}

	template<typename T = AActor>
	T* SpawnActor(FVector Location = {}, FRotator Rotation = {}, AActor* Owner = nullptr)
	{
		return SpawnActor<T>(T::StaticClass());
	}
}