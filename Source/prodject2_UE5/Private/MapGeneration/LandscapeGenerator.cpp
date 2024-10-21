﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGeneration/LandscapeGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Async/Async.h"
#include "Containers/Array.h"

#define DEBUG_BP_FUNCTIONS

// Sets default values
ALandscapeGenerator::ALandscapeGenerator() {
	PrimaryActorTick.bCanEverTick = false;
	RandomStream.GenerateNewSeed();
}

void ALandscapeGenerator::BeginPlay() {
	Super::BeginPlay();
}

void ALandscapeGenerator::ClearFirst() {
	arrVertix.Empty();
	arrUV.Empty();
	arrTriangles.Empty();
}
#pragma region ObjectsSpawn
void ALandscapeGenerator::CreateRandomLandscape(UProceduralMeshComponent *MeshComponent,
												float (ALandscapeGenerator::*Function)(float x)) {
	width = pow(2, power2) + 1;
	int count = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < width; j++) {

			// Vertix Generation
			arrVertix.Add(FVector(i * poligonSize, j * poligonSize, 0));

			// Triangles Generation
			//  quad convert to two triangls, a quade has vertices at points 0123, triangls in 012 and 023

			if (i != width - 1 && j != width - 1) {
				arrTriangles.Add(j + width * i);		   // 0
				arrTriangles.Add(j + 1 + width * i);	   // 1
				arrTriangles.Add(j + 1 + width * (i + 1)); // 2
				arrTriangles.Add(j + width * i);		   // 0
				arrTriangles.Add(j + 1 + width * (i + 1)); // 2
				arrTriangles.Add(j + width * (i + 1));	   // 3
			}

			// UV Generation (material ad it size)
			arrUV.Add(FVector2D(1.0 / (width - 1) * i * scaleUV, 1.0 / (width - 1) * j * scaleUV));
		}
	}

	indexPointA = 0;
	indexPointB = width - 1;
	indexPointC = arrVertix.Num() - width;
	indexPointD = arrVertix.Num() - 1;

	GenerationHeightsInPeacks(Function);

	DiamondSquare(Function);

	MeshComponent->CreateMeshSection(0, arrVertix, arrTriangles, TArray<FVector>(), arrUV, TArray<FColor>(),
									 TArray<FProcMeshTangent>(), true);
	MeshComponent->UpdateBounds();
}

TArray<int32> ALandscapeGenerator::GenerationHeights(int32 A, int32 B, int32 C, int32 D,
													 float (ALandscapeGenerator::*Function)(float x)) {

	int32 half = MidlPointLineIndex(A, C);

	int32 indexMid = MidlPointSquareIndex(A, B);
	arrVertix[indexMid].Z = (this->*Function)(indexMid);

	int32 indexLeft = half + A;

	if (arrVertix[indexLeft].Z == 0)
		arrVertix[indexLeft].Z = (this->*Function)(indexLeft);

	int32 indexTop = indexMid - half;
	if (arrVertix[indexTop].Z == 0)
		arrVertix[indexTop].Z = (this->*Function)(indexTop);

	int32 indexRight = B + half;
	if (arrVertix[indexRight].Z == 0)
		arrVertix[indexRight].Z = (this->*Function)(indexRight);

	int32 indexBot = indexMid + half;
	if (arrVertix[indexBot].Z == 0)
		arrVertix[indexBot].Z = (this->*Function)(indexBot);

	TArray<int32> arrReturn;
	arrReturn.Add(A);
	arrReturn.Add(B);
	arrReturn.Add(C);
	arrReturn.Add(D);
	arrReturn.Add(indexMid);
	arrReturn.Add(indexLeft);
	arrReturn.Add(indexTop);
	arrReturn.Add(indexRight);
	arrReturn.Add(indexBot);

	return arrReturn;
}

void ALandscapeGenerator::GenerationHeightsInPeacks(float (ALandscapeGenerator::*Function)(float x)) {
	arrVertix[indexPointA].Z = (this->*Function)(indexPointA);
	arrVertix[indexPointB].Z = (this->*Function)(indexPointB);
	arrVertix[indexPointC].Z = (this->*Function)(indexPointC);
	arrVertix[indexPointD].Z = (this->*Function)(indexPointD);
}

