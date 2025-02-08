// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGeneration/MapGeneration.h"
#pragma region Override func and class parametrs
AMapGeneration::AMapGeneration() {
	PrimaryActorTick.bCanEverTick = false;
}

void AMapGeneration::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) {
	ALandscapeGenerator::PostEditChangeProperty(PropertyChangedEvent);
#pragma region TaskCountBorders
	// Если изменилось свойство power2
	// if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("power2")) {
	//	// Обновляем TaskCountBorders, если оно больше нового значения power2
	//	if (TaskCountBorder > power2)
	//		TaskCountBorder = power2;
	//}
	// if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("TaskCountBorder")) {
	//	// Обновляем TaskCountBorders, если оно больше нового значения power2
	//	if (TaskCountBorder > power2)
	//		TaskCountBorder = power2;
	//}
#pragma endregion
#pragma region DistanceBeatween
	// if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("poligonSize")) {
	//	// Обновляем TaskCountBorders, если оно больше нового значения power2
	//	if (DistanceBetween > poligonSize)
	//		DistanceBetween = poligonSize;
	// }
	// if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("DistanceBetween")) {
	//	// Обновляем TaskCountBorders, если оно больше нового значения power2
	//	if (DistanceBetween > poligonSize)
	//		DistanceBetween = poligonSize;
	// }
#pragma endregion
}
#pragma endregion

#pragma region BP Functions
TArray<FChanceGroup> AMapGeneration::NormalizeChanses(TArray<FChance> arrChance) {
	return Normalize::NormalizeChance(arrChance);
}

void AMapGeneration::GenerateObjectsMap_Hills(FDataMap &DataBorders) {
	k1_f = RandomStream.RandRange(kf_min, kf_max);
	k2_f = RandomStream.RandRange(kf_min, kf_max);
	k1_a = RandomStream.RandRange(ka_min, ka_max);
	k2_a = RandomStream.RandRange(ka_min, ka_max);
	BordersObjectsCreate(DataBorders, &AMapGeneration::FunctionBorders_Hills);
	//  PlayeblMapObjectsGeneration(Objects_Map, densities, deltaQualityObjects);
}
#pragma endregion

FVector AMapGeneration::CreateVectorBy2Points(FVector &startPoint, FVector &endPoint) {
	return FVector(endPoint.X - startPoint.X, endPoint.Y - startPoint.Y, endPoint.Z - startPoint.Z);
}

#pragma region Generation Objects

