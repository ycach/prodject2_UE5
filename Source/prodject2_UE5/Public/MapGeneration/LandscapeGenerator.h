// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "LandscapeGenerator.generated.h"

UCLASS()
class PRODJECT2_UE5_API ALandscapeGenerator : public AActor {

	GENERATED_BODY()

  public:
	// Sets default values for this actor's properties
	ALandscapeGenerator();

	///////////////////////////////values////////////////////////////////
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") int8 power2; // power of 2 fo width
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") int32 poligonSize;
	UPROPERTY(EditAnywhere, Category = "Poligons parametrs") double scaleUV;

	////values for different landscape/////
	////Hills
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 c1_min;
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 c1_max;
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 c2_min;
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 c2_max;

	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 H_min;
	UPROPERTY(EditAnywhere, Category = "Landscape|Hills landscape parametrs") int16 H_max;

	/////////////////events and functions////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void GenerateHills(UProceduralMeshComponent *MeshComponent); // create flat surface

  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 width;
	TArray<FVector> arrVertix;
	TArray<FVector2D> arrUV;
	TArray<int32> arrTriangles;

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

  public:
	void CreateRandomLandscape(UProceduralMeshComponent *MeshComponent,
							   TArray<int32> (ALandscapeGenerator::*IndexFind)(int32 A, int32 B, int32 C, int32 D),
							   void (ALandscapeGenerator::*StartPointsHights)(void),
							   void (ALandscapeGenerator::*ObjectCrate)(void));

	// Functions for algorithm dimond - square
	void DiamondSquare(TArray<int32> (ALandscapeGenerator::*IndexFind)(int32 A, int32 B, int32 C, int32 D));

	int32 MidlPointSquareIndex(int32 A, int32 C); // functionn for find midl vertix index in array of vertix
												  // can be only i  nteger becorce square has n + 1 vertix, where n -
												  // power of 2 and return vertix always is node

	int32 MidlPointLineIndex(int32 A, int32 C); // function for find central vertix index on line
												// can be only integer becorce square has n + 1 vertix, where n - power
												// of 2 and return vertix always is node

	//////////////////////////////////////////////////////////specific
	/// functions///////////////////////////////////////////////////////////////

	TArray<int32> GenerationHillsHeights(int32 A, int32 B, int32 C, int32 D);

	void GenerationHeightsInPeacksHills(); // function for find all indexes for one tick algorithm dimonnd - square for
										   // hills
										   //  indexes: midl, top, right, left, bot
	//!!!				  // must be clear in faunctions abroad!
	// retutn arr: a - 0, b - 1, c - 2 d - 3, mid - 4
	// left - 5, top - 6, right - 7, bot - 8

	void GenerationMapObjects();
};
