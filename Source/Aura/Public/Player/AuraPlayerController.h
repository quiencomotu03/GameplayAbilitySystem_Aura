// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Input/AuraInputConfig.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/DamageTextComponent.h"
#include "AuraPlayerController.generated.h"

/**
 * 
 */

class UDamageTextComponent;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;
private:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
    TObjectPtr<UInputAction> ShiftAction;

    void ShiftPressed() {bShiftKeyDown = true;};
    void ShiftReleased() {bShiftKeyDown = false;};
    bool bShiftKeyDown = false;
    
	void Move(const struct FInputActionValue& value);
	void CursorTrace();

	FHitResult CursorHit;
	TScriptInterface<class IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor; 

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UAuraInputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<class UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UAuraAbilitySystemComponent* GetASC();
	
	FVector CachedDestination = FVector::ZeroVector;
	
	//How long LMB being pressed == the amount of time that player is following the mouse cursor
	float FollowTime = 0.0f;
	//how much time until we no longer consider that to be a short press
	float ShortPressThreshold = 1.f;
	bool bAutoRunning = false;
	
	bool bTargeting = false;
	
	//How close we get to that destination should be a parameter we can change
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USplineComponent> Spline;
	
	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	
};
