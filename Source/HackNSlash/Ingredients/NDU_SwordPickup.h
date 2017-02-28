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
	FORCEINLINE	class UBoxComponent* GetActivationTrigger() const { return ActivationTrigger; }
	UFUNCTION(BlueprintCallable, Category = "TriggerOverlap")
		void OnActivationTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "TriggerOverlap")
		void OnActivationTriggerBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword Base", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* ActivationTrigger;
};
