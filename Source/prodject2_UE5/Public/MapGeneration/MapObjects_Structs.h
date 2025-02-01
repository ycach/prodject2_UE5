#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalTypes/GlobalTypes_Structs.h"
#include "MapObjects_Structs.generated.h"

USTRUCT(BlueprintType)
struct FMapObject {
	GENERATED_BODY()
  public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Objects")
	FChanceGroup Chance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Objects")
	TSubclassOf<AActor> Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Objects")
	int RadiusColision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Objects")
	bool RotationWithMesh;

	FVector Location;
	FRotator Rotation;
	FActorSpawnParameters SpawnParams;
	
	FMapObject()
		: Actor(nullptr), RadiusColision(0), RotationWithMesh(0), Location(FVector::ZeroVector),
		  Rotation(FRotator::ZeroRotator) {
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	}
	FMapObject(const TSubclassOf<AActor> actor, int radiusColision, bool rotateble)
		: Actor(actor), RadiusColision(radiusColision), RotationWithMesh(rotateble), Location(FVector::ZeroVector),
		  Rotation(FRotator::ZeroRotator) {
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	}
	void SetLocation(FVector location) {
		Location = location;
	}
	void SetRotation(FRotator rotator) {
		Rotation = rotator;
	}
};
