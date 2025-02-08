#include "MapGeneration/MapObjects_Structs.h"

float FCollisionPoint::radiusColision = 0.0f;
float FCollisionPoint::FindSuareSquare() {
	float a = 2 * radiusColision / 1.42;
	return a * a;
}

float FCollisionPoint::FindCircleSquare() {
	return radiusColision * radiusColision * 3.2;
}

void FSimpleObject::SetupFSimpleObject() {
	ASimpleObject_class *DefaultObject;
	if (Object) {
		DefaultObject = Object->GetDefaultObject<ASimpleObject_class>();
		RadiusCollision = DefaultObject->RadiusCollision;
		RotatesWithMesh = DefaultObject->RotatesWithMesh;
		InteractCollision = DefaultObject->InteractCollision;
		InteractWater = DefaultObject->InteractWater;
		InteractRoads = DefaultObject->InteractRoads;
		InteractPits = DefaultObject->InteractPits;
	}
}
