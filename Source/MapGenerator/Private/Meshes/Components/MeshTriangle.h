// Devoloper name: PenkovGS

#pragma once

#include "Vertix.h"
#include "CoreMinimal.h"


/**
 A class that represents a procedural mesh triangle and implements various operations with it.
 */
class MeshTriangle
{
public:
	MeshTriangle(TSharedPtr<Vertix> v1, TSharedPtr<Vertix> v2, TSharedPtr<Vertix> v3);
	~MeshTriangle();

  private:
	TSharedPtr<Vertix> vertix1;
	TSharedPtr<Vertix> vertix2;
	TSharedPtr<Vertix> vertix3;

  public:
	float GetZCoordinate(const FVector2d point) const;
	FVector3d GetNormal() const;
	FVector GetSafeNormal() const;

  private:
	bool PointInTriangle(const FVector2d point) const;
	void CheckNullPtrVertix() const;
};
