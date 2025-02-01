#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GlobalTypes_Structs.generated.h"

namespace Normalize{
	TArray<FChanceGroup> NormalizeChance(TArray<FChance> chanceGroup);
}

USTRUCT(BlueprintType)
struct FChance {
	GENERATED_BODY()

  public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlobalTypes|Chance",
			  meta = (ClampMin = "0.0", ClampMax = "100.0"))
	double Chance;
	FChance() : Chance(0.0) {
	}
};
USTRUCT(BlueprintType)
struct FChanceGroup {
	GENERATED_BODY()

  public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double Chance;
	FChanceGroup() : Chance(0.0) {}
	FChanceGroup(FChance chance) : Chance(chance.Chance) {
	}
};