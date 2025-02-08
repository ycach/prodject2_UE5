#include "MapGeneration/Chunk.h"
TArray<FVector2D> Chunk::GetPoints() {
	TArray<FVector2D> returnArr;
	returnArr.Add(point1);
	returnArr.Add(point2);
	returnArr.Add(point3);
	returnArr.Add(point4);
	return returnArr;
}
Chunk::Chunk(const Chunk &other)
	: point1(other.point1), point2(other.point2), point3(other.point3), point4(other.point4),
	  ObjectsInChunk(other.ObjectsInChunk), CriticalSection() // Инициализируем новый мьютекс, а не копируем старый
{
}
bool Chunk::inChunk(const FVector2D *point) {
	if (point->X >= point1.X && point->X <= point3.X && point->Y >= point1.Y && point->Y <= point2.Y)
		return true;
	return false;
}
bool Chunk::inChunk(const FVector *point) {
	if (point->X >= point1.X && point->X <= point3.X && point->Y >= point1.Y && point->Y <= point2.Y)
		return true;
	return false;
}

void Chunk::AddObject(FSimpleObject object) {
	FScopeLock Lock(&CriticalSection);
	ObjectsInChunk.Add(object);
}