// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "ProceduralMeshComponent.h"
#include "LandscapeGenerator.generated.h"

#pragma region Structs
USTRUCT(BlueprintType)
struct FMapObject {
	GENERATED_BODY()

  public:
	// Число от 0 до 255
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape|Objects")
	float Chance;

	// Объект Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape|Objects")
	TSubclassOf<AActor> Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape|Objects")
	int RadiusColision;

	// Конструктор по умолчанию
	FMapObject() : Chance(0), Actor(nullptr) {
	}

	// Конструктор с параметрами
	FMapObject(const TSubclassOf<AActor> InActor, uint8 InChance) : Chance(InChance), Actor(InActor) {
	}
};

#pragma endregion

UCLASS()
class PRODJECT2_UE5_API ALandscapeGenerator : public AActor {

	GENERATED_BODY()

  public:
	// Sets default values for this actor's properties
	ALandscapeGenerator();

#pragma region Blueprint vision
#pragma region values
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") int8 power2; // power of 2 fo width
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") int32 poligonSize;
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") double scaleUV;

	////values for different landscape/////
	/////borders func parametrs
	/// k1 - parameter before function
	UPROPERTY(EditAnywhere, Category = "Landscape") int16 ka_min;
	UPROPERTY(EditAnywhere, Category = "Landscape") int16 ka_max;

	/// k2- parameter before argument
	UPROPERTY(EditAnywhere, Category = "Landscape") int16 kf_min;
	UPROPERTY(EditAnywhere, Category = "Landscape") int16 kf_max;

	////Hills
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 c1_min;
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 c1_max;
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 c2_min;
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 c2_max;

	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 H_min;
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 H_max;
#pragma endregion
#pragma region events and fubctions
	/////////////////events and functions////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Landscape|Hills")
	void GenerateHills(UProceduralMeshComponent *MeshComponent); // create flat surface

	UFUNCTION(BlueprintCallable, Category = "Landscape|Hills")
	void GenerateObjectsOnMap_Hills(int indentForBorders, const TArray<FMapObject> &Objects);

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void ClearFirst();

	// UFUNCTION(BlueprintCallable, Category = "Landscape|Bordes")

#pragma endregion
#pragma endregion
  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  private:
#pragma region values
	int32 width;
	TArray<FVector> arrVertix;
	TArray<FVector2D> arrUV;
	TArray<int32> arrTriangles;
	TArray<AActor *> arrBorders;

	int32 indexPointA;
	int32 indexPointB;
	int32 indexPointC;
	int32 indexPointD;

	/////////////////////values for landscape/////////////////////////////////////////////

	// variables for leveling height in function GenerationHillsHeights()
	int16 C1;
	int16 C2;
	int16 C3;
	int16 C4;
	int16 H1;
	int16 H2;

	/////////////////////////////////////Random Generates objects for all types
	/// landscape//////////////////////////////////////////////////
	int16 k1_f;
	int16 k2_f;
	int16 k1_a;
	int16 k2_a;
#pragma endregion

#pragma region functions
	void CreateRandomLandscape(UProceduralMeshComponent *MeshComponent,
							   float (ALandscapeGenerator::*Function)(float x));

	void BordersObjectsCreate(int indentForBorders, const TArray<FMapObject> &Objects,
							  float (ALandscapeGenerator::*Function)(float x));

	void CalculateNormals(const TArray<FVector> &Vertices, const TArray<int32> &Triangles, TArray<FVector> &Normals);

	void CalculateTangents(const TArray<FVector> &Vertices, const TArray<int32> &Triangles,
						   const TArray<FVector> &Normals, const TArray<FVector2D> &UV0,
						   TArray<FProcMeshTangent> &Tangents);

	// Functions for algorithm dimond - square
	void DiamondSquare(float (ALandscapeGenerator::*Function)(float x));

	int32 MidlPointSquareIndex(int32 A, int32 C); // functionn for find midl vertix index in array of vertix
												  // can be only i  nteger becorce square has n + 1 vertix, where n -
												  // power of 2 and return vertix always is node

	int32 MidlPointLineIndex(int32 A, int32 C); // function for find central vertix index on line
												// can be only integer becorce square has n + 1 vertix, where n - power
												// of 2 and return vertix always is node

	void CreateObject(const TArray<FMapObject> &Objects, float (ALandscapeGenerator::*Function)(float x));
	void SpawnObjectsBetweenVertices(const FVector &Start, const FVector &End, TArray<FMapObject> Object);
	TArray<TArray<FVector>> NodesYPoints(float (ALandscapeGenerator::*Function)(float x));
	float ZCoordinateFind(FVector point, bool bOy_true);

	TArray<int32> GenerationHeights(int32 A, int32 B, int32 C, int32 D,
									float (ALandscapeGenerator::*Function)(float x));

	void GenerationHeightsInPeacks(
		float (ALandscapeGenerator::*Function)(float x)); // function for find all indexes for one tick algorithm
														  // dimonnd - square for
														  // hills
														  //  indexes: midl, top, right, left, bot
	//!!!				  // must be clear in faunctions abroad!
	// retutn arr: a - 0, b - 1, c - 2 d - 3, mid - 4
	// left - 5, top - 6, right - 7, bot - 8

	/// borders
	float FunctionBorders(float x);

	//////////////////////////////////////////////////////////specific
	/// functions///////////////////////////////////////////////////////////////

	float FunctionHillsHeights(float point);
#pragma endregion
};
