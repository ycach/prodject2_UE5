// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "NoiseGeneration/Components/MatrixNoise2D.h"
#include "NoiseGeneration/Interpolation/InterpolationNoise.h"
#include "NoiseGeneration/Hashers/Hashers.h"

/**
 *
 */
class FractalGenerator;

class BaseNoise2DCreater {
  public:
	BaseNoise2DCreater(const int32 seed, const float scale);
	virtual ~BaseNoise2DCreater() = default;

  private:
	friend class FractalGenerator;

  protected:
	TSharedPtr<MatrixNoise2D> matrix;
	TUniquePtr<Hasher2D> hasher;
	InterpolationNoise interpolator;

	float coordinate_scale;

  public:
	TSharedPtr<MatrixNoise2D> GetNoiseMatrix(const int32 length, const int32 width);

	void SetSeed(const int32 seed);
	void SetCoordinateScale(const float scale);
	int32 GetSeed() const;

  protected:
	virtual FVector2D ScaleStartCoordinate(const float x, const float y);
	virtual float PointNoize(const float x, const float y) = 0;

  private:
	void GenerateNoise(const int32 length, const int32 width);
};
