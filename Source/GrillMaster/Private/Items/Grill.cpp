// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Grill.h"

AGrill::AGrill()
{
	ItemState = EItemState::EIS_Equipped;
	FireEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffect"));
	FireEffect->SetupAttachment(RootComponent);
	FireEffect->bAutoActivate = false;
	bIsBurning = false;

    GrillMusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GrillMusicComponent"));
    GrillMusicComponent->SetupAttachment(RootComponent);

    GrillMusicComponent->bAutoActivate = false;
    GrillMusicComponent->bStopWhenOwnerDestroyed = true;
    GrillMusicComponent->SetUISound(false);

    GrillMusicComponent->bAllowSpatialization = true;
    GrillMusicComponent->bOverrideAttenuation = true;
    GrillMusicComponent->AttenuationOverrides.AttenuationShapeExtents = FVector(200.f, 0.f, 0.f);
    GrillMusicComponent->AttenuationOverrides.FalloffDistance = 1000.f;
}

void AGrill::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AGrill::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AGrill::SetFireEnabled(bool bEnabled)
{
    bIsBurning = bEnabled;

    if (FireEffect)
    {
        bIsBurning ? FireEffect->Activate() : FireEffect->Deactivate();
    }

    if (GrillMusicComponent)
    {
        if (bIsBurning)
        {

            if (GrillAttenuationAsset)
            {
                GrillMusicComponent->bOverrideAttenuation = false;
                GrillMusicComponent->SetAttenuationSettings(GrillAttenuationAsset);
            }
            else
            {
                GrillMusicComponent->bOverrideAttenuation = true;
            }
            GrillMusicComponent->Stop();
            GrillMusicComponent->SetVolumeMultiplier(0.1f);
            GrillMusicComponent->Play();

            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
                {
                    if (GrillMusicComponent)
                    {
                        GrillMusicComponent->SetVolumeMultiplier(1.0f);
                    }
                }, 3.0f, false);
        }
        else
        {
            GrillMusicComponent->FadeOut(1.0f, 0.0f);
        }
    }

    OnFireStateChanged.Broadcast(bIsBurning);

}
