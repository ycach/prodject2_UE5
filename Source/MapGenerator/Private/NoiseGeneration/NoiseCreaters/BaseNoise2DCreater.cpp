// Devoloper name: PenkovGS

#include "NoiseGeneration/NoiseCreaters/BaseNoise2DCreater.h"

BaseNoise2DCreater::BaseNoise2DCreater(const int32 seed, const float scale) {
	hasher = MakeUnique<Hasher2D>(seed);
	coordinate_scale = scale;
}
TSharedPtr<MatrixNoise2D> BaseNoise2DCreater::GetNoiseMatrix(const int32 length, const int32 width){
	GenerateNoise(length, width);
	if (!matrix) {
		UE_LOG(LogTemp, Warning, TEXT("Noise matrix null pointer was return"));
	}
	return matrix;
}

FVector2D BaseNoise2DCreater::ScaleStartCoordinate(const float x, const float y) {
	return FVector2D(x * coordinate_scale, y * coordinate_scale);
}

void BaseNoise2DCreater::GenerateNoise(const int32 length, const int32 width) {
	matrix = MakeShared<MatrixNoise2D>(length, width);
	ParallelFor(length, [&](int32 index){ 
		for (int32 i = 0; i < width; i++){
			(*matrix)[index][i] = PointNoize(i, index);
		}
	});
}

void BaseNoise2DCreater::SetSeed(const int32 seed) {
	hasher->SetSeed(seed);
}

void BaseNoise2DCreater::SetCoordinateScale(const float scale) {
	coordinate_scale = scale;
}

int32 BaseNoise2DCreater::GetSeed() const {
	return hasher->GetSeed();
}


