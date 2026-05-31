// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/NPCAnimInstance.h"
#include "Characters/NonPlayerCharacter.h"

void UNPCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(TryGetPawnOwner());
    if (NPC)
    {
        bIsWatchingFire = NPC->bIsWatchingFire;
    }
}