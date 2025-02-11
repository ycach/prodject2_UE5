#pragma once

#include "CoreMinimal.h"
#include "Generation/MapGeneration/ProceduralGeneration.h"
#include "GenerationObjectStructs.h"
#include "EazyObjectSpawn.generated.h"

UCLASS()
class PRODJECT2_UE5_API AEazyObjectSpawn : public AProceduralGeneration {
	GENERATED_BODY()
  public:
	AEazyObjectSpawn();

  protected:
	virtual void BeginPlay() override;

	FRandomStream RandomStream;

	void SpawnObject(TArray<FObjectThatHaveLocation> &arrayObjects);
	int8 IndexOfObjectToGeneration(const TArray<FMapObject> &Objects);
	bool IsLocationValid(const FVector &Location, const TArray<FObjectThatHaveLocation> &SpawnedLocations,
						 int32 RadiusColision);
	bool IsLocationValid(const FVector &Location, const TArray<TArray<FObjectThatHaveLocation>> &SpawnedLocations,
						 TArray<int32> &indexesOfSpawnedLocations, 
						 int32 RadiusColision);

	float ZCoordinateFind(FVector point, bool bOy_true); // если точка лежит на прямой базового вертикса (arrVertix)

	float ZCoordinateFind(FVector point); // если точка лежит между вертиксами

	FRotator FindRotationWithMesh(FVector location); //// НЕ СДЕЛАНА
};
