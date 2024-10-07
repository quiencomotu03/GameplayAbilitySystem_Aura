// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
    TSubclassOf<class UGameplayEffect> PrimaryAttributes;

};
/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
	
public:

    UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
    TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;	
    
    UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
    TSubclassOf<class UGameplayEffect> SecondaryAttributes;
    
    UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
    TSubclassOf<class UGameplayEffect> VitalAttributes;
    
    UPROPERTY(EditDefaultsOnly, Category ="Common Class Defaults")
    TArray<TSubclassOf<class UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults|Damage")
	TObjectPtr<class UCurveTable> DamageCalculationCoefficients;
	
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
	
};