void ALandscapeGenerator::CalculateNormals(const TArray<FVector> &Vertices, const TArray<int32> &Triangles,
										   TArray<FVector> &Normals) {
	Normals.SetNum(Vertices.Num());

	for (int32 i = 0; i < Triangles.Num(); i += 3) {
		int32 Index0 = Triangles[i];
		int32 Index1 = Triangles[i + 1];
		int32 Index2 = Triangles[i + 2];

		FVector Vertex0 = Vertices[Index0];
		FVector Vertex1 = Vertices[Index1];
		FVector Vertex2 = Vertices[Index2];

		FVector Edge1 = Vertex1 - Vertex0;
		FVector Edge2 = Vertex2 - Vertex0;

		FVector Normal = -FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

		Normals[Index0] += Normal;
		Normals[Index1] += Normal;
		Normals[Index2] += Normal;
	}

	for (FVector &Normal : Normals) {
		Normal.Normalize();
	}
}

void ALandscapeGenerator::CalculateTangents(const TArray<FVector> &Vertices, const TArray<int32> &Triangles,
											const TArray<FVector> &Normals, const TArray<FVector2D> &UV0,
											TArray<FProcMeshTangent> &Tangents) {
	Tangents.SetNum(Vertices.Num());

	for (int32 i = 0; i < Triangles.Num(); i += 3) {
		int32 Index0 = Triangles[i];
		int32 Index1 = Triangles[i + 1];
		int32 Index2 = Triangles[i + 2];

		FVector Vertex0 = Vertices[Index0];
		FVector Vertex1 = Vertices[Index1];
		FVector Vertex2 = Vertices[Index2];

		FVector2D UV00 = UV0[Index0];
		FVector2D UV10 = UV0[Index1];
		FVector2D UV20 = UV0[Index2];

		FVector Edge1 = Vertex1 - Vertex0;
		FVector Edge2 = Vertex2 - Vertex0;

		FVector2D DeltaUV1 = UV10 - UV00;
		FVector2D DeltaUV2 = UV20 - UV00;

		float r = 1.0f / (DeltaUV1.X * DeltaUV2.Y - DeltaUV1.Y * DeltaUV2.X);
		FVector Tangent = (Edge1 * DeltaUV2.Y - Edge2 * DeltaUV1.Y) * r;
		FVector Bitangent = (Edge2 * DeltaUV1.X - Edge1 * DeltaUV2.X) * r;

		Tangents[Index0].TangentX += Tangent;
		Tangents[Index1].TangentX += Tangent;
		Tangents[Index2].TangentX += Tangent;
	}

	for (int32 i = 0; i < Vertices.Num(); ++i) {
		FVector Normal = Normals[i];
		FVector Tangent = Tangents[i].TangentX;

		// Ортогонализация тангента
		Tangent = (Tangent - Normal * (Normal | Tangent)).GetSafeNormal();

		// Вычисление бинормали
		FVector Bitangent = FVector::CrossProduct(Normal, Tangent);

		Tangents[i] = FProcMeshTangent(Tangent, Bitangent.Z < 0.0f);
	}
}

