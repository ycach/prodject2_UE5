// Fill out your copyright notice in the Description page of Project Settings.

#include "Generation/MapGeneration/LandscapeGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Async/Async.h"
#include "Containers/Array.h"

#define DEBUG_BP_FUNCTIONS


TArray<TArray<FObjectThatHaveLocation>> ALandscapeGenerator::SpawnedObjectsInChanks;

// Sets default values
ALandscapeGenerator::ALandscapeGenerator() {
	PrimaryActorTick.bCanEverTick = false;
}

void ALandscapeGenerator::BeginPlay() {
	Super::BeginPlay();
}

#pragma region Random Generates objects for all types landscape
void ALandscapeGenerator::SetupGenerationObject() {
	SpawnedObjectsInChanks.SetNum(CountOfChanksInSide * CountOfChanksInSide);
}
#pragma region Borders
void ALandscapeGenerator::BordersObjectsCreate(int Spacing, const TArray<FMapObject> &Objects,
											   float (ALandscapeGenerator::*Function)(float x)) {
	k1_f = FMath::RandRange(kf_min, kf_max);
	k2_f = FMath::RandRange(kf_min, kf_max);
	k1_a = FMath::RandRange(ka_min, ka_max);
	k2_a = FMath::RandRange(ka_min, ka_max);

	TArray<TArray<FVector>> arrayNodes = NodesYPoints(Function);

	TArray<TFuture<void>> Futures;
	TArray<FObjectThatHaveLocation> AB_objects;
	TArray<FObjectThatHaveLocation> AC_objects;
	TArray<FObjectThatHaveLocation> BD_objects;
	TArray<FObjectThatHaveLocation> CD_objects;

	for (int i = 0; i < width - Spacing; i += Spacing) {
		Futures.Add(Async(EAsyncExecution::Thread, [this, i, Spacing, &arrayNodes, &Objects, &AB_objects, &AC_objects,
													&BD_objects, &CD_objects]() {
			// AB
			FindBordersObjectCoordinate(arrayNodes[0][i], arrayNodes[0][i + Spacing], Objects, &AB_objects);

			// AC
			FindBordersObjectCoordinate(arrayNodes[1][i], arrayNodes[1][i + Spacing], Objects, &AC_objects);

			// BD
			FindBordersObjectCoordinate(arrayNodes[2][i], arrayNodes[2][i + Spacing], Objects, &BD_objects);

			// CD
			FindBordersObjectCoordinate(arrayNodes[3][i], arrayNodes[3][i + Spacing], Objects, &CD_objects);
		}));
	}

	// Дождитесь завершения всех задач
	for (auto &Future : Futures) {
		Future.Wait();
	}

	AnalyzeBorderPoints(arrayNodes, maxY_inside, minY_inside, maxX_inside, minX_inside);
	arrayNodes.Empty();

	arrBorders.Add(AB_objects);
	AB_objects.Empty();

	arrBorders.Add(AC_objects);
	AC_objects.Empty();

	arrBorders.Add(BD_objects);
	BD_objects.Empty();

	arrBorders.Add(CD_objects);
	CD_objects.Empty();
}

