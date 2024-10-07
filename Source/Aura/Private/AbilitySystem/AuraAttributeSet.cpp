// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AbilitySystem/AuraAttributeSet.h"
//#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AuraGameplayTags.h"
#include "Interface/CombatInterface.h"
#include "kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"


UAuraAttributeSet::UAuraAttributeSet()
{
	//By #define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)  All the Attributes can be initialize by Initter
	//InitHealth(10.f);
	//InitMaxHealth(100.f);
	//InitMana(10.f);
	//InitMaxMana(50.f);
	
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	FAttributeSignature StrengthDelegate;
	StrengthDelegate.BindStatic(GetStrengthAttribute);
//	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, StrengthDelegate);

	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
	
	/* Secondary Attributes */
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);	
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
    	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*
	 * Vital Attributes
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	
	/*
	 * Primary Attributes
	 */

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/*
	 * Secondary Attributes
	 */
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	/*
	* 핵심 내용: 어떤 기능(예: 값을 반환하는 함수나 메서드)을 호출해서 얻은 결과를 "고정"시키거나 "잠그는(clamp)" 
	* 작업을 할 수 있지만, 이 작업이 단지 그 순간의 결과를 잠그는 것이지, 그 기능(수정자 혹은 modifier)을 다시 호출하면 
	* 새로운 값이 계산될 것
      
      예를 들어
      수정자(modifier): 특정 값을 계산하거나 반환하는 기능이 있다고 가정합니다. 
      * 이 수정자는 예를 들어 현재 시간, 사용자 입력, 외부 데이터 등 다양한 값을 반환할 수 있습니다.
      클램프(clamp): 이 기능을 한 번 호출해서 나온 결과를 고정시키는 작업을 의미합니다. 예를 들어, 
      * 현재 시간 수정자를 호출했을 때 나온 시간이 "3:00 PM"이라면, 이 시간을 고정시켜서 이후에는 "3:00 PM"이 
      * 계속 반환되도록 할 수 있습니다.
      
      재계산(recalculate): 그러나, 동일한 수정자를 다시 호출하게 되면, 
      * 그 수정자는 다시 새로운 값을 계산해서 반환하게 됩니다. 예를 들어, 다시 호출하면 "3:05 PM" 같은 
      * 새로운 시간이 반환될 수 있습니다.
      
      결론적으로: 이 문장에서 설명하는 것은, "클램프(clamp)" 작업이 일시적으로 값을 고정시키지만,
      * 중요한 것은 이 Clamp 작업이 단지 그 순간의 값을 고정하는 것일 뿐, 다음에 Attribute를 다시 쿼리하면
      *  (즉, 다시 계산하거나 수정자를 호출하면) 새로운 값이 다시 계산될 수 있다는 점
      * 수정자를 다시 호출하게 되면 원래 기능에 따라 새로운 값이 계산되고 반환된다는 것입니다. 
      * 따라서 고정된 값에만 의존하지 말고, 수정자를 다시 호출할 때의 변화를 염두에 두어야 한다는 점을 강조하고 있습니다.
	* */
	
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		//AuraLOG(Warning, TEXT("Health: %f"), NewValue);
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		//AuraLOG(Warning, TEXT("Mana : %f"), NewValue);
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	/*
	if (Data.EvaluatedData.Attribute ==  GetHealthAttribute())
	{
		AuraLOG(Warning, TEXT("Post GetHealth : %f"), GetHealth());
		AuraLOG(Warning, TEXT("Magnitude of Health Change : %f"), Data.EvaluatedData.Magnitude);
	}*/
	
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	
	/*
	* We are going to actually set the value to a clamped value here in post gameplay effect execute.
	* */
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
	    SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	    //UE_LOG(LogTemp, Warning, TEXT("Changed health on %s, Health: %f"), *Props.TargetAvatarActor->GetName(), GetHealth());
	    //AuraLOG(Warning, TEXT("Health: %f"), GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
	    SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	    //AuraLOG(Warning, TEXT("Mana: %f"), GetMana());
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    	{
    		const float LocalIncomingDamage = GetIncomingDamage();
    		SetIncomingDamage(0.f);
    		if (LocalIncomingDamage > 0.f)
    		{
    			const float NewHealth = GetHealth() - LocalIncomingDamage;
    			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
    			const bool bFatal = NewHealth <= 0.f;
    			
    			if (bFatal)
    			{
    				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
    				if (CombatInterface)
    				{
    					CombatInterface->Die();
    				}
    			}
    			else
    			{
    			    FGameplayTagContainer TagContainer;
    			    TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
    			    Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
    			}
    			ShowFloatingText(Props, LocalIncomingDamage);
    		}
    	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	//Source = causer of the effect, Target = Target of the effect(owner of this AS)

	//SourceAvilityComponent, AvatarActor, Controller, Actor, 
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();//source avilitysystemc component

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			//ACharacter* SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	//TargetAvilityocmponent, AvatarActor, Controller, Actor, 
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if(AAuraPlayerController* PC = Cast<AAuraPlayerController>(UGameplayStatics::GetPlayerController(Props.SourceCharacter, 0)))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter);
		}
	}
}
