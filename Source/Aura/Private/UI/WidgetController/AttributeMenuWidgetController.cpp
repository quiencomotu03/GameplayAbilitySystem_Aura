// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "Character/AbilitySystem/Data/AttributeInfo.h"
#include "Character/AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	   UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
        check(AttributeInfo);
        
       /*FAuraAttributeInfo StrengthInfo = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
        StrengthInfo.AttributeValue = AS->GetStrength();
        AttributeInfoDelegate.Broadcast(StrengthInfo);*/ 
        
        for (auto& Pair : AS->TagsToAttributes)
        {
          BroadcastAttributeInfo(Pair.Key, Pair.Value());
        }
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
    	check(AttributeInfo);
    	for (auto& Pair : AS->TagsToAttributes)
    	{
    		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
    		[this, Pair](const FOnAttributeChangeData& Data)
    		{
    			BroadcastAttributeInfo(Pair.Key, Pair.Value());
    		}
    	);
    	}
}


void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}