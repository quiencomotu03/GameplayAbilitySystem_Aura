// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraEnemy.h"
#include "SNegativeActionButton.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "UI/Widget/AuraUserWidget.h"
#include "Components/WidgetComponent.h"
#include "Aura/Aura.h"


AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	check(AbilitySystemComponent);
	//AbilitySystemComponent->InitAbilityActorInfo(this, this);
	//AuraLOG(Warning, TEXT("Ability System Component Initialized"));
	InitAbilityActorInfo();
	
	if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
    	{
    		AuraUserWidget->SetWidgetController(this);
    	}
    	
    	if (const UAuraAttributeSet* AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
    	{
    		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
    			[this](const FOnAttributeChangeData& Data)
    			{
    				OnHealthChanged.Broadcast(Data.NewValue);
    			}
    		);
    		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
    			[this](const FOnAttributeChangeData& Data)
    			{
    				OnMaxHealthChanged.Broadcast(Data.NewValue);
    			}
    		);
    		OnHealthChanged.Broadcast(AuraAS->GetHealth());
    		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
    	}
}
void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
    InitializeDefaultAttributes();
	
}

void AAuraEnemy::HighlightActor()
{
	//bHighlighted = true;
	//AuraLOG_S(Warning);
	AuraLOG(Warning, TEXT("%s is overlapped and highlighted"), *GetName());
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, FColor::Blue);

	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	//bHighlighted = false;
	//AuraLOG_S(Warning);
	AuraLOG(Warning, TEXT("%s is now not overlapped and UNhighlighted"), *GetName());
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, FColor::Red);

	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}
