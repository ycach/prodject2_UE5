// Devoloper name: PenkovGS

#include "NoiseGeneration/NoiseCreaters/FractalNoiseCreater/FractalGenerator.h"

FractalGenerator::FractalGenerator(const float gain, const float lacunarity, const float weight_octave,
								   const float fractal_bounding) {
	this->gain = gain;
	this->lacunarity = lacunarity;
	this->weight_octave = weight_octave;
	this->fractal_bounding = fractal_bounding;
}

void FractalGenerator::GenerateFractalNoise(BaseNoise2DCreater *noise_creater, const int32 length, const int32 width,
											const int32 octaves) {
	switch (fractal_type) {
	case FractalGenerator::FractalType::FBM:
	default:
		FBMFractal(noise_creater, length, width, octaves);
		break;
	}
	
}

void FractalGenerator::SetGain(const float new_gain) {
	gain = new_gain;
}

void FractalGenerator::SetLacunarity(const float new_lacunarity) {
	lacunarity = new_lacunarity;
}

void FractalGenerator::SetOctaveWeight(const float new_weight_octave) {
	weight_octave = new_weight_octave;
}

void FractalGenerator::SetFractalBounding(const float new_fractal_bounding) {
	fractal_bounding = new_fractal_bounding;
}

void FractalGenerator::SetFractalType(FractalType type) {
	fractal_type = type;
}

void FractalGenerator::FBMFractal(BaseNoise2DCreater *noise_creater, const int32 length, const int32 width,
								  const int32 octaves) {
	noise_creater->matrix = MakeShared<MatrixNoise2D>(length, width);
	for (int32 l = 0; l < length; l++) {
		for (int32 w = 0; w < width; w++) {
			int32 start_seed = noise_creater->GetSeed();
			float sum = 0;
			float amp = fractal_bounding;
			for (int octave = 0; octave < octaves; octave++) {
				float noise = noise_creater->PointNoize(w, l);
				noise_creater->SetSeed(start_seed + octave);
				sum = amp * noise;
				amp *= FMath::Lerp(1.0f, FMath::Min(noise + 1, 2) * 0.5f, fractal_bounding);
				w *= lacunarity;
				l *= lacunarity;
				amp *= gain;
			}
			(*noise_creater->matrix)[l][w] = sum;
			noise_creater->SetSeed(start_seed);
		}
	}
}
