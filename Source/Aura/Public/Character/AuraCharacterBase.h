// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:

	AAuraCharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

	
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<class USkeletalMeshComponent> Weapon;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	/*
	  ICombatInterface
	 */
	
	virtual FVector GetCombatSocketLocation() override;

	/*
	 ~ ICombatInterface
	 */

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UAttributeSet> AttributeSet;

	UFUNCTION()
	virtual void InitAbilityActorInfo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> DefaultVitalAttributes;

	UFUNCTION()
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();
private:
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;
	
};
