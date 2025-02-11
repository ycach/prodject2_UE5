#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralGeneration.generated.h"

UCLASS()
class PRODJECT2_UE5_API AProceduralGeneration : public AActor {
	GENERATED_BODY()

  public:
	AProceduralGeneration();

  protected:
	virtual void BeginPlay() override;

  public:
#pragma region Blueprint value
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") int8 BPSet_Power2; // power of 2 fo width
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") int32 BPSet_PoligonSize;
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") double BPSet_ScaleUV;
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") int32 BPSet_countOfChanksInSide; // width*2^power2 должна быть кратна этому числу

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

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void ClearFirst();

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void CreateProceduralMash(UProceduralMeshComponent *MeshComponent);

	UFUNCTION(BlueprintCallable, Category = "Landscape|Hills")
	void GenerateHills(UProceduralMeshComponent *MeshComponent); // create flat surface


	int32 ChunkNumberInWhichObjectLocated(FVector location);
#pragma endregion

  protected:
	static int8 power2; // power of 2 fo width
	static int32 poligonSize;
	static double scaleUV;

	static int32 width;
	static TArray<FVector> arrVertix;
	static TArray<FVector2D> arrUV;
	static TArray<int32> arrTriangles;

	static float minX_inside;
	static float maxX_inside;
	static float minY_inside;
	static float maxY_inside;

	static int32 indexPointA;
	static int32 indexPointB;
	static int32 indexPointC;
	static int32 indexPointD;

	static double oneChankSizeSide;
	static int CountOfChanksInSide;

	// variables for leveling height in function GenerationHillsHeights()
	int16 C1;
	int16 C2;
	int16 C3;
	int16 C4;
	int16 H1;
	int16 H2;

	float FunctionHillsHeights(float point);

	bool IsBorderChank(int32 indexChank);
	
	TArray<double> ChankBordersCoordinate(int32 indexChank);// minX, minY, maxX, maxY

	

  private:
	void SetValuesWorld();

	#pragma region Dimond-Squre
	// Functions for algorithm dimond - square
	void DiamondSquare(float (AProceduralGeneration::*Function)(float x));

	int32 MidlPointSquareIndex(int32 A, int32 C); // functionn for find midl vertix index in array of vertix
												  // can be only i  nteger becorce square has n + 1 vertix, where n -
												  // power of 2 and return vertix always is node

	int32 MidlPointLineIndex(int32 A, int32 C); // function for find central vertix index on line
												// can be only integer becorce square has n + 1 vertix, where n - power
												// of 2 and return vertix always is node

#pragma endregion
	void CreateRandomLandscape(float (AProceduralGeneration::*Function)(float x));
	void CalculateNormals(const TArray<FVector> &Vertices, const TArray<int32> &Triangles, TArray<FVector> &Normals);

	void CalculateTangents(const TArray<FVector> &Vertices, const TArray<int32> &Triangles,
						   const TArray<FVector> &Normals, const TArray<FVector2D> &UV0,
						   TArray<FProcMeshTangent> &Tangents);

	TArray<int32> GenerationHeights(int32 A, int32 B, int32 C, int32 D,
									float (AProceduralGeneration::*Function)(float x));

	void GenerationHeightsInPeacks(
		float (AProceduralGeneration::*Function)(float x)); // function for find all indexes for one tick algorithm
															// dimonnd - square for
															// hills
															//  indexes: midl, top, right, left, bot
															//!!!				  // must be clear in faunctions abroad!
															// retutn arr: a - 0, b - 1, c - 2 d - 3, mid - 4
															// left - 5, top - 6, right - 7, bot - 8

	void SetChanksCount();
};
