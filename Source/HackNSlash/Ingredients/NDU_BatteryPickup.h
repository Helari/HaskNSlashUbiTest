// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ingredients/NDU_Pickup.h"
#include "NDU_BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class HACKNSLASH_API ANDU_BatteryPickup : public ANDU_Pickup
{
	GENERATED_BODY()

public:
	ANDU_BatteryPickup();
	
	//Override WasCollected_Implementation because it's a blueprint nativenet
	void WasCollected_Implementation() override;

	float GetPower();

protected:
	//Amount of battery it gives to the player
		//We've made this variable protected in code, no C++ class can change it except the ones inheriting from BatteryPickup
		//We need a separate flag to protect it in blueprints (only BatteryPickup and childs can edit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Power", Meta = (BlueprintProtected = "true")) 
	float BatteryPower;
};
