// Devoloper name: PenkovGS

#include "Meshes/ActorsChilds/BaseMeshCreater.h"

ABaseMeshCreater::ABaseMeshCreater() {
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseMeshCreater::CreateMesh(UProceduralMeshComponent *procedural_mesh) {
	UE_LOG(LogTemp, Warning, TEXT("CreateMesh is not overridden"));
}
