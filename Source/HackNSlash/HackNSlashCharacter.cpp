// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "HackNSlash.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "HackNSlashCharacter.h"
#include "Ingredients/NDU_Pickup.h"
#include "Ingredients/NDU_BatteryPickup.h"
#include "Ingredients/NDU_SwordPickup.h"
#include "Ingredients/NDU_SpawnVolume.h"
#include "Ingredients/NDU_SwordBase.h"
#include "HackNSlashGameMode.h"

//////////////////////////////////////////////////////////////////////////
// AHackNSlashCharacter

AHackNSlashCharacter::AHackNSlashCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 4.f;
	CameraBoom->CameraLagMaxDistance = 500.f;

	BaseTurnRate = 100.f;
	BaseLookUpRate = -70.f;
	DashStrengh = 10000;
	SmashStrengh = 10;
	SmashMaxRange = 2500.f;
	CameraSideRotationMax = 60.f;
	BackwardCameraDistance = 800.f;
	
	OnCharacterMovementUpdated.AddDynamic(this, &AHackNSlashCharacter::UpdateMovement);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create the collection sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);
	CollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHackNSlashCharacter::OnCollectionSphereOverlapBegin);
	CollectionSphere->OnComponentEndOverlap.AddDynamic(this, &AHackNSlashCharacter::OnCollectionSphereOverlapEnd);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	InitialPower = 1000.0f;
	CurrentPower = InitialPower;
	SpeedFactor = 0.75f;
	BaseSpeed = 10.0f;

	SmashTarget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SmashTarget"));
	SmashTarget->SetupAttachment(RootComponent);
	SmashTarget->SetVisibility(false);

	JumpGravityTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("JumpGravityTimeline"));

	//Bind the Callbackfuntion for the float return value
	InterpGravityFunction.BindUFunction(this, FName{ TEXT("JumpGravityTimelineFloatReturn") });
	//InterpGravityCallback.BindUFunction(this, FName{ TEXT("JumpGravityTimelineCallback") });

	DashTrailFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DashTrailFX"));
	DashTrailFX->SetupAttachment(RootComponent);
}

void AHackNSlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Add the float curve to the timeline and connect it to your timelines's interpolation function
	JumpGravityTimeline->AddInterpFloat(fJumpGravityCurve, InterpGravityFunction, FName{ TEXT("GravityFloat") });
	JumpGravityTimeline->SetTimelineLength(1.50f);
	//JumpGravityTimeline->SetTimelineFinishedFunc(InterpGravityCallback);
}


//////////////////////////////////////////////////////////////////////////
// Input
void AHackNSlashCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHackNSlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHackNSlashCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHackNSlashCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHackNSlashCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &AHackNSlashCharacter::CollectPickups);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AHackNSlashCharacter::Dash);
	PlayerInputComponent->BindAction("Smash", IE_Pressed, this, &AHackNSlashCharacter::Smash);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AHackNSlashCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AHackNSlashCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AHackNSlashCharacter::OnResetVR);
}

void AHackNSlashCharacter::CollectPickups()
{
	//Get All overlapping actos and store them in an array
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);

	float CollectedPower = 0;

	//Foreach collected actor
	for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
	{
		ANDU_Pickup* const temp = Cast<ANDU_Pickup>(CollectedActors[iCollected]);

		if (temp && !temp->IsPendingKill() && temp->IsActive())
		{
			temp->WasCollected();
			ANDU_BatteryPickup* const tmpBattery = Cast<ANDU_BatteryPickup>(temp);
			if (tmpBattery)
			{
				CollectedPower += tmpBattery->GetPower();
				//When CurrentSwordBase exists
				if (GetCurrentSwordBase())
				{
					//CurrentSwordBase->AddPower(1.0f);//UpdatePower(1.0f);
				}
				/*ANDU_SwordBase* swordBase = Cast<ANDU_SwordBase>(tmpBattery->GetLinkedSwordBase());
				if (swordBase)  
				{
					swordBase->UpdatePower(1.0f);
				}*/
				//CurrentSwordBase->UpdateCompletionPercentage();
				temp->SetActive(false);
			}
			ANDU_SwordPickup* const tmpSword = Cast<ANDU_SwordPickup>(temp);
			if (tmpSword)
			{
				if (GetCanCollectSword())
				{
					FAttachmentTransformRules attachementRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
					CurrentSword->AttachToComponent(GetMesh(), attachementRules, FName("Weapon"));
					CurrentSword->WasCollected(this);
					//CurrentSword->AttachRootComponentTo(GetMesh(), FName("Weapon"), EAttachLocation::SnapToTarget);
					SetCanCollectSword(false);
					CurrentSwordBase->ShutdownPositionIndicator();
				}
				//If CanCollectSword
				// Attach Sword to this->GetMesh, socket name = "Weapon", Location = SnapToTarget, Rotation = SnapToTarget, Scale = KeepWorld
				// CanCollectSword = false;
				// CurrentSwordBase->ShutDownPositionIndicator
			}
		}
	}
	if (CollectedPower > 0)
	{
		UpdatePower(CollectedPower);
	}
}

