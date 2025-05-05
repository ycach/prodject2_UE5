// Devoloper name: PenkovGS

#include "Meshes/ActorsChilds/FlatMeshCreater.h"

AFlatMeshCreater::AFlatMeshCreater() {
	PrimaryActorTick.bCanEverTick = false;
}

void AFlatMeshCreater::CreateMesh(UProceduralMeshComponent *procedural_mesh) {
	if (procedural_mesh) {
		mesh = MakeUnique<FlatMesh>(length, width, poligon_size, GetActorLocation());
		mesh->SetUVScale(scale_uv);
		mesh->GenerationGrid();
		mesh->GenerationSimpleGrid();
		procedural_mesh->CreateMeshSection(0, mesh->GetSimpleVertixes(), mesh->GetSimpleTriangles(), TArray<FVector>(),
										   mesh->GetUV(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Procedural mesh is null"));
	}
}
