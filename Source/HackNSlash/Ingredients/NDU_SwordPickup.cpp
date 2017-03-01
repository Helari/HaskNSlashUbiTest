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

	DecayRate = 0.f;
	LifeSpan = 5.0f;
	CurrentLifeSpan = LifeSpan;
}

void ANDU_SwordPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetCurrentLifeSpan() > 0)
	{
		UpdateLifeSpan(-DeltaTime*DecayRate*(GetLifeSpan()));
		LifeSpanChangeEffect();
	}
	else
	{
		HolderCharacter->CurrentSword = NULL;
		Destroy();
	}
}
float ANDU_SwordPickup::GetLifeSpan()
{
	return LifeSpan;
}
float ANDU_SwordPickup::GetCurrentLifeSpan()
{
	return CurrentLifeSpan;
}
void ANDU_SwordPickup::UpdateLifeSpan(float LifeSpanAmount)
{
	CurrentLifeSpan = CurrentLifeSpan + LifeSpanAmount;
}
void ANDU_SwordPickup::OnActivationTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		AHackNSlashCharacter* character = Cast<AHackNSlashCharacter>(OtherActor);
		if (character)
		{
			if (!character->CurrentSword)
			{
				character->CurrentSword = this;
				character->SetCanCollectSword(true);
			}
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

void ANDU_SwordPickup::WasCollected(AHackNSlashCharacter* _HolderCharacter)
{
	HolderCharacter = _HolderCharacter;
	CurrentLifeSpan = LifeSpan;
	DecayRate = 0.1f;
}