float AHackNSlashCharacter::GetInitialPower()
{
	return InitialPower;
}

float AHackNSlashCharacter::GetCurrentPower()
{
	return CurrentPower;
}

void AHackNSlashCharacter::UpdatePower(float PowerAmount)
{
	CurrentPower = CurrentPower + PowerAmount;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + SpeedFactor * CurrentPower;
	PowerChangeEffect();
}

bool AHackNSlashCharacter::GetCanCollectSword()
{
	return CanCollectSword;
}

void AHackNSlashCharacter::SetCanCollectSword(bool bCanCollect)
{
	CanCollectSword = bCanCollect;
}

void AHackNSlashCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AHackNSlashCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AHackNSlashCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AHackNSlashCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHackNSlashCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHackNSlashCharacter::MoveForward(float Value)
{
	if ((Controller != NULL))
	{
		if ((Value != 0.0f))
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}

		IsCameraDistantForBackWalk = false;
		CamToMeshDirection = (GetMesh()->GetComponentLocation() - FollowCamera->GetComponentLocation()).GetSafeNormal();
		if (Dot3(GetMesh()->GetRightVector().GetSafeNormal(), CamToMeshDirection) < -0.3f)
		{
			IsCameraDistantForBackWalk = true;
		}
		BackwardCameraDistance = 300.f;
		//Interp SpringArm to 800 (backward value)
		if (IsCameraDistantForBackWalk)
		{
			BackwardCameraDistance = 800.f;
		}
		//Interp SpringArm to 600 (jumping value)
		else if (GetCharacterMovement()->IsFalling())
		{
			BackwardCameraDistance = 600.f;
		}
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, BackwardCameraDistance, DeltaSecondsInternal, 1.5f);
	}
}

void AHackNSlashCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


//void AHackNSlashCharacter::JumpGravityTimelineCallback()
//{
//	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Magenta, FString::Printf(TEXT("END CALLBACK TIMELINE")));
//}
//Your Callback Function for the timeline float value
void AHackNSlashCharacter::JumpGravityTimelineFloatReturn(float val)
{
	//Your float val from curve returns here
	if (GEngine)
	{
	}
	if (IsDashing)
	{
		SmashTarget->SetVisibility(false);
		IsSmashLocationValid = false;
		//JumpGravityTimeline->SetNewTime(0.1f);
		//JumpGravityTimeline->SetPlaybackPosition(0.95f, false);
		GetCharacterMovement()->GravityScale = 1.0f;
		JumpGravityTimeline->Stop();
	}
	else
	{
		SmashTarget->SetVisibility(true);
		GetCharacterMovement()->GravityScale = val;
	}
}

void AHackNSlashCharacter::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
	//const FVector disVec = Hit.ImpactPoint;

	/*if (GEngine) {
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Magenta, FString::Printf(TEXT("LANDED")));
	}*/
	SmashTarget->SetVisibility(false);
	IsDashing = false;
	if (IsSmashRotation)
	{
		IsSmashRotation = false;
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayCameraShake(SmashCameraShake, 2.5f, ECameraAnimPlaySpace::CameraLocal, FRotator::ZeroRotator);

		FVector characterLocation = GetActorLocation();
		if (SmashExplosionFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmashExplosionFX, characterLocation);
		}
	}
}

void AHackNSlashCharacter::UpdateMovement(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	if (GEngine)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			const FName TraceTag("MyTraceTag");
			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(TraceTag, true, this);
			GetWorld()->DebugDrawTraceTag = TraceTag;
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;

			FHitResult RV_Hit(ForceInit);
			FVector cameraForward = FollowCamera->GetForwardVector();
			bool touchedSomething = GetWorld()->LineTraceSingleByChannel(
				RV_Hit,
				GetActorLocation(),
				GetActorLocation() + cameraForward * 2500,
				ECC_WorldStatic,
				RV_TraceParams,
				FCollisionResponseParams());
			FVector2D hitLocation = FVector2D(RV_Hit.Location);

			FVector2D characterLocation = FVector2D(GetActorLocation());
			float distSmashTarget = (hitLocation - characterLocation).Size();
			if (touchedSomething && distSmashTarget < SmashMaxRange)
			{
				SmashLocation = RV_Hit.Location;
				SmashTarget->SetWorldLocation(SmashLocation);
				IsSmashLocationValid = true;
				/*GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Magenta, FString::Printf(TEXT("Some variable values: x: %s"), *RV_Hit.Location.ToString()));*/
			}
			else
			{
				cameraForward.Normalize();
				FVector2D cameraMaxForward = characterLocation + FVector2D(cameraForward * SmashMaxRange);
				SmashLocation = FVector(cameraMaxForward.X, cameraMaxForward.Y, 140);
				SmashTarget->SetWorldLocation(SmashLocation);
				IsSmashLocationValid = true;
			}
		}
		DeltaSecondsInternal = DeltaSeconds;
	}
}

