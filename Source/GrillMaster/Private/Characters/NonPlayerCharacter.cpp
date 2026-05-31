// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/NonPlayerCharacter.h"
#include "Items/Grill.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANonPlayerCharacter::ANonPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	bIsWatchingFire = false;
	MyTargetGrill = nullptr;

}

// Called when the game starts or when spawned
void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();


	//for (int i = 0; i < 10000000; i++);

	if (MyTargetGrill)
	{
		MyTargetGrill->OnFireStateChanged.AddDynamic(this, &ANonPlayerCharacter::OnGrillStateChanged);
		bIsWatchingFire = MyTargetGrill->bIsBurning;
		UE_LOG(LogTemp, Log, TEXT("%s po³¹czy³ siê z grillem: %s"), *GetName(), *MyTargetGrill->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s nie ma przypisanego grilla!"), *GetName());
	}
	
}

// Called every frame
void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANonPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANonPlayerCharacter::OnGrillStateChanged(bool bIsBurning)
{
	bIsWatchingFire = bIsBurning;
}

