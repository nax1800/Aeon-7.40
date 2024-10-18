#pragma once
#include "framework.h"

namespace Hooks
{
	namespace PlayerController
	{
		void hkServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, APawn* Pawn)
		{
			auto PlayerState = static_cast<AFortPlayerStateAthena*>(PlayerController->PlayerState);
			auto FortPawn = static_cast<APlayerPawn_Athena_C*>(Pawn);

			PlayerController->AcknowledgedPawn = FortPawn;
			if (PlayerState && FortPawn)
			{
				auto& CustomizationLoadout = PlayerController->CustomizationLoadout;

				if (CustomizationLoadout.Character)
				{
					if (CustomizationLoadout.Character->HeroDefinition)
					{
						FortPawn->CustomizationLoadout = CustomizationLoadout;
						FortPawn->OnRep_CustomizationLoadout();

						PlayerState->HeroType = CustomizationLoadout.Character->HeroDefinition;
						PlayerState->OnRep_HeroType();

						LogInfo("PlayerController::ServerAcknowledgePossession: Applying Character Customiaztion.");
						Defines::PlayerState::ApplyCharacterCustomization(PlayerState, FortPawn);
					}
				}
			}
			else
				LogInfo("PlayerController::ServerAcknowledgePossession: PlayerState or FortPawn is NULL.");
		}

		void hkServerReadyToStartMatch(AFortPlayerControllerAthena* PlayerController)
		{
			LogInfo("PlayerController::ServerReadyToStartMatch: Called by ({}).", PlayerController->PlayerState->GetPlayerName().ToString());
			if (PlayerController)
			{
				auto PlayerState = static_cast<AFortPlayerStateAthena*>(PlayerController->PlayerState);
				if (PlayerState)
				{
					LogInfo("PlayerController::ServerReadyToStartMatch: Called for ({})", PlayerState->GetPlayerName().ToString());
					AbilitiesHandler::ApplyAbilities(PlayerState);
				}
			}

			return Defines::PlayerController::ServerReadyToStartMatch(PlayerController);
		}

		void hkServerLoadingScreenDropped(AFortPlayerControllerAthena* PlayerController)
		{
			//Inventory::SetupForPlayer(PlayerController);
			return Defines::PlayerController::ServerLoadingScreenDropped(PlayerController);
		}

		void Initialize()
		{
			auto DefaultObject = AAthena_PlayerController_C::GetDefaultObj();
			HookingManager::VirtualHook(DefaultObject, 0x106, hkServerAcknowledgePossession);
			HookingManager::VirtualHook(DefaultObject, 0x262, hkServerReadyToStartMatch, (void**)&Defines::PlayerController::ServerReadyToStartMatch);
			//HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerLoadingScreenDropped, hkServerLoadingScreenDropped, (void**)&Defines::PlayerController::ServerLoadingScreenDropped);
		}
	}
}