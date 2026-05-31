// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainPlayer.h"
#include "GameFramework\SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Sword.h"
#include "Animation/AnimMontage.h"
#include "Items/Grill.h"
#include "TimerManager.h"
#include "Items/Beer.h"
#include "Items/Sausage.h"
#include "Logging/StructuredLog.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	GetWorldTimerManager().SetTimer(
		StatDepletionTimerHandle,
		this,
		&AMainPlayer::HandleMinuteDepletion,
		30.0f,
		true
	);
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AMainPlayer::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMainPlayer::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMainPlayer::LookUp);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &AMainPlayer::EKeyPressed);
	//PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AMainPlayer::Attack);
	PlayerInputComponent->BindAction(FName("Fire"), IE_Pressed, this, &AMainPlayer::Fire);

	PlayerInputComponent->BindAction(FName("Slot1"), IE_Pressed, this, &AMainPlayer::Slot1Pressed);
	PlayerInputComponent->BindAction(FName("Slot2"), IE_Pressed, this, &AMainPlayer::Slot2Pressed);
	PlayerInputComponent->BindAction(FName("Slot3"), IE_Pressed, this, &AMainPlayer::Slot3Pressed);
	PlayerInputComponent->BindAction(FName("Drop"), IE_Pressed, this, &AMainPlayer::OnQPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AMainPlayer::OnLMBPressed);

}

void AMainPlayer::MoveForward(float value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AMainPlayer::MoveRight(float value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}

void AMainPlayer::Turn(float value)
{
	AddControllerYawInput(value);
}

void AMainPlayer::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void AMainPlayer::EKeyPressed()
{
	for (AItem* Item : OverlappingItems)
	{
		if (Inventory.Contains(Item)) 
		{
			continue;
		}

		if (ASword* SwordOnGround = Cast<ASword>(Item))
		{
			if (EquippedWeapon == nullptr)
			{
				SwordOnGround->Equip(GetMesh(), FName("RightHandSocket"));
				EquippedWeapon = SwordOnGround;
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				//RemoveOverlappingItem(SwordOnGround);
				AddToInventory(SwordOnGround);
				return;
			}
		}
		else if (Cast<ABeer>(Item) || Cast<ASausage>(Item))
		{
			AddToInventory(Item);
			return;
		}
	}
}

void AMainPlayer::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
		UpdateStat("Rest", -5.0f);
	}

}

void AMainPlayer::Fire()
{
	for (AItem* Item : OverlappingItems)
	{
		if (AGrill* OverlappingGrill = Cast<AGrill>(Item))
		{
			bool bNewState = !OverlappingGrill->bIsBurning;
			OverlappingGrill->SetFireEnabled(bNewState);

			FVector PlayerLocation = GetActorLocation();

			if (LightTheGrillSound && bNewState)
			{
				UGameplayStatics::PlaySoundAtLocation(this, LightTheGrillSound, PlayerLocation);
			}
		}
	}
}

bool AMainPlayer::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMainPlayer::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState != ECharacterState::ECS_Unequipped
		&& EquipMontage;
}

void AMainPlayer::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}

}

bool AMainPlayer::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState == ECharacterState::ECS_Unequipped
		&& EquippedWeapon;
}

void AMainPlayer::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AMainPlayer::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AMainPlayer::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainPlayer::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		default:
			break;

		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void AMainPlayer::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainPlayer::AddOverlappingItem(AItem* Item)
{
	if (Item)
	{
		OverlappingItems.AddUnique(Item);
	}
}

void AMainPlayer::RemoveOverlappingItem(AItem* Item)
{
	if (Item)
	{
		OverlappingItems.Remove(Item);
	}
}

void AMainPlayer::UpdateStat(FString StatName, float Amount)
{
	if (StatName == "Health") Health = FMath::Clamp(Health + Amount, 0.f, MaxStat);
	else if (StatName == "Hunger") Hunger = FMath::Clamp(Hunger + Amount, 0.f, MaxStat);
	else if (StatName == "Rest") Rest = FMath::Clamp(Rest + Amount, 0.f, MaxStat);
	else if (StatName == "Beer") BeerDesire = FMath::Clamp(BeerDesire + Amount, 0.f, MaxStat);
}

void AMainPlayer::PlayVicovaro()
{
	FVector PlayerLocation = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(this, BeerIntoxicationSound, PlayerLocation);
}

void AMainPlayer::HandleMinuteDepletion()
{

	UpdateStat("Hunger", -20.0f);
	UpdateStat("Beer", -20.0f);

	FVector PlayerLocation = GetActorLocation();

	if (ThirstSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ThirstSound, PlayerLocation);
	}

	UE_LOG(LogTemp, Warning, TEXT("Statystyki spadly: -10"));
}

void AMainPlayer::AddToInventory(AItem* Item)
{
	if (Item)
	{
		Inventory.Add(Item);

		ASword* Sword = Cast<ASword>(Item);

		if (!Sword)
		{
			Item->SetActorHiddenInGame(true);
			Item->SetActorEnableCollision(false);
		}

		RemoveOverlappingItem(Item);
		UE_LOG(LogTemp, Warning, TEXT("Podniesiono przedmiot: %s"), *Item->GetName());
		OnInventoryChanged.Broadcast();
	}
}

