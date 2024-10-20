#pragma once
#include "framework.h"

namespace GOffsets
{
	static vector<uintptr_t> BytesToPatch{
		0xdb564d
	};

	static vector<uintptr_t> FuncsToNull{

	};

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
		static uintptr_t ReadyToStartMatch = 0x2d44e00;
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

	namespace PlayerState
	{
		static uintptr_t ApplyCharacterCustomization = 0x14801d0;
	}

	namespace Server
	{
		static uintptr_t CreateNetDriver = 0x2c69d00;
		static uintptr_t InitListen = 0x4659d0;
		static uintptr_t SetWorld = 0x29fbce0;
		static uintptr_t KickPlayer = 0x28cade0;
		static uintptr_t TickFlush = 0x29fc860;
		static uintptr_t WorldGetNetMode = 0x2cd6a40;
		static uintptr_t ActorGetNetMode = 0;
		static uintptr_t CollectGarbage = 0x64b5288;
		static uintptr_t ChangeGameSessionId = 0x1061160;
		static uintptr_t DispatchRequest = 0xa7d080;
		static uintptr_t NoMCP = 0x10b1410;
		static uintptr_t GetMaxTickRate = 0x2c75fe0;
	}
}

namespace GIndexs
{
	namespace Pawn
	{
		static uintptr_t ServerHandlePickup = 0x0;
	}

	namespace PlayerController
	{
		static uintptr_t ServerReadyToStartMatch = 0x260;
		static uintptr_t ServerLoadingScreenDropped = 0x262;
		static uintptr_t ServerAcknowledgePossession = 0x106;
		static uintptr_t ServerExecuteInventoryItem = 0x1f9;
		static uintptr_t ServerAttemptInventoryDrop = 0x20b;
		static uintptr_t ServerCreateBuildingActor = 0x21F;
		static uintptr_t ServerBeginEditingBuildingActor = 0x226;
		static uintptr_t ServerEditBuildingActor = 0x221;
		static uintptr_t ServerEndEditingBuildingActor = 0x224;
		static uintptr_t ServerPlayEmoteItem = 0x1B9;
	}
}