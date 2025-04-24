// Devoloper name: Penkov G.S.

#pragma once

#include "CoreMinimal.h"

/**
 A class describing a procedural mesh map 
 vertex and serving for convenient storage 
 of information about it.
 */
class Vertix{
public:
	Vertix(const FVector3d position);
	Vertix(const float x, const float y, const float z);
	~Vertix();
private:
	float X;
	float Y;
	float Z;

  public:
	void SetX(const float x);
	void SetY(const float y);
	void SetZ(const float z);
	void SetPosition(const FVector3d position);
	void SetPosition(const float x, const float y, const float z);

	float GetX() const;
	float GetY() const;
	float GetZ() const;
	FVector3d GetPosition() const;
	FVector2d GetFlatPosition() const;
	

};
