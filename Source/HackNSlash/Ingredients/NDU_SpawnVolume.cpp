// Fill out your copyright notice in the Description page of Project Settings.

#include "HackNSlash.h"
#include "NDU_SpawnVolume.h"
#include "Kismet/KismetMathLibrary.h"
#include "NDU_Pickup.h"


// Sets default values
ANDU_SpawnVolume::ANDU_SpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Create the box component to represent the volume
	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
	RootComponent = WhereToSpawn;

	SpawnDelayRangeLow = 1.0f;
	SpawnDelayRangeHigh = 4.5f;
}

// Called when the game starts or when spawned
void ANDU_SpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	if (LinkedSwordBase)
	{
		LinkedSwordBase->AddLinkedSpawnVolume(this);
	}
}

// Called every frame
void ANDU_SpawnVolume::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FVector ANDU_SpawnVolume::GetRandomPointInVolume()
{
	FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
	FVector SpawnExtend = WhereToSpawn->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtend);
}

void ANDU_SpawnVolume::SetSpawningVolumeActive(bool bShouldSpawn)
{
	if (bShouldSpawn)
	{
		SpawnDelay = FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ANDU_SpawnVolume::SpawnPickup, SpawnDelay, false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpawnTimer);
	}
}

void ANDU_SpawnVolume::SpawnPickup()
{
	//If we have set smth to span
	if (WhatToSpawn != NULL)
	{
		//Check for a valid world
		UWorld* const World = GetWorld();
		if (World)
		{
			//Set the spawn params
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			//Get a random location to spawn
			FVector SpawnLocation = GetRandomPointInVolume();

			FRotator SpawnRotation;
			SpawnRotation.Yaw = FMath::FRand() * 360.0f;
			SpawnRotation.Pitch = FMath::FRand() * 360.0f;
			SpawnRotation.Roll = FMath::FRand() * 360.0f;
			
			//Spawn the pickup
			ANDU_Pickup* /*const*/ SpawnedPickup = World->SpawnActor<ANDU_Pickup>(WhatToSpawn, SpawnLocation, SpawnRotation);
			//SpawnedPickup->ParentSpawnVolume = this;
			SpawnedPickup->SetLinkedSwordBase(LinkedSwordBase);
			SpawnDelay = FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ANDU_SpawnVolume::SpawnPickup, SpawnDelay, false);
		}
	}
}

