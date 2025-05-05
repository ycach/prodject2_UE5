// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "NoiseMatrixRow.h"
/*
 *
 */
class MatrixNoise2D {
  public:
	MatrixNoise2D(const int32 lenght, const int32 width);
	~MatrixNoise2D();

  private:
	TArray<NoiseMatrixRow> matrix;
	int32 length;
	int32 width;

  public:
	bool isNormalize() const;
	void Normalize();

	int32 GetLength() const;
	int32 GetWidth() const;

	NoiseMatrixRow &operator[](int32 index);

};
