// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "NoiseGeneration/NoiseCreaters/BaseNoise2DCreater.h"
#include "NoiseGeneration/Components/MatrixNoise2D.h"

/**
 *
 */
class FractalGenerator {
  public:
	enum class FractalType {
		FBM
	};
	FractalGenerator(const float gain, const float lacunarity, const float weight_octave, const float fractal_bounding);
	~FractalGenerator() = default;

  private:
	float gain;
	float lacunarity;
	float weight_octave;
	float fractal_bounding;

	FractalType fractal_type;
  public:
	void GenerateFractalNoise(BaseNoise2DCreater *noise_creater, const int32 length, const int32 width,
							  const int32 octaves);
	void SetGain(const float new_gain);
	void SetLacunarity(const float new_lacunarity);
	void SetOctaveWeight(const float new_weight_octave);
	void SetFractalBounding(const float new_fractal_bounding);

	void SetFractalType(FractalType type);

  private:
	void FBMFractal(BaseNoise2DCreater *noise_creater, const int32 length, const int32 width, const int32 octaves);
};
