#pragma once
#include "framework.h"

namespace Defines
{

	namespace Abilities
	{
		FGameplayAbilitySpecHandle* (*GiveAbility)(UAbilitySystemComponent* _this, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec inSpec) = decltype(GiveAbility)(Memory::GetAddress(GOffsets::Abilities::GiveAbility));
		__int64 (*GiveAbilityAndActivateOnce)(void*, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec) = decltype(GiveAbilityAndActivateOnce)(Memory::GetAddress(GOffsets::Abilities::GiveAbilityAndActivateOnce));
		bool (*InternalTryActivateAbility)(UAbilitySystemComponent* _this, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, FGameplayEventData* TriggerEventData) = decltype(InternalTryActivateAbility)(Memory::GetAddress(GOffsets::Abilities::InternalTryActivateAbility));
		__int64 (*SpecConstructor)(FGameplayAbilitySpec* spec, UObject* Ability, int Level, int InputID, UObject* SourceObject) = decltype(SpecConstructor)(Memory::GetAddress(GOffsets::Abilities::SpecConstructor));
	}

	namespace Actor
	{
		void (*OnDamageServer)(ABuildingActor*, float, FGameplayTagContainer, FVector, FHitResult, AActor*, AActor*, FGameplayEffectContextHandle);
	}

	namespace GameMode
	{
		bool (*ReadyToStartMatch)(AGameMode* GameMode);
	}

	namespace PlayerController
	{
		void (*ServerReadyToStartMatch)(AFortPlayerController* PlayerController);
		void (*ServerLoadingScreenDropped)(AFortPlayerControllerAthena* PlayerController);
		void (*EnterAircraft)(AFortPlayerControllerAthena* PlayerController, unsigned __int64 a2);
	}

	namespace PlayerState
	{
		static void* (*ApplyCharacterCustomization)(void* a1, void* a2) = decltype(ApplyCharacterCustomization)(Memory::GetAddress(GOffsets::PlayerState::ApplyCharacterCustomization));
	}

	namespace Server
	{
		static UNetDriver* (*CreateNetDriver)(UEngine* Engine, UWorld* World, FName NetDriverName) = decltype(CreateNetDriver)(Memory::GetAddress(GOffsets::Server::CreateNetDriver));
		static bool (*InitListen)(UNetDriver* a1, void* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error) = decltype(InitListen)(Memory::GetAddress(GOffsets::Server::InitListen));
		static void (*SetWorld)(UNetDriver* a1, UWorld* a2) = decltype(SetWorld)(Memory::GetAddress(GOffsets::Server::SetWorld));
		static void (*TickFlush)(UNetDriver* NetDriver);
		static void (*ServerReplicateActors)(UReplicationDriver* ReplicationDriver);
	}
}