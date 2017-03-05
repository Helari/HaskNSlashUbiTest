// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ingredients/NDU_Pickup.h"
#include "NDU_SwordPickup.generated.h"

/**
 * 
 */
UCLASS()
class HACKNSLASH_API ANDU_SwordPickup : public ANDU_Pickup
{
	GENERATED_BODY()

public:
	ANDU_SwordPickup();

	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Sword")
		float GetInitialLifeSpan();
	UFUNCTION(BlueprintPure, Category = "Sword")
		float GetCurrentLifeSpan();
	UFUNCTION(BlueprintCallable, Category = "Sword")
		void UpdateLifeSpan(float LifeSpanAmount);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword", Meta = (BlueprintProtected = "true"))
		float LifeSpan;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword", Meta = (BlueprintProtected = "true"))
		float CurrentLifeSpan;
	UFUNCTION(BlueprintImplementableEvent, Category = "Power")
		void LifeSpanChangeEffect();

	FORCEINLINE	class UBoxComponent* GetActivationTrigger() const { return ActivationTrigger; }
	UFUNCTION(BlueprintCallable, Category = "TriggerOverlap")
		void OnActivationTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "TriggerOverlap")
		void OnActivationTriggerBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void WasCollected(AHackNSlashCharacter* const _HolderCharacter);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword Base", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* ActivationTrigger;

	float DecayRate;
	AHackNSlashCharacter* HolderCharacter;
};
