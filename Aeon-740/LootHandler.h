#pragma once
#include "framework.h"
#include <random>

namespace LootHandler
{
	vector<string> ItemsToRemove = {
	"WID_RC_Rocket_Athena_SR_T03", // Guided Missle
	"WID_RC_Rocket_Athena_VR_T03", // Guided Missle
	"AGID_BoostJumpPack", // Jetpack
	"Athena_Mimic", // Mimic
	"WID_Sniper_Crossbow_Athena_VR_Ore_T03", // Crossbow
	"WID_Sniper_Crossbow_Athena_R_Ore_T03",  // Crossbow
	};

	struct LootRow
	{
		LootRow(string AssetPath, string rowName, int dropCount = 1, int loadedAmmo = 0, float weight = 1.00, bool bplaygroundOnly = true)
		{
			RowName = rowName;
			ItemDefinition = SDKUtils::FindObject<UFortItemDefinition>(AssetPath);
			DropCount = dropCount;
			LoadedAmmo = loadedAmmo;
			Weight = weight;
			bPlaygroundOnly = bplaygroundOnly;
		}

		LootRow(UFortItemDefinition* itemDefinition, string rowName, int dropCount = 1, int loadedAmmo = 0, float weight = 1.00, bool bplaygroundOnly = true)
		{
			RowName = rowName;
			ItemDefinition = itemDefinition;
			DropCount = dropCount;
			LoadedAmmo = loadedAmmo;
			Weight = weight;
			bPlaygroundOnly = bplaygroundOnly;
		}

	public:
		string RowName = "";
		UFortItemDefinition* ItemDefinition = nullptr;
		int DropCount = 1;
		int LoadedAmmo = 0;
		float Weight = 0.0;
		bool bPlaygroundOnly = false;

		LootRow() = default;
	};

	vector<LootRow> LootRows;
	LootRow CurrentLootRow;

	int GetClipSize(UFortWeaponRangedItemDefinition* ItemDefinition)
	{
		if (!ItemDefinition)
			return 0;

		auto DataTable = ItemDefinition->WeaponStatHandle.DataTable;
		auto RowName = ItemDefinition->WeaponStatHandle.RowName;

		if (DataTable && RowName.ComparisonIndex)
		{
			auto& RowMap = DataTable->RowMap;
			if (RowMap.IsValid())
			{
				for (int i = 0; i < RowMap.Num(); ++i)
				{
					if (RowMap[i].Second)
						if (RowMap[i].First == RowName)
							if (auto Stats = reinterpret_cast<FFortRangedWeaponStats*>(RowMap[i].Second))
									return Stats->ClipSize;
				}
			}
		}

		return 0;
	}

	bool IsItemRemoved(string ItemDefinition)
	{
		for (string ItemToRemove : ItemsToRemove)
		{
			if (ItemDefinition.contains(ItemToRemove))
				return true;
		}
		return false;
	}


	bool bInitialized = false;
	void Initialize()
	{
		if (bInitialized)
			return;

		bInitialized = true;
		LogInfo("Initializing Loot.");

		UFortPlaylistAthena* Playlist = Globals::GetGameState()->CurrentPlaylistInfo.BasePlaylist ;
		UDataTable* LootPackagesDataTable = SDKUtils::FindObject<UDataTable>(UKismetStringLibrary::Conv_NameToString(Playlist->LootPackages.ObjectID.AssetPathName).ToString());
		UDataTable* LootTierGroupDataTable = nullptr;

		if (!LootPackagesDataTable)
			LootPackagesDataTable = SDKUtils::FindObject<UDataTable>("/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client");

		LogInfo("LootManager::Initialize: LootPackagesDataTable: {}", LootPackagesDataTable->GetName());

		if (LootPackagesDataTable)
		{
			auto& RowMap = LootPackagesDataTable->RowMap;
			for (int i = 0; i < RowMap.Num(); ++i)
			{
				auto& CurrentRow = RowMap[i];
				FName RowName = CurrentRow.First;
				if (!RowName.ComparisonIndex)
					continue;
				string RowNameStr = UKismetStringLibrary::Conv_NameToString(RowName).ToString();
				auto PackageData = reinterpret_cast<FFortLootPackageData*>(CurrentRow.Second);

				if (RowNameStr.contains("WorldPKG") || RowNameStr.contains("WorldList.AthenaLoot.VendingMachine")) // not using WorldPKG yet and don't want VendingMachine loot yet.
					continue;

				if (PackageData->Weight == 0.00)
					continue;

				LootRow Row = LootRow();

				string ItemDefStr = UKismetStringLibrary::Conv_NameToString(PackageData->ItemDefinition.ObjectID.AssetPathName).ToString();
				if (IsItemRemoved(ItemDefStr))
					continue;

				auto ItemDefinition = SDKUtils::FindObject<UFortWeaponRangedItemDefinition>(ItemDefStr);
				if (!ItemDefinition)
					continue;

				LogInfo("LootManager::Initialize: [{} - {}]: Asset Path Name: {}", i, RowNameStr, ItemDefStr);
				Row.RowName = PackageData->LootPackageID.ToString();
				Row.ItemDefinition = ItemDefinition;
				Row.DropCount = PackageData->Count;
				Row.Weight = PackageData->Weight;

				if (!ItemDefStr.contains("AGID_") && !ItemDefStr.contains("ItemData") && !ItemDefStr.contains("Consumables") && !ItemDefStr.contains("ForagedItems")) // For some reason crashes if i don't do this.
					Row.LoadedAmmo = GetClipSize(ItemDefinition);

				if (ItemDefinition->GetName() != "None")
					LootRows.push_back(Row);
			}
		}
	}

