#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "ProceduralMeshComponent.h"
#include "MapGeneration/Chunk.h"
#include "LandscapeGenerator.generated.h"

UCLASS()
class PRODJECT2_UE5_API ALandscapeGenerator : public AActor {

	GENERATED_BODY()
	
  public:
	ALandscapeGenerator();
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;

  protected:
	virtual void BeginPlay() override;

	// Blueprint calleble functions for generate
  public:
	UFUNCTION(BlueprintCallable, Category = "Generator|Landscape|Hills")
	void GenerateHills(UProceduralMeshComponent *MeshComponent);

  protected:
	// Sid - generation generate sid for all procedural generation map
	// возможно надо сделать несколько разных, под объекты, ландшафт, границы...
	FRandomStream RandomStream;

#pragma region Generate Landscape
	// value - landscape settings 
  public:
	UPROPERTY(
		EditAnywhere,
		Category = "Generator|Landscape|Poligons parametrs",
		meta = (ClampMin = "1",
				ClampMax = "16",
				ToolTip = "Power of two determining the width of the game field by the formula: width = 2^power2."))
	int8 power2; // power of 2 for width
	// количество найденных вершин не линейным методом
	UPROPERTY(EditAnywhere,
			  Category = "Generator|Landscape|Poligons parametrs",
			  meta = (ClampMin = "1", ClampMax = "16", ToolTip = "Depth of the landscape function smoothing"))
	int8 depthSmoothing;

	UPROPERTY(EditAnywhere,
			  Category = "Generator|Landscape|Poligons parametrs",
			  meta = (ClampMin = "1", ClampMax = "100000", ToolTip = "Size of one polygon in coordinate grid units."))
	int32 poligonSize;

	UPROPERTY(EditAnywhere,
			  Category = "Generator|Landscape|Poligons parametrs",
			  meta = (ClampMin = "1", ToolTip = "Texture scaling factor."))
	float scaleUV; // Используем float вместо double

  protected:
	int32 width;

	int32 indexPointA;
	int32 indexPointB;
	int32 indexPointC;
	int32 indexPointD;

	TArray<FVector> arrVertix;
	TArray<FVector2D> arrUV;
	TArray<int32> arrTriangles;
	TArray<TArray<Chunk>> arrChunks;

#pragma region Specific parametrs Landscape
	//public parametrs for landscape
  public:
	// Hills
	UPROPERTY(EditAnywhere, Category = "Generator|Landscape|Hills") int16 c1_min;
	UPROPERTY(EditAnywhere, Category = "Generator|Landscape|Hills") int16 c1_max;
	UPROPERTY(EditAnywhere, Category = "Generator|Landscape|Hills") int16 c2_min;
	UPROPERTY(EditAnywhere, Category = "Generator|Landscape|Hills") int16 c2_max;

	UPROPERTY(EditAnywhere, Category = "Generator|Landscape|Hills") int16 H_min;
	UPROPERTY(EditAnywhere, Category = "Generator|Landscape|Hills") int16 H_max;

	// values for difrent landscape Functions
  private:
	// Hills
	int16 C1;
	int16 C2;
	int16 C3;
	int16 C4;
	int16 H1;
	int16 H2;

	// function for difrent landscapes
  private:
	float FunctionHillsHeights(float point);
	float FunctionFlatHeights(float point);
#pragma endregion

	// functions for generate landscape
  private:
	void CreateRandomLandscape(UProceduralMeshComponent *MeshComponent,
							   float (ALandscapeGenerator::*Function)(float x));
	TArray<int32> GenerationHeights(int32 A,
									int32 B,
									int32 C,
									int32 D,
									float (ALandscapeGenerator::*Function)(float x));

	void GenerationHeightsInPeacks(
		float (ALandscapeGenerator::*Function)(float x)); // function for find all indexes for one tick algorithm
														  // dimonnd - square for
														  // hills
														  //  indexes: midl, top, right, left, bot
														  //!!!				  // must be clear in faunctions abroad!
														  // retutn arr: a - 0, b - 1, c - 2 d - 3, mid - 4
														  // left - 5, top - 6, right - 7, bot - 8

	TArray<int32> LinearInterpolation(int32 A, int32 B, int32 C, int32 D);
#pragma region Dimond -Squre
	// Functions for algorithm dimond - square
  private:
	void DiamondSquare(float (ALandscapeGenerator::*Function)(float x));

	int32 MidlPointSquareIndex(int32 A, int32 C); // functionn for find midl vertix index in array of vertix
												  // can be only i  nteger becorce square has n + 1 vertix, where n -
												  // power of 2 and return vertix always is node

	int32 MidlPointLineIndex(int32 A, int32 C); // function for find central vertix index on line
												// can be only integer becorce square has n + 1 vertix, where n - power
												// of 2 and return vertix always is node

#pragma endregion
#pragma endregion

#pragma region Chanks
  public:
	UPROPERTY(
		EditAnywhere,
		Category = "Generator|Landscape|Chancks parametrs",
		meta = (ClampMin = "0",
				ClampMax = "16",
				ToolTip =
					"Power of four determining how many chunks the field will be divided into. (Can't exceed power2)"))
	uint8 power4; // power of 4 fo width
 

  protected:
	int32 ChunkSize;
	FVector2D PointInChunkIndex(FVector point);
	TArray<FVector2D> FindChunksAround(FVector2D centralCunck);

	 private:
	void ChunksGenerator();
#pragma endregion
};
