// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"	
#include "NDU_SpawnVolume.h"
#include "NDU_SwordBase.generated.h"

UCLASS()
class HACKNSLASH_API ANDU_SwordBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANDU_SwordBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintPure, Category = "Sword Base")
		float GetInitialPower();
	UFUNCTION(BlueprintPure, Category = "Sword Base")
		float GetCurrentPower();
	UFUNCTION(BlueprintCallable, Category = "Sword Base")
		void UpdatePower(float PowerAmount);
	UFUNCTION(BlueprintCallable, Category = "Sword Base")
		void AddLinkedSpawnVolume(class ANDU_SpawnVolume* volume);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword Base", Meta = (BlueprintProtected = "true"))
		float InitialPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword Base", Meta = (BlueprintProtected = "true"))
		float MaxPower;
	//Return the mesh for the pickup -> Can't be blueprint callable, use UFUNCTION if needed instead of FORCEINLINE
	FORCEINLINE	class UStaticMeshComponent* GetMesh() const { return BaseMesh; }
	FORCEINLINE	class UBoxComponent* GetActivationTrigger() const { return ActivationTrigger; }
	UFUNCTION(BlueprintPure, Category = "Sword Base")
		bool IsActive();
	UFUNCTION(BlueprintCallable, Category = "Sword Base")
		void SetActive(bool bNewSwordBaseState);

	//BoxComponent ActivationTrigger
	//Shutdown PositionIndicator -> Public blueprint event
	UFUNCTION(BlueprintImplementableEvent, Category = "TriggerOverlap")
		void ActivationTriggerBoxOverlapBegin();
	UFUNCTION(BlueprintImplementableEvent, Category = "TriggerOverlap")
		void ActivationTriggerBoxOverlapEnd();
	UFUNCTION(BlueprintImplementableEvent, Category = "Sword Base")
		void ShutdownPositionIndicator();
	UFUNCTION(BlueprintImplementableEvent, Category = "Sword Base")
		void UpdateCompletionPercentage();
	UFUNCTION(BlueprintCallable, Category = "TriggerOverlap")
		void OnActivationTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "TriggerOverlap")
		void OnActivationTriggerBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Sword Base")
	void AddPower(float PowerAmount);

private:
	bool bIsActive;
	float PowerUpdateDelay;
	float PowerUpdateDelta;
	float PowerRateToAdd;
	bool ShouldUpdatePower;
	UPROPERTY(VisibleAnywhere, Category = "Sword Base")
		float CurrentPower;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword Base", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* BaseMesh; //class = forward declaration - pointer to the static mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword Base", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* ActivationTrigger;
	UPROPERTY(EditAnywhere, Category = "Sword Base")
		TArray<class ANDU_SpawnVolume*> LinkedSpawnVolumes;

};
