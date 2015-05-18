// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class GAMELAB2_API APickup : public AActor
{
	GENERATED_UCLASS_BODY()

public:

	/** True when the pickup is able to be picked up, false if something deactivates the pickup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bIsActive;

	/** Simple collision primitive to use as the root component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	class USphereComponent* BaseCollisionComponent;

	/** StaticMeshComponent to represent the pickup in the level */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	class UStaticMeshComponent* PickupMesh;

	/** Function to call when Pickup is collected */
	UFUNCTION(BlueprintNativeEvent) // if not overwritten by blueprint, use default;
	void OnPickedUp();

	// Sets default values for this actor's properties
	APickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
};
