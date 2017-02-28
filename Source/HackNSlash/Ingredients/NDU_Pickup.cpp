// Fill out your copyright notice in the Description page of Project Settings.

#include "HackNSlash.h"
#include "NDU_Pickup.h"


// Sets default values
ANDU_Pickup::ANDU_Pickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bIsActive = true;

	//Create the static mesh component
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh-"));
	//Attach the mesh to the root component
	RootComponent = PickupMesh;
}

// Called when the game starts or when spawned
void ANDU_Pickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANDU_Pickup::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

ANDU_SwordBase * ANDU_Pickup::GetLinkedSwordBase() const
{
	return LinkedSwordBase;
}

bool ANDU_Pickup::IsActive()
{
	return bIsActive;
}

void ANDU_Pickup::SetActive(bool bNewPickupState)
{
	bIsActive = bNewPickupState;
}

void ANDU_Pickup::SetLinkedSwordBase(ANDU_SwordBase * _linkedSwordBase)
{
	LinkedSwordBase = _linkedSwordBase;
}

void ANDU_Pickup::WasCollected_Implementation()
{
	FString PickupDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("You have colleted %s"), *PickupDebugString);
}

