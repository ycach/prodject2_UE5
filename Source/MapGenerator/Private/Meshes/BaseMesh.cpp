// Devoloper name: PenkovGS

#include "Meshes/BaseMesh.h"

BaseMesh::BaseMesh(uint32 width, uint32 length, uint16 poligon_size, FVector start_pos, FRotator start_rotation) {
	this->poligons_in_width = width;
	this->poligons_in_length = length;
	this->poligon_size = poligon_size;

	this->start_pos = start_pos;
	this->start_rotation = start_rotation;
	this->start_rotation.Roll = 0.0f;
	this->start_rotation.Yaw = 0.0f;

	scale_uv = 100.0f;

	vertixes.SetNum(length + 1);
	triangles.SetNum(length);

	global_bounds.SetNum(4);
	FindGlobalBounds();
}

void BaseMesh::GenerationSimpleGrid(){
	auto thread_triangles = Async(EAsyncExecution::Thread, [&]() { GenerateSimpleVertixes(); });
	auto thread_vertixes = Async(EAsyncExecution::Thread, [&]() { GenerateSimpleTriangles(); });

	thread_triangles.Wait();
	thread_vertixes.Wait();

}

float BaseMesh::GetLength() const {
	return poligons_in_length * poligon_size;
}

float BaseMesh::GetWidth() const {
	return poligons_in_width * poligon_size;
}

uint32 BaseMesh::GetLengthInPoligons() const {
	return poligons_in_length;
}

uint32 BaseMesh::GetWidthInPoligons() const {
	return poligons_in_width;
}

uint16 BaseMesh::GetPoligonSize() const {
	return poligon_size;
}

TArray<TArray<TSharedPtr<Vertix>>> &BaseMesh::GetVertixes() {
	return vertixes;
}
TArray<FVector> &BaseMesh::GetSimpleVertixes() {
	return simples_vertixes;
}

TArray<TArray<TSharedPtr<MeshTriangle>>> &BaseMesh::GetTriangles() {
	return triangles;
}
TArray<int32> &BaseMesh::GetSimpleTriangles() {
	return simples_triangles;
}

TArray<FVector2d> &BaseMesh::GetUV() {
	return uv;
}

TArray<FVector> BaseMesh::GetGlobalBounds() const {
	return global_bounds;
}

void BaseMesh::SetNewStartPos(const FVector new_point) {
	start_pos = new_point;
	ParallelFor(poligons_in_length, [&](int32 index) {
		for (TSharedPtr<Vertix> vertix : vertixes[index]) {
			*vertix += new_point;
		}
	});
	FindGlobalBounds();
}

void BaseMesh::SetUVScale(const float scale) {
	scale_uv = scale;
}

bool BaseMesh::PointInMesh() const {
	return false;
}
void BaseMesh::GenerateSimpleTriangles() {
	for (TArray<TSharedPtr<Vertix>> &vertix_line : vertixes) {
		for (TSharedPtr<Vertix> vertix : vertix_line) {
			simples_vertixes.Add(vertix->GetPosition());
		}
	}
}

void BaseMesh::GenerateSimpleTriangles() {
	for (TArray<TSharedPtr<MeshTriangle>> &triangle_line : triangles) {
		for (TSharedPtr<MeshTriangle> triangle : triangle_line) {
			simples_triangles.Append(triangle->GetIndex());
		}
	}
}

void BaseMesh::FindGlobalBounds() {
	FQuat Quaternion = start_rotation.Quaternion();

	FVector local_bounds[4] = {{0, 0, 0}, {0, GetLength(), 0}, {GetWidth(), 0, 0}, {GetWidth(), GetLength(), 0}};

	for (int i = 0; i < 4; ++i) {
		FVector bound_vertix = local_bounds[i];
		FVector rotate_vertix = Quaternion.RotateVector(bound_vertix);
		global_bounds[i] = rotate_vertix + start_pos;
	}
}
