// Devoloper name: PenkovGS

#include "Meshes/ActorsChilds/FlatMeshCreater.h"



AFlatMeshCreater::AFlatMeshCreater() {

	PrimaryActorTick.bCanEverTick = false;
}

void AFlatMeshCreater::CreateMesh(UProceduralMeshComponent *procedural_mesh) {
	if (procedural_mesh){
		mesh = MakeUnique<FlatMesh>(length, width, poligon_size, GetActorLocation());
		mesh->SetUVScale(scale_uv);
		mesh->Generation(procedural_mesh);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Procedural mesh is null"));
	}
	
}
