// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);//동적할당 
	return MyObj;
	
}

void UTargetDataUnderMouse::Activate()
{
	Super::Activate();
/*
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	ValidData.Broadcast(CursorHit.Location);*/
	
	//Clients -> Send the data to the server
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		//TODO:: Server, so listen for target data
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey;
		GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle,
			ActivationPredictionKey).AddUObject(this,
				&UTargetDataUnderMouse::OnTargetDataReplicatedCallBack);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,
			ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
	
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle, 
	                                          FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),
	GetActivationPredictionKey());
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}

}