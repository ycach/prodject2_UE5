// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "BaseMeshCreater.generated.h"

UCLASS()
class ABaseMeshCreater : public AActor {
	GENERATED_BODY()

  public:
	ABaseMeshCreater();

	UPROPERTY(EditAnywhere, Meta = (ToolTip = "Mesh Y-size in poligons", ClampMin = "2", ClampMax = "100000"),
			  Category = "MapGenerator|MeshCreate|MeshParametrs")
	int32 length;
	UPROPERTY(EditAnywhere, Meta = (ToolTip = "Mesh X-size in poligons", ClampMin = "2", ClampMax = "100000"),
			  Category = "MapGenerator|MeshCreate|MeshParametrs")
	int32 width;
	UPROPERTY(EditAnywhere, Meta = (ToolTip = "Polygon size", ClampMin = "10", ClampMax = "500"),
			  Category = "MapGenerator|MeshCreate|MeshParametrs")
	int32 poligon_size;

	UPROPERTY(EditAnywhere, Meta = (ToolTip = "Length UV scale "),
			  Category = "MapGenerator|MeshCreate|UV Parametrs")
	float scale_uv;

  public:
	UFUNCTION(BlueprintCallable, Category = "MapGenerator|MeshCreate")
	virtual void CreateMesh(UProceduralMeshComponent *procedural_mesh);
};
