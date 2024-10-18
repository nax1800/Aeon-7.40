#pragma once
#include "framework.h"

namespace Hooks
{
	namespace Abilities
	{
        void hkInternalServerTryActivateAbility(UAbilitySystemComponent* ASc, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, FGameplayEventData* TriggerEventData)
        {
            FGameplayAbilitySpec* Spec = AbilitiesHandler::FindAbilitySpecFromHandle(ASc, Handle);
            if (!Spec)
            {
                ASc->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
                return;
            }

            const UGameplayAbility* AbilityToActivate = Spec->Ability;

            UGameplayAbility* InstancedAbility = nullptr;
            Spec->InputPressed = true;

            if (Defines::Abilities::InternalTryActivateAbility(ASc, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
            {
            }
            else
            {
                ASc->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
                Spec->InputPressed = false;
                ASc->ActivatableAbilities.MarkItemDirty(*Spec);
            }
        }

        void Initialize()
        {
            auto DefaultObject = UFortAbilitySystemComponentAthena::StaticClass()->DefaultObject;
            HookingManager::VirtualHook(DefaultObject, 0xf5, hkInternalServerTryActivateAbility);
        }
	}
}