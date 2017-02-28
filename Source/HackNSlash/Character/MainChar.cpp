// Fill out your copyright notice in the Description page of Project Settings.

#include "HackNSlash.h"
#include "MainChar.h"


// Sets default values
AMainChar::AMainChar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainChar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainChar::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

