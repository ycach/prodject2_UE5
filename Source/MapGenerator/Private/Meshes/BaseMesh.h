// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshTriangle.h"
#include "ProceduralMeshComponent.h"

/**
 * Core procedural mesh class
 */
class BaseMesh {
  public:
	BaseMesh(uint32 width, uint32 length, uint16 poligon_size, FVector start_pos = FVector(0, 0, 0),
			 FRotator start_rotation = FRotator::ZeroRotator);
	virtual ~BaseMesh() = default;

  protected:
	uint32 poligons_in_width;
	uint32 poligons_in_length;
	uint16 poligon_size;
	float scale_uv;

	FVector start_pos;
	FRotator start_rotation;

	TArray<TArray<TSharedPtr<Vertix>>> vertixes;
	TArray<FVector> simples_vertixes;
	TArray<TArray<TSharedPtr<MeshTriangle>>> triangles;
	TArray<int32> simples_triangles;
	TArray<FVector2D> uv;

	TArray<FVector> global_bounds;

  public:
	virtual void GenerationGrid() = 0;
	void GenerationSimpleGrid();

	float GetLength() const;
	float GetWidth() const;

	uint32 GetLengthInPoligons() const;
	uint32 GetWidthInPoligons() const;

	uint16 GetPoligonSize() const;

	TArray<TArray<TSharedPtr<Vertix>>> &GetVertixes();
	TArray<FVector> &GetSimpleVertixes();
	TArray<TArray<TSharedPtr<MeshTriangle>>> &GetTriangles();
	TArray<int32> &GetSimpleTriangles();
	TArray<FVector2d> &GetUV();

	TSharedPtr<MeshTriangle> &FindParentTriangle(const FVector2D &point);

	TArray<FVector> GetGlobalBounds() const;

	void SetNewStartPos(const FVector new_point);

	void SetUVScale(const float scale);



  protected:
	bool PointInMesh() const;
	void GenerateSimpleVertixes();
	void GenerateSimpleTriangles();

  private:
	void FindGlobalBounds();
};
