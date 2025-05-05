// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "FlatMesh.h"

/**
 *
 */
class NoiseMesh : public FlatMesh {
  public:
	NoiseMesh(uint32 width, uint32 length, uint16 poligon_size, FVector start_pos = FVector(0, 0, 0));
	~NoiseMesh();

	virtual void Generation(UProceduralMeshComponent *procedural_mesh) override;
};
