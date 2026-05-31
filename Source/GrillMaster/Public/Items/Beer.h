// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Beer.generated.h"

/**
 * 
 */
UCLASS()
class GRILLMASTER_API ABeer : public AItem
{
	GENERATED_BODY()
	
public:
	void Drink();
};
