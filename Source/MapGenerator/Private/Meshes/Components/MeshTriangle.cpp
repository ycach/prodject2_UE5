

// Devoloper name: PenkovGS

#include "Meshes/Components/MeshTriangle.h"

MeshTriangle::MeshTriangle(TSharedPtr<Vertix> v1, TSharedPtr<Vertix> v2, TSharedPtr<Vertix> v3)
	: vertix1(v1), vertix2(v2), vertix3(v3) {
	CheckNullPtrVertix();
}

MeshTriangle::~MeshTriangle() {
}

float MeshTriangle::GetZCoordinate(const FVector2D& point) const {
	FVector3d normal = GetNormal();

	// d-coefficient in the plane equation (nx*X + ny*Y + nz*Z + d = 0)
	float d = -(normal.X * vertix1->GetX() + normal.Y * vertix1->GetY() + normal.Z * vertix1->GetZ());
	float z = -(normal.X * point.X + normal.Y * point.Y) / normal.Z;
	return z;
}

FVector MeshTriangle::GetNormal() const {
	CheckNullPtrVertix();
	return FVector::CrossProduct(vertix2->GetPosition() - vertix1->GetPosition(),
								 vertix3->GetPosition() - vertix1->GetPosition());
}

FVector MeshTriangle::GetSafeNormal() const {
	return GetNormal().GetSafeNormal();
}

TArray<int32> MeshTriangle::GetIndex() const{
	return TArray<int32>({vertix1->GetIndex(), vertix2->GetIndex(), vertix3->GetIndex()});
}

bool MeshTriangle::PointInTriangle(const FVector2D& point) const {
	CheckNullPtrVertix();
	FVector2d a = vertix1->GetFlatPosition();
	FVector2d b = vertix2->GetFlatPosition();
	FVector2d c = vertix3->GetFlatPosition();

	auto OrientCalc = [](const FVector2D& point1, const FVector2D& point2, const FVector2D& point3) {
		return (float)((point2.X - point1.X) * (point3.Y - point1.Y) - (point3.X - point1.X) * (point2.Y - point1.Y));
	};

	float orient_abp = OrientCalc(a,b,point);
	float orient_bcp = OrientCalc(b,c,point);
	float orient_cap = OrientCalc(c, a, point);

	return (orient_abp >= 0 && orient_bcp >= 0 && orient_cap >= 0 || 
		orient_abp <= 0 && orient_bcp <= 0 && orient_cap <= 0) ? true : false;
}

void MeshTriangle::CheckNullPtrVertix() const {
	if (!vertix1 || !vertix2 || !vertix3)
		UE_LOG(LogTemp, Fatal, TEXT("Vertex pointer cannot be null!!"));
}
