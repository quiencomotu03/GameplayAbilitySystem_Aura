// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AAuraProjectile();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	//meta = (ExposeOnSpawn = true) --> expose pin on spawn
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	
protected:

	void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;
	
    UPROPERTY(EditDefaultsOnly)
    float LifeSpan = 15.f;
    
    bool bHit = false;
    
    UPROPERTY(EditAnywhere)
    TObjectPtr<class UNiagaraSystem> ImpactEffect;
    
    UPROPERTY(EditAnywhere)
    TObjectPtr<class USoundBase> ImpactSound;
    
    UPROPERTY(EditAnywhere)
    TObjectPtr<class USoundBase> LoopingSound;
    
    UPROPERTY()
    TObjectPtr<class UAudioComponent> LoopingSoundComponent;


};
