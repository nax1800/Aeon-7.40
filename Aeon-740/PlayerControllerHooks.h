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
			if (PlayerController)
			{
				auto PlayerState = static_cast<AFortPlayerStateAthena*>(PlayerController->PlayerState);
				if (PlayerState)
				{
					LogInfo("PlayerController::ServerReadyToStartMatch: Called for ({})", PlayerState->GetPlayerName().ToString());
					AbilitiesHandler::ApplyAbilities(PlayerState);

					if (!PlayerController->MatchReport)
						PlayerController->MatchReport = (UAthenaPlayerMatchReport*)UGameplayStatics::GetDefaultObj()->SpawnObject(UAthenaPlayerMatchReport::StaticClass(), PlayerController);

					PlayerController->MatchReport->bHasMatchStats = true;
					PlayerController->MatchReport->bHasTeamStats = true;
					PlayerController->MatchReport->bHasRewards = true;

					PlayerState->SquadId = PlayerState->TeamIndex - 2;
					PlayerState->OnRep_PlayerTeam();
					PlayerState->OnRep_TeamIndex(0);
					PlayerState->OnRep_SquadId();

					FGameMemberInfo test{ -1,-1,-1 };
					test.TeamIndex = PlayerState->TeamIndex;
					test.SquadId = PlayerState->SquadId;
					test.MemberUniqueId = PlayerState->UniqueId;

					Globals::GetGameState()->GameMemberInfoArray.Members.Add(test);
					Globals::GetGameState()->GameMemberInfoArray.MarkArrayDirty();
				}
			}

			return Defines::PlayerController::ServerReadyToStartMatch(PlayerController);
		}

		void hkServerLoadingScreenDropped(AFortPlayerControllerAthena* PlayerController)
		{
			InventoryHandler::InitializePlayer(PlayerController);
			LootHandler::SpawnFloorLoot();

			return Defines::PlayerController::ServerLoadingScreenDropped(PlayerController);
		}

		void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, FGuid ItemGuid)
		{
			FFortItemEntry* FoundItemEntry = InventoryHandler::FindItem(PlayerController, ItemGuid);

			auto Pawn = static_cast<AFortPlayerPawn*>(PlayerController->Pawn);
			if (!Pawn)
				return;

			if (FoundItemEntry)
			{
				auto ItemDefinition = static_cast<UFortWeaponItemDefinition*>(FoundItemEntry->ItemDefinition);
				if (ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()))
				{
					auto DecoDefinition = static_cast<UFortDecoItemDefinition*>(ItemDefinition);
					Pawn->PickUpActor(Pawn, DecoDefinition);
					Pawn->CurrentWeapon->ItemEntryGuid = ItemGuid;

					if (auto ContextTrapTool = reinterpret_cast<AFortDecoTool_ContextTrap*>(Pawn->CurrentWeapon))
						ContextTrapTool->ContextTrapItemDefinition = static_cast<UFortContextTrapItemDefinition*>(ItemDefinition);

					ItemDefinition = nullptr;
				}

				if (!ItemDefinition)
					return;

				Pawn->EquipWeaponDefinition(ItemDefinition, ItemGuid);
			}
		}

		bool(__fastcall* CantBuild)(UWorld*, UObject*, FVector, FRotator, char, void*, char*) = decltype(CantBuild)(Memory::GetAddress(GOffsets::Building::CantBuild));
		void hkServerCreateBuildingActor(AFortPlayerControllerAthena* PlayerController, FBuildingClassData& BuildingClassData, FVector_NetQuantize10& BuildLoc, FRotator& BuildRot, bool bMirrored)
		{
			auto Class = BuildingClassData.BuildingClass.Get();
			TArray<AActor*> BuildingActorsToDestroy;
			char Result;

			if (!CantBuild(Globals::GetWorld(), Class, BuildLoc, BuildRot, bMirrored, &BuildingActorsToDestroy, &Result))
			{
				for (int i = 0; i < BuildingActorsToDestroy.Num(); i++)
				{
					BuildingActorsToDestroy[i]->K2_DestroyActor();
				}

				if (auto NewBuilding = SDKUtils::SpawnActor<ABuildingSMActor>(Class, BuildLoc, BuildRot))
				{
					NewBuilding->InitializeKismetSpawnedBuildingActor(NewBuilding, PlayerController, true);
					NewBuilding->bPlayerPlaced = true;
					NewBuilding->Team = EFortTeam(((AFortPlayerStateAthena*)PlayerController->PlayerState)->TeamIndex);
					NewBuilding->TeamIndex = ((AFortPlayerStateAthena*)PlayerController->PlayerState)->TeamIndex;
					NewBuilding->OnRep_Team();

					if (!PlayerController->bBuildFree)
						InventoryHandler::RemoveItem(PlayerController, UFortKismetLibrary::K2_GetResourceItemDefinition(NewBuilding->ResourceType), 10);
				}
			}

			BuildingActorsToDestroy.Free();
		}

		void hkServerBeginEditingBuildingActor(AFortPlayerController* PlayerController, ABuildingSMActor* BuildingActorToEdit)
		{
			auto Pawn = static_cast<AFortPlayerPawnAthena*>(PlayerController->Pawn);
			if (Pawn && BuildingActorToEdit)
			{
				static auto EditToolDef = SDKUtils::FindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
				if (Pawn->CurrentWeapon->WeaponData != EditToolDef)
				{
					AFortInventory* WorldInventory = PlayerController->WorldInventory;
					TArray<FFortItemEntry> ReplicatedEntries = WorldInventory->Inventory.ReplicatedEntries;
					FFortItemEntry Entry;

					for (int i = 0; i < ReplicatedEntries.Num(); i++)
					{
						FFortItemEntry ReplicatedEntry = ReplicatedEntries[i];
						if (ReplicatedEntry.ItemDefinition == EditToolDef)
						{
							Entry = ReplicatedEntry;
						}
					}

					PlayerController->ServerExecuteInventoryItem(Entry.ItemGuid);
				}

				auto EditTool = (AFortWeap_EditingTool*)Pawn->CurrentWeapon;
				EditTool->EditActor = BuildingActorToEdit;
				EditTool->OnRep_EditActor();
				BuildingActorToEdit->EditingPlayer = static_cast<AFortPlayerStateAthena*>(PlayerController->PlayerState);
				BuildingActorToEdit->OnRep_EditingPlayer();
			}
		}

		void hkServerEditBuildingActor(AFortPlayerController* PlayerController, ABuildingSMActor* BuildingActorToEdit, UClass* NewBuildingClass, uint8 RotationIterations, bool bMirrored)
		{
			if (!PlayerController)
				return;

			auto PlayerState = static_cast<AFortPlayerStateAthena*>(PlayerController->PlayerState);
			if (!PlayerState)
				return;

			if (BuildingActorToEdit && NewBuildingClass)
			{
				FVector BuildLocation = BuildingActorToEdit->K2_GetActorLocation();
				FRotator BuildRotation = BuildingActorToEdit->K2_GetActorRotation();
				FVector BuildForwardVector = BuildingActorToEdit->GetActorForwardVector();
				FVector BuildRightVector = BuildingActorToEdit->GetActorRightVector();

				int yaw = round(float((int(BuildRotation.Yaw) + 360) % 360) / 10) * 10;

				if (BuildingActorToEdit->BuildingType != EFortBuildingType::Wall)
				{
					switch (RotationIterations)
					{
					case 1:
						BuildLocation = BuildLocation + BuildForwardVector * 256.0f + BuildRightVector * 256.0f;
						break;
					case 2:
						BuildLocation = BuildLocation + BuildRightVector * 512.0f;
						break;
					case 3:
						BuildLocation = BuildLocation + BuildForwardVector * -256.0f + BuildRightVector * 256.0f;
					}
				}

				BuildRotation.Yaw = yaw + 90 * RotationIterations;
				float HealthPercent = BuildingActorToEdit->GetHealthPercent();

				BuildingActorToEdit->SilentDie();
				if (auto NewBuildingActor = SDKUtils::SpawnActor<ABuildingSMActor>(NewBuildingClass, BuildLocation, BuildRotation, PlayerController))
				{
					if (!BuildingActorToEdit->bIsInitiallyBuilding)
						NewBuildingActor->ForceBuildingHealth(NewBuildingActor->GetMaxHealth() * HealthPercent);

					NewBuildingActor->InitializeKismetSpawnedBuildingActor(NewBuildingActor, PlayerController, true);
					NewBuildingActor->bPlayerPlaced = true;
					NewBuildingActor->Team = EFortTeam(((AFortPlayerStateAthena*)PlayerController->PlayerState)->TeamIndex);
					NewBuildingActor->TeamIndex = ((AFortPlayerStateAthena*)PlayerController->PlayerState)->TeamIndex;
					NewBuildingActor->OnRep_Team();

					if (!NewBuildingActor->IsStructurallySupported())
						NewBuildingActor->K2_DestroyActor();
				}
			}
		}

		void hkServerEndEditingBuildingActor(AFortPlayerController* PlayerController, ABuildingSMActor* BuildingActorToStopEditing)
		{
			if (PlayerController && PlayerController->Pawn && BuildingActorToStopEditing)
			{
				auto Pawn = static_cast<APlayerPawn_Athena_C*>(PlayerController->Pawn);

				BuildingActorToStopEditing->EditingPlayer = nullptr;
				BuildingActorToStopEditing->OnRep_EditingPlayer();

				AFortWeap_EditingTool* EditTool = static_cast<AFortWeap_EditingTool*>(Pawn->CurrentWeapon);
				if (EditTool)
				{
					EditTool->bEditConfirmed = true;
					EditTool->EditActor = nullptr;
					EditTool->OnRep_EditActor();
				}
			}
		}

		void hkEnterAircraft(AFortPlayerControllerAthena* PlayerController, unsigned __int64 a2)
		{
			TArray<FFortItemEntry> ReplicatedEntries = PlayerController->WorldInventory->Inventory.ReplicatedEntries;
			for (int i = 0; i < ReplicatedEntries.Num(); i++)
			{
				if (static_cast<UFortWorldItemDefinition*>(ReplicatedEntries[i].ItemDefinition)->bCanBeDropped)
					InventoryHandler::RemoveItem(PlayerController, ReplicatedEntries[i].ItemGuid);
			}

			return Defines::PlayerController::EnterAircraft(PlayerController, a2);
		}

		void Initialize()
		{
			auto DefaultObject = AAthena_PlayerController_C::GetDefaultObj();
			HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerAcknowledgePossession, hkServerAcknowledgePossession);
			HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerReadyToStartMatch, hkServerReadyToStartMatch, (void**)&Defines::PlayerController::ServerReadyToStartMatch);
			HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerLoadingScreenDropped, hkServerLoadingScreenDropped, (void**)&Defines::PlayerController::ServerLoadingScreenDropped);
			HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerExecuteInventoryItem, ServerExecuteInventoryItem);
			HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerCreateBuildingActor, hkServerCreateBuildingActor);
			HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerEditBuildingActor, hkServerEditBuildingActor);
			HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerBeginEditingBuildingActor, hkServerBeginEditingBuildingActor);
			HookingManager::VirtualHook(DefaultObject, GIndexs::PlayerController::ServerEndEditingBuildingActor, hkServerEndEditingBuildingActor);

			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::PlayerController::EnterAircraft), hkEnterAircraft, (void**)&Defines::PlayerController::EnterAircraft);
		}
	}
}