#pragma endregion
//////////////////////////////////////////////////////////////////////////////////Dimond -
#pragma region Dimond -Square
void ALandscapeGenerator::DiamondSquare(float (ALandscapeGenerator::*Function)(float x)) {
	TArray<int32> arrA_result;
	arrA_result.Init(0, pow(width, 2) / 4);
	arrA_result[0] = indexPointA;

	TArray<int32> arrB_result;
	arrB_result.Init(0, pow(width, 2) / 4);
	arrB_result[0] = indexPointB;

	TArray<int32> arrC_result;
	arrC_result.Init(0, pow(width, 2) / 4);
	arrC_result[0] = indexPointC;

	TArray<int32> arrD_result;
	arrD_result.Init(0, pow(width, 2) / 4);
	arrD_result[0] = indexPointD;

	int32 counter = 1;

	for (int16 i = 0; i < power2; i++) {
		TArray<int32> arrA_temp;
		TArray<int32> arrB_temp;
		TArray<int32> arrC_temp;
		TArray<int32> arrD_temp;

		// Резервируем память для временных массивов
		arrA_temp.SetNum(counter * 4);
		arrB_temp.SetNum(counter * 4);
		arrC_temp.SetNum(counter * 4);
		arrD_temp.SetNum(counter * 4);

		// Параллельное выполнение
		ParallelFor(counter, [&](int32 j) {
			TArray<int32> pointer =
				GenerationHeights(arrA_result[j], arrB_result[j], arrC_result[j], arrD_result[j], Function);
			if (i + 1 < power2) {
				int32 indexCount = j * 4; // Индекс для вставки в временные массивы

				arrA_temp[indexCount] = pointer[0];
				arrB_temp[indexCount] = pointer[6];
				arrC_temp[indexCount] = pointer[5];
				arrD_temp[indexCount] = pointer[4];

				arrA_temp[indexCount + 1] = pointer[6];
				arrB_temp[indexCount + 1] = pointer[1];
				arrC_temp[indexCount + 1] = pointer[4];
				arrD_temp[indexCount + 1] = pointer[7];

				arrA_temp[indexCount + 2] = pointer[5];
				arrB_temp[indexCount + 2] = pointer[4];
				arrC_temp[indexCount + 2] = pointer[2];
				arrD_temp[indexCount + 2] = pointer[8];

				arrA_temp[indexCount + 3] = pointer[4];
				arrB_temp[indexCount + 3] = pointer[7];
				arrC_temp[indexCount + 3] = pointer[8];
				arrD_temp[indexCount + 3] = pointer[3];
			}

			pointer.Empty();
		});

		if (i + 1 < power2) {
			for (int32 k = 0; k < counter * 4; k++) {
				arrA_result[k] = arrA_temp[k];
				arrB_result[k] = arrB_temp[k];
				arrC_result[k] = arrC_temp[k];
				arrD_result[k] = arrD_temp[k];
			}
		}

		counter *= 4;
	}

	// Очистка массивов
	arrA_result.Empty();
	arrB_result.Empty();
	arrC_result.Empty();
	arrD_result.Empty();
}

int32 ALandscapeGenerator::MidlPointSquareIndex(int32 A, int32 B) {
	return ((B - A) / 2) * (width + 1) + A;
}

int32 ALandscapeGenerator::MidlPointLineIndex(int32 point1, int32 point2) {
	return (point2 - point1) / 2;
}
#pragma endregion

#pragma region Random Generates objects for all types landscape

bool ALandscapeGenerator::IsLocationValid(const FVector &Location,
										  const TArray<FObjectThatHaveLocation> &SpawnedLocations,
										  int32 RadiusColision) {
	// Проверяем каждую уже размещённую локацию
	for (const auto &SpawnedLocation : SpawnedLocations) {
		float DistanceToSpawned = FVector::Dist(Location, SpawnedLocation.Location);

		// Проверяем условия на расстояние
		if (DistanceToSpawned < RadiusColision && DistanceToSpawned < SpawnedLocation.RadiusColision &&
			FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(minX_inside, Location.Y, 0.0f)) <
				RadiusColision &&
			FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(maxX_inside, Location.Y, 0.0f)) <
				RadiusColision &&
			FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(Location.X, minY_inside, 0.0f)) <
				RadiusColision &&
			FVector::Dist(FVector(Location.X, Location.Y, 0.0f), FVector(Location.X, maxY_inside, 0.0f)) <
				RadiusColision) {
			return false; // Слишком близко к другому объекту
		}
	}
	return true; // Место свободно
}

void ALandscapeGenerator::SpawnObject(TArray<FObjectThatHaveLocation> &arrayObjects) {
	for (int i = 0; i < arrayObjects.Num(); i++) {
		GetWorld()->SpawnActor<AActor>(arrayObjects[i].Object, arrayObjects[i].Location, arrayObjects[i].Rotation,
									   arrayObjects[i].SpawnParams);
	}
}

