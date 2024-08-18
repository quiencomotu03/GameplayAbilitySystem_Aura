// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Aura.h"
#include "EngineMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Player/AuraPlayerState.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:

	AAuraCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

protected:
	virtual void BeginPlay() override;
	
private:
	void InitAbilityActorInfo();
	
};
