// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"
#include "../Aura.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SWarningOrErrorBox.h"
#include "Character/AbilitySystem/AuraAttributeSet.h"


AAuraEffectActor::AAuraEffectActor()
{
 	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	//Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraEffectActor::OnOverlap);
	//Sphere->OnComponentEndOverlap.AddDynamic(this, &AAuraEffectActor::EndOverlap);
	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<class UGameplayEffect> GameplayEffectClass)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	checkf(GameplayEffectClass, TEXT("GameplayEffectClass is not Valid"));
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);//TargetActor를 ContextHandle에 저장/ e.g. targetActor , Location ect..

	// 폭발 위치 설정
	FVector ExplosionLocation = GetActorLocation();
	UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	if (RootPrimitiveComponent)
	{
		FHitResult HitResult;
		HitResult.Location = ExplosionLocation;
		HitResult.ImpactPoint = ExplosionLocation;
		HitResult.Component = RootPrimitiveComponent;
		HitResult.HitObjectHandle = FActorInstanceHandle(this);

		EffectContextHandle.AddHitResult(HitResult);//HitResult(location impactPoint등을 contextHandle에 저장
	}//
	
	const FGameplayEffectSpecHandle EffectSpecHandle =
		TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	// HitResult에서 위치 정보를 가져와서 폭발 효과 생성
	const FHitResult* StoredHitResult = EffectContextHandle.GetHitResult();//contexthandle에 저장된 HitResult결과를 저장
	if (StoredHitResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("Triggering explosion from hit result"));
		TriggerExplosionEffect(StoredHitResult->ImpactPoint);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Triggering explosion from actor location"));
		TriggerExplosionEffect(ExplosionLocation);
	}//

	//if EffectSpecHandl에서 durationpolicy= infinite일때... ->true면 
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == 
	EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
	    ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);//ActiveEffectHandle Map에 저장 -> Endoverlap시 종료해주기 위해
	}
	if (!bIsInfinite)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
    	ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
    	ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
    	ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }
    if (InfiniteEffectRemovalPolicy  == EEffectRemovalPolicy::RemoveOnEndOverlap)
    {
		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    	if (!IsValid(TargetASC))return;

    	TArray<FActiveGameplayEffectHandle> HandlesToRemove;
    	for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
    	{
    		//TargetASC가 저장되어있는 HandlePair의 ASC Value값과 같으면
    		if (TargetASC == HandlePair.Value)
    		{
    			TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
    			HandlesToRemove.Add(HandlePair.Key);
    		}
    	}
    	for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
    	{
    		ActiveEffectHandles.FindAndRemoveChecked(Handle);
    	}
    }
}

void AAuraEffectActor::TriggerExplosionEffect(const FVector& Location)
{
	if (ExplosionEffect)
	{
		// Niagara 시스템을 Location에서 생성하고, 반환된 컴포넌트를 확인합니다.
		UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, Location);
		if (SpawnedEffect)
		{
			UE_LOG(LogTemp, Warning, TEXT("Niagara Effect successfully spawned at: %s"), *Location.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Niagara Effect at: %s"), *Location.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ExplosionEffect is null!"));
	}
}



void AAuraEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

