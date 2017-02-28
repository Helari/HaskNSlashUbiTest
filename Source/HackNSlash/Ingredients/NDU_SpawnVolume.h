// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NDU_SwordBase.h"
#include "NDU_SpawnVolume.generated.h"

UCLASS()
class HACKNSLASH_API ANDU_SpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANDU_SpawnVolume();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const { return WhereToSpawn; }

	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetSpawningVolumeActive(bool bShouldSpawn);

	//Could just use a UClass pointer here but using TSubClassOf -> enforce that only APickup or child classes can be used
	UPROPERTY(EditAnywhere, Category = "Spawning")
		class ANDU_SwordBase* LinkedSwordBase;

	FORCEINLINE	class ANDU_SwordBase* GetLinkedSwordBase() const { return LinkedSwordBase; }

protected:
	//Could just use a UClass pointer here but using TSubClassOf -> enforce that only APickup or child classes can be used
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ANDU_Pickup> WhatToSpawn;

	FTimerHandle SpawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayRangeLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayRangeHigh;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WhereToSpawn;

	void SpawnPickup();
	
	float SpawnDelay;
};
