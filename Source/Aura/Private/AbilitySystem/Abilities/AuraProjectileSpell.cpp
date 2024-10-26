// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interface/CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile/AuraProjectile.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility (C++)"), true, true, FLinearColor::Yellow, 3);

	
	    
	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), FAuraGameplayTags::Get().Montage_Attack_Weapon);
    
            FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
    		FTransform SpawnTransform;
    		SpawnTransform.SetLocation(SocketLocation);
    		//TODO : DONE (Set the projectile rotation)
    		SpawnTransform.SetRotation(Rotation.Quaternion());
    
    		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
    			ProjectileClass,
    			SpawnTransform,
    			GetOwningActorFromActorInfo(),
    			Cast<APawn>(GetOwningActorFromActorInfo()),
    				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    
    		//TODO : Give the Projectile a gameplay Effect spec for causing Damage.
            const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
            
            FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
            EffectContextHandle.SetAbility(this);
            EffectContextHandle.AddSourceObject(Projectile);
            TArray<TWeakObjectPtr<AActor>> Actors;
            Actors.Add(Projectile);
            EffectContextHandle.AddActors(Actors);
            FHitResult HitResult;
            HitResult.Location = ProjectileTargetLocation;
            EffectContextHandle.AddHitResult(HitResult);
            
            const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
            
            const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
            //const float ScaledDamage = Damage.GetValueAtLevel(10);
    
    		for (auto& Pair:DamageTypes)
    		{
    			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
    			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
    		}
    		
            //UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaledDamage);
            
            Projectile->DamageEffectSpecHandle = SpecHandle;
    		Projectile->FinishSpawning(SpawnTransform);
}
