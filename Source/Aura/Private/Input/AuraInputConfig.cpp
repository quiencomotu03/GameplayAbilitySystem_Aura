// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"
#include "../Aura.h"

const UInputAction* UAuraInputConfig::FindInputAction(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
	{
		AuraLOG(Warning, TEXT("Can not find AbilityInputAction for Input [%s], on InputConfig[%s]"),
			*InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
