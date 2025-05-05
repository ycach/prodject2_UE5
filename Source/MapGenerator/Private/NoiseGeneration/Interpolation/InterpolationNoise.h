// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class InterpolationNoise {
  public:
	enum class InterpolationConstants {
		Defalt,
		DaveHoskins,
		Gustavson,
		PerlinOriginal
	};
	enum class InterpolationFunction {
		Quintic
	};
	InterpolationNoise();
	~InterpolationNoise();

  private:
	InterpolationFunction InterpFunc;
	// Interpolition Const
	int32 OutX;
	int32 OutY;
	int32 OutZ;

  public:
	void SetInterpolationConstants(InterpolationConstants type);
	FVector GetInterpolationConstants() const;

	float FuncInterpolation(const float value) const;
	void SetInterpolationFunction(InterpolationFunction function);

  private:
	float FuncInterpolation_Quintic(const float value) const;
};
