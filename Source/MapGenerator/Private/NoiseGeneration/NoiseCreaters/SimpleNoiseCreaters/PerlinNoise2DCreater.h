// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "NoiseGeneration/NoiseCreaters/BaseNoise2DCreater.h"
#include "NoiseGeneration/Components/MatrixNoise2D.h"
/**
 *
 */
class PerlinNoise2DCreater : public BaseNoise2DCreater {
  public:
	PerlinNoise2DCreater(const int32 seed, const float scale );
	~PerlinNoise2DCreater() = default;
  protected:
	float PointNoize(const float x, const float y) override; 
};
