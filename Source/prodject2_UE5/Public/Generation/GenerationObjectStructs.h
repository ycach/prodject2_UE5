#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenerationObjectStructs.generated.h"

class AHardObjectSpawn;

#pragma region Structs
USTRUCT(BlueprintType)
struct FMapObject {
	GENERATED_BODY()

  public:
	// Число от 0 до 255
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structs Objects Generation|Objects")
	float Chance;

	// Объект Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structs Objects Generation|Objects")
	TSubclassOf<AActor> Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structs Objects Generation|Objects")
	int RadiusColision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structs Objects Generation|Objects")
	bool RotationWithMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structs Objects Generation|Objects")
	bool ISHard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structs Objects Generation|Objects")
	TSubclassOf<AHardObjectSpawn> HardActor;

	// Конструктор по умолчанию
	FMapObject() : Chance(0), Actor(nullptr) {
	}

	// Конструктор с параметрами
	FMapObject(const TSubclassOf<AActor> InActor, uint8 InChance) : Chance(InChance), Actor(InActor) {
	}
};

USTRUCT(BlueprintType)
struct FObjectThatHaveLocation {
	GENERATED_BODY()

  public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape|Objects")
	FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape|Objects")
	FRotator Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape|Objects")
	TSubclassOf<AActor> Object;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape|Objects")
	int RadiusColision;

	FActorSpawnParameters SpawnParams;
	// Конструктор по умолчанию
	FObjectThatHaveLocation()
		: Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator), Object(nullptr),
		  RadiusColision(0) {
	}

	// Конструктор с параметрами
	FObjectThatHaveLocation(FVector location, FRotator rotation, TSubclassOf<AActor> actor,
							FActorSpawnParameters spawnParams, int32 radiusColision)
		: Location(location), Rotation(rotation), Object(actor), SpawnParams(spawnParams),
		  RadiusColision(radiusColision) {
	}
};

#pragma endregion