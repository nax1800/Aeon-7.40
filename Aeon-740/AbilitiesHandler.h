#pragma once
#include "framework.h"

namespace AbilitiesHandler
{
    FGameplayAbilitySpec* FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
    {
        for (int i = 0; i < AbilitySystemComponent->ActivatableAbilities.Items.Num(); i++)
        {
            if (AbilitySystemComponent->ActivatableAbilities.Items[i].Handle.Handle == Handle.Handle)
            {
                return &AbilitySystemComponent->ActivatableAbilities.Items[i];
            }
        }

        return nullptr;
    }

    void GrantGameplayAbility(AFortPlayerStateAthena* PlayerState, UClass* GameplayAbilityClass)
    {
        LogInfo("GrantGameplayAbility");
        auto AbilitySystemComponent = PlayerState->AbilitySystemComponent;

        if (!AbilitySystemComponent)
            return;

        FGameplayAbilitySpec NewSpec{};

        Defines::Abilities::SpecConstructor(&NewSpec, GameplayAbilityClass->DefaultObject, 1, -1, nullptr);
        NewSpec.RemoveAfterActivation = 0;

        Defines::Abilities::GiveAbility(AbilitySystemComponent, &NewSpec.Handle, NewSpec);
        return;
    }

    void ApplyAbilities(AFortPlayerStateAthena* PlayerState)
    {
        static auto AbilitySet = SDKUtils::FindObject<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");
        for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
        {
            GrantGameplayAbility(PlayerState, AbilitySet->GameplayAbilities[i].Get());
        }
    }
}