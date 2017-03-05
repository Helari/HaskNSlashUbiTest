// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "HackNSlashCharacter.generated.h"

UCLASS(config=Game)
class AHackNSlashCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Collection sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* CollectionSphere;
public:
	AHackNSlashCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	//Called when we press a key to collect pickups inside the CollectionSphere
	UFUNCTION(BlueprintCallable, Category = "Pickups")
	void CollectPickups();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float InitialPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float SpeedFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float BaseSpeed;

	UFUNCTION(BlueprintImplementableEvent, Category = "Power")
	void PowerChangeEffect();

	//True when the player is near a sword and hasn't one in his hands yet
	bool CanCollectSword;

private:
	UPROPERTY(VisibleAnywhere, Category = "Power")
	float CurrentPower;

public:
	UFUNCTION(BlueprintPure, Category = "Power")
		float GetInitialPower();
	UFUNCTION(BlueprintPure, Category = "Power")
		float GetCurrentPower();
	UFUNCTION(BlueprintCallable, Category = "OnPickup")
		void OnCollectionSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category = "OnPickup")
		void OnCollectionSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/**
	Function to update the character's power
	* @param PowerAmount This is the amount to change the power by, and it can be positive or negative
	*/
	UFUNCTION(BlueprintCallable, Category = "Power") //It is actually an effect on the character's power so we want it to be directly executed 
		void UpdatePower(float PowerAmount);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CollectionSphere subobject **/
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }
	
	UFUNCTION(BlueprintPure, Category = "Sword")
		bool GetCanCollectSword();
	UFUNCTION(BlueprintCallable, Category = "Sword")
		void SetCanCollectSword(bool bCanCollect);

	UPROPERTY(EditAnywhere, Category = "Sword")
		class ANDU_SwordBase* CurrentSwordBase;

	UFUNCTION(BlueprintCallable, Category = "Sword")
		class ANDU_SwordBase* GetCurrentSwordBase() const;

	UPROPERTY(EditAnywhere, Category = "Sword")
		class ANDU_SwordPickup* CurrentSword;

	UFUNCTION(BlueprintCallable, Category = "Sword")
		class ANDU_SwordPickup* GetCurrentSword() const;

//RobotControlelr integration
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		bool IsPunching;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		bool IsSmashLocationValid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		bool IsSmashRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		bool IsCameraRotationForced;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		bool IsDashing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		bool IsCameraDistantForBackWalk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		int32 DashStrengh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		int32 SmashStrengh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		float SmashMaxRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		float DeltaSecondsInternal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		float SmashYawRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		float HorizontalDirectionFromCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		float CameraSideRotationMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		float BackwardCameraDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		FVector SmashLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom character controller", Meta = (BlueprintProtected = "true"))
		FVector CamToMeshDirection;

	UFUNCTION(BlueprintCallable, Category = "Custom character controller")
		void UpdateMovement(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);
};

