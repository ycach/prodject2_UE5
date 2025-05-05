// Devoloper name: PenkovGS

#include "NoiseGeneration/NoiseCreaters/SimpleNoiseCreaters/PerlinNoise2DCreater.h"

PerlinNoise2DCreater::PerlinNoise2DCreater(const int32 seed, const float scale) : BaseNoise2DCreater(seed, scale) {
}

float PerlinNoise2DCreater::PointNoize(const float x, const float y) {
	FVector2D coord = ScaleStartCoordinate(x, y);

	int32 bot_left_x = FMath::FloorToInt(coord.X);
	int32 bot_left_y = FMath::FloorToInt(coord.Y);

	float distanse_left_line = (float)(coord.X - bot_left_x);
	float distanse_bot_line = (float)(coord.Y - bot_left_y);
	float distanse_right_line = 1 - distanse_left_line;
	float distanse_top_line = 1 - distanse_bot_line;

	float weight_x = interpolator.FuncInterpolation(distanse_left_line);
	float weight_y = interpolator.FuncInterpolation(distanse_bot_line);

	FVector interpolation_consts = interpolator.GetInterpolationConstants();

	int x0 = bot_left_x * interpolation_consts.X;
	int y0 = bot_left_x * interpolation_consts.Y;

	int x1 = x0 + interpolation_consts.X;
	int y1 = y0 + interpolation_consts.Y;

	float line_x0 = FMath::Lerp(hasher->GradientValue(x0, y0, distanse_left_line, distanse_bot_line),
							hasher->GradientValue(x1, y0, distanse_right_line, distanse_bot_line), weight_x);
	float line_x1 = FMath::Lerp(hasher->GradientValue(x0, y1, distanse_left_line, distanse_top_line),
							hasher->GradientValue(x1, y1, distanse_right_line, distanse_top_line), weight_x);

	return FMath::Lerp(line_x0, line_x1, weight_y);
}
