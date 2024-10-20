#pragma once
#include "framework.h"

namespace InventoryHandler
{
	void Update(AFortPlayerControllerAthena* PlayerController, FFortItemEntry* ItemEntry = nullptr)
	{
		PlayerController->WorldInventory->HandleInventoryLocalUpdate();
		PlayerController->WorldInventory->Inventory.MarkArrayDirty();

		if (ItemEntry)
			PlayerController->WorldInventory->Inventory.MarkItemDirty(*ItemEntry);
	}

	EFortQuickBars GetItemQuickbar(UFortItemDefinition* ItemDefinition)
	{
		if (!ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortConsumableItemDefinition::StaticClass())
			&& !ItemDefinition->IsA(UFortBuildingItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortEditToolItemDefinition::StaticClass())
			&& !ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
			return EFortQuickBars::Primary;

		return EFortQuickBars::Secondary;
	}

	bool IsPrimaryFull(AFortPlayerControllerAthena* PlayerController)
	{
		AFortInventory* WorldInventory = PlayerController->WorldInventory;
		TArray<FFortItemEntry>& ReplicatedEntries = WorldInventory->Inventory.ReplicatedEntries;
		int Slots = 0;
		for (int i = 0; i < ReplicatedEntries.Num(); i++)
		{
			FFortItemEntry ReplicatedEntry = ReplicatedEntries[i];
			if (GetItemQuickbar(ReplicatedEntry.ItemDefinition) == EFortQuickBars::Primary)
			{
				Slots++;

				if (Slots >= 5)
					break;
			}
		}

		return Slots >= 5;
	}

	FFortItemEntry* FindItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* ItemDefinition)
	{
		TArray<FFortItemEntry> ReplicatedEntries = PlayerController->WorldInventory->Inventory.ReplicatedEntries;
		for (int i = 0; i < ReplicatedEntries.Num(); i++)
		{
			if (ReplicatedEntries[i].ItemDefinition == ItemDefinition)
				return &ReplicatedEntries[i];
		}

		return nullptr;
	}

	FFortItemEntry* FindItem(AFortPlayerControllerAthena* PlayerController, FGuid ItemGuid)
	{
		TArray<FFortItemEntry> ReplicatedEntries = PlayerController->WorldInventory->Inventory.ReplicatedEntries;
		for (int i = 0; i < ReplicatedEntries.Num(); i++)
		{
			if (ReplicatedEntries[i].ItemGuid == ItemGuid)
				return &ReplicatedEntries[i];
		}

		return nullptr;
	}

	bool RemoveItem(AFortPlayerController* PlayerController, FGuid ItemGuid, int Count = -1)
	{
		if (!PlayerController)
			return false;

		AFortInventory* WorldInventory = PlayerController->WorldInventory;
		if (!WorldInventory)
			return false;

		bool bWasSuccessful = false;

		for (int i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == ItemGuid)
			{
				if (Count == -1 || Count >= WorldInventory->Inventory.ReplicatedEntries[i].Count)
				{
					WorldInventory->Inventory.ReplicatedEntries.Remove(i);
					break;
				}

				WorldInventory->Inventory.ReplicatedEntries[i].Count -= Count;
				WorldInventory->Inventory.MarkItemDirty(WorldInventory->Inventory.ReplicatedEntries[i]);
				bWasSuccessful = true;
			}
		}

		WorldInventory->Inventory.MarkArrayDirty();
		WorldInventory->HandleInventoryLocalUpdate();
		return bWasSuccessful;
	}

	bool RemoveItem(AFortPlayerController* PlayerController, UFortItemDefinition* ItemDefinition, int Count = -1)
	{
		if (!PlayerController || !ItemDefinition)
			return false;

		AFortInventory* WorldInventory = PlayerController->WorldInventory;
		if (!WorldInventory)
			return false;

		bool bWasSuccessful = false;

		for (int i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition == ItemDefinition)
			{
				if (Count == -1 || Count >= WorldInventory->Inventory.ReplicatedEntries[i].Count)
				{
					WorldInventory->Inventory.ReplicatedEntries.Remove(i);
					break;
				}

				WorldInventory->Inventory.ReplicatedEntries[i].Count -= Count;
				WorldInventory->Inventory.MarkItemDirty(WorldInventory->Inventory.ReplicatedEntries[i]);
				bWasSuccessful = true;
			}
		}

		WorldInventory->Inventory.MarkArrayDirty();
		WorldInventory->HandleInventoryLocalUpdate();
		return bWasSuccessful;
	}

	UFortWorldItem* CreateItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* ItemDefinition, int Count)
	{
		auto WorldItem = static_cast<UFortWorldItem*>(ItemDefinition->CreateTemporaryItemInstanceBP(Count, 1));
		WorldItem->SetOwningControllerForTemporaryItem(PlayerController);
		return WorldItem;
	}

	bool AddItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo = 0, bool bForceCreateNewItem = false)
	{
		FFortItemEntry* FoundItemEntry = FindItem(PlayerController, ItemDefinition);
		bool bCreateNewItem = bForceCreateNewItem;

		if (FoundItemEntry && !bCreateNewItem)
		{
			int NewCount = FoundItemEntry->Count + Count;
			if (NewCount > ItemDefinition->MaxStackSize)
			{
				PlayerController->ServerAttemptInventoryDrop(FoundItemEntry->ItemGuid, NewCount - ItemDefinition->MaxStackSize);
				FoundItemEntry->Count = ItemDefinition->MaxStackSize;
			}
			else
			{
				FoundItemEntry->Count = NewCount;
			}

			PlayerController->WorldInventory->HandleInventoryLocalUpdate();
			PlayerController->WorldInventory->Inventory.MarkArrayDirty();
			PlayerController->WorldInventory->Inventory.MarkItemDirty(*FoundItemEntry);

			return true;
		}
		else
			bCreateNewItem = true;

		if (bCreateNewItem)
		{
			if (UFortWorldItem* NewItem = CreateItem(PlayerController, ItemDefinition, Count))
			{
				NewItem->ItemEntry.LoadedAmmo = LoadedAmmo;
				if (ItemDefinition->MaxStackSize < Count)
				{
					NewItem->ItemEntry.Count = ItemDefinition->MaxStackSize;
					PlayerController->ServerAttemptInventoryDrop(NewItem->ItemEntry.ItemGuid, Count - ItemDefinition->MaxStackSize);
				}

				PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(NewItem->ItemEntry);
				PlayerController->WorldInventory->HandleInventoryLocalUpdate();
				PlayerController->WorldInventory->Inventory.MarkArrayDirty();
				PlayerController->WorldInventory->Inventory.MarkItemDirty(NewItem->ItemEntry);

				return true;
			}
		}

		return false;
	}

	void InitializePlayer(AFortPlayerControllerAthena* PlayerController)
	{
		static auto PickaxeDefinition = UObject::FindObject<UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		AddItem(PlayerController, PickaxeDefinition, 1);

		TArray<FItemAndCount> StartingItems = Globals::GetGameMode()->StartingItems;
		for (int i = 0; i < StartingItems.Num(); i++)
		{
			AddItem(PlayerController, StartingItems[i].Item, StartingItems[i].Count);
		}
	}
}