#pragma once

#include "CoreMinimal.h"
#include "MapGeneration/LandscapeGenerator.h"
#include "MapGeneration.generated.h"

UCLASS()
class PRODJECT2_UE5_API AMapGeneration : public ALandscapeGenerator {
	GENERATED_BODY()
  public:
	AMapGeneration();
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;

	// ������� ��� ������������ ������ ������ �������� � �������� 0 - 100 �� �����
	UFUNCTION(BlueprintCallable, Category = "Generator|Normalize")
	TArray<FChanceGroup> NormalizeChanses(TArray<FChance> arrChance);

	UFUNCTION(BlueprintCallable, Category = "Generator|Hills")
	void GenerateObjectsMap_Hills(FDataMap &DataBorders);

  private:
	FVector CreateVectorBy2Points(FVector &startPoint, FVector &endPoint);
#pragma region Generation Objects
  public:
	UPROPERTY(EditAnywhere, Category = "Generator|Map", meta = (ToolTip = "Is square Colision Point"))
	bool bSquare;

	UPROPERTY(EditAnywhere,
			  Category = "Generator|Borders",
			  meta = (ClampMin = "1", ClampMax = "20", ToolTip = "The number of try for spawn Objects"))
	int16 CountsTry2Spawn;

	// ������� ��� ������ ���������
	// ��������� ���������� ��� ������ �� �������������, ����������!!!!!!! ���������� ��������� � ������������ � �� � ��������
	float ZCoordinateFind(FVector point);

  private:
	TArray<FCollisionPoint> CollisionPoints;

	TArray<uint16> FindProcentQualityObjects(TArray<FSimpleObject> &Objects, FCollisionPoint &point, float &densities);

	bool FindLocationForObject(FSimpleObject &Object, FCollisionPoint &point);

	bool LocationSimpleObjectsValid(TArray<FVector2D> &chunksIndexes, FSimpleObject &addedObject);
	// ������� ������� �� ���������
	// �� �����������
	FRotator FindRotationWithMesh(FVector location);
	// ����������� ������� �� ������ ��� ������

	void SpawnObject(TArray<FSimpleObject> &arrayObjects);

	int8 RandomObjectIndex(const TArray<FSimpleObject> &Objects);
	FVector RandomPointInSquare(FCollisionPoint &point);
	FVector RandomPointInCircle(FCollisionPoint &point);

	void GenerateSimpleObjects(TArray<FCollisionPoint> &collisionPoints, FDataMap Data);
#pragma region Hard objects

#pragma region Triangles Finde

	TArray<TSet<TArray<int32>>> FindTrianglesAroundVertex(TArray<int32> startPoints, int MaxDepth);

	void RemoveDuplicates(TArray<TSet<TArray<int32>>> &depthTriangles);
#pragma endregion

#pragma region Terraforming

#pragma endregion
#pragma endregion

#pragma region Borders
  public:
	// ���������� ����������� �����
	  // ���� �� ������������
	UPROPERTY(EditAnywhere,
			  Category = "Generator|Borders",
			  meta = (ClampMin = "1", ClampMax = "16", ToolTip = "The number of threads for generating one boundary"))
	int16 TaskCountBorder;

	// ���������� ����� �������� ������� colisionPOint ��� ������
	// ��� ������ ���������� ��� ������ �����, �� ��� ������ ������� ������� ��� ���������, ��-�� ���� ����� ��������
	// �������� ��������-�� �������� �� ������� ��� ��� ���� ����� ����� �������
	UPROPERTY(EditAnywhere,
			  Category = "Generator|Borders",
			  meta = (ClampMin = "1", ToolTip = "Constant distance between objects along the boundary axis."))
	int16 BorderStepLenght;

#pragma region Special borders parametrs
  public:
	// ���������� ��� ��������� ����: ������� �����, ������� �����, �������� ���������� �������������
	/// k1 - parameter before function
	UPROPERTY(EditAnywhere, Category = "Generator|Borders|Hills") int16 ka_min;
	UPROPERTY(EditAnywhere, Category = "Generator|Borders|Hills") int16 ka_max;

	/// k2- parameter before argument
	UPROPERTY(EditAnywhere, Category = "Generator|Borders|Hills") int16 kf_min;
	UPROPERTY(EditAnywhere, Category = "Generator|Borders|Hills") int16 kf_max;
	// #end last com

  private:
	// ��� ������� ������ (�����)
	int16 k1_f;
	int16 k2_f;
	int16 k1_a;
	int16 k2_a;

	// functions
	float FunctionBorders_Hills(float x);
#pragma endregion

  private:
	// for max min borders coordinate incide map for spawn sqare colision point
	// ���������� ������ �������� ��������� �������� ������ ������ ������� x = k, y = k
	float minX_inside;
	float maxX_inside;
	float minY_inside;
	float maxY_inside;

	// functions
	TArray<TArray<FCollisionPoint>> BordersRegionCreater(float (AMapGeneration::*Function)(float x));
	void BordersObjectsCreate(FDataMap &Objects, float (AMapGeneration::*Function)(float x));

#pragma endregion
#pragma endregion
	
	

#pragma region Debug Functions
	UFUNCTION(BlueprintCallable, Category = "Generator|DEBUG")
	void Debug_BordersSpawn();
	// UFUNCTION(BlueprintCallable, Category = "Generator|DEBUG")
	// void CallFunctionFromMyActor(TSubclassOf<ATerraObject> MyActorInstance);
#pragma endregion
};
