// Devoloper name: Penkov G.S.

#pragma once

#include "CoreMinimal.h"

/**
 A class describing a procedural mesh map
 vertex and serving for convenient storage
 of information about it.
 */
class Vertix {
  public:
	Vertix(const uint64 index, const FVector position);
	Vertix(const uint64 index, const float x, const float y, const float z);
	~Vertix();

  private:
	FVector position;
	int32 index;

  public:
	void SetX(const float x);
	void SetY(const float y);
	void SetZ(const float z);
	void SetPosition(const FVector& pos);
	void SetPosition(const float x, const float y, const float z);

	int32 GetIndex() const;
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	FVector GetPosition() const;
	FVector2D GetFlatPosition() const;

	void operator+=(const FVector &point);
	void operator-=(const FVector &point);

	void operator+=(const FVector2D &point);
	void operator-=(const FVector2D &point);

  private:
	void CheckIndex();
};
