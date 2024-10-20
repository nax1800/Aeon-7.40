#pragma once
#include "framework.h"

namespace Hooks
{
	namespace Pawn
	{
		void hkServerHandlePickup(APlayerPawn_Athena_C* Pawn, AFortPickup* Pickup, float InFlyTime, FVector InStartDirection, bool bPlayPickupSound)
		{
			if (!Pawn)
				return;

			if (Pickup->bPickedUp)
				return;

			auto PlayerController = static_cast<AFortPlayerControllerAthena*>(Pawn->Controller);
			if (!PlayerController)
				return;

			if (InventoryHandler::GetItemQuickbar(Pickup->PrimaryPickupItemEntry.ItemDefinition) == EFortQuickBars::Primary && InventoryHandler::IsPrimaryFull(PlayerController))
			{
				AFortWeapon* CurrentWeapon = Pawn->CurrentWeapon;
				if (!CurrentWeapon)

				if (!CurrentWeapon->WeaponData->bCanBeDropped)
					return;

				AFortInventory* WorldInventory = PlayerController->WorldInventory;
				FFortItemEntry* ReplicatedEntry = InventoryHandler::FindItem(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition);

				if (ReplicatedEntry)
				{
					if (ReplicatedEntry->ItemDefinition->MaxStackSize > 1)
						InventoryHandler::AddItem(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo);
				}
				else
				{
					for (int i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
					{
						if (WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition == CurrentWeapon->WeaponData)
						{
							ReplicatedEntry = &WorldInventory->Inventory.ReplicatedEntries[i];
							break;
						}
					}

					if (ReplicatedEntry->ItemDefinition == Pickup->PrimaryPickupItemEntry.ItemDefinition)
					{
						if (ReplicatedEntry->ItemDefinition->MaxStackSize <= 1)
						{
							PlayerController->ServerAttemptInventoryDrop(ReplicatedEntry->ItemGuid, ReplicatedEntry->Count);
							InventoryHandler::AddItem(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo);
						}
						else
							InventoryHandler::AddItem(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo);
					}
					else
					{
						if (ReplicatedEntry->ItemDefinition->MaxStackSize <= 1)
						{
							PlayerController->ServerAttemptInventoryDrop(ReplicatedEntry->ItemGuid, ReplicatedEntry->Count);
							InventoryHandler::AddItem(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo);
						}
						else
							InventoryHandler::AddItem(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo);
					}
				}

			}
			else
				InventoryHandler::AddItem(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo, true);

			Pawn->IncomingPickups.Add(Pickup);
			auto& LocData = Pickup->PickupLocationData;
			LocData.StartDirection = static_cast<FVector_NetQuantizeNormal>(InStartDirection);
			LocData.FlyTime = 0.40f;
			LocData.PickupTarget = Pawn;
			LocData.ItemOwner = Pawn;
			LocData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
			Pickup->OnRep_PickupLocationData();

			Pickup->bPickedUp = true;
			Pickup->OnRep_bPickedUp();
		}

		void Initialize()
		{
			auto DefaultObject = APlayerPawn_Athena_C::GetDefaultObj();
			HookingManager::VirtualHook(DefaultObject, 0x1B4, hkServerHandlePickup);
		}
	}
}