int8 ALandscapeGenerator::IndexOfObjectToGeneration(const TArray<FMapObject> &Objects) {
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

FRotator ALandscapeGenerator::FindRotationWithMesh(FVector location) {

	return FRotator();
}

float ALandscapeGenerator::ZCoordinateFind(FVector point) {
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

float ALandscapeGenerator::ZCoordinateFind(FVector point, bool bOy_true) {
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
		FObjectThatHaveLocation object_to_add =
			FObjectThatHaveLocation(location, rotation, Objects[selectedIndex].Actor, SpawnParams);
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

#pragma region ObjectsGenerate

void ALandscapeGenerator::PlayeblMapObjectsGeneration(const TArray<FMapObject> &Objects, float densities, int8 delta) {

	const int8 MaxTry = 10; // максимальное количество попыток сгенерировать точку с объектом

	TArray<uint16> count_of_objects = FindProcentQualityObjects(Objects, densities);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

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

			// определяем какой объект будет создан
			int8 indexOfObject = IndexOfObjectToGeneration(Objects);

			// Генерируем случайную точку внутри BoundingBox
			FVector RandomLocation = GetRandomPointInPlaybleMap();
			RandomLocation.Z = ZCoordinateFind(RandomLocation);

			if (IsLocationValid(RandomLocation, SpawnedObjectsOnMap, Objects[indexOfObject].RadiusColision)) {
				if (Objects[indexOfObject].RotationWithMesh) {
					rotation = FindRotationWithMesh(RandomLocation);
				}

				FObjectThatHaveLocation object_to_add =
					FObjectThatHaveLocation(RandomLocation, rotation, Objects[indexOfObject].Actor, SpawnParams);
				SpawnedObjectsOnMap.Add(object_to_add);
			}
		}
	}
}

TArray<uint16> ALandscapeGenerator::FindProcentQualityObjects(const TArray<FMapObject> &Objects, float &densities) {

	float k = 0; // коээфициент относительной вместимости
	float sum = 0;

	TArray<uint16> RelativeDensities; // массив количества объектов для каждого типа

	sum += FindSquare(Objects[0].RadiusColision) * Objects[0].Chance;
	for (int i = 1; i < Objects.Num(); i++) {
		sum += FindSquare(Objects[i].RadiusColision) * (Objects[i].Chance - Objects[i - 1].Chance);
	}

	k = (FindSquarePlaybleMap() * densities / 100.0f) / sum;

	RelativeDensities.Add(Objects[0].Chance * k);
	for (int i = 1; i < Objects.Num(); i++) {
		RelativeDensities.Add((Objects[i].Chance - Objects[i - 1].Chance) * k);
	}

	return RelativeDensities;
}

float ALandscapeGenerator::FindSquare(float r) {
	return FMath::Pow(r, 2.0f) * 3.2;
}

float ALandscapeGenerator::FindSquarePlaybleMap() {
	return (maxY_inside - minY_inside) * (maxX_inside - minX_inside);
}

FVector ALandscapeGenerator::GetRandomPointInPlaybleMap() {
	// Генерируем случайные координаты внутри ограничивающего прямоугольника

	float RandomX = FMath::FRandRange(minX_inside, maxX_inside);
	float RandomY = FMath::FRandRange(minY_inside, maxY_inside);

	return FVector(RandomX, RandomY, 0.0f);
}

#pragma endregion

#pragma endregion

///////////////////////////////////////////////////////////////////////////////////Specific
/// Landscape////////////////////////////////////////////////////////////////////////////
#pragma region Hills
void ALandscapeGenerator::GenerateHills(UProceduralMeshComponent *MeshComponent) {
	if (MeshComponent) {
		C1 = FMath::RandRange(c1_min, c1_max);
		C2 = FMath::RandRange(c1_min, c1_max);
		C3 = FMath::RandRange(c2_min, c2_max);
		C4 = FMath::RandRange(c2_min, c2_max);
		H1 = FMath::RandRange(H_min, H_max);
		H2 = FMath::RandRange(H_min, H_max);
		CreateRandomLandscape(MeshComponent, &ALandscapeGenerator::FunctionHillsHeights);
	}
}

float ALandscapeGenerator::FunctionHillsHeights(float point) {
	return H1 * FMath::Sin((float)((int)point % width) / C1) * FMath::Cos((float)(point / width) / C2) +
		   H2 * FMath::Sin((float)(point / width) / C3) * FMath::Cos((float)((int)point % width) / C4);
}

void ALandscapeGenerator::GenerateObjectsOnMap_Hills(int indentForBorders, const TArray<FMapObject> &Objects_Borders,
													 const TArray<FMapObject> &Objects_Map, float densities,
													 int deltaQualityObjects) {
	BordersObjectsCreate(indentForBorders, Objects_Borders, &ALandscapeGenerator::FunctionBorders);
	PlayeblMapObjectsGeneration(Objects_Map, densities, deltaQualityObjects);
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

void ALandscapeGenerator::Debug_ObjectsSpawn(bool bSpawn) {
	if (bSpawn)
		SpawnObject(SpawnedObjectsOnMap);
}
#pragma endregion
#endif //  DEBUG_BP_FUNCTIONS
