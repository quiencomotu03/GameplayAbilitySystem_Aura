// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AuraPlayerController.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "../Aura.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Character/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//check is going to halt execution if this condition fails or AuraContext* has not been set yet = false
	// will get crash if run without setting AuraContext
	check(AuraContext);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//Will get a crach if this cast fails
	//UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed,
		&ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInPutTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::Move(const struct FInputActionValue& value)
{
	const FVector2d InputAxisVector = value.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation (0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
	
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	//ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	ThisActor = CursorHit.GetActor();

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			//Lastactor nullptr , This actor is not nullptr -> thisactor->highlight
			ThisActor->HighlightActor();
		}
		else
		{
			//Cast - both are null, do nothing
			
		}
	}
	else //LastActor is valid
	{
		if  (ThisActor == nullptr)
		{
			LastActor->UnHighlightActor();
		}
		else
		{
			if (LastActor != ThisActor)//Both valid, but lastactor is not this actor
			{
				LastActor->UnHighlightActor(); // 이전 액터는 unhighlight
				ThisActor->HighlightActor(); //this actor ->highlight
			}
			else //both are valid, and they are same
			{
				//Do nothing
			}
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//AuraLOG(Warning, TEXT("InputTag : %s"), *InputTag.ToString());
	//GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, *InputTag.ToString());
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
	    //if this actor is not a null pointer, then this ternary operator expression returns true.
	    bTargeting = ThisActor ? true : false;
	    
	    //set be auto running to false because we don't know yet if it's a short press.
        //If it's a short press, we're going to auto run
        // But we don't know that yet until we've released the mouse button.
	    bAutoRunning = false;
	    
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//AuraLOG(Warning, TEXT("InputTag : %s"), *InputTag.ToString());
	//GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Blue, *InputTag.ToString());

	//if (GetASC() == nullptr) return;
	//GetASC()->AbilityInputTagReleased(InputTag);
	
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	if (bTargeting)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
	}
	else //ClicktoMove 여기 안들어오는듯 ?!?!?
	{
		APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			AuraLOG(Warning, TEXT("AbilityInputTagReleased_01"));
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				AuraLOG(Warning, TEXT("AbilityInputTagReleased_02"));
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					AuraLOG(Warning, TEXT("AbilityInputTagReleased_03"));
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					DrawDebugSphere(GetWorld(), PointLoc, 8.f, FColor::Green);
					AuraLOG(Warning, TEXT("AbilityInputTagReleased_04"));
				}
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInPutTagHeld(FGameplayTag InputTag)
{
	//AuraLOG(Warning, TEXT("InputTag : %s"), *InputTag.ToString());
	//GEngine->AddOnScreenDebugMessage(3, 3.0f, FColor::Green, *InputTag.ToString());

	//if (GetASC() == nullptr) return;
	//GetASC()->AbilityInputTagHeld(InputTag);
	
	// When LMB is NOT pressed, try to activate the ability 
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
	    if (GetASC())
	    {
	        GetASC()->AbilityInputTagHeld(InputTag);
	    }
	    return;
	}
	//LMB is pressed, and Target the enemy actor
	if (bTargeting)
	{
	    if (GetASC())
	    {
	        GetASC()->AbilityInputTagHeld(InputTag);
	    }
	}
	else //if LMB, and Not Targeting Enemy actor -> concerned about Click to Move
	{
	    FollowTime += GetWorld()->GetTimeSeconds();
	    
	    FHitResult Hit;
	    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
	    {
	        CachedDestination = Hit.ImpactPoint;
	    }
	    if (APawn* ControlledPawn = GetPawn())
	    {
	        const FVector WorldDirection = 
	            (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
	        ControlledPawn->AddMovementInput(WorldDirection);
	    }
	}
	
	
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>
		(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
		
	}
	return AuraAbilitySystemComponent;
}
