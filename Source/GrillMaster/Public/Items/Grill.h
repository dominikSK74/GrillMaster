// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Grill.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireStateChanged, bool, bIsCurrentlyBurning);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GRILLMASTER_API AGrill : public AItem
{
	GENERATED_BODY()
public:
	AGrill();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
	UNiagaraComponent* FireEffect;

	UFUNCTION(BlueprintCallable, Category = "Grill")
	void SetFireEnabled(bool bEnabled);

	UPROPERTY(BlueprintReadOnly, Category = "Grill")
	bool bIsBurning;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFireStateChanged OnFireStateChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UAudioComponent> GrillMusicComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")
	class USoundAttenuation* GrillAttenuationAsset;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
