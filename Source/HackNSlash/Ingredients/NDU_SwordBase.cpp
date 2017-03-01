// Fill out your copyright notice in the Description page of Project Settings.

#include "HackNSlash.h"
#include "NDU_SwordBase.h"
#include "../HackNSlashCharacter.h"
#include "HackNSlashGameMode.h"


// Sets default values
ANDU_SwordBase::ANDU_SwordBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitialPower = 0.0f;

	//Create the static mesh component
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword Base"));
	//Attach the mesh to the root component
	RootComponent = BaseMesh;
	bIsActive = true;

	//ActivationTrigger
	ActivationTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationTrigger"));
	ActivationTrigger->SetupAttachment(RootComponent);
	//ActivationTriggerBox->SetSphereRadius(200.0f);
	
	ActivationTrigger->OnComponentBeginOverlap.AddDynamic(this, &ANDU_SwordBase::OnActivationTriggerBoxOverlapBegin);
	ActivationTrigger->OnComponentEndOverlap.AddDynamic(this, &ANDU_SwordBase::OnActivationTriggerBoxOverlapEnd);
}

// Called when the game starts or when spawned
void ANDU_SwordBase::BeginPlay()
{
	Super::BeginPlay();
	SetActive(true);
	PowerUpdateDelay = 0.f;
	PowerUpdateDelta = 0.0f;
	ShouldUpdatePower = false;
}

// Called every frame
void ANDU_SwordBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (ShouldUpdatePower)
	{
		PowerUpdateDelta += DeltaTime;
		if (PowerUpdateDelta >= PowerUpdateDelay)
		{
			ShouldUpdatePower = false;
			PowerUpdateDelta = 0.0f;
			AddPower(PowerRateToAdd);
		}
	}
}

float ANDU_SwordBase::GetInitialPower()
{
	return InitialPower;
}

float ANDU_SwordBase::GetCurrentPower()
{
	return CurrentPower;
}

void ANDU_SwordBase::UpdatePower(float PowerAmount)
{
	ShouldUpdatePower = true;
	PowerRateToAdd = PowerAmount;
}
void ANDU_SwordBase::AddPower(float PowerAmount)
{
	CurrentPower = CurrentPower + PowerAmount;

	//PowerChangeEffect();
	if (GetCurrentPower() > MaxPower)
	{
		AHackNSlashGameMode* MyGameMode = GetWorld()->GetAuthGameMode<AHackNSlashGameMode>();
		int linkedVolumesSize = LinkedSpawnVolumes.Num();
		for (int i = 0; i < linkedVolumesSize; i++)
		{
			LinkedSpawnVolumes[i]->SetSpawningVolumeActive(false);
		}
		//MyGameMode->SetCurrentState(EHackNSlashPlayState::EVictory);
		SetActive(false);
		//Destroy();
	}
}

void ANDU_SwordBase::AddLinkedSpawnVolume(ANDU_SpawnVolume * volume)
{
	LinkedSpawnVolumes.Add(volume);
}

bool ANDU_SwordBase::IsActive()
{
	return bIsActive;
}

void ANDU_SwordBase::SetActive(bool bNewSwordBaseState)
{
	bIsActive = bNewSwordBaseState;
}

void ANDU_SwordBase::OnActivationTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		AHackNSlashCharacter* character = Cast<AHackNSlashCharacter>(OtherActor);
		if (character)
		{
			if (character->CurrentSwordBase)
			{
				character->CurrentSwordBase->ActivationTriggerBoxOverlapEnd();
			}
			ActivationTriggerBoxOverlapBegin();
			character->CurrentSwordBase = this;
		}
		//Cast to NDU_HackNSlashCharacter
		//If Character != null
		//Should have a widget of the class PositionIndicator but I'm lazy so it will be a blueprint event
		//If !widget->SwordTaken
		//widget->SetActive(true);
	}
}
void ANDU_SwordBase::OnActivationTriggerBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		AHackNSlashCharacter* const character = Cast<AHackNSlashCharacter>(OtherActor);
		if (character)
		{
			ActivationTriggerBoxOverlapEnd();
		}
		//Cast to NDU_HackNSlashCharacter
		//If Character != null
		//Should have a widget of the class PositionIndicator but I'm lazy so it will be a blueprint event
		//widget->SetActive(false);
	}
}