	LootRow* GetRandomItem(string RequiredRowName)
	{
		float TotalWeight = 0.0f;
		for (const LootRow& Row : LootRows)
		{
			if (Row.RowName.contains(RequiredRowName))
				TotalWeight += Row.Weight;
		}

		if (TotalWeight == 0.0f)
			return &LootRows[0];

		std::random_device RandomDevice;
		std::mt19937 Generator(RandomDevice());
		std::uniform_real_distribution<float> Distribution(0.0f, TotalWeight);
		float RandomWeight = Distribution(Generator);

		float CumulativeWeight = 0.0f;
		for (LootRow& Row : LootRows)
		{
			if (Row.RowName.contains(RequiredRowName))
			{
				CumulativeWeight += Row.Weight;
				if (RandomWeight <= CumulativeWeight)
					return &Row;
			}
		}

		return &LootRows[0];
	}

	static vector<string> InvalidAmmoTypes = {
		"AmmoInfiniteCrossbow_Athena",
		"AmmoTrollData",
		"AmmoInfinite",
		"AmmoDataSmokeBomb"
	};

	bool IsValidAmmoType(UFortItemDefinition* AmmoDefinition)
	{
		for (string InvalidAmmoType : InvalidAmmoTypes)
		{
			if (AmmoDefinition->GetName() == InvalidAmmoType)
				return false;
		}

		return true;
	}

	vector<LootRow*> GetFloorLoot()
	{
		vector<LootRow*> FloorLootRows;
		auto bConsumable = rand() % 100 > 70;

		if (bConsumable)
			if (rand() % 100 > 80)
				FloorLootRows.push_back(GetRandomItem("WorldList.AthenaHighConsumables"));
			else
				FloorLootRows.push_back(GetRandomItem("WorldList.AthenaLoot.Resources"));
		else
		{
			auto WeaponItem = GetRandomItem("WorldList.AthenaLoot.Weapon");
			if (WeaponItem && WeaponItem->ItemDefinition)
			{
				auto AmmoDefinition = static_cast<UFortWorldItemDefinition*>(WeaponItem->ItemDefinition)->GetAmmoWorldItemDefinition_BP();
				if (AmmoDefinition && AmmoDefinition != WeaponItem->ItemDefinition)
				{
					FloorLootRows.push_back(new LootRow(AmmoDefinition, "", AmmoDefinition->DropCount));
				}

				FloorLootRows.push_back(WeaponItem);
			}
		}

		return FloorLootRows;
	}

	vector<LootRow*> GetChestLoot()
	{
		vector<LootRow*> ChestRows;

		ChestRows.push_back(GetRandomItem("WorldList.AthenaHighConsumables"));
		ChestRows.push_back(GetRandomItem("WorldList.AthenaLoot.Resources"));
		auto WeaponItem = GetRandomItem("WorldList.AthenaLoot.Weapon");
		if (WeaponItem && WeaponItem->ItemDefinition)
		{
			auto AmmoDefinition = static_cast<UFortWorldItemDefinition*>(WeaponItem->ItemDefinition)->GetAmmoWorldItemDefinition_BP();
			if (AmmoDefinition && AmmoDefinition != WeaponItem->ItemDefinition)
			{
				ChestRows.push_back(new LootRow(AmmoDefinition, "", AmmoDefinition->DropCount));
			}

			ChestRows.push_back(WeaponItem);
		}

		return ChestRows;
	}

	vector<LootRow*> GetAmmoLoot()
	{
		vector<LootRow*> AmmoRows;

		AmmoRows.push_back(GetRandomItem("WorldList.AthenaAmmoSmall"));
		AmmoRows.push_back(GetRandomItem("WorldList.AthenaAmmoSmall"));

		return AmmoRows;
	}