float AMapGeneration::ZCoordinateFind(FVector point) {
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

TArray<uint16> AMapGeneration::FindProcentQualityObjects(TArray<FSimpleObject> &Objects,
														 FCollisionPoint &point,
														 float &densities) {

	float k = 0; // коээфициент относительной вместимости
	float sum = 0;

	TArray<uint16> RelativeDensities; // массив количества объектов для каждого типа

	sum += Objects[0].FindSquare() * Objects[0].Chance.Chance;
	for (int i = 1; i < Objects.Num(); i++) {
		sum += Objects[0].FindSquare() * (Objects[i].Chance.Chance - Objects[i - 1].Chance.Chance);
	}
	if (bSquare) {
		if (sum != 0)
			k = (point.FindSuareSquare() * densities / 100.0f) / sum;
		else
			k = 0;

	} else {
		if (sum != 0)
			k = (point.FindSuareSquare() * densities / 100.0f) / sum;
		else
			k = 0;
	}

	RelativeDensities.Add(Objects[0].Chance.Chance * k);
	for (int i = 1; i < Objects.Num(); i++) {
		RelativeDensities.Add((Objects[i].Chance.Chance - Objects[i - 1].Chance.Chance) * k);
	}

	return RelativeDensities;
}

bool AMapGeneration::FindLocationForObject(FSimpleObject &Object, FCollisionPoint &point) {
	FVector spawnPoint;
	int counter = 0;
	while (counter < CountsTry2Spawn) {
		if (bSquare)
			spawnPoint = RandomPointInSquare(point);
		else
			spawnPoint = RandomPointInCircle(point);
		Object.Location = spawnPoint;
		TArray<FVector2D> ChunkIndexes = FindChunksAround(PointInChunkIndex(FVector(spawnPoint.X, spawnPoint.Y, 0.0f)));
		if (point.bBorder) {
			if (LocationSimpleObjectsValid(ChunkIndexes, Object)) {
				return true;
			} else {
				counter++;
			}
		}
	}
	return false;
}

bool AMapGeneration::LocationSimpleObjectsValid(TArray<FVector2D> &chunksIndexes, FSimpleObject &addedObject) {
	for (auto &index : chunksIndexes) {
		for (auto &object : arrChunks[index.X][index.Y].ObjectsInChunk) {
			FVector tempVector = FVector(object.Location.X, object.Location.Y, 0.0f);
			float lenghtBeatwean = CreateVectorBy2Points(tempVector, addedObject.Location).Size();
			float maxR = (addedObject.RadiusCollision > object.RadiusCollision) ? (addedObject.RadiusCollision)
																				: (object.RadiusCollision);
			if (lenghtBeatwean < maxR)
				return false;
		}
	}
	return true;
}

FRotator AMapGeneration::FindRotationWithMesh(FVector location) {
	return FRotator::ZeroRotator;
}

void AMapGeneration::SpawnObject(TArray<FSimpleObject> &arrayObjects) {
	for (int i = 0; i < arrayObjects.Num(); i++) {
		GetWorld()->SpawnActor<AActor>(arrayObjects[i].Object,
									   arrayObjects[i].Location,
									   arrayObjects[i].Rotation,
									   arrayObjects[i].SpawnParams);
	}
}

int8 AMapGeneration::RandomObjectIndex(const TArray<FSimpleObject> &Objects) {
	int8 randomValue = RandomStream.RandRange(0, 100);
	int8 selectedIndex = -1;
	for (int j = 0; j < Objects.Num(); j++) {
		if (randomValue <= Objects[j].Chance.Chance) {
			selectedIndex = j;
			break;
		}
	}
	return selectedIndex;
}

FVector AMapGeneration::RandomPointInSquare(FCollisionPoint &point) {
	int counter = 0;
	float HalfSize = point.radiusColision / 1.42f;
	float X = point.Location.X + RandomStream.FRandRange(-HalfSize, HalfSize);
	float Y = point.Location.Y + RandomStream.FRandRange(-HalfSize, HalfSize);

	return FVector(X, Y, 0.0f);
}

FVector AMapGeneration::RandomPointInCircle(FCollisionPoint &point) {
	float Theta = RandomStream.FRand() * 2.0f * PI;

	// Генерируем случайное расстояние (используем sqrt для равномерного распределения)
	float Distance = FMath::Sqrt(RandomStream.FRand()) * point.radiusColision;

	// Вычисляем координаты точки
	float X = point.Location.X + Distance * FMath::Cos(Theta);
	float Y = point.Location.Y + Distance * FMath::Sin(Theta);

	return FVector(X, Y, 0.0f);
}

void AMapGeneration::GenerateSimpleObjects(TArray<FCollisionPoint> &collisionPoints, FDataMap Data) {
	TArray<uint16> qulityOfObjects = FindProcentQualityObjects(Data.simpleObjects, collisionPoints[0], Data.Density);
	for (int i = 0; i < collisionPoints.Num(); i++) {
		for (int j = 0; j < Data.simpleObjects.Num(); j++) {
			FSimpleObject addedObject = Data.simpleObjects[j];
			for (int k = 0; k < qulityOfObjects[j]; k++) {
				if (FindLocationForObject(addedObject, collisionPoints[i])) {
					FRotator rotation = FRotator::ZeroRotator;
					if (addedObject.RotatesWithMesh) {
						rotation = FindRotationWithMesh(addedObject.Location);
					}
					FVector2D indexChunk = PointInChunkIndex(addedObject.Location);
					arrChunks[indexChunk.X][indexChunk.Y].AddObject(addedObject);
				} else {
					break;
				}
			}
		}
	}
}
#pragma region Hard objects

#pragma region Triangls Find

TArray<TSet<TArray<int32>>> AMapGeneration::FindTrianglesAroundVertex(TArray<int32> startPoints, int MaxDepth) {
	TArray<TSet<TArray<int32>>> returnTriangles;

	TSet<int32> points(startPoints);
	int depth = 0;
	while (depth < MaxDepth || depth >= width) {
		TSet<TArray<int32>> depthTriangles;
		returnTriangles.Add(depthTriangles);
		for (int32 &point : points) {
			// right up square
			TArray<int32> triangle1;
			TArray<int32> triangle2;
			if (point + width + 1 < width * width - 1) {
				triangle1.Add(point);
				triangle1.Add(point + width + 1);
				triangle1.Add(point + width);

				triangle2.Add(point);
				triangle2.Add(point + width);
				triangle2.Add(point + width + 1);

				depthTriangles.Add(triangle1);
				depthTriangles.Add(triangle2);
			}
			// down right triangle
			TArray<int32> triangle3;
			if (point + 1 < width * width - 1 && point - width > 0) {
				triangle3.Add(point - width);
				triangle3.Add(point + 1);
				triangle3.Add(point);

				depthTriangles.Add(triangle3);
			}
			// left down square
			TArray<int32> triangle4;
			TArray<int32> triangle5;
			if (point - width - 1 > 0) {
				triangle4.Add(point - width - 1);
				triangle4.Add(point - 1);
				triangle4.Add(point);

				triangle5.Add(point - width - 1);
				triangle5.Add(point);
				triangle5.Add(point - 1);

				depthTriangles.Add(triangle4);
				depthTriangles.Add(triangle5);
			}
			// up left triangle
			TArray<int32> triangle6;
			if (point - 1 > 0 && point + width < width * width - 1) {
				triangle5.Add(point - 1);
				triangle5.Add(point);
				triangle5.Add(point + width);

				depthTriangles.Add(triangle6);
			}
		}
	}
	RemoveDuplicates(returnTriangles);
	return returnTriangles;
}

void AMapGeneration::RemoveDuplicates(TArray<TSet<TArray<int32>>> &depthTriangles) {
	// Структура для отслеживания уже использованных данных
	TSet<TArray<int32>> UsedData;

	// Перебираем все TSet в порядке их индексов
	for (TSet<TArray<int32>> &currentSet : depthTriangles) {
		TArray<TArray<int32>> itemsToRemove;

		// Проверяем каждый элемент текущего TSet
		for (const TArray<int32> &item : currentSet) {
			if (UsedData.Contains(item)) {
				// Если элемент уже используется, помечаем его для удаления
				itemsToRemove.Add(item);
			} else {
				// Иначе добавляем его в UsedData
				UsedData.Add(item);
			}
		}

		// Удаляем помеченные элементы из текущего TSet
		for (const TArray<int32> &item : itemsToRemove) {
			currentSet.Remove(item);
		}
	}
}
#pragma endregion
#pragma endregion

#pragma region Borders

#pragma region Special borders functions

float AMapGeneration::FunctionBorders_Hills(float x) {
	return FMath::Abs(k1_f * FMath::Sin(x / k1_a) + k2_f * FMath::Cos(x / k2_a) + k1_f + k2_f) + BorderStepLenght;
}
#pragma endregion

TArray<TArray<FCollisionPoint>> AMapGeneration::BordersRegionCreater(float (AMapGeneration::*Function)(float x)) {
	TArray<FCollisionPoint> AB_CollisionPointsBorder;
	TArray<FCollisionPoint> AC_CollisionPointsBorder;
	TArray<FCollisionPoint> BD_CollisionPointsBorder;
	TArray<FCollisionPoint> CD_CollisionPointsBorder;

	minX_inside = 0;
	minY_inside = 0;
	maxX_inside = poligonSize * width;
	maxY_inside = poligonSize * width;

	FCollisionPoint tempCollisionPoint;
	tempCollisionPoint.radiusColision = BorderStepLenght * 0.75f;
	tempCollisionPoint.bBorder = true;
	for (int i = BorderStepLenght; i < width * poligonSize - BorderStepLenght; i += BorderStepLenght) {
		// AB
		tempCollisionPoint.Location = FVector((this->*Function)(i), i, 0.0f);
		CollisionPoints.Add(tempCollisionPoint);

		if (minY_inside < tempCollisionPoint.Location.Y)
			minY_inside = tempCollisionPoint.Location.Y;

		// CD
		tempCollisionPoint.Location = FVector(arrVertix[indexPointD - 1].X - (this->*Function)(i), i, 0.0f);
		CollisionPoints.Add(tempCollisionPoint);

		if (maxY_inside > tempCollisionPoint.Location.Y)
			maxY_inside = tempCollisionPoint.Location.Y;

		// AC
		tempCollisionPoint.Location = FVector(i, (this->*Function)(i), 0.0f);
		CollisionPoints.Add(tempCollisionPoint);

		if (minX_inside < tempCollisionPoint.Location.X)
			minX_inside = tempCollisionPoint.Location.X;
		// BD
		tempCollisionPoint.Location = FVector(i, arrVertix[indexPointB].Y - (this->*Function)(i), 0.0f);
		CollisionPoints.Add(tempCollisionPoint);

		if (maxX_inside > tempCollisionPoint.Location.X)
			maxX_inside = tempCollisionPoint.Location.X;
	}

	auto GenerateExtraCollisionPoints =
		[&](FVector &nodesPoint1, FVector &nodesPoint2, TArray<FCollisionPoint> &result) {
			FVector vector = CreateVectorBy2Points(nodesPoint1, nodesPoint2);
			if (vector.Size() > BorderStepLenght * 1.25) {
				FVector direction = vector.GetSafeNormal();
				FVector step = direction * BorderStepLenght;
				FVector currentPoint = nodesPoint1;

				while (vector.Size() > BorderStepLenght * 1.25) {
					FVector tempPoint = currentPoint + step;
					FCollisionPoint tempCollisionPoint;
					tempCollisionPoint.Location = tempPoint;
					CollisionPoints.Add(tempCollisionPoint);

					currentPoint = tempPoint;
					vector -= step;
				}
			}
		};
	int rangeArray = AB_CollisionPointsBorder.Num();
	for (int i = 0; i < rangeArray - 1; i++) {
		GenerateExtraCollisionPoints(AB_CollisionPointsBorder[i].Location,
									 AB_CollisionPointsBorder[i + 1].Location,
									 AB_CollisionPointsBorder);
		GenerateExtraCollisionPoints(AC_CollisionPointsBorder[i].Location,
									 AC_CollisionPointsBorder[i + 1].Location,
									 AC_CollisionPointsBorder);
		GenerateExtraCollisionPoints(CD_CollisionPointsBorder[i].Location,
									 CD_CollisionPointsBorder[i + 1].Location,
									 CD_CollisionPointsBorder);
		GenerateExtraCollisionPoints(BD_CollisionPointsBorder[i].Location,
									 BD_CollisionPointsBorder[i + 1].Location,
									 BD_CollisionPointsBorder);
	}
	GenerateExtraCollisionPoints(AB_CollisionPointsBorder[rangeArray - 1].Location,
								 BD_CollisionPointsBorder[0].Location,
								 AB_CollisionPointsBorder);
	GenerateExtraCollisionPoints(BD_CollisionPointsBorder[rangeArray - 1].Location,
								 CD_CollisionPointsBorder[rangeArray - 1].Location,
								 BD_CollisionPointsBorder);
	GenerateExtraCollisionPoints(AC_CollisionPointsBorder[rangeArray - 1].Location,
								 CD_CollisionPointsBorder[0].Location,
								 CD_CollisionPointsBorder);
	GenerateExtraCollisionPoints(AB_CollisionPointsBorder[0].Location,
								 AC_CollisionPointsBorder[0].Location,
								 AC_CollisionPointsBorder);

	TArray<TArray<FCollisionPoint>> returnArray;
	returnArray.Add(AB_CollisionPointsBorder);
	returnArray.Add(AC_CollisionPointsBorder);
	returnArray.Add(BD_CollisionPointsBorder);
	returnArray.Add(CD_CollisionPointsBorder);

	return returnArray;
}

void AMapGeneration::BordersObjectsCreate(FDataMap &Data, float (AMapGeneration::*Function)(float x)) {
	TArray<TArray<FCollisionPoint>> BorderCollisionPoints = BordersRegionCreater(Function);

	TArray<TFuture<void>> Futures;

	for (int i = 0; i < 4; i++) {
		Futures.Add(Async(EAsyncExecution::Thread, [this, i, &Data, &BorderCollisionPoints]() {
			GenerateSimpleObjects(BorderCollisionPoints[i], Data);
		}));
	}

	// Дожидаемся задач
	for (auto &Future : Futures) {
		Future.Wait();
	}
	BorderCollisionPoints.Empty();
}
#pragma endregion

#pragma endregion

#pragma region Debug Funclions
void AMapGeneration::Debug_BordersSpawn() {
	for (auto &chunkLine : arrChunks) {
		for (auto &chunk : chunkLine) {
			SpawnObject(chunk.ObjectsInChunk);
		}
	}
}
#pragma endregion
