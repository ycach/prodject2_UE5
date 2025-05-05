// Devoloper name: PenkovGS

#include "Meshes/NoiseMesh.h"

NoiseMesh::NoiseMesh(uint32 width, uint32 length, uint16 poligon_size, FVector start_pos)
	: FlatMesh(width, length, poligon_size, start_pos) {
}

NoiseMesh::~NoiseMesh() {
}

void NoiseMesh::Generation(UProceduralMeshComponent *procedural_mesh) {
	auto thread_grid = Async(EAsyncExecution::Thread, [&]() { GenerateGrid(); });

}
