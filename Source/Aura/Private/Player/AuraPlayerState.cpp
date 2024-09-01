// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Character/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{
	//how often the server will try to update clients.
	//So as changes occur on the server for the player state, then the server will be sending updates out
	//to all clients so that they can sync up with the server version
	NetUpdateFrequency = 100.f;

	MyAbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	MyAbilitySystemComponent->SetIsReplicated(true);
	//How gameplay effects will be replicated to clients
	MyAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return MyAbilitySystemComponent;
}
void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{

}