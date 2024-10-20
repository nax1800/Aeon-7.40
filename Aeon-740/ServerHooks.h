#pragma once
#include "framework.h"

namespace Hooks
{
	namespace Server
	{
		void hkTickFlush(UNetDriver* NetDriver)
		{
			if (!NetDriver)
				return;

			if (NetDriver->ReplicationDriver && NetDriver->ClientConnections.Num() > 0 && !NetDriver->ClientConnections[0]->InternalAck)
				Defines::Server::ServerReplicateActors(NetDriver->ReplicationDriver);

			return Defines::Server::TickFlush(NetDriver);
		}

		__int64 hkUWorld_GetNetMode(UWorld* a1)
		{
			return 1;
		}

		__int64 hkAActor_GetNetMode(AActor* a1)
		{
			return 1;
		}

		char hkKickPlayer(__int64 a1, __int64 a2, __int64 a3)
		{
			return 1;
		}

		__int64 (*oCollectGarbage)(__int64);
		__int64 hkCollectGarbage(__int64)
		{
			return 0;
		}

		__int64 hkNoMCP()
		{
			return !Globals::bMcp;
		}

		void (*oDispatchRequest)(__int64 a1, __int64* a2, int a3);
		void hkDispatchRequest(__int64 a1, __int64* a2, int a3)
		{
			return oDispatchRequest(a1, a2, 3);
		}

		void Initialize()
		{
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::Server::TickFlush), hkTickFlush, (void**)&Defines::Server::TickFlush);
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::Server::WorldGetNetMode), hkUWorld_GetNetMode);
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::Server::KickPlayer), hkKickPlayer);
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::Server::CollectGarbage), hkCollectGarbage);
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::Server::WorldGetNetMode), hkUWorld_GetNetMode);
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::Server::NoMCP), hkNoMCP);
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::Server::DispatchRequest), hkDispatchRequest);
		}
	}
}