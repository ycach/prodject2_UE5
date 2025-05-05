#include "Hashers.h"

Hasher2D::Hasher2D(int32 seed) : BaseHasher(seed) {
	SetGradType(GradType::CalcWithHash);
}


float Hasher2D::Hash(const FVector2D point) const {
	return HashAlgoritm((int32)point.X, (int32)point.Y);
}

float Hasher2D::Hash(const int32 x, const int32 y) const {
	return HashAlgoritm(x, y);
}

float Hasher2D::GradientValue(const FVector2D noize_point, const float x_distance, const float y_distance) const {
	return GradientValue(noize_point.X, noize_point.Y, x_distance, y_distance);
}

float Hasher2D::GradientValue(const float x, const float y, const float x_distance, const float y_distance) const {
	int hash = Hash(x, y);
	hash ^= hash >> 15;
	FVector2D grad;
	if (grad_type == GradType::Table) {
		grad = GradientVectorWithTable(hash);
	} else {
		grad = GradientVectorWithFunction(hash);
	}
	return grad.X * x_distance + grad.Y * y_distance;
}

void Hasher2D::SetGradType(GradType type) {
	grad_type = type;
	if (type == GradType::CalcWithHash) {
		EmptyTable();
		function_type = GradFunctions::SimpleTrigonometric;
	} else{
		LoadDefaultTable();
	}
}

float Hasher2D::HashAlgoritm(const int32 x, const int32 y) const {
	float hash_value = seed ^ x ^ y;
	hash_value *= 0x27d4eb2d;
	return hash_value;
}

FVector2D Hasher2D::GradientVectorWithTable(const int32 hash) const {
	int32 index = GetGradientIndex(hash, grad_table.Num());
	return grad_table[index];
}

FVector2D Hasher2D::GradientVectorWithFunction(const int32 hash) const {
	FVector2D grad;
	switch (function_type) {
	case Hasher2D::GradFunctions::SimpleTrigonometric:
	default:
		grad = SimpleTrigonometricFunction(hash);
		break;
	}
	return grad;
}

void Hasher2D::LoadTable(const Tables table) {
	switch (table) {
	case Hasher2D::Tables::DefaultTable:
	default:
		LoadDefaultTable();
		break;
	}
}

void Hasher2D::EmptyTable() {
	if (!grad_table.IsEmpty())
		grad_table.Empty();
}

void Hasher2D::ChangeFunction(const GradFunctions function) {
	function_type = function;
}
