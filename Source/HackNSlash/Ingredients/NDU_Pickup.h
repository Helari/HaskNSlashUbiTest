// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NDU_Pickup.generated.h"

UCLASS()
class HACKNSLASH_API ANDU_Pickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANDU_Pickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//Return the mesh for the pickup -> Can't be blueprint callable, use UFUNCTION if needed instead of FORCEINLINE
	FORCEINLINE	class UStaticMeshComponent* GetMesh() const { return PickupMesh; }
	//FORCEINLINE	class ANDU_SpawnVolume* GetParentSpawnVolume() const { return ParentSpawnVolume; }
	UFUNCTION(BlueprintCallable, Category = "Pickup-")
		class ANDU_SwordBase* GetLinkedSwordBase() const;

	//Defined in code but can be called from blueprint (can modify thing but shouldn't in general :
	// Run everytime their "output"? is used, so everytime it pops smth on the stack that is called twice (loop or 2 separate nodes) will pop 2 things on the stack, but Fast calculations done frequently are better with this
	//No execution pins
	UFUNCTION(BlueprintPure, Category ="Pickup-")
	bool IsActive();

	//Defined in code but can be found in blueprint : only runs when the INPUT execution is hit, and save the result in the OUTPUT pin, better for expensive calculations that will change frequently
	//Execution pins
	UFUNCTION(BlueprintCallable, Category = "Pickup-")
	void SetActive(bool bNewPickupState);

	//UPROPERTY(EditAnywhere, Category = "Pickup-")
	//	class ANDU_SpawnVolume* ParentSpawnVolume;
	UPROPERTY(EditAnywhere, Category = "Pickup-")
		class ANDU_SwordBase* LinkedSwordBase;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void SetLinkedSwordBase(ANDU_SwordBase* _linkedSwordBase);
	//Some behavior setup in code and some behavior in blueprint (optionnal), override possible in blueprint and make callbacks to the code -> blueprint is called first, to call the parent after right click on the WasCollected in the object blueprint and select "Add call to parent function"
	UFUNCTION(BlueprintNativeEvent)
	void WasCollected(); //Only call it as this one as it can be called as both 
	virtual void WasCollected_Implementation();	//Need an implementation in codeto this function, will be overriden in any child class code

protected:
	//True when the pickup can be used and false when deactivated
	bool bIsActive;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup-", meta=(AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PickupMesh; //class = forward declaration - pointer to the static mesh component
	
};
