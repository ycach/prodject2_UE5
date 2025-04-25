// Devoloper name: PenkovGS

#include "Meshes/Components/Vertix.h"

Vertix::Vertix(const uint64 index, const FVector position) {
	this->index = index;
	CheckIndex();
	SetPosition(position);
}

Vertix::Vertix(const uint64 index, const float x, const float y, const float z) {
	this->index = index;
	CheckIndex();
	SetPosition(x, y, z);
}

Vertix::~Vertix() {
}

void Vertix::SetX(const float x) {
	position.X = x;
}

void Vertix::SetY(const float y) {
	position.Y = y;
}

void Vertix::SetZ(const float z) {
	position.Z = z;
}

void Vertix::SetPosition(const FVector& pos) {
	this->position = position;
}

void Vertix::SetPosition(const float x, const float y, const float z) {
	this->position = FVector(x, y, z);
}

int32 Vertix::GetIndex() const {
	return index;
}

float Vertix::GetX() const {
	return position.X;
}

float Vertix::GetY() const {
	return position.Y;
}

float Vertix::GetZ() const {
	return position.Z;
}

FVector3d Vertix::GetPosition() const {
	return position;
}

FVector2d Vertix::GetFlatPosition() const {
	return FVector2d(position.X, position.Y);
}

void Vertix::operator+=(const FVector &point) {
	position += point;
}

void Vertix::operator-=(const FVector &point) {
	position -= point;
}

void Vertix::operator+=(const FVector2D &point) {
	position += FVector(point.X, point.Y, 0.0f);
}

void Vertix::operator-=(const FVector2D &point) {
	position -= FVector(point.X, point.Y, 0.0f);
}

void Vertix::CheckIndex() {
	if (index < 0)
		UE_LOG(LogTemp, Fatal, TEXT("Index < 0!!!"));
}
