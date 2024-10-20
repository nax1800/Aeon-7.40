#pragma once
#include "framework.h"

namespace Hooks
{
	namespace Actor
	{
        void hkOnDamageServer(ABuildingActor* Actor, float Damage, FGameplayTagContainer DamageTags, FVector Momentum, FHitResult HitInfo, AActor* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle EffectContext)
        {
            auto BuildingActor = static_cast<ABuildingSMActor*>(Actor);
            if (!BuildingActor)
                return Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

            if (!DamageCauser->IsA(AFortWeapon::StaticClass()))
                return Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

            auto Weapon = static_cast<AFortWeapon*>(DamageCauser);
            if (!Weapon)
                return Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

            if (!Weapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
                return Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

            auto PlayerController = static_cast<AFortPlayerControllerAthena*>(InstigatedBy);
            if (!PlayerController)
                return Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

            auto PlayerPawn = static_cast<AFortPlayerPawn*>(PlayerController->Pawn);
            if (!PlayerPawn)
                return Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

            if (BuildingActor->bDestroyed || BuildingActor->bPlayerPlaced)
                return Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

            bool bWeakSpotHit = (Damage == 100.f);
            auto ResourceDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingActor->ResourceType);
            if (!ResourceDefinition)
                return  Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);


            int BaseResourceAmount = UKismetMathLibrary::RandomIntegerInRange(5, 8);
            if (bWeakSpotHit)
                BaseResourceAmount += UKismetMathLibrary::RandomIntegerInRange(2, 4);

            PlayerController->ClientReportDamagedResourceBuilding(BuildingActor, BuildingActor->ResourceType, BaseResourceAmount, Actor->bDestroyed, bWeakSpotHit);

            InventoryHandler::AddItem(PlayerController, ResourceDefinition, BaseResourceAmount);

            Defines::Actor::OnDamageServer(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
        }

        void Initialize()
        {
            HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::Actor::OnDamageServer), hkOnDamageServer, (void**)&Defines::Actor::OnDamageServer);
            LogInfo("Logic:Farming: Initialized.");
        }
    }
}