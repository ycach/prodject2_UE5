// Fill out your copyright notice in the Description page of Project Settings.

#include "Generation/EazyObjectSpawn.h"

AEazyObjectSpawn::AEazyObjectSpawn() {
	PrimaryActorTick.bCanEverTick = false;
	RandomStream.GenerateNewSeed();
}

void AEazyObjectSpawn::BeginPlay() {
	Super::BeginPlay();
}

bool AEazyObjectSpawn::IsLocationValid(const FVector &Location, const TArray<FObjectThatHaveLocation> &SpawnedLocations,
									   int32 RadiusColision) {
	// Проверяем каждую уже размещённую локацию
	if (FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(minX_inside, Location.Y, 0.0f)) < RadiusColision &&
		FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(maxX_inside, Location.Y, 0.0f)) < RadiusColision &&
		FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(Location.X, minY_inside, 0.0f)) < RadiusColision &&
		FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(Location.X, maxY_inside, 0.0f)) < RadiusColision) {
		bool bIsTooClose = false;
		FCriticalSection CriticalSection;

		ParallelFor(SpawnedLocations.Num(), [&](int32 Index) {
			const auto &SpawnedLocation = SpawnedLocations[Index];
			float DistanceToSpawned = FVector::Dist(Location, SpawnedLocation.Location);

			if (DistanceToSpawned < RadiusColision && DistanceToSpawned < SpawnedLocation.RadiusColision) {
				CriticalSection.Lock();
				bIsTooClose = true;
				CriticalSection.Unlock();
			}
		});

		if (bIsTooClose) {
			return false; // Слишком близко к другому объекту
		}
	}
	return true; // Место свободно
	
}

bool AEazyObjectSpawn::IsLocationValid(const FVector &Location,
									   const TArray<TArray<FObjectThatHaveLocation>> &SpawnedLocations,
									   TArray<int32> &indexesOfSpawnedLocations, int32 RadiusColision) {
	if (FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(minX_inside, Location.Y, 0.0f)) < RadiusColision &&
		FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(maxX_inside, Location.Y, 0.0f)) < RadiusColision &&
		FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(Location.X, minY_inside, 0.0f)) < RadiusColision &&
		FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(Location.X, maxY_inside, 0.0f)) < RadiusColision) {
		bool bIsTooClose = false;
		FCriticalSection CriticalSection;

		ParallelFor(indexesOfSpawnedLocations.Num(), [&](int32 Index) {
			int32 SpawnedIndex = indexesOfSpawnedLocations[Index];
			if (!IsLocationValid(Location, SpawnedLocations[SpawnedIndex], RadiusColision)) {
				CriticalSection.Lock();
				bIsTooClose = true;
				CriticalSection.Unlock();
			}
		});

		if (bIsTooClose) {
			return false; // Слишком близко к другому объекту
		}
	}

	return true;
}

void AEazyObjectSpawn::SpawnObject(TArray<FObjectThatHaveLocation> &arrayObjects) {
	for (int i = 0; i < arrayObjects.Num(); i++) {
		GetWorld()->SpawnActor<AActor>(arrayObjects[i].Object, arrayObjects[i].Location, arrayObjects[i].Rotation,
									   arrayObjects[i].SpawnParams);
	}
}

int8 AEazyObjectSpawn::IndexOfObjectToGeneration(const TArray<FMapObject> &Objects) {
	int8 randomValue = RandomStream.RandRange(0, 100);
	int8 selectedIndex = -1;
	for (int j = 0; j < Objects.Num(); j++) {
		if (randomValue <= Objects[j].Chance) {
			selectedIndex = j;
			break;
		}
	}
	return selectedIndex;
}

FRotator AEazyObjectSpawn::FindRotationWithMesh(FVector location) {

	return FRotator();
}

float AEazyObjectSpawn::ZCoordinateFind(FVector point) {
	// Вычисляем индексы вершин полигона
	int32 Index_point1 = (int)(point.Y / poligonSize) + (int)(point.X / poligonSize) * width;
	int32 Index_point2 = Index_point1 + 1;
	int32 Index_point3 = Index_point1 + width;
	int32 Index_point4 = Index_point3 + 1;

	// Вычисляем смещения
	float step_to_x = point.X - arrVertix[Index_point1].X;
	float step_to_y = point.Y - arrVertix[Index_point1].Y;

	// Находим нормализованные направления
	FVector direction13 = (arrVertix[Index_point3] - arrVertix[Index_point1]).GetSafeNormal();
	FVector direction24 = (arrVertix[Index_point4] - arrVertix[Index_point2]).GetSafeNormal();

	// Находим точки на гранях полигона
	FVector vector13_point = arrVertix[Index_point1] + direction13 * step_to_x;
	FVector vector24_point = arrVertix[Index_point2] + direction24 * step_to_x;

	// Вычисляем направление между двумя точками
	FVector direction = (vector24_point - vector13_point).GetSafeNormal();

	// Находим конечную точку и возвращаем её Z-координату
	FVector point_point = vector13_point + direction * step_to_y;

	return point_point.Z;
}

float AEazyObjectSpawn::ZCoordinateFind(FVector point, bool bOy_true) {
	int32 FirstVertiIndex;
	int32 SecondVertiIndex;
	float step;
	if ((int)point.X % poligonSize == 0 && (int)point.Y % poligonSize == 0) {
		FirstVertiIndex = (int)(point.Y / poligonSize) + (int)(point.X / poligonSize) * width;
		return arrVertix[FirstVertiIndex].Z;
	}
	if (bOy_true) { // x - const before FUNCTION BORDERS USE
		FirstVertiIndex = (int)(point.Y / poligonSize) + (int)(point.X / poligonSize) * width;
		SecondVertiIndex = (int)(point.Y / poligonSize) + (int)(point.X / poligonSize) * (width + 1);
		step = point.X - arrVertix[FirstVertiIndex].X;
	} else { // y - const before FUNCTION BORDERS USE
		FirstVertiIndex = (int)(point.X / poligonSize) * width + (int)(point.Y / poligonSize);
		SecondVertiIndex = (int)(point.X / poligonSize) * width + (int)(point.Y / poligonSize) + 1;
		step = point.Y - arrVertix[FirstVertiIndex].Y;
	}
	FVector direction = (arrVertix[SecondVertiIndex] - arrVertix[FirstVertiIndex]).GetSafeNormal();
	FVector location = arrVertix[FirstVertiIndex] + direction * step;
	return location.Z;
}