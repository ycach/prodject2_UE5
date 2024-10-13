// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGeneration/LandscapeGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Async/Async.h"
#include "Containers/Array.h"

// Sets default values
ALandscapeGenerator::ALandscapeGenerator() {
	PrimaryActorTick.bCanEverTick = true;
}

void ALandscapeGenerator::BeginPlay() {
	Super::BeginPlay();
}

void ALandscapeGenerator::CreateRandomLandscape(UProceduralMeshComponent *MeshComponent,
												TArray<int32> (ALandscapeGenerator::*IndexFind)(int32 A, int32 B,
																								int32 C, int32 D),
												void (ALandscapeGenerator::*StartPointsHights)(void),
												void (ALandscapeGenerator::*ObjectCrate)(void)) {
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

	(this->*StartPointsHights)();

	DiamondSquare(IndexFind);

	MeshComponent->CreateMeshSection(0, arrVertix, arrTriangles, TArray<FVector>(), arrUV, TArray<FColor>(),
									 TArray<FProcMeshTangent>(), true);
	MeshComponent->UpdateBounds();

	(this->*ObjectCrate)();

	arrVertix.Empty();
	arrUV.Empty();
	arrTriangles.Empty();
}

//////////////////////////////////////////////////////////////////////////////////Dimond -
/// Square////////////////////////////////////////////////////////////////////////////

void ALandscapeGenerator::DiamondSquare(TArray<int32> (ALandscapeGenerator::*IndexFind)(int32 A, int32 B, int32 C,
																						int32 D)) {
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
			TArray<int32> pointer = (this->*IndexFind)(arrA_result[j], arrB_result[j], arrC_result[j], arrD_result[j]);
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

///////////////////////////////////////////////////////////////////////////////////Specific
/// Landscape////////////////////////////////////////////////////////////////////////////

////////////Hills///////////////////
#pragma region Hills
void ALandscapeGenerator::GenerateHills(UProceduralMeshComponent *MeshComponent) {
	if (MeshComponent) {
		C1 = FMath::RandRange(c1_min, c1_max);
		C2 = FMath::RandRange(c1_min, c1_max);
		C3 = FMath::RandRange(c2_min, c2_max);
		C4 = FMath::RandRange(c2_min, c2_max);
		H1 = FMath::RandRange(H_min, H_max);
		H2 = FMath::RandRange(H_min, H_max);
		CreateRandomLandscape(MeshComponent, &ALandscapeGenerator::GenerationHillsHeights,
							  &ALandscapeGenerator::GenerationHeightsInPeacksHills,
							  &ALandscapeGenerator::GenerationMapObjects);
	}
}
TArray<int32> ALandscapeGenerator::GenerationHillsHeights(int32 A, int32 B, int32 C, int32 D) {

	int32 half = MidlPointLineIndex(A, C);

	int32 indexMid = MidlPointSquareIndex(A, B);
	arrVertix[indexMid].Z =
		H1 * FMath::Sin((float)(indexMid % width) / C1) * FMath::Cos((float)(indexMid / width) / C2) +
		H2 * FMath::Sin((float)(indexMid / width) / C3) * FMath::Cos((float)(indexMid % width) / C4);

	int32 indexLeft = half + A;
	// if (arrVertix[indexLeft].Z == 0)
	arrVertix[indexLeft].Z =
		H1 * FMath::Sin((float)(indexLeft % width) / C1) * FMath::Cos((float)(indexLeft / width) / C2) +
		H2 * FMath::Sin((float)(indexLeft / width) / C3) * FMath::Cos((float)(indexLeft % width) / C4);

	int32 indexTop = indexMid - half;
	// if (arrVertix[indexTop].Z == 0)
	arrVertix[indexTop].Z =
		H1 * FMath::Sin((float)(indexTop % width) / C1) * FMath::Cos((float)(indexTop / width) / C2) +
		H2 * FMath::Sin((float)(indexTop / width) / C3) * FMath::Cos((float)(indexTop % width) / C4);

	int32 indexRight = B + half;
	// if (arrVertix[indexRight].Z == 0)
	arrVertix[indexRight].Z =
		H1 * FMath::Sin((float)(indexRight % width) / C1) * FMath::Cos((float)(indexRight / width) / C2) +
		H2 * FMath::Sin((float)(indexRight / width) / C3) * FMath::Cos((float)(indexRight % width) / C4);

	int32 indexBot = indexMid + half;
	// if (arrVertix[indexBot].Z == 0)
	arrVertix[indexBot].Z =
		H1 * FMath::Sin((float)(indexBot % width) / C1) * FMath::Cos((float)(indexBot / width) / C2) +
		H2 * FMath::Sin((float)(indexBot / width) / C3) * FMath::Cos((float)(indexBot % width) / C4);

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

void ALandscapeGenerator::GenerationHeightsInPeacksHills() {
	arrVertix[indexPointA].Z =
		H1 * FMath::Sin((float)(indexPointA % width) / C1) * FMath::Cos((float)(indexPointA / width) / C2) +
		H2 * FMath::Sin((float)(indexPointA / width) / C3) * FMath::Cos((float)(indexPointA % width) / C4);

	arrVertix[indexPointB].Z =
		H1 * FMath::Sin((float)(indexPointB % width) / C1) * FMath::Cos((float)(indexPointB / width) / C2) +
		H2 * FMath::Sin((float)(indexPointB / width) / C3) * FMath::Cos((float)(indexPointB % width) / C4);

	arrVertix[indexPointC].Z =
		H1 * FMath::Sin((float)(indexPointC % width) / C1) * FMath::Cos((float)(indexPointC / width) / C2) +
		H2 * FMath::Sin((float)(indexPointC / width) / C3) * FMath::Cos((float)(indexPointC % width) / C4);

	arrVertix[indexPointD].Z =
		H1 * FMath::Sin((float)(indexPointD % width) / C1) * FMath::Cos((float)(indexPointD / width) / C2) +
		H2 * FMath::Sin((float)(indexPointD / width) / C3) * FMath::Cos((float)(indexPointD % width) / C4);
}

void ALandscapeGenerator::GenerationMapObjects() {
}
#pragma endregion
///////////////////////////////////