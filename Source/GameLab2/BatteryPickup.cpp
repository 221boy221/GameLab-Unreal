// Fill out your copyright notice in the Description page of Project Settings.

#include "GameLab2.h"
#include "BatteryPickup.h"


ABatteryPickup::ABatteryPickup(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Base power level of the battery
	PowerLevel = 150.f;

}

void ABatteryPickup::OnPickedUp_Implementation()
{
	// Call the parent implementation of OnPickedUp first.
	Super::OnPickedUp_Implementation();
	// Debug log / print
	UE_LOG(LogTemp, Warning, TEXT("Picked up Battery!"));
	// Destroy the battery
	Destroy();
}

