// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterType.h"
#include "Blueprint/UserWidget.h"
#include "MainPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class ASword;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS()
class GRILLMASTER_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);
	void EKeyPressed();
	void Attack();
	void Fire();

	void PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	bool CanAttack();
	bool CanDisarm();
	void PlayEquipMontage(FName SectionName);
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<AItem*> OverlappingItems;



	// ------ STATYSTYKI ------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Hunger = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Rest = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BeerDesire = 100.f;

	float MaxStat = 100.f;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	UUserWidget* HUDWidget;


	FTimerHandle StatDepletionTimerHandle;

	void HandleMinuteDepletion();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<AItem*> Inventory;


	// ----- INVENTORY----
	void AddToInventory(AItem* Item);

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 SelectedIndex = 0;

	void Slot1Pressed() { SelectSlot(0); }
	void Slot2Pressed() { SelectSlot(1); }
	void Slot3Pressed() { SelectSlot(2); }
	void OnQPressed();
	void OnLMBPressed();

	void SelectSlot(int32 Index);
	void UseItem();
	void DropItem();

	AItem* GetActiveItem() const;

	// --- dzwieki ----
	UPROPERTY(EditDefaultsOnly, Category = "Audio | Items")
	USoundBase* BeerDrinkSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio | Items")
	USoundBase* SausageEatSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio | Items")
	USoundBase* LightTheGrillSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio | Items")
	USoundBase* ThirstSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio | Items")
	USoundBase* BeerIntoxicationSound;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	void AddOverlappingItem(AItem* Item);
	void RemoveOverlappingItem(AItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void UpdateStat(FString StatName, float Amount);

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealthPercent() const { return Health / 100.0f; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetBeerPercent() const { return BeerDesire / 100.0f; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHungerPercent() const { return Hunger / 100.0f; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetRestPercent() const { return Rest / 100.0f; }

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInventoryChanged OnInventoryChanged;

	// RESPAWN PRzedmiotow
	UPROPERTY(EditAnywhere, Category = "Respawn")
	TSubclassOf<class ABeer> BeerClass;

	UPROPERTY(EditAnywhere, Category = "Respawn")
	TSubclassOf<class ASausage> SausageClass;

	UPROPERTY(EditAnywhere, Category = "Respawn", Meta = (MakeEditWidget = true))
	FTransform BeerSpawnTransform;

	UPROPERTY(EditAnywhere, Category = "Respawn", Meta = (MakeEditWidget = true))
	FTransform SausageSpawnTransform;


private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	ASword* EquippedWeapon;

	void PlayVicovaro();

	void RespawnItem(TSubclassOf<AItem> ClassToSpawn, const FTransform& SpawnTransform);

};