void AHackNSlashCharacter::Jump()
{
	if (!IsSmashRotation && !IsDashing)
	{
		Super::Jump();
		IsSmashLocationValid = false;
		if (SmashTarget)
		{
			SmashTarget->SetVisibility(true, false);
		}

		// Start your Timeline or PlayFromStart() etc, can be called anywhere in this class
		JumpGravityTimeline->PlayFromStart();
	}
	if (GetCharacterMovement()->IsFalling())
	{
		Smash();
	}
}

void AHackNSlashCharacter::Dash()
{
	UWorld* const World = GetWorld();
	if (World)
	{
		bool IsFalling = GetCharacterMovement()->IsFalling();
		if (!IsFalling || (IsFalling && !IsDashing))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Magenta, FString::Printf(TEXT("DASH")));
			IsDashing = true;

			FVector characterLocation = GetActorLocation();
			if (DashExplosionFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, DashExplosionFX, characterLocation);
			}
			if (DashTrailFX)
			{
				DashTrailFX->Activate(true);
			}

			GetWorldTimerManager().SetTimer(DashHideTimer, this, &AHackNSlashCharacter::DashHide, 0.1f, false);
		}
	}
}

void AHackNSlashCharacter::Smash()
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Magenta, FString::Printf(TEXT("SMASH")));
}

void AHackNSlashCharacter::DashHide()
{
	GetWorldTimerManager().ClearTimer(DashHideTimer);
	GetMesh()->SetVisibility(false);
	//if (GetCharacterMovement()->IsFalling())
	//{
	//	//Divide the Impulse depending on the velocity, if velocity = 1000, it's ok and divided by 1, if vel = 2000, the impulse is /2, vel = 3000 -> /3 ...etc. 
	//	//Avoids taking to much speed when spamming dash in jumps
	//	float velocity = GetCharacterMovement()->Velocity.Size();
	//	float velocityFactor = 1.0f;
	//	if (velocity > 1000.f)
	//	{
	//		velocityFactor = GetCharacterMovement()->Velocity.Size()*0.0003;
	//	}
	//	GetCharacterMovement()->AddImpulse(GetActorForwardVector()*DashStrengh*0.1f/velocityFactor, true);
	//	//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Magenta, FString::Printf(TEXT("Some variable values: x: %f"), (GetCharacterMovement()->Velocity.Size()*0.001)));
	//}
	//else
	{
		GetCharacterMovement()->AddImpulse(GetActorForwardVector()*DashStrengh, true);
	}
	GetWorldTimerManager().SetTimer(DashHideTimer, this, &AHackNSlashCharacter::DashShow, 0.2f, false);
}
void AHackNSlashCharacter::DashShow()
{
	GetWorldTimerManager().ClearTimer(DashHideTimer);
	GetMesh()->SetVisibility(true);

	UWorld* const World = GetWorld();
	if (World)
	{
		FVector characterLocation = GetActorLocation();
		if (DashExplosionFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, DashExplosionFX, characterLocation);
		}
	}
	GetWorldTimerManager().SetTimer(DashHideTimer, this, &AHackNSlashCharacter::DeactivateDashTrail, 0.1f, false);
}
void AHackNSlashCharacter::DeactivateDashTrail()
{
	if (DashTrailFX)
	{
		DashTrailFX->Deactivate();
	}
	//if (!GetCharacterMovement()->IsFalling())
	{
		IsDashing = false;
	}
}

void AHackNSlashCharacter::OnCollectionSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		/*GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Red, TEXT("Entered"));
		ANDU_SwordPickup* const sword = Cast<ANDU_SwordPickup>(OtherActor);
		if (sword)
		{
			SetCanCollectSword(true);
			CurrentSword = sword;
		}
		ANDU_SwordBase* const swordBase = Cast<ANDU_SwordBase>(OtherActor);
		if (swordBase)
		{
			GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Yellow, TEXT("SwordBase"));
			CurrentSwordBase = swordBase;
		}*/
		//Cast to NDU_SwordPickup
		//If SwordPickup != null {
		//CanCollectSword = true;
		// CurrentSword = SwordPickup }
		//else Cast to NDU_SwordBase
		//If SwordBase != null
		// CurrentSwordBase = SwordBase
	}
}
void AHackNSlashCharacter::OnCollectionSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		/*ANDU_SwordPickup* const sword = Cast<ANDU_SwordPickup>(OtherActor);
		if (sword)
		{
			SetCanCollectSword(false);
		}*/
		//Cast to NDU_SwordPickup
		//If SwordPickup != null
		//CanCollectSword = false;
	}
}

ANDU_SwordBase * AHackNSlashCharacter::GetCurrentSwordBase() const
{
	return CurrentSwordBase;
}
ANDU_SwordPickup * AHackNSlashCharacter::GetCurrentSword() const
{ 
	return CurrentSword; 
}