	vector<LootRow*> GetSupplyDropLoot()
	{
		vector<LootRow*> SupplyDropRows;

		SupplyDropRows.push_back(GetRandomItem("WorldList.AthenaHighConsumables"));
		SupplyDropRows.push_back(GetRandomItem("WorldList.AthenaLoot.Resources"));
		SupplyDropRows.push_back(GetRandomItem("WorldList.AthenaTraps"));
		auto WeaponItem = GetRandomItem("WorldList.AthenaSupplyDrop");
		if (WeaponItem && WeaponItem->ItemDefinition)
		{
			auto AmmoDefinition = static_cast<UFortWorldItemDefinition*>(WeaponItem->ItemDefinition)->GetAmmoWorldItemDefinition_BP();
			if (AmmoDefinition && AmmoDefinition != WeaponItem->ItemDefinition)
			{
				SupplyDropRows.push_back(new LootRow(AmmoDefinition, "", AmmoDefinition->DropCount));
			}

			SupplyDropRows.push_back(WeaponItem);
		}

		return SupplyDropRows;
	}

	AFortPickupAthena* SpawnPickup(UFortItemDefinition* ItemDefinition, int OverrideCount, int LoadedAmmo, FVector Loc, bool bTossedFromContainer, EFortPickupSourceTypeFlag InPickupSourceTypeFlags, EFortPickupSpawnSource InPickupSpawnSource)
	{
		if (!ItemDefinition)
			return nullptr;

		if (OverrideCount > 1000)
			return nullptr;

		auto SpawnedPickup = SDKUtils::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Loc);
		if (!SpawnedPickup)
			return nullptr;

		SpawnedPickup->bRandomRotation = true;
		FFortItemEntry* PickupEntry = &SpawnedPickup->PrimaryPickupItemEntry;
		FFortItemEntryStateValue Value = {};
		Value.IntValue = 1;
		Value.StateType = EFortItemEntryState::ShouldShowItemToast;
		Value.NameValue = FName(0);
		PickupEntry->StateValues.Add(Value);
		PickupEntry->ItemDefinition = ItemDefinition;
		PickupEntry->Count = OverrideCount;
		PickupEntry->LoadedAmmo = LoadedAmmo;
		PickupEntry->ReplicationKey++;
		SpawnedPickup->OnRep_PrimaryPickupItemEntry();

		SpawnedPickup->TossPickup(Loc, nullptr, 1, true, InPickupSourceTypeFlags, InPickupSpawnSource);

		SpawnedPickup->SetReplicateMovement(true);
		SpawnedPickup->MovementComponent = reinterpret_cast<UProjectileMovementComponent*>(UGameplayStatics::SpawnObject(UProjectileMovementComponent::StaticClass(), SpawnedPickup));

		SpawnedPickup->bTossedFromContainer = bTossedFromContainer;
		SpawnedPickup->OnRep_TossedFromContainer();

		return SpawnedPickup;
	}

	static bool bAlreadySpawned = false;
	bool SpawnFloorLoot()
	{
		bAlreadySpawned = true;

		if (!bAlreadySpawned)
			return false;

		LogInfo("Spawning Floor Loot.");

		int SpawnedLoot = 0;
		auto BR_FloorLoot_Class = SDKUtils::FindObject<UBlueprintGeneratedClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
		TArray<AActor*> BR_FloorLootActors;
		UGameplayStatics::GetAllActorsOfClass(Globals::GetWorld(), BR_FloorLoot_Class, &BR_FloorLootActors);
		for (int i = 0; i < BR_FloorLootActors.Num(); i++)
		{
			auto CurrentActor = BR_FloorLootActors[i];
			if (CurrentActor)
			{
				FVector Loc = CurrentActor->K2_GetActorLocation();
				Loc.Z += 30;
				SpawnedLoot++;
				auto loot = GetFloorLoot();
				for (auto& LootItem : loot)
				{
					if (LootItem->ItemDefinition)
						SpawnPickup(LootItem->ItemDefinition, LootItem->DropCount, LootItem->LoadedAmmo, Loc, false, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset);
				}

				CurrentActor->K2_DestroyActor();
			}
		}
		BR_FloorLootActors.Free();

		auto Warmup_Floorloot_Class = SDKUtils::FindObject<UBlueprintGeneratedClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
		TArray<AActor*> Warmup_FloorlootActors;
		UGameplayStatics::GetAllActorsOfClass(Globals::GetWorld(), Warmup_Floorloot_Class, &Warmup_FloorlootActors);
		for (int i = 0; i < Warmup_FloorlootActors.Num(); i++)
		{
			auto CurrentActor = Warmup_FloorlootActors[i];
			if (CurrentActor)
			{
				FVector Loc = CurrentActor->K2_GetActorLocation();
				SpawnedLoot++;
				auto loot = GetFloorLoot();
				for (auto& LootItem : loot)
				{
					SpawnPickup(LootItem->ItemDefinition, LootItem->DropCount, LootItem->LoadedAmmo, Loc, false, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset);
				}

				CurrentActor->K2_DestroyActor();
			}
		}
		Warmup_FloorlootActors.Free();

		LogInfo("SpawnedLoot: {}", SpawnedLoot);

		return true;
	}
}