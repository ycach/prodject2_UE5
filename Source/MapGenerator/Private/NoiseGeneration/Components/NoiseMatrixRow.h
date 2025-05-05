// Devoloper name: PenkovGS

#pragma once
#include "CoreMinimal.h"
/**
 *
 */
class NoiseMatrixRow {
  public:
	NoiseMatrixRow(int32 length);
	~NoiseMatrixRow();

  private:
	TArray<float> row;
	int32 length;

  public:
	bool isNormalize() const;
	void Normalize();

	int32 GetLength() const;

	float &operator[](int32 index);
};
