// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGeneration/MapGeneration.h"

AMapGeneration::AMapGeneration() {
	PrimaryActorTick.bCanEverTick = false;
}

void AMapGeneration::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) {
	ALandscapeGenerator::PostEditChangeProperty(PropertyChangedEvent);
#pragma region TaskCountBorders
	// Если изменилось свойство power2
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("power2")) {
		// Обновляем TaskCountBorders, если оно больше нового значения power2
		if (TaskCountBorder > power2)
			TaskCountBorder = power2;
	}
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("TaskCountBorder")) {
		// Обновляем TaskCountBorders, если оно больше нового значения power2
		if (TaskCountBorder > power2)
			TaskCountBorder = power2;
	}
#pragma endregion
#pragma region DistanceBeatween
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("poligonSize")) {
		// Обновляем TaskCountBorders, если оно больше нового значения power2
		if (DistanceBetween > poligonSize)
			DistanceBetween = poligonSize;
	}
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("DistanceBetween")) {
		// Обновляем TaskCountBorders, если оно больше нового значения power2
		if (DistanceBetween > poligonSize)
			DistanceBetween = poligonSize;
	}
#pragma endregion
}

TArray<FChanceGroup> AMapGeneration::NormalizeChanses(TArray<FChance> arrChance) {
	return Normalize::NormalizeChance(arrChance);
}

FRotator AMapGeneration::FindRotationWithMesh(FVector location) {
	return FRotator::ZeroRotator;
}

