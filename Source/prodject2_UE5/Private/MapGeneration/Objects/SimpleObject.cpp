#include "MapGeneration/Objects/SimpleObject.h"

ASimpleObject_class::ASimpleObject_class() {
	PrimaryActorTick.bCanEverTick = false;
}
void ASimpleObject_class::BeginPlay() {
	Super::BeginPlay();
}

void ASimpleObject_class::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
#pragma region Collision
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("InteractCollision")) {
		if (InteractCollision == true)
			RadiusCollision = 0;
	}
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("RadiusCollision")) {
		if (InteractCollision == true && RadiusCollision > 0.)
			InteractCollision = false;
	}
#pragma endregion
}


