// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBoids.h"

// Sets default values
ABaseBoids::ABaseBoids()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	BoidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoidMesh"));
	BoidMesh->SetupAttachment(RootComponent);
	BoidMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->SetSphereRadius(50.0f);

	// Initialize with random velocity
	Velocity = FVector(
		FMath::RandRange(-1.0f, 1.0f),
		FMath::RandRange(-1.0f, 1.0f),
		FMath::RandRange(-1.0f, 1.0f)
	).GetSafeNormal() * MaxSpeed * 0.5f;

	TraceAvoidanceForce = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void ABaseBoids::BeginPlay()
{
	Super::BeginPlay();
}

FVector ABaseBoids::GetPlayerLocation()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn)
	{
		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		return PlayerLocation;
	}

	return FVector::ZeroVector;
}

void ABaseBoids::GetNeighbors(TArray<ABaseBoids*>& OutNeighbors)
{
	OutNeighbors.Empty();

	for (TActorIterator<ABaseBoids> It(GetWorld()); It; ++It)
	{
		ABaseBoids* OtherBoid = *It;
		if (OtherBoid && OtherBoid != this)
		{
			float Distance = FVector::Dist(GetActorLocation(), OtherBoid->GetActorLocation());
			if (Distance < PerceptionRadius)
				OutNeighbors.Add(OtherBoid);
		}
	}
}

FVector ABaseBoids::CalculatePlayerInteraction()
{
	if (!bFollowPlayer)
		return FVector::ZeroVector;

	FVector PlayerLocation = GetPlayerLocation();
	if (PlayerLocation.IsZero())
		return FVector::ZeroVector;

	FVector CurrentLocation = GetActorLocation();
	FVector ToPlayer = PlayerLocation - CurrentLocation;
	float DistanceToPlayer = ToPlayer.Size();

	FVector SteeringForce = FVector::ZeroVector;

	if (DistanceToPlayer < PlayerMinDistance)
	{
		FVector AwayFromPlayer = -ToPlayer.GetSafeNormal();
		SteeringForce = AwayFromPlayer * MaxSpeed;
		SteeringForce -= Velocity;
		return LimitVector(SteeringForce, MaxForce);
	}
	else if (DistanceToPlayer > PlayerMinDistance)
	{
		//FVector ToPlayerNormalized = ToPlayer.GetSafeNormal();

		//FVector Tangent = FVector::CrossProduct(ToPlayerNormalized, FVector::UpVector);
		//if (Tangent.IsNearlyZero())
		//	Tangent = FVector::ForwardVector;

		//Tangent.Normalize();

		//FVector DesiredDirection = (ToPlayerNormalized + Tangent).GetSafeNormal();

		//FVector DesiredVelocity = DesiredDirection * MaxSpeed;
		//SteeringForce = DesiredVelocity - Velocity;
		//return LimitVector(SteeringForce, MaxForce);

		FVector ToPlayerNormalized = ToPlayer.GetSafeNormal();

		FVector Tangent = FVector::CrossProduct(ToPlayerNormalized, FVector::UpVector);
		if (Tangent.IsNearlyZero())
			Tangent = FVector::CrossProduct(ToPlayerNormalized, FVector::RightVector);

		Tangent.Normalize();

		float DistanceFactor = FMath::Clamp((DistanceToPlayer - PlayerOrbitRadius) / PlayerOrbitRadius, -1.0f, 1.0f);

		FVector AttractionForce = ToPlayerNormalized * DistanceFactor * PlayerAttractionWeight;
		FVector OrbitForce = Tangent * PlayerOrbitStrength;

		float VerticalVariation = FMath::Sin(GetWorld()->GetTimeSeconds() + CurrentLocation.X * 0.001f) * 0.3f; // sine wave
		FVector VerticalForce = FVector::UpVector * VerticalVariation;

		FVector DesiredVelocity = (AttractionForce + OrbitForce + VerticalForce).GetSafeNormal() * MaxSpeed;
		SteeringForce = DesiredVelocity - Velocity;
		return LimitVector(SteeringForce, MaxForce);
	}

	return FVector::ZeroVector;
}