int8 AMapGeneration::RandomObjectIndex(const TArray<FMapObject> &Objects) {
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

float AMapGeneration::ZCoordinateFind(FVector point, bool bOy_true) {
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

void AMapGeneration::SpawnObject(TArray<FMapObject> &arrayObjects) {
	for (int i = 0; i < arrayObjects.Num(); i++) {
		GetWorld()->SpawnActor<AActor>(arrayObjects[i].Actor,
									   arrayObjects[i].Location,
									   arrayObjects[i].Rotation,
									   arrayObjects[i].SpawnParams);
	}
}

#pragma region Borders
float AMapGeneration::FunctionBorders_Hills(float x) {
	return k1_f * FMath::Sin(x / k1_a) + k2_f * FMath::Cos(x / k2_a) + k1_f + k2_f;
}

// Доработать, не учитывается radius colision в обе стороны
void AMapGeneration::FindBordersObjectCoordinate(const FVector &Start,
												 const FVector &End,
												 const TArray<FMapObject> &Objects,
												 TArray<FMapObject> &returnObjects) {
	float Spacing = DistanceBetween;
	FVector direction = (End - Start).GetSafeNormal();
	for (float i = 0; i < poligonSize * TaskCountBorder; i += Spacing) {
		// Используем локальный генератор
		int8 selectedIndex = -1; // Инициализируем индекс
		selectedIndex = RandomObjectIndex(Objects);

		Spacing = Objects[selectedIndex].RadiusColision;
		FVector location = Start + direction * i;

		FRotator rotation = FRotator::ZeroRotator;
		if (Objects[selectedIndex].RotationWithMesh) {
			rotation = FindRotationWithMesh(location);
		}
		FMapObject temp = Objects[selectedIndex];
		temp.SetLocation(location);
		temp.SetRotation(rotation);

		int chanksInRow = arrChunks.Num();
		if ((arrChunks[0][0].inChunk(&Start) && arrChunks[0][chanksInRow - 1].inChunk(&End)) ||
			(arrChunks[0][0].inChunk(&End) && arrChunks[0][chanksInRow - 1].inChunk(&Start))) { // AB
			for (Chunk &chunk : arrChunks[0]) {
				if (chunk.inChunk(&location)) {
					chunk.AddObject(temp);
					break;
				}
			}
		}
		else if ((arrChunks[0][0].inChunk(&Start) && arrChunks[chanksInRow - 1][0].inChunk(&End)) ||
				   (arrChunks[0][0].inChunk(&End) && arrChunks[chanksInRow - 1][0].inChunk(&Start))) { // AC
			for (int j = 0; j <= chanksInRow * chanksInRow - chanksInRow; j += chanksInRow) {
				if (arrChunks[j][0].inChunk(&location)) {
					arrChunks[j][0].AddObject(temp);
					break;
				}
			}
		} 
		else if ((arrChunks[chanksInRow - 1][0].inChunk(&Start) &&
					arrChunks[chanksInRow - 1][chanksInRow - 1].inChunk(&End)) ||
				   (arrChunks[chanksInRow - 1][0].inChunk(&End) &&
					arrChunks[chanksInRow - 1][chanksInRow - 1].inChunk(&Start))) { // CD
			for (Chunk &chunk : arrChunks[chanksInRow - 1]) {
				if (chunk.inChunk(&location)) {
					chunk.AddObject(temp);
					break;
				}
			}
		} 
		else if ((arrChunks[chanksInRow - 1][0].inChunk(&Start) &&
					   arrChunks[chanksInRow - 1][chanksInRow - 1].inChunk(&End)) ||
				   (arrChunks[chanksInRow - 1][0].inChunk(&End) &&
					arrChunks[chanksInRow - 1][chanksInRow - 1].inChunk(&Start))) { // BD
			for (int j = 0; j <= chanksInRow * chanksInRow - chanksInRow; j += chanksInRow) {
				if (arrChunks[j][chanksInRow - 1].inChunk(&location)) {
					arrChunks[j][chanksInRow - 1].AddObject(temp);
					break;
				}
			}
		} 
		else {
			for (int row = 0; row < chanksInRow; row++) {
				for (int col = 0; col < chanksInRow; col++) {
					if (arrChunks[row][col].inChunk(&location)) {
						arrChunks[row][col].AddObject(temp);
						break;
					}

				}	
			}
		}

		returnObjects.Add(temp);
	}
}

void AMapGeneration::BordersObjectsCreate(const TArray<FMapObject> &Objects,
										  float (AMapGeneration::*Function)(float x)) {
	int Spacing = TaskCountBorder;

	k1_f = FMath::RandRange(kf_min, kf_max);
	k2_f = FMath::RandRange(kf_min, kf_max);
	k1_a = FMath::RandRange(ka_min, ka_max);
	k2_a = FMath::RandRange(ka_min, ka_max);

	TArray<TArray<FVector>> arrayNodes = NodesYPoints(Function);

	TArray<TFuture<void>> Futures;
	TArray<FMapObject> AB_objects;
	TArray<FMapObject> AC_objects;
	TArray<FMapObject> BD_objects;
	TArray<FMapObject> CD_objects;

	for (int i = 0; i < width - Spacing; i += Spacing) {
		Futures.Add(
			Async(EAsyncExecution::Thread,
				  [this, i, Spacing, &arrayNodes, &Objects, &AB_objects, &AC_objects, &BD_objects, &CD_objects]() {
					  // AB
					  FindBordersObjectCoordinate(arrayNodes[0][i], arrayNodes[0][i + Spacing], Objects, AB_objects);

					  // AC
					  FindBordersObjectCoordinate(arrayNodes[1][i], arrayNodes[1][i + Spacing], Objects, AC_objects);

					  // BD
					  FindBordersObjectCoordinate(arrayNodes[2][i], arrayNodes[2][i + Spacing], Objects, BD_objects);

					  // CD
					  FindBordersObjectCoordinate(arrayNodes[3][i], arrayNodes[3][i + Spacing], Objects, CD_objects);
				  }));
	}

	// Дожидаемся задач
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

TArray<TArray<FVector>> AMapGeneration::NodesYPoints(float (AMapGeneration::*Function)(float x)) {
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
							 arrVertix[width * (width - 1) + i].Y,
							 0.0f);
		tempVector.Z = ZCoordinateFind(tempVector, 1); // 1
		CD_NodesPointsBorder.Add(tempVector);

		// AC
		tempVector = FVector(arrVertix[i * width].X, (this->*Function)(arrVertix[i * width].X), 0.0f);
		tempVector.Z = ZCoordinateFind(tempVector, 0); // 0
		AC_NodesPointsBorder.Add(tempVector);
		// BD
		tempVector = FVector(arrVertix[(width - 1) + i * width].X,
							 arrVertix[indexPointB - 1].Y - (this->*Function)(arrVertix[(width - 1) + i * width].X),
							 0.0f);
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

void AMapGeneration::AnalyzeBorderPoints(const TArray<TArray<FVector>> &BorderPoints,
										 float &MaxY,
										 float &MinY,
										 float &MaxX,
										 float &MinX) {
	MinY = poligonSize * (width - 1) / 2.;
	MaxY = poligonSize * (width - 1) / 2.;
	MinY = poligonSize * (width - 1) / 2.;
	MinY = poligonSize * (width - 1) / 2.;

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

void AMapGeneration::GenerateObjectsMap_Hills(const TArray<FMapObject> &Objects_Borders,
											  const TArray<FMapObject> &Objects_Map,
											  float densities,
											  int deltaQualityObjects) {
	BordersObjectsCreate(Objects_Borders, &AMapGeneration::FunctionBorders_Hills);
	// PlayeblMapObjectsGeneration(Objects_Map, densities, deltaQualityObjects);
}

#pragma region Debug Funclions
void AMapGeneration::Debug_BordersSpawn(bool bAB_spawn, bool bAC_spawn, bool bBD_spawn, bool bCD_spawn) {
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
