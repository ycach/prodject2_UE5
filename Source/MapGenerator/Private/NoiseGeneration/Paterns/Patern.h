// Devoloper name: PenkovGS

#pragma once

#include "CoreMinimal.h"

float PATERN_VALUE = 2.0f;

/**
 *
 */
class Patern {
  public:
	Patern();
	virtual ~Patern() = default;

  protected:
	virtual TArray<float> GetPoints() = 0;
};