FVector ABaseBoids::LimitVector(FVector Vector, float MaxMagnitude)
{
	if (Vector.SizeSquared() > MaxMagnitude * MaxMagnitude)
	{
		Vector.Normalize();
		Vector *= MaxMagnitude;
	}

	return Vector;
}

FVector ABaseBoids::CalculateSeparation()
{
	//if (Neighbors.Num() == 0)
	//	return FVector::ZeroVector;

	FVector SeparationForce = FVector::ZeroVector;
	FVector CurrentLocation = GetActorLocation();
	int32 Count = 0;

	//for (ABaseBoids* OtherBoid : Neighbors)
	//{
	//	FVector ToOther = CurrentLocation - OtherBoid->GetActorLocation();
	//	float Distance = ToOther.Size();

	//	if (Distance > 0.0f && Distance < AvoidanceRadius)
	//	{
	//		FVector AwayFromOther = ToOther.GetSafeNormal();
	//		float Strength = (1.0f - (Distance / AvoidanceRadius)); // ]0,1] based on distance
	//		SeparationForce += AwayFromOther * Strength;
	//		Count++;
	//	}
	//}

	TArray<FHitResult> HitResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(AvoidanceRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->SweepMultiByChannel(
		HitResults,
		CurrentLocation,
		CurrentLocation,
		FQuat::Identity,
		ECC_WorldStatic,
		SphereShape,
		QueryParams
	);

	// Add separation from obstacles
	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && Hit.GetActor() != this)
		{
			FVector ToObstacle = CurrentLocation - Hit.ImpactPoint;
			float Distance = ToObstacle.Size();

			if (Distance > 0.0f && Distance < AvoidanceRadius)
			{
				FVector AwayFromObstacle = ToObstacle.GetSafeNormal();
				float Strength = (1.0f - (Distance / AvoidanceRadius));

				SeparationForce += AwayFromObstacle * Strength;
				Count++;

				// Debug visualization
				if (bDrawDebugLine)
					DrawDebugLine(GetWorld(), CurrentLocation, Hit.ImpactPoint,
						FColor::Cyan, false, 0.0f, 0, 3.0f);
			}
		}
	}

	if (Count > 0)
	{
		SeparationForce /= Count;
		SeparationForce.Normalize();
		SeparationForce *= MaxSpeed;
		SeparationForce -= Velocity;
		SeparationForce = LimitVector(SeparationForce, MaxForce);
	}

	return SeparationForce;
}

// Called every frame
void ABaseBoids::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TArray<ABaseBoids*> Neighbors;
	//GetNeighbors(Neighbors);

	FVector CurrentLocation = GetActorLocation();
	FVector SeparationForce = CalculateSeparation();
	FVector PlayerInteraction = CalculatePlayerInteraction();
	FVector Acceleration;

	bool bNeedsSeparation = !SeparationForce.IsNearlyZero();

	if (bNeedsSeparation)
	{
		Acceleration = (SeparationForce * 2.0f) + (PlayerInteraction * 0.1f);

		if (bDrawDebugLine)
			DrawDebugLine(GetWorld(), CurrentLocation,
				CurrentLocation + SeparationForce * 3.0f,
				FColor::Yellow, false, 0.0f, 0, 5.0f);
	}
	else
	{
		Acceleration = PlayerInteraction * PlayerAttractionWeight;
	}

	Velocity += Acceleration * DeltaTime;
	Velocity = LimitVector(Velocity, MaxSpeed);

	FVector NewLocation = CurrentLocation + (Velocity * DeltaTime);
	SetActorLocation(NewLocation);

	if (!Velocity.IsNearlyZero())
	{
		FRotator NewRotation = Velocity.Rotation();
		SetActorRotation(NewRotation);
	}
}