#include "MapGeneration/Objects/ColisionPoint.h"

float AColisionPoint::radiusColision = 0.0;

AColisionPoint::AColisionPoint() {
	PrimaryActorTick.bCanEverTick = false;
	bRoad = 0;
	bWater = 0;
	bPit = 0;
}
void AColisionPoint::SetupRadiusColision(int32 poligonSize) {
	radiusColision = 1.42 * poligonSize; // 1,42 - sqrt(2)
}

 void AColisionPoint::SetupRadiusCollisionWrapper(int32 poligonSize){
	SetupRadiusColision(poligonSize);
}

 bool AColisionPoint::isRoad(){
	return bRoad;
 }
bool AColisionPoint::isWater(){
	return bWater;
 }
 bool AColisionPoint::isPit(){
	return bPit;
 }
 FVector AColisionPoint::GetLoction() {
	return Location;
 }

 float AColisionPoint::GetRadiusColision() {
	return radiusColision;
 }

void AColisionPoint::SetRoad(){
	bRoad = 1;
 }
 void AColisionPoint::SetWater(){
	bWater = 1;
 }
void AColisionPoint::SetPit(){
	bPit = 1;
 }
 void AColisionPoint::SetLocation(FVector location){
	Location = location;
 }