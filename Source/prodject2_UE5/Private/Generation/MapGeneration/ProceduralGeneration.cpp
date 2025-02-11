#include "Generation/MapGeneration/ProceduralGeneration.h"

int8 AProceduralGeneration::power2;
int32 AProceduralGeneration::poligonSize;
double AProceduralGeneration::scaleUV;
int32 AProceduralGeneration::width;
TArray<FVector> AProceduralGeneration::arrVertix;
TArray<FVector2D> AProceduralGeneration::arrUV;
TArray<int32> AProceduralGeneration::arrTriangles;
float AProceduralGeneration::minX_inside;
float AProceduralGeneration::maxX_inside;
float AProceduralGeneration::minY_inside;
float AProceduralGeneration::maxY_inside;
double AProceduralGeneration::oneChankSizeSide;
int AProceduralGeneration::CountOfChanksInSide;
int32 AProceduralGeneration::indexPointA;
int32 AProceduralGeneration::indexPointB;
int32 AProceduralGeneration::indexPointC;
int32 AProceduralGeneration::indexPointD;

AProceduralGeneration::AProceduralGeneration() {
	PrimaryActorTick.bCanEverTick = false;
}

void AProceduralGeneration::BeginPlay() {
	Super::BeginPlay();
}


void AProceduralGeneration::ClearFirst() {
	arrVertix.Empty();
	arrUV.Empty();
	arrTriangles.Empty();
}


#pragma region Generation Mesh
void AProceduralGeneration::CreateRandomLandscape(float (AProceduralGeneration::*Function)(float x)) {
	SetValuesWorld();



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
}

TArray<int32> AProceduralGeneration::GenerationHeights(int32 A, int32 B, int32 C, int32 D,
													   float (AProceduralGeneration::*Function)(float x)) {

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

void AProceduralGeneration::GenerationHeightsInPeacks(float (AProceduralGeneration::*Function)(float x)) {
	arrVertix[indexPointA].Z = (this->*Function)(indexPointA);
	arrVertix[indexPointB].Z = (this->*Function)(indexPointB);
	arrVertix[indexPointC].Z = (this->*Function)(indexPointC);
	arrVertix[indexPointD].Z = (this->*Function)(indexPointD);
}

void AProceduralGeneration::CalculateNormals(const TArray<FVector> &Vertices, const TArray<int32> &Triangles,
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

void AProceduralGeneration::CalculateTangents(const TArray<FVector> &Vertices, const TArray<int32> &Triangles,
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
void AProceduralGeneration::DiamondSquare(float (AProceduralGeneration::*Function)(float x)) {
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

int32 AProceduralGeneration::MidlPointSquareIndex(int32 A, int32 B) {
	return ((B - A) / 2) * (width + 1) + A;
}

int32 AProceduralGeneration::MidlPointLineIndex(int32 point1, int32 point2) {
	return (point2 - point1) / 2;
}

#pragma endregion

#pragma region Generation Procedural Mesp In World

void AProceduralGeneration::CreateProceduralMash(UProceduralMeshComponent *MeshComponent) {

	MeshComponent->CreateMeshSection(0, arrVertix, arrTriangles, TArray<FVector>(), arrUV, TArray<FColor>(),
									 TArray<FProcMeshTangent>(), true);
	MeshComponent->UpdateBounds();
}


#pragma region Hills

float AProceduralGeneration::FunctionHillsHeights(float point) {
	return H1 * FMath::Sin((float)((int)point % width) / C1) * FMath::Cos((float)(point / width) / C2) +
		   H2 * FMath::Sin((float)(point / width) / C3) * FMath::Cos((float)((int)point % width) / C4);
}



void AProceduralGeneration::GenerateHills(UProceduralMeshComponent *MeshComponent) {
	if (MeshComponent) {
		C1 = FMath::RandRange(c1_min, c1_max);
		C2 = FMath::RandRange(c1_min, c1_max);
		C3 = FMath::RandRange(c2_min, c2_max);
		C4 = FMath::RandRange(c2_min, c2_max);
		H1 = FMath::RandRange(H_min, H_max);
		H2 = FMath::RandRange(H_min, H_max);
		CreateRandomLandscape(&AProceduralGeneration::FunctionHillsHeights);
	}
}

#pragma endregion

#pragma endregion



void AProceduralGeneration::SetValuesWorld() {
	power2 = this->BPSet_Power2;
	poligonSize = this->BPSet_PoligonSize;
	scaleUV = this->BPSet_ScaleUV;

	width = FMath::Pow(2.0, power2) + 1;

	SetChanksCount();

	minX_inside = 0;
	maxX_inside = (width - 1) * poligonSize;

	minY_inside = 0;
	maxY_inside = (width - 1) * poligonSize;
}


void AProceduralGeneration::SetChanksCount() {
	oneChankSizeSide = ((poligonSize * (width - 1)) / BPSet_countOfChanksInSide);
	CountOfChanksInSide = BPSet_countOfChanksInSide;
}

int32 AProceduralGeneration::ChunkNumberInWhichObjectLocated(FVector location) {

	if (location.X == (poligonSize * (width - 1)) && location.Y == (poligonSize * (width - 1))) {
		return CountOfChanksInSide * CountOfChanksInSide - 1;
	}
	if (location.X == (poligonSize * (width - 1))) {
		return ((int)(location.X / oneChankSizeSide) - 1) * CountOfChanksInSide +
			   (int)(location.Y / oneChankSizeSide) - 1;
	}
	if (location.Y == (poligonSize * (width - 1))){
		return (int)(location.X / oneChankSizeSide) * CountOfChanksInSide +
			   (int)(location.Y / oneChankSizeSide) - 1;
	}

	return (int)(location.X / oneChankSizeSide) * CountOfChanksInSide +
		   (int)(location.Y / oneChankSizeSide);

}

bool AProceduralGeneration::IsBorderChank(int32 indexChank) {
	if ((int)(indexChank / CountOfChanksInSide) == 0 || (indexChank % CountOfChanksInSide) == 0 ||
		(int)(indexChank / CountOfChanksInSide) == CountOfChanksInSide - 1 ||
		(indexChank % CountOfChanksInSide) == CountOfChanksInSide - 1
		)
		return 1;
	return 0;
}

TArray<double> AProceduralGeneration::ChankBordersCoordinate(int32 indexChank) {
	TArray<double> arryBordersPoints; // minX, minY, maxX, maxY
	// Вычисляем minX и minY
	double minX = (indexChank / CountOfChanksInSide) * oneChankSizeSide;
	double minY = (indexChank % CountOfChanksInSide) * oneChankSizeSide;

	// Вычисляем maxX и maxY
	double maxX = minX + oneChankSizeSide;
	double maxY = minY + oneChankSizeSide;

	// Добавляем значения в массив
	arryBordersPoints.Add(minX);
	arryBordersPoints.Add(minY);
	arryBordersPoints.Add(maxX);
	arryBordersPoints.Add(maxY);
	
	return arryBordersPoints; // minX, minY, maxX, maxY
}

