// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Misc/Optional.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBoids.generated.h"

UCLASS()
class BOIDSSYSTEM_API ABaseBoids : public AActor
{
	GENERATED_BODY()

private:

	FVector Velocity;
	FVector TraceAvoidanceForce;

	void GetNeighbors(TArray<ABaseBoids*>& OutNeighbors);

	FVector CalculateSeparation();
	FVector CalculatePlayerInteraction();
	FVector GetPlayerLocation();
	FVector LimitVector(FVector Vector, float MaxMagnitude);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Boids Settings")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, Category = "Boids Settings")
	UStaticMeshComponent* BoidMesh;

	UPROPERTY(EditAnywhere, Category = "Boids Settings")
	USphereComponent* CollisionSphere;

	// Boids behavior properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids Settings")
	float PerceptionRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids Settings")
	bool bDrawDebugLine = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids Settings")
	float MaxSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids Settings")
	float MaxForce = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids Settings")
	float AvoidanceRadius = 150.0f;

	// Player interaction settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Interaction")
	bool bFollowPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Interaction")
	float PlayerAttractionWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Interaction")
	float PlayerOrbitRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Interaction")
	float PlayerMinDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Interaction")
	float PlayerOrbitStrength = 1.2f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ABaseBoids();
};
