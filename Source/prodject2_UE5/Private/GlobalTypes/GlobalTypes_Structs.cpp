#include "GlobalTypes/GlobalTypes_Structs.h"

TArray<FChanceGroup> Normalize::NormalizeChance(TArray<FChance> chanceGroup) {
	double sum = 0.0;
	for (int i = 0; i < chanceGroup.Num(); i++) {
		sum += chanceGroup[i].Chance;
	}

	TArray<FChanceGroup> normalizedGroup;

	if (sum == 0.0) {
		int numElements = chanceGroup.Num();
		if (numElements > 0) {
			double temp = 100.0 / static_cast<double>(numElements);
			double accumulatedSum = 0.0;

			for (int i = 0; i < numElements; i++) {
				FChanceGroup accumulatedChance;
				accumulatedSum += (i + 1) * temp;
				if (i == numElements - 1) {
					accumulatedSum = 100.0;
				}
				accumulatedChance.Chance = accumulatedSum;
				normalizedGroup.Add(accumulatedChance);
			}
		}
		return normalizedGroup;
	}

	// Создаем новый массив для нормализованных значений
	double accumulatedSum = 0.0;
	for (FChance &chance : chanceGroup) {
		// Нормализуем значение Chance
		FChance normalizedChance;
		normalizedChance.Chance = static_cast<double>(chance.Chance / sum * 100.0);

		// Накапливаем сумму
		accumulatedSum += normalizedChance.Chance;

		// Сохраняем накопленное значение в новом элементе
		FChanceGroup accumulatedChance;
		accumulatedChance.Chance = accumulatedSum;
		normalizedGroup.Add(accumulatedChance);
	}
	return normalizedGroup;
}
