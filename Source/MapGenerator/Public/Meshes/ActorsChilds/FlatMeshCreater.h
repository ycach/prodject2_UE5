// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "Meshes/FlatMesh.h"
#include "Meshes/ActorsChilds/BaseMeshCreater.h"
#include "FlatMeshCreater.generated.h"

UCLASS()
class MAPGENERATOR_API AFlatMeshCreater : public ABaseMeshCreater {
	GENERATED_BODY()
  public:
	AFlatMeshCreater();
  private:
	TUniquePtr<FlatMesh> mesh;
  public:
	void CreateMesh(UProceduralMeshComponent *procedural_mesh) override;
};
