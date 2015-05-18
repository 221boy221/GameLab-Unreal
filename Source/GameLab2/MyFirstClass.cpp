// Fill out your copyright notice in the Description page of Project Settings.

#include "GameLab2.h"
#include "MyFirstClass.h"


// Sets default values
AMyFirstClass::AMyFirstClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	//TotalDamage = 200;
	//DamagePerSecond = 1.f;

}

// Called when the game starts or when spawned
void AMyFirstClass::BeginPlay()
{
	Super::BeginPlay();
	
}
/*
void AMyFirstClass::PostInitProperties()
{
	Super::PostInitProperties();
	DamagePerSecond = TotalDamage / DamageTimeInSeconds;
}

void AMyFirstClass::CalculateValues()
{
	DamagePerSecond = TotalDamage / DamageTimeInSeconds;
}

#if WITH_EDITOR
void AMyFirstClass::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent){
	CalculateValues();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

// Called every frame
void AMyFirstClass::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}
*/
