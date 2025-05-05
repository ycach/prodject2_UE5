// Devoloper name: PenkovGS

#include "NoiseGeneration/Interpolation/InterpolationNoise.h"

InterpolationNoise::InterpolationNoise() {
	InterpFunc = InterpolationFunction::Quintic;
	OutX = 501125321;
	OutY = 1136930381;
	OutZ = 1720413743;
}

InterpolationNoise::~InterpolationNoise() {
}

void InterpolationNoise::SetInterpolationConstants(InterpolationConstants type) {
	switch (type) {
	case InterpolationConstants::DaveHoskins:
		OutX = 357149;
		OutY = 689269;
		OutZ = 84637;
		break;
	case InterpolationConstants::Gustavson:
		OutX = 1031;
		OutY = 70717;
		OutZ = 38473;
		break;
	case InterpolationConstants::PerlinOriginal:
		OutX = 7;
		OutY = 13;
		OutZ = 17;
		break;
	case InterpolationConstants::Defalt:
	default:
		OutX = 501125321;
		OutY = 1136930381;
		OutZ = 1720413743;
		break;
	}
}

FVector InterpolationNoise::GetInterpolationConstants() const{
	return FVector(OutX, OutY, OutZ);
}

float InterpolationNoise::FuncInterpolation(const float value) const {
	switch (InterpFunc) {
	case InterpolationFunction::Quintic:
	default:
		FuncInterpolation_Quintic(value);
		break;
	}
	return FuncInterpolation_Quintic(value);
}

void InterpolationNoise::SetInterpolationFunction(InterpolationFunction function) {
	InterpFunc = function;
}

float InterpolationNoise::FuncInterpolation_Quintic(const float value) const {
	// Optimal C^2-continuous quintic function
	return value * value * value * (value * (value * 6 - 15) + 10);
}