void ALandscapeGenerator::FindBordersObjectCoordinate(const FVector &Start, const FVector &End,
													  const TArray<FMapObject> &Objects,
													  TArray<FObjectThatHaveLocation> *result) {
	float Spacing = 300;
	FVector direction = (End - Start).GetSafeNormal();

	for (float i = 0; i < poligonSize; i += Spacing) {
		// Используем локальный генератор
		int8 selectedIndex = -1; // Инициализируем индекс
		selectedIndex = IndexOfObjectToGeneration(Objects);

		Spacing = Objects[selectedIndex].RadiusColision;
		FVector location = Start + direction * i;

		FRotator rotation = FRotator::ZeroRotator;
		if (Objects[selectedIndex].RotationWithMesh) {
			rotation = FindRotationWithMesh(location);
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FObjectThatHaveLocation object_to_add = FObjectThatHaveLocation(
			location, rotation, Objects[selectedIndex].Actor, SpawnParams, Objects[selectedIndex].RadiusColision);
		result->Add(object_to_add);
	}
}
TArray<TArray<FVector>> ALandscapeGenerator::NodesYPoints(float (ALandscapeGenerator::*Function)(float x)) {
	TArray<FVector> AB_NodesPointsBorder;
	TArray<FVector> AC_NodesPointsBorder;
	TArray<FVector> BD_NodesPointsBorder;
	TArray<FVector> CD_NodesPointsBorder;
	for (int32 i = 0; i < width; i++) {
		// AB
		FVector tempVector = FVector((this->*Function)(arrVertix[i].Y), arrVertix[i].Y, 0.0f);
		tempVector.Z = ZCoordinateFind(tempVector, 1); // 1
		AB_NodesPointsBorder.Add(tempVector);

		// CD
		tempVector = FVector(arrVertix[indexPointD - 1].X - (this->*Function)(arrVertix[width * (width - 1) + i].Y),
							 arrVertix[width * (width - 1) + i].Y, 0.0f);
		tempVector.Z = ZCoordinateFind(tempVector, 1); // 1
		CD_NodesPointsBorder.Add(tempVector);

		// AC
		tempVector = FVector(arrVertix[i * width].X, (this->*Function)(arrVertix[i * width].X), 0.0f);
		tempVector.Z = ZCoordinateFind(tempVector, 0); // 0
		AC_NodesPointsBorder.Add(tempVector);
		// BD
		tempVector =
			FVector(arrVertix[(width - 1) + i * width].X,
					arrVertix[indexPointB - 1].Y - (this->*Function)(arrVertix[(width - 1) + i * width].X), 0.0f);
		tempVector.Z = ZCoordinateFind(tempVector, 0); // 0
		BD_NodesPointsBorder.Add(tempVector);
	}
	TArray<TArray<FVector>> result;
	result.Add(AB_NodesPointsBorder);
	result.Add(AC_NodesPointsBorder);
	result.Add(BD_NodesPointsBorder);
	result.Add(CD_NodesPointsBorder);

	return result;
}

float ALandscapeGenerator::FunctionBorders(float x) {
	return k1_f * FMath::Sin(x / k1_a) + k2_f * FMath::Cos(x / k2_a) + k1_f + k2_f;
}

void ALandscapeGenerator::AnalyzeBorderPoints(const TArray<TArray<FVector>> &BorderPoints, float &MaxY, float &MinY,
											  float &MaxX, float &MinX) {
	MinY = poligonSize * (width - 1) / 2;
	MaxY = poligonSize * (width - 1) / 2;
	MinY = poligonSize * (width - 1) / 2;
	MinY = poligonSize * (width - 1) / 2;

	// Проверяем границу AB
	for (const FVector &Point : BorderPoints[0]) {
		if (Point.X < MinX) {
			MinX = Point.X;
		}
	}

	// Проверяем границу AC
	for (const FVector &Point : BorderPoints[1]) {
		if (Point.Y < MinY) {
			MinY = Point.Y;
		}
	}
	// Проверяем границу BD
	for (const FVector &Point : BorderPoints[2]) {
		if (Point.Y > MaxY) {
			MaxY = Point.Y;
		}
	}
	// Проверяем границу CD
	for (const FVector &Point : BorderPoints[3]) {
		if (Point.X > MaxX) {
			MaxX = Point.X;
		}
	}
}

#pragma endregion

#pragma endregion

/////////////////////////////////////Specific Landscape/////////////////////
#pragma region Hills
void ALandscapeGenerator::GenerateObjectsOnMap_Hills(
	int indentForBorders, const TArray<FMapObject> &Objects_Borders,
													 const TArray<FMapObject> &Objects_Map, float densities,
													 int deltaQualityObjects) {
	BordersObjectsCreate(indentForBorders, Objects_Borders, &ALandscapeGenerator::FunctionBorders);

	auto count_of_objects = FindQualityObjects(Objects_Map, densities, FindSquareRectengleZone(FVector2D(minX_inside, minY_inside), FVector2D(maxX_inside, maxY_inside)));
	ObjectsGenerationInZone(
		Objects_Map, densities, count_of_objects,
							&AHardObjectSpawn::GetRandomPointInRectengleZone,
							FVector2D(minX_inside, minY_inside), FVector2D(maxX_inside, maxY_inside));
}
#pragma endregion
///////////////////////////////////

#ifdef DEBUG_BP_FUNCTIONS
#pragma region DEBUG_Functions
void ALandscapeGenerator::Debug_BordersSpawn(bool bAB_spawn, bool bAC_spawn, bool bBD_spawn, bool bCD_spawn) {
	if (bAB_spawn)
		SpawnObject(arrBorders[0]);
	if (bAC_spawn)
		SpawnObject(arrBorders[1]);
	if (bBD_spawn)
		SpawnObject(arrBorders[2]);
	if (bCD_spawn)
		SpawnObject(arrBorders[3]);
}
#pragma endregion
#endif //  DEBUG_BP_FUNCTIONS



