// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerAnimInstance.h"
//#include "PlayerAnimInstance.h"
#include "Characters/MainPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MainPlayer = Cast<AMainPlayer>(TryGetPawnOwner());

	if (MainPlayer)
	{
		MainPlayerMovement = MainPlayer->GetCharacterMovement();
	}
	
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MainPlayerMovement) 
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(MainPlayerMovement->Velocity);
		isFalling = MainPlayerMovement->IsFalling();
		CharacterState = MainPlayer->GetCharacterState();
	}
}
