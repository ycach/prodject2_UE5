// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class BaseHasher {

  public:
	enum class GradType {
		Table,
		CalcWithHash
	};

	BaseHasher(int32 seed);
	virtual ~BaseHasher() = default;

  protected:
	int32 seed;
	GradType grad_type;

  public:
	int32 GetSeed() const;
	void SetSeed(const int32 new_seed);
	virtual void SetGradType(GradType type) = 0;

  protected:
	int32 GetGradientIndex(const int32 hash, const int32 size_table) const;
};

class Hasher2D : public BaseHasher {
  public:
	enum class Tables {
		DefaultTable
	};
	enum class GradFunctions {
		SimpleTrigonometric
	};
	Hasher2D(const int32 seed);
	~Hasher2D() = default;

  private:
	TArray<FVector2D> grad_table;
	GradFunctions function_type;

  public:
	void LoadTable(const Tables table);
	void EmptyTable();
	void ChangeFunction(const GradFunctions table);

	float GradientValue(const FVector2D noize_point, const float x_distance, const float y_distance) const;
	float GradientValue(const float x, const float y, const float x_distance, const float y_distance) const;

	void SetGradType(GradType type) override;

  private:
	float Hash(const FVector2D point) const;
	float Hash(const int32 x, const int32 y) const;
	float HashAlgoritm(const int32 x, const int32 y) const;

	FVector2D GradientVectorWithTable(const int32 hash) const;
	FVector2D GradientVectorWithFunction(const int32 hash) const;

	void LoadDefaultTable();
	FVector2D SimpleTrigonometricFunction(const int32 hash) const;

};