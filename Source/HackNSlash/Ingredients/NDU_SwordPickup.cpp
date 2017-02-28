// Fill out your copyright notice in the Description page of Project Settings.

#include "HackNSlash.h"
#include "../HackNSlashCharacter.h"
#include "NDU_SwordPickup.h"


// Sets default values
ANDU_SwordPickup::ANDU_SwordPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ActivationTrigger
	ActivationTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationTrigger"));
	ActivationTrigger->SetupAttachment(RootComponent);
	//ActivationTriggerBox->SetSphereRadius(200.0f);

	ActivationTrigger->OnComponentBeginOverlap.AddDynamic(this, &ANDU_SwordPickup::OnActivationTriggerBoxOverlapBegin);
	ActivationTrigger->OnComponentEndOverlap.AddDynamic(this, &ANDU_SwordPickup::OnActivationTriggerBoxOverlapEnd);
}

void ANDU_SwordPickup::OnActivationTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		AHackNSlashCharacter* character = Cast<AHackNSlashCharacter>(OtherActor);
		if (character)
		{
			character->CurrentSword = this;
			character->SetCanCollectSword(true);
		}
	}
}
void ANDU_SwordPickup::OnActivationTriggerBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{ 
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		AHackNSlashCharacter* const character = Cast<AHackNSlashCharacter>(OtherActor);
		if (character)
		{
			character->SetCanCollectSword(false);
		}
	}
}