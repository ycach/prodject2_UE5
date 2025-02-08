#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/SimpleObject.h"
#include "GlobalTypes/GlobalTypes_Structs.h"
#include "MapObjects_Structs.generated.h"

#ifndef SIMPLEOBJECT_H
class ASimpleObject_class; // Forward declaration
#endif

USTRUCT(BlueprintType)
struct FMapObject {
	GENERATED_BODY()
  public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Objects")
	FChanceGroup Chance;
	FMapObject(){}
};

USTRUCT(BlueprintType)
struct FCollisionPoint {
	GENERATED_BODY()
  public:
	FCollisionPoint() {
		bRoad = false;
		bWater = false;
		bPit = false;
		bBorder = false;
	}
	static float radiusColision;
	bool bRoad;
	bool bWater;
	bool bPit;
	bool bBorder;
	FVector Location;
	float FindSuareSquare();
	float FindCircleSquare();
};

#pragma region Terra Objects
USTRUCT()
struct FTerraObject_Base : public FMapObject {
	GENERATED_BODY()
  public:
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Base",
			  meta = (ClampMin = "-10000",
					  ClampMax = "10000",
					  ToolTip = "Object depth."))
	int32 Depth;
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Base",
			  meta = (ClampMin = "0", ClampMax = "32", ToolTip = "Number of vertices in width divided by 2."))
	int32 WidthDepth;

	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Type",
			  meta = (ToolTip = "The depression will be filled with water."))
	bool bWaterObject;

  protected:
	TStaticArray<int32, 8> vectorsSpreading;
};
USTRUCT(BlueprintType)
struct FTerraObject_VectorSpreading {
	GENERATED_BODY()
  public:
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Spreading",
			  meta = (ClampMin = "0", ClampMax = "10", ToolTip = "Factor determining how winding the object is."))
	int32 tortuosityCoefficient;
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Spreading",
			  meta = (ClampMin = "0", ClampMax = "100000", ToolTip = "Maximum object Length."))
	int32 Lenght;
};
USTRUCT(BlueprintType)
struct FTerraObject_PointSpreading {
	GENERATED_BODY()
  public:
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Spreading",
			  meta = (ClampMin = "0", ClampMax = "10", ToolTip = "Number of initial vectors."))
	int32 qulityStartVectors;
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Spreading",
			  meta = (ClampMin = "0", ClampMax = "10", ToolTip = "Maximum length of vectors at vertices."))
	int32 maxLenghtVectors;
};
#pragma endregion

#pragma region Map
// простой объект карты
USTRUCT(BlueprintType)
struct FSimpleObject : public FMapObject{
	GENERATED_BODY()
  public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Objects")
	TSubclassOf<ASimpleObject_class> Object;

	FVector Location;
	FRotator Rotation;
	FActorSpawnParameters SpawnParams;

	FSimpleObject() : Object(nullptr), Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator) {
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		RadiusCollision = 0.0f;
		RotatesWithMesh = false;
		InteractCollision = false;
		InteractWater = false;
		InteractRoads = false;
		InteractPits = false;

	}
	FSimpleObject(const TSubclassOf<AActor> actor, int radiusColision, bool rotateble)
		: Object(actor), Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator) {
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		RadiusCollision = 0.0f;
		RotatesWithMesh = false;
		InteractCollision = false;
		InteractWater = false;
		InteractRoads = false;
		InteractPits = false;
	}
	void SetLocation(FVector location) {
		Location = location;
	}
	void SetRotation(FRotator rotator) {
		Rotation = rotator;
	}
	float FindSquare() const {
		return RadiusCollision * RadiusCollision * 3.2;
	}

	void SetupFSimpleObject();

	//Simple Parametrs
	float RadiusCollision;
	bool RotatesWithMesh;
	bool InteractCollision;
	bool InteractWater;
	bool InteractRoads;
	bool InteractPits;

};

USTRUCT(BlueprintType)
struct FDataMap {
	GENERATED_BODY()
  public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Map")
	TArray<FTerraObject_Base> terraObjects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Map")
	TArray<FSimpleObject> simpleObjects;

	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "Generator|Landscape|Map",
			  meta = (ClampMin = "0", ClampMax = "80", ToolTip = "Maximum depth to which the object is submerged."))
	float Density;

	// другие параметры генерации объектов в мире
};
USTRUCT(BlueprintType)
struct FDataMap_ExtraData {
	GENERATED_BODY()
  public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator|Landscape|Map")
	FDataMap data;
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "Generator|Landscape|Map",
			  meta = (ToolTip = "Maximum depth to which the object is submerged."))
	bool VectorObject;
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Spreading",
			  meta = (ClampMin = "0", ClampMax = "10", ToolTip = "Maximum depth to which the object is submerged."))
	int32 tortuosityCoefficient;
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Spreading",
			  meta = (ClampMin = "0", ClampMax = "100000", ToolTip = "Maximum depth to which the object is submerged."))
	int32 Lenght;
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Spreading",
			  meta = (ClampMin = "0", ClampMax = "10", ToolTip = "Maximum depth to which the object is submerged."))
	int32 qulityStartVectors;
	UPROPERTY(EditAnywhere,
			  BlueprintReadWrite,
			  Category = "TerraObject|Spreading",
			  meta = (ClampMin = "0", ClampMax = "10", ToolTip = "Maximum depth to which the object is submerged."))
	int32 maxLenghtVectors;
};
#pragma endregion
