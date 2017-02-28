// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "HackNSlash.h"
#include "HackNSlashGameMode.h"
#include "HackNSlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Ingredients/NDU_SpawnVolume.h"

AHackNSlashGameMode::AHackNSlashGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	DecayRate = 0.01f;
}

void AHackNSlashGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANDU_SpawnVolume::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		ANDU_SpawnVolume* SpawnVolumeActor = Cast<ANDU_SpawnVolume>(Actor);
		if (SpawnVolumeActor)
		{
			SpawnVolumeActors.AddUnique(SpawnVolumeActor);
		}
	}

	SetCurrentState(EHackNSlashPlayState::EPlaying);

	AHackNSlashCharacter* myCharacter = Cast<AHackNSlashCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (myCharacter)
	{
		PowerToWin = (myCharacter->GetInitialPower()) * 1.25f;
	}

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AHackNSlashGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AHackNSlashCharacter* myCharacter = Cast<AHackNSlashCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (myCharacter)
	{
		if (myCharacter->GetCurrentPower() > PowerToWin)
		{
			SetCurrentState(EHackNSlashPlayState::EVictory);
		}
		else if (myCharacter->GetCurrentPower() > 0)
		{
			myCharacter->UpdatePower(-DeltaTime*DecayRate*(myCharacter->GetInitialPower()));
		}
		else
		{
			SetCurrentState(EHackNSlashPlayState::EGameOver);
		}
	}
}

float AHackNSlashGameMode::GetPowerToWin() const
{
	return PowerToWin;
}

EHackNSlashPlayState AHackNSlashGameMode::GetCurrentState() const
{
	return CurrentState;
}

void AHackNSlashGameMode::SetCurrentState(EHackNSlashPlayState newState)
{
	CurrentState = newState;
	HandleNewState(CurrentState);
}

void AHackNSlashGameMode::HandleNewState(EHackNSlashPlayState newState)
{
	switch (newState)
	{
		case EHackNSlashPlayState::EPlaying:
			for (ANDU_SpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningVolumeActive(true);
			}
			break;
		case EHackNSlashPlayState::EVictory:
			for (ANDU_SpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningVolumeActive(false);
			}
			break;
		case EHackNSlashPlayState::EGameOver:
			for (ANDU_SpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningVolumeActive(false);
			}
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				PlayerController->SetCinematicMode(true, false, false, true, true);
			}
			ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
			if (MyCharacter)
			{
				MyCharacter->GetMesh()->SetSimulatePhysics(true);
				MyCharacter->GetMovementComponent()->MovementState.bCanJump = false;
			}
			break;
		/*default:
		case EHackNSlashPlayState::EUnknown:
			break;*/
	}
}
