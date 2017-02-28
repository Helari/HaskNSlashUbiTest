// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "HackNSlashGameMode.generated.h"

//Current state of Gameplay
UENUM(BlueprintType)
enum class EHackNSlashPlayState
{
	EPlaying,
	EGameOver,
	EVictory,
	EUnknown
};

UCLASS(minimalapi)
class AHackNSlashGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHackNSlashGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWin() const;

	UFUNCTION(BlueprintPure, Category = "Power")
	EHackNSlashPlayState GetCurrentState() const;

	// Can be accessible in blueprint by adding UFUNCTION macro here
	void SetCurrentState(EHackNSlashPlayState newState);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true")) //edit only defaults class values like for a constructor as it's not mmeant to change at runtime ?
	float DecayRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float PowerToWin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass; //The widget class to use for our HUD screen

	UPROPERTY()
	class UUserWidget* CurrentWidget; //The specific object spawned and created - instance
private:
	EHackNSlashPlayState CurrentState;

	TArray<class ANDU_SpawnVolume*> SpawnVolumeActors;

	void HandleNewState(EHackNSlashPlayState newState);
};
