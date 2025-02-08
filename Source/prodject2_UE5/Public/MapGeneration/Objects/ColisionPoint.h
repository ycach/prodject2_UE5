#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColisionPoint.generated.h"

UCLASS()
class PRODJECT2_UE5_API AColisionPoint : public AActor {
	GENERATED_BODY()

  public:
	AColisionPoint();
	bool isRoad();
	bool isWater();
	bool isPit();
	FVector GetLoction();
	float GetRadiusColision();

	void SetRoad();
	void SetWater();
	void SetPit();
	void SetLocation(FVector location);

	
	static void SetupRadiusColision(int32 poligonSize);
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void SetupRadiusCollisionWrapper(int32 poligonSize);

  private:
	static float radiusColision;
	bool bRoad;
	bool bWater;
	bool bPit;
	FVector Location;
};
