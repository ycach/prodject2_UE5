#pragma once

#include "CoreMinimal.h"
#include "MapGeneration/MapObjects_Structs.h"

class Chunk {
  public:
	Chunk(FVector2D point1, FVector2D point2, FVector2D point3, FVector2D point4)
		: point1(point1), point2(point2), point3(point3), point4(point4){}
	~Chunk(){}

  protected:
  private:
	FVector2D point1;
	FVector2D point2;
	FVector2D point3;
	FVector2D point4;
  public:
	TArray<FMapObject> ObjectsInChunk;
	TArray<FVector2D> GetPoints();
	bool inChunk(const FVector2D *point);
	bool inChunk(const FVector *point);
	void AddObject(FMapObject object);
	
};
