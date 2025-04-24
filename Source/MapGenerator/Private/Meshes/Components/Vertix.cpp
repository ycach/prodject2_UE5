// Devoloper name: PenkovGS

#include "Meshes/Components/Vertix.h"

Vertix::Vertix(const FVector3d position) {
	SetPosition(position);
}

Vertix::Vertix(const float x, const float y, const float z) {
	SetPosition(x,y,z);
}

Vertix::~Vertix() {
}

void Vertix::SetX(const float x) {
	X = x;
}

void Vertix::SetY(const float y) {
	Y = y;
}

void Vertix::SetZ(const float z) {
	Z = z;
}

void Vertix::SetPosition(const FVector3d position) {
	X = position.X;
	Y = position.Y;
	Z = position.Z;
}

void Vertix::SetPosition(const float x, const float y, const float z) {
	X = x;
	Y = y;
	Z = z;
}

float Vertix::GetX() const {
	return X;
}

float Vertix::GetY() const {
	return Y;
}

float Vertix::GetZ() const {
	return Z;
}

FVector3d Vertix::GetPosition() const {
	return FVector3d(X, Y, Z);
}

FVector2d Vertix::GetFlatPosition() const {
	return FVector2d(X, Y);
}
