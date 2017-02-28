// Fill out your copyright notice in the Description page of Project Settings.

#include "HackNSlash.h"
#include "NDU_BatteryPickup.h"

ANDU_BatteryPickup::ANDU_BatteryPickup()
{
	GetMesh()->SetSimulatePhysics(true);

	BatteryPower = 150.0f;
}

void ANDU_BatteryPickup::WasCollected_Implementation()
{
	Super::WasCollected_Implementation();

	Destroy();
}

float ANDU_BatteryPickup::GetPower()
{
	return BatteryPower;
}
