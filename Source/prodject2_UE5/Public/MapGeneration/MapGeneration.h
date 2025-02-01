#pragma once

#include "CoreMinimal.h"
#include "MapGeneration/LandscapeGenerator.h"
#include "MapGeneration.generated.h"

UCLASS()
class PRODJECT2_UE5_API AMapGeneration : public ALandscapeGenerator {
	GENERATED_BODY()
  public:
	AMapGeneration();

	// Обертка для нормализации шансов группы объектов в интервал 0 - 100 по сумме

	

	UFUNCTION(BlueprintCallable, Category = "Generator|Normalize")
	TArray<FChanceGroup> NormalizeChanses(TArray<FChance> arrChance);

	// Функции для поиска координат
	// Оставлены публичными для работы со способностями
	float ZCoordinateFind(FVector point);
	float ZCoordinateFind(FVector point, bool bOy_true);

	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;

  protected:
  private:
	TArray<FMapObject> SpawnedObjectsOnMap; // array of objects on map

	// поворот объекта по ландшафту
	// Не реализована
	FRotator FindRotationWithMesh(FVector location);
	// Определение объекта из группы для спавна
	int8 RandomObjectIndex(const TArray<FMapObject> &Objects);
	void SpawnObject(TArray<FMapObject> &arrayObjects);

#pragma region Borders
  public:
	// не используется, нужно сделать заполнение внутренней части границы
	UPROPERTY(EditAnywhere,
			  Category = "Generator|Borders",
			  meta = (ClampMin = "1",
					  ClampMax = "16",
					  ToolTip = "The number of threads for generating one boundary, the total number of threads: 4 * TaskCountBorder."))
	int16 TaskCountBorder;
	UPROPERTY(EditAnywhere,
			  Category = "Generator|Borders",
			  meta = (ClampMin = "1", ClampMax = "1000", ToolTip = "Object density on the borders"))
	int16 DensityBorders;

	UPROPERTY(EditAnywhere,
			  Category = "Generator|Borders",
			  meta = (ClampMin = "1", ToolTip = "Constant distance between objects along the boundary axis."))
	int16 DistanceBetween;
	// переделать под параметры вида: глубина пиков, частота пиков, величина отклонения вариативности
	/// k1 - parameter before function
	UPROPERTY(EditAnywhere, Category = "Generator|Borders|Hills") int16 ka_min;
	UPROPERTY(EditAnywhere, Category = "Generator|Borders|Hills") int16 ka_max;

	/// k2- parameter before argument
	UPROPERTY(EditAnywhere, Category = "Generator|Borders|Hills") int16 kf_min;
	UPROPERTY(EditAnywhere, Category = "Generator|Borders|Hills") int16 kf_max;
	// #end last com
	UFUNCTION(BlueprintCallable, Category = "Generator|Hills")
	void GenerateObjectsMap_Hills(const TArray<FMapObject> &Objects_Borders,
								  const TArray<FMapObject> &Objects_Map,
								  float densities,
								  int deltaQualityObjects);

  private:
	// для функции границ (холмы)
	int16 k1_f;
	int16 k2_f;
	int16 k1_a;
	int16 k2_a;

	/// for max min borders coordinate incide map for spawn colision box
	// координаты сторон квадрата полностью лежащего внутри границ формата x = k, y = k
	float minX_inside;
	float maxX_inside;
	float minY_inside;
	float maxY_inside;

	TArray<TArray<FMapObject>> arrBorders; // array of arrays of borders objects

	float FunctionBorders_Hills(float x);
	TArray<TArray<FVector>> NodesYPoints(float (AMapGeneration::*Function)(float x));
	void AnalyzeBorderPoints(
		const TArray<TArray<FVector>> &BorderPoints, float &MaxY, float &MinY, float &MaxX, float &MinX);
	void FindBordersObjectCoordinate(const FVector &Start,
									 const FVector &End,
									 const TArray<FMapObject> &Objects,
									 TArray<FMapObject> &returnObjects);
	void BordersObjectsCreate(const TArray<FMapObject> &Objects, float (AMapGeneration::*Function)(float x));
#pragma endregion

#pragma region Debug Functions
	UFUNCTION(BlueprintCallable, Category = "Generator|DEBUG")
	void Debug_BordersSpawn(bool bAB_spawn, bool bAC_spawn, bool bBD_spawn, bool bCD_spawn);
#pragma endregion
};
