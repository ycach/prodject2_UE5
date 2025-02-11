#pragma once

#include "CoreMinimal.h"
#include "Generation/HardObjectSpawn.h"
#include "LandscapeGenerator.generated.h"

/**
 *
 */
UCLASS()
class PRODJECT2_UE5_API ALandscapeGenerator : public AHardObjectSpawn {
	GENERATED_BODY()

  public:
	ALandscapeGenerator();

  protected:
	virtual void BeginPlay() override;

  public:
	static TArray<TArray<FObjectThatHaveLocation>> SpawnedObjectsInChanks;

#pragma region DEBUG_BP_Functions

	UFUNCTION(BlueprintCallable, Category = "Landscape|DEBUG")
	void Debug_BordersSpawn(bool bAB_spawn, bool bAC_spawn, bool bBD_spawn, bool bCD_spawn);

#pragma endregion

#pragma region Blueprint vision
#pragma region values

	/////borders func parametrs
	/// k1 - parameter before function
	UPROPERTY(EditAnywhere, Category = "Landscape") int16 ka_min;
	UPROPERTY(EditAnywhere, Category = "Landscape") int16 ka_max;

	/// k2- parameter before argument
	UPROPERTY(EditAnywhere, Category = "Landscape") int16 kf_min;
	UPROPERTY(EditAnywhere, Category = "Landscape") int16 kf_max;

#pragma endregion
#pragma region events and functions
	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void SetupGenerationObject(); /////!!!!! обязательна к вызову перед генерацией
#pragma region Hills
	UFUNCTION(BlueprintCallable, Category = "Landscape|Hills")
	void GenerateObjectsOnMap_Hills(int indentForBorders,
														  const TArray<FMapObject> &Objects_Borders,
														  const TArray<FMapObject> &Objects_Map, float densities,
														  int deltaQualityObjects);
#pragma endregion

#pragma endregion
#pragma endregion

  private:
#pragma region values

	/// for Border Function
	int16 k1_f;
	int16 k2_f;
	int16 k1_a;
	int16 k2_a;

	TArray<TArray<FObjectThatHaveLocation>> arrBorders; // array of arrays of borders objects

#pragma endregion

#pragma region functions
#pragma region Landscape Generator

	////////////////////////////////////////////////////////
#pragma endregion

#pragma region Objects Generators

#pragma region Borders
	void BordersObjectsCreate(int indentForBorders, const TArray<FMapObject> &Objects,
							  float (ALandscapeGenerator::*Function)(float x));

	float FunctionBorders(float x);

	void AnalyzeBorderPoints(const TArray<TArray<FVector>> &BorderPoints, float &MaxY, float &MinY, float &MaxX,
							 float &MinX);

	void FindBordersObjectCoordinate(const FVector &Start, const FVector &End, const TArray<FMapObject> &Objects,
									 TArray<FObjectThatHaveLocation> *result);

	TArray<TArray<FVector>> NodesYPoints(float (ALandscapeGenerator::*Function)(float x));

#pragma endregion
#pragma endregion
#pragma endregion
};
