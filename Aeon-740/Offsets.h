#pragma once
#include "framework.h"

namespace GOffsets
{
	static uintptr_t StaticFindObject = 0x1b51790;
	static uintptr_t StaticLoadObject = 0x1b528f0;

	static uintptr_t GIsClient = 0x5a8ab39;

	namespace Actor
	{
		static uintptr_t OnDamageServer = 0x16001c0;
	}

	namespace Abilities
	{
		static uintptr_t InternalTryActivateAbility = 0x69d9e0;
		static uintptr_t GiveAbility = 0x69c150;
		static uintptr_t GiveAbilityAndActivateOnce = 0x69c270;
		static uintptr_t SpecConstructor = 0x6c2e20;
	}

	namespace Building
	{
		static uintptr_t CantBuild = 0x109e9a0;
		static uintptr_t ReplaceBuildingActor = 0xea7be0;
	}

	namespace GameMode
	{
		static uintptr_t PickTeam = 0xd4d810;
		static uintptr_t RemoveFromAlivePlayers = 0xd52a10;
		static uintptr_t ReadyToStartMatch = 0xd517e0;
		static uintptr_t SpawnDefaultPawnFor = 0xd58fc0;
	}

	namespace GameSession
	{
		static uintptr_t GetSquadIdForCurrentPlayer = 0x1262260;
	}

	namespace NetDriver
	{
		static uintptr_t InitListen = 0x4659d0;
		static uintptr_t SetWorld = 0x29fbce0;
	}

	namespace PlayerController
	{
		static uintptr_t EnterAircraft = 0xd851f0;
		static uintptr_t GetPlayerViewpoint = 0x14033f0;
	}

	namespace Server
	{
		static uintptr_t KickPlayer = 0x28cade0;
		static uintptr_t TickFlush = 0x29fc860;
		static uintptr_t WorldGetNetMode = 0x2cd6a40;
		static uintptr_t CollectGarbage = 0x1acee20;
		static uintptr_t ChangeGameSessionId = 0x1061160;
		static uintptr_t DispatchRequest = 0xa7d080;
		static uintptr_t GetMaxTickRate = 0x2c75fe0;
	}
}