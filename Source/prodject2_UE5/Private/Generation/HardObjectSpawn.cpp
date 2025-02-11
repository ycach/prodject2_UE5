// Fill out your copyright notice in the Description page of Project Settings.

#include "Generation/HardObjectSpawn.h"
#include "Generation/MapGeneration/LandscapeGenerator.h"
#include "Async/Async.h"	  // Для использования Async
#include "Containers/Array.h" // Для использования TArray
#define DEBUG_BP_FUNCTIONS

AHardObjectSpawn::AHardObjectSpawn() {

	PrimaryActorTick.bCanEverTick = false;
}

void AHardObjectSpawn::BeginPlay() {
	Super::BeginPlay();
}

#pragma region ObjectsGenerate

void AHardObjectSpawn::ObjectsGenerationInZone(const TArray<FMapObject> &Objects, int8 delta,
											   TArray<uint16> count_of_objects,
											   FVector (AHardObjectSpawn::*FunctionRandomPoint)(const FVector2D par_1,
																								const FVector2D par_2),
											   FVector2D par1, FVector2D par2) {

	// максимальное количество попыток сгенерировать точку с объектом

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<FObjectThatHaveLocation> ArrHardObjects;

	for (int i = 0; i < Objects.Num(); i++) {

		FRotator rotation = FRotator::ZeroRotator;

		// Генерируем случайное количество объектов для данного типа

		int downBordet_to_spawn = count_of_objects[i] - (int)(count_of_objects[i] * delta / 100);
		int upBordet_to_spawn = count_of_objects[i] + (int)(count_of_objects[i] * delta / 100);

		if (downBordet_to_spawn < 0) {
			downBordet_to_spawn = 0;
			if (upBordet_to_spawn == 0)
				upBordet_to_spawn = 1;
		}

		count_of_objects[i] = RandomStream.RandRange(downBordet_to_spawn, upBordet_to_spawn);

		int8 counter_of_notFind = 0;
		for (int32 j = 0; j < count_of_objects[i]; j++) {

			// Генерируем случайную точку
			FVector RandomLocation = (this->*FunctionRandomPoint)(par1, par2);
			RandomLocation.Z = ZCoordinateFind(RandomLocation);

			int32 indexOfChank = ChunkNumberInWhichObjectLocated(RandomLocation);

			if (!Objects[i].ISHard) {

				TArray<int32> arrNeighbourChankIndexes =
					IndexesOfChanksCheckBeforeSpawn(indexOfChank, RandomLocation, Objects[i].RadiusColision);

				if (IsLocationValid(RandomLocation, ALandscapeGenerator::SpawnedObjectsInChanks,
									arrNeighbourChankIndexes, Objects[i].RadiusColision)) {
					FObjectThatHaveLocation object_to_add = FObjectThatHaveLocation(
						RandomLocation, rotation, Objects[i].Actor, SpawnParams, Objects[i].RadiusColision);
					ALandscapeGenerator::SpawnedObjectsInChanks[i].Add(object_to_add);
				} else {
					if (counter_of_notFind < MaxTry) {
						counter_of_notFind++;
						j--;
					}
				}

			} else {

				FTransform Transform(FRotator::ZeroRotator, RandomLocation);

				AHardObjectSpawn *HardObjectSpawn = GetWorld()->SpawnActor<AHardObjectSpawn>(
					Objects[i].HardActor, RandomLocation, FRotator::ZeroRotator, SpawnParams);

				HardObjectSpawn->HO_SetRadiusColision();
				int32 RadiusColision_temp = HardObjectSpawn->GetRadiusColision();

				if (IsLocationValid(RandomLocation, ArrHardObjects, RadiusColision_temp)) {
					FObjectThatHaveLocation object_to_add = FObjectThatHaveLocation(
						RandomLocation, rotation, Objects[i].HardActor, SpawnParams, RadiusColision_temp);
					ArrHardObjects.Add(object_to_add);

					HardObjectSpawn->HO_GenerationSubObjectsCoordinate();

				} else {

					HardObjectSpawn->Destroy();

					if (counter_of_notFind < MaxTry) {
						counter_of_notFind++;
						j--;
					}
				}
			}
		}
	}
}

TArray<uint16> AHardObjectSpawn::FindQualityObjects(const TArray<FMapObject> &Objects, float &densities,
													float squareZone) {

	float k = 0; // коээфициент относительной вместимости
	float sum = 0;

	TArray<uint16> RelativeDensities; // массив количества объектов для каждого типа

	sum += FindSquareOfRadiusColisionZone(Objects[0].RadiusColision) * Objects[0].Chance;
	for (int i = 1; i < Objects.Num(); i++) {
		sum += FindSquareOfRadiusColisionZone(Objects[i].RadiusColision) * (Objects[i].Chance - Objects[i - 1].Chance);
	}

	k = (squareZone * densities / 100.0f) / sum;

	RelativeDensities.Add(Objects[0].Chance * k);
	for (int i = 1; i < Objects.Num(); i++) {
		RelativeDensities.Add((Objects[i].Chance - Objects[i - 1].Chance) * k);
	}

	return RelativeDensities;
}

#pragma endregion

#pragma region Square
float AHardObjectSpawn::FindSquareOfRadiusColisionZone(const float r) {
	return FMath::Pow(r, 2.0f) * 3.2;
}

float AHardObjectSpawn::FindSquareOfEllipseZone(const FVector2d r) {
	return r.X * r.Y * 3.2;
}

float AHardObjectSpawn::FindSquareRectengleZone(const FVector2D Point_1, const FVector2D Point_2) {
	return (Point_2.Y - Point_1.Y) * (Point_2.X - Point_1.X);
}
#pragma endregion

#pragma region RandomPoint
FVector AHardObjectSpawn::GetRandomPointInRectengleZone(const FVector2D Point_1, const FVector2D Point_2) {
	// Генерируем случайные координаты внутри ограничивающего прямоугольника

	float RandomX = RandomStream.FRandRange(Point_1.X, Point_2.X);
	float RandomY = RandomStream.FRandRange(Point_1.Y, Point_2.Y);

	return FVector(RandomX, RandomY, 0.0f);
}

FVector AHardObjectSpawn::GetRandomPointInEllipseZone(const FVector2D EllipseCenter, const FVector2D EllipseRadius) {
	// Генерируем случайный угол в радианах
	float RandomAngle = RandomStream.FRandRange(0.0f, 2.0f * PI);

	// Генерируем случайный радиус, который гарантированно попадает внутрь эллипса
	float RandomRadius = RandomStream.FRandRange(0.0f, 1.0f);
	RandomRadius = FMath::Sqrt(RandomRadius); // Это гарантирует равномерное распределение внутри эллипса

	// Масштабируем радиус в соответствии с радиусами эллипса
	float ScaledX = RandomRadius * EllipseRadius.X * FMath::Cos(RandomAngle);
	float ScaledY = RandomRadius * EllipseRadius.Y * FMath::Sin(RandomAngle);

	// Смещаем координаты к центру эллипса
	FVector2D RandomPoint2D(ScaledX + EllipseCenter.X, ScaledY + EllipseCenter.Y);

	// Возвращаем точку в трехмерном пространстве (предполагая, что Z = 0)
	return FVector(RandomPoint2D.X, RandomPoint2D.Y, 0.0f);
}
#pragma endregion

#pragma region Special Hard Objects

#pragma region Grove

void AHardObjectSpawn::GroveSetRadiusColision(const TArray<FMapObject> &Objects) {
	ElipsR1 = RandomStream.FRandRange(minRadius, minRadius);
	ElipsR2 = RandomStream.FRandRange(minRadius, minRadius);
	int deltaR = 0;
	for (int i = 0; i < Objects.Num(); i++) {
		if (Objects[i].RadiusColision > deltaR)
			deltaR = Objects[i].RadiusColision;
	}
	RadiusColisionHO = ((ElipsR2 >= ElipsR1) ? ((int)ElipsR2) : ((int)ElipsR1)) + deltaR;
}

double AHardObjectSpawn::GetRadiusColision() {
	return RadiusColisionHO;
}

void AHardObjectSpawn::GroveGenerationObject(const TArray<FMapObject> &Objects, float densities, int delta) {
	auto count_of_objects =
		FindQualityObjects(Objects, densities, FindSquareOfEllipseZone(FVector2D(ElipsR1, ElipsR2)));

	ObjectsGenerationInZone(Objects, delta, count_of_objects, &AHardObjectSpawn::GetRandomPointInEllipseZone,
							FVector2D(ElipsR1, ElipsR2),
							FVector2D(this->GetActorLocation().X, this->GetActorLocation().Y));
}
#pragma endregion

#pragma endregion

#ifdef DEBUG_BP_FUNCTIONS
#pragma region DEBUG_Functions
void AHardObjectSpawn::Debug_ObjectsSpawn(bool bSpawn) {
	if (bSpawn) {
		for (int i = 0; i < ALandscapeGenerator::SpawnedObjectsInChanks.Num(); i++) {
			SpawnObject(ALandscapeGenerator::SpawnedObjectsInChanks[i]);
		}
	}
}
#pragma endregion
#endif
void AHardObjectSpawn::HO_GenerationSubObjectsCoordinate_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("HO_GenerationCoordinates Not overrid in Blueprint"));
}

void AHardObjectSpawn::HO_SetRadiusColision_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("SetRadiusColision Not overrid in Blueprint"));
}

TArray<int32> AHardObjectSpawn::IndexesOfChanksCheckBeforeSpawn(int32 indexMidlChank, FVector location,
																int radiusColision) {
	TArray<int32> result;
	result.Add(indexMidlChank);

	TArray<double> arrBordersCoordinatesForChank = ChankBordersCoordinate(indexMidlChank);
	double minX = arrBordersCoordinatesForChank[0];
	double minY = arrBordersCoordinatesForChank[1];
	double maxX = arrBordersCoordinatesForChank[2];
	double maxY = arrBordersCoordinatesForChank[3];

	// Объявляем массив индексов для проверки
	TArray<int32> indexesToCheck = {
		indexMidlChank - CountOfChanksInSide,	  // Left
		indexMidlChank + CountOfChanksInSide,	  // Right
		indexMidlChank - 1,						  // Down
		indexMidlChank + 1,						  // Up
		indexMidlChank - CountOfChanksInSide - 1, // Bottom Left
		indexMidlChank + CountOfChanksInSide + 1, // Top Right
		indexMidlChank + CountOfChanksInSide - 1, // Bottom Right
		indexMidlChank - CountOfChanksInSide + 1  // Top Left
	};

	// Используем TArray для хранения результатов от асинхронных задач
	TArray<int32> indexesToAdd;

	for (int32 checkIndex : indexesToCheck) {
		if (checkIndex >= 0 && checkIndex < ALandscapeGenerator::SpawnedObjectsInChanks.Num()) {
			double distance = 0.0;

			// Вычисляем расстояние в зависимости от индекса
			if (checkIndex == indexMidlChank - CountOfChanksInSide)
				distance = location.X - minX; // Left
			else if (checkIndex == indexMidlChank + CountOfChanksInSide)
				distance = maxX - location.X; // Right
			else if (checkIndex == indexMidlChank - 1)
				distance = location.Y - minY; // Down
			else if (checkIndex == indexMidlChank + 1)
				distance = maxY - location.Y; // Up
			else if (checkIndex == indexMidlChank - CountOfChanksInSide - 1)
				distance = HypotenuseFind(FVector2D(location.X, location.Y), FVector2D(minX, minY)); // Bottom Left
			else if (checkIndex == indexMidlChank + CountOfChanksInSide + 1)
				distance = HypotenuseFind(FVector2D(location.X, location.Y), FVector2D(maxX, maxY)); // Top Right
			else if (checkIndex == indexMidlChank + CountOfChanksInSide - 1)
				distance = HypotenuseFind(FVector2D(location.X, location.Y), FVector2D(maxX, minY)); // Bottom Right
			else if (checkIndex == indexMidlChank - CountOfChanksInSide + 1)
				distance = HypotenuseFind(FVector2D(location.X, location.Y), FVector2D(minX, maxY)); // Top Left

			if (distance > radiusColision) {
				FScopeLock lock(&resultCriticalSection); // Защита от конкурентного доступа
				indexesToAdd.Add(checkIndex);
			}
		}
	}

	result.Append(indexesToAdd);

	return result;
}

double AHardObjectSpawn::HypotenuseFind(const FVector2D &PointA, const FVector2D &PointB) {
	double DeltaX = PointB.X - PointA.X;
	double DeltaY = PointB.Y - PointA.Y;

	return FMath::Sqrt(DeltaX * DeltaX + DeltaY * DeltaY);
}