void AMainPlayer::SelectSlot(int32 Index)
{
	AItem* PrevItem = GetActiveItem();

	UE_LOG(LogTemp, Warning, TEXT("TEST"));
	ASword* PrevSword = Cast<ASword>(PrevItem);

	if (!PrevSword && PrevItem)
	{
		PrevItem->SetActorHiddenInGame(true);
		PrevItem->SetActorEnableCollision(false);
	}

	SelectedIndex = Index;
	UE_LOG(LogTemp, Warning, TEXT("Wybrano slot: %d"), SelectedIndex + 1);

	AItem* ActiveItem = GetActiveItem();

	UE_LOG(LogTemp, Warning, TEXT("ActionState: %d, CharacterState: %d"),
		static_cast<int32>(ActionState),
		static_cast<int32>(CharacterState));

	ASword* Sword = Cast<ASword>(ActiveItem);
	if (Sword)
	{
		if (CanDisarm())
		{
			UE_LOG(LogTemp, Warning, TEXT("Unequip"));
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm())
		{
			UE_LOG(LogTemp, Warning, TEXT("Equip"));
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
	else if (ActiveItem)
	{
		if (PrevSword) 
		{
			PrevSword->Equip(GetMesh(), FName("SpineSocket"));
		}
		ASausage* Sausage = Cast<ASausage>(ActiveItem);

		if (Sausage) {
			ActiveItem->EquipItem(GetMesh(), FName("RightHandSocketKielbasa"));
		}
		else
		{
			ActiveItem->EquipItem(GetMesh(), FName("RightHandSocketPiwo"));
		}

		
		ActiveItem->SetActorHiddenInGame(false);
		ActiveItem->SetActorEnableCollision(true);

		CharacterState = ECharacterState::ECS_Unequipped;
	}

	OnInventoryChanged.Broadcast();
}

AItem* AMainPlayer::GetActiveItem() const
{
	if (Inventory.IsValidIndex(SelectedIndex))
	{
		return Inventory[SelectedIndex];
	}
	return nullptr;
}

void AMainPlayer::OnLMBPressed()
{
	AItem* ActiveItem = GetActiveItem();


	UE_LOG(LogTemp, Warning, TEXT("ActionItem: %d, !EquippedWeapon: %d"),
		static_cast<bool>(ActiveItem),
		static_cast<bool>(!EquippedWeapon));

	if (ActiveItem && CharacterState == ECharacterState::ECS_Unequipped)
	{
		UseItem();
	}

	else if (EquippedWeapon)
	{
		Attack();
	}
}

void AMainPlayer::UseItem()
{
	AItem* ItemToUse = GetActiveItem();
	if (!ItemToUse) return;

	FVector PlayerLocation = GetActorLocation();

	TSubclassOf<AItem> ClassToRespawn = nullptr;
	FTransform TargetTransform;

	if (Cast<ABeer>(ItemToUse))
	{
		if (BeerDesire == MaxStat)
		{
			if (BeerIntoxicationSound)
			{
				GetWorldTimerManager().SetTimer(
					StatDepletionTimerHandle,
					this,
					&AMainPlayer::PlayVicovaro,
					5.0f,
					false
				);
			}
		}

		ClassToRespawn = BeerClass;
		TargetTransform = BeerSpawnTransform;

		UpdateStat("Beer", 20.0f);
		UE_LOG(LogTemp, Warning, TEXT("Wypito piwo!"));

		if (BeerDrinkSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, BeerDrinkSound, PlayerLocation);
		}

	}
	else if (Cast<ASausage>(ItemToUse))
	{

		ClassToRespawn = SausageClass;
		TargetTransform = SausageSpawnTransform;

		UpdateStat("Hunger", 25.0f);
		UE_LOG(LogTemp, Warning, TEXT("Zjedzono kielbase!"));

		if (SausageEatSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SausageEatSound, PlayerLocation);
		}
	}

	if (ClassToRespawn)
	{
		RespawnItem(ClassToRespawn, TargetTransform);
	}

	Inventory.RemoveAt(SelectedIndex);
	ItemToUse->Destroy();
	OnInventoryChanged.Broadcast();
}

void AMainPlayer::OnQPressed()
{
	AItem* ItemToDrop = GetActiveItem();
	if (!ItemToDrop) return;

	ASword* Sword = Cast<ASword>(ItemToDrop);
	if (Sword)
	{
		CharacterState = ECharacterState::ECS_Unequipped;
		EquippedWeapon = nullptr;
	}

	ItemToDrop->SetActorHiddenInGame(false);
	ItemToDrop->SetActorEnableCollision(true);

	ItemToDrop->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	ItemToDrop->SetActorLocation(DropLocation);

	Inventory.RemoveAt(SelectedIndex);
	OnInventoryChanged.Broadcast();

	UE_LOG(LogTemp, Warning, TEXT("Wyrzucono przedmiot!"));
}

void AMainPlayer::RespawnItem(TSubclassOf<AItem> ClassToSpawn, const FTransform& SpawnTransform)
{
	if (!ClassToSpawn) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AItem>(
		ClassToSpawn,
		SpawnTransform.GetLocation(),
		SpawnTransform.GetRotation().Rotator(),
		SpawnParams
	);
}





