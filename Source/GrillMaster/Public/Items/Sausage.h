// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Sausage.generated.h"

/**
 * 
 */
UCLASS()
class GRILLMASTER_API ASausage : public AItem
{
	GENERATED_BODY()
	
public:
	void Eat();
};
