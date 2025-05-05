// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "BaseMesh.h"

/**
 A class for generating and storing flat procedural mesh geometry
 */
class FlatMesh : public BaseMesh {
  public:
	FlatMesh(uint32 width, uint32 length, uint16 poligon_size, FVector start_pos = FVector(0, 0, 0));
	~FlatMesh();
	virtual void GenerationGrid() override;

  private:
	void GenerateUV();
	void GenerateGrid();
};
