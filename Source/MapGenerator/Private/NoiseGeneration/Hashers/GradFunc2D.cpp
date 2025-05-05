#include "Hashers.h"

FVector2D Hasher2D::SimpleTrigonometricFunction(const int32 hash) const{
	float angle = ((hash & 0xFFFF) / 65535.0f) * UE_TWO_PI;
	return FVector2D(FMath::Cos(angle), FMath::Sin(angle));
}