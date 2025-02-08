#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimpleObject.generated.h"

#ifndef MAPOBJECTS_STRUCTS_H
struct FSimpleObject; // Forward declaration
#endif

UCLASS()
class PRODJECT2_UE5_API ASimpleObject_class : public AActor {
	GENERATED_BODY()

  public:
	ASimpleObject_class();
	UPROPERTY(EditDefaultsOnly,
			  Category = "Object|Spawn parameters",
			  meta = (ClampMin = "0", ClampMax = "10000", ToolTip = "Radius in which other objects cannot appear."))
	float RadiusCollision;
	UPROPERTY(EditDefaultsOnly,
			  Category = "Object|Spawn parameters",
			  meta = (ToolTip = "Rotates together with the mesh."))
	bool RotatesWithMesh;
	UPROPERTY(EditDefaultsOnly,
			  Category = "Object|Spawn parameters|Interaction",
			  meta = (ToolTip = "Can appear inside another object."))
	bool InteractCollision;

	UPROPERTY(EditDefaultsOnly,
			  Category = "Object|Spawn parameters|Interaction",
			  meta = (ToolTip = "Can interact with water."))
	bool InteractWater;

	UPROPERTY(EditDefaultsOnly,
			  Category = "Object|Spawn parameters|Interaction",
			  meta = (ToolTip = "Can interact with roads."))
	bool InteractRoads;

	UPROPERTY(EditDefaultsOnly,
			  Category = "Object|Spawn parameters|Interaction",
			  meta = (ToolTip = "Can interact with pits."))
	bool InteractPits;

	

	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
	

  protected:
	virtual void BeginPlay() override;
};
