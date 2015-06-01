// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "GameLab2.h"
#include "GameLab2Pawn.h"
#include "BatteryPickup.h"

AGameLab2Pawn::AGameLab2Pawn(const FObjectInitializer & ObjectInitializer) 
	: Super(ObjectInitializer)
{
	// Set a base power level for the character
	PowerLevel = 1000.f;
	// Set the dependence of speed on the powerlevel
	SpeedFactor = 0.4f;
	BaseSpeed = 10.f;

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	RootComponent = PlaneMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Create our battery collection value
	CollectionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(200.f);
	CollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGameLab2Pawn::CollectBatteries);

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 300.f;
	CurrentForwardSpeed = 300.f;
}

void AGameLab2Pawn::Tick(float DeltaSeconds)
{
	SpeedFactor = .25f * (PowerLevel / 100);
	const FVector LocalMove = FVector((CurrentForwardSpeed * SpeedFactor) * DeltaSeconds, 0.f, 0.f);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);

	if (PowerLevel > 0) {
		PowerLevel -= DeltaSeconds * 20;
	}
	else {
		PowerLevel = 0;
	}
	UE_LOG(LogTemp, Warning, TEXT("PowerLevel: %f"), this->PowerLevel);
}

void AGameLab2Pawn::ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Set velocity to zero upon collision
	CurrentForwardSpeed = 0.f;
}


void AGameLab2Pawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Bind our control axis' to callback functions
	InputComponent->BindAxis("MoveUp", this, &AGameLab2Pawn::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &AGameLab2Pawn::MoveRightInput);
	InputComponent->BindAxis("Brake", this, &AGameLab2Pawn::ThrustInput);
}

void AGameLab2Pawn::BrakeInput(float Val)
{
	//CurrentForwardSpeed = 0.f;
}

void AGameLab2Pawn::ThrustInput(float Val)
{
	Val *= 100.f;
	UE_LOG(LogTemp, Warning, TEXT("BrakeInput Val: %f"), Val);

	// Is there no input?
	//bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	bool bHasInput = (Val != NULL);
	UE_LOG(LogTemp, Warning, TEXT("bHasInput: %s"), bHasInput ? TEXT("true") : TEXT("false"));

	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	UE_LOG(LogTemp, Warning, TEXT("CurrentAcc: %f"), CurrentAcc);

	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	UE_LOG(LogTemp, Warning, TEXT("NewForwardSpeed: %f"), NewForwardSpeed);

	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
	UE_LOG(LogTemp, Warning, TEXT("CurrentForwardSpeed: %f"), CurrentForwardSpeed);
}

void AGameLab2Pawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AGameLab2Pawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AGameLab2Pawn::CollectBatteries(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	float BatteryPower = 0.f;

	// Get all overlapping actors and store them in a CollectedActors array
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);
	
	//for each Actor collected
	for (int32 i = 0; i < CollectedActors.Num(); ++i)
	{
		// Cast the collected Actor to ABatteryPickup
		ABatteryPickup* const TestBattery = Cast<ABatteryPickup>(CollectedActors[i]);
		// if the cast is successful, and the battery is valid and active
		if (TestBattery && !TestBattery->IsPendingKill() && TestBattery->bIsActive)
		{
			// Store its battery power for adding to the character's power
			BatteryPower = BatteryPower + TestBattery->PowerLevel;
			// Call the battery's OnPickedUp function
			TestBattery->OnPickedUp();
			// Deactivate the battery
			TestBattery->bIsActive = false;
		}
	}

	if (BatteryPower > 0.f)
	{
		// Call the Blueprinted implementation of PowerUp with the total battery power as input
		PowerUp(BatteryPower);
		UE_LOG(LogTemp, Warning, TEXT("BatteryPower > 0, putting it into the player's powerlvl"));
	}
}

void AGameLab2Pawn::PowerUp(float power)
{
	this->PowerLevel += power;
}