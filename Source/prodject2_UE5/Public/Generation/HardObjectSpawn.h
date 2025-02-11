#pragma once

#include "CoreMinimal.h"
#include "Generation/EazyObjectSpawn.h"
#include "HardObjectSpawn.generated.h"

/**
 *
 */
UCLASS()
class PRODJECT2_UE5_API AHardObjectSpawn : public AEazyObjectSpawn {
	GENERATED_BODY()
  public:
	AHardObjectSpawn();

	UFUNCTION(BlueprintNativeEvent, Category = "HardObject|Generation")
	void HO_GenerationSubObjectsCoordinate();
	virtual void HO_GenerationSubObjectsCoordinate_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "HardObject|Generation")
	void HO_SetRadiusColision();
	virtual void HO_SetRadiusColision_Implementation();

	double GetRadiusColision();

  protected:
	virtual void BeginPlay() override;
	FCriticalSection resultCriticalSection;

  private:
	UPROPERTY(EditAnywhere, Category = "HardObject|Optimization") int8 MaxTry;

	UPROPERTY(EditAnywhere, Category = "HardObject|Generation|Grove") int minRadius;
	UPROPERTY(EditAnywhere, Category = "HardObject|Generation|Grove") int maxRadius;

	UFUNCTION(BlueprintCallable, Category = "HardObject|Generation|Grove")
	void GroveGenerationObject(const TArray<FMapObject> &Objects, float densities, int delta);
	
	UFUNCTION(BlueprintCallable, Category = "HardObject|Generation|Grove")
	void GroveSetRadiusColision(const TArray<FMapObject> &Objects);

	UFUNCTION(BlueprintCallable, Category = "Landscape|DEBUG")
	void Debug_ObjectsSpawn(bool bSpawn);

	TArray<int32> IndexesOfChanksCheckBeforeSpawn(int32 indexMidlChank, FVector location, int radiusColision);

	double HypotenuseFind(const FVector2D &PointA, const FVector2D &PointB);

	double RadiusColisionHO;

	#pragma region Grove
	float ElipsR1;
	float ElipsR2;
#pragma endregion


  public:
	void ObjectsGenerationInZone(
		const TArray<FMapObject> &Objects, int8 delta, TArray<uint16> count_of_objects,
		FVector (AHardObjectSpawn::*FunctionRandomPoint)(const FVector2D par_1, const FVector2D par_2), FVector2D par1,
		FVector2D par2); // массив объектов, процент заполнения карты,
																	 // погрешность количества объектов, массив
	// количества объектов,функция случаенной точки внутри зоны в зависимости от ее геометрии, параметры для функции

	TArray<uint16> FindQualityObjects(const TArray<FMapObject> &Objects, float &densities, float squareZone);

#pragma region RandomPoint
	FVector GetRandomPointInRectengleZone(const FVector2D EllipseCenter, const FVector2D EllipseRadius);

	FVector GetRandomPointInEllipseZone(const FVector2D EllipseCenter, const FVector2D EllipseRadius);
#pragma endregion

#pragma region Square
	float FindSquareRectengleZone(const FVector2D Point_1, const FVector2D Point_2);
	float FindSquareOfRadiusColisionZone(float r);
	float FindSquareOfEllipseZone(FVector2d r);
#pragma endregion



  protected:
